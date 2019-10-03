
#include <stdint.h>
#include <stdlib.h>

#define DEBUG
#if !defined (DEBUG)
#define malloc_debug(...)
#else
#define _MALLOC_DEBUG
#include <stdio.h>
#define malloc_debug(...) printf(__VA_ARGS__)
#endif

#if (__SIZEOF_SIZE_T__ == 4)
#define MALLOC_ALIGN_LOG    (3)
#else
#define MALLOC_ALIGN_LOG    (4)
#endif

#define MALLOC_ALIGN        (1 << MALLOC_ALIGN_LOG)
#define MALLOC_ALIGN_MASK   ((1 << MALLOC_ALIGN_LOG) - 1)

/* These are the bits in size_this - there are 3 (or 4) spare */
#define BLK_USED            (1<<0)
#define BLK_END             (1<<1)
#define BLK_BITMASK         (BLK_USED|BLK_END)

#define MALBLK_TO_PTR(__p) ((char *)__p + sizeof(size_t))
#define PTR_TO_MALLOC_BLOCK(__p) ((malblk_t *)((char *)__p - sizeof(size_t)))

struct malblk
{
    /*
     * size_this is the entire size of the malloc block. This includes the
     * user data and the 2 size_this-es
     */
    size_t size_this;

    /* Malloc blocks are variable size. We represent it with a single byte */
    char data[1];

    /*
     * There's another size_this at the end but we don't know exactly where
     * it is. It can be accessed using the macro below
     */
    //size_t size_this2;
};
typedef struct malblk malblk_t;

static malblk_t *freelist_get(size_t size);
static void freelist_put(malblk_t *blk);
static void freelist_remove(malblk_t *blk);

static malblk_t *_Sysmem_get(size_t size);
#if defined (_MALLOC_DEBUG)
static int _SysmemCheck(void);
#endif

static malblk_t *malblk_split(malblk_t *blk, size_t size);
static size_t malblk_round_up(size_t val);
static malblk_t *malblk_try_join_prev(malblk_t *blk);
static malblk_t *malblk_try_join_next(malblk_t *blk);


static void blk_size_and_flags_set(malblk_t *pblk, size_t size_and_flags);
static size_t blk_size_get(malblk_t *pblk);
static void blk_used_set(malblk_t *pblk);
static void blk_used_clr(malblk_t *pblk);

static malblk_t *blk_prev(malblk_t *pblk);
static malblk_t *blk_next(malblk_t *pblk);
static int blk_used_get(malblk_t *pblk);
static int blk_end_get(malblk_t *pblk);

static malblk_t *_Core;

extern char __ebss__;
extern char __eram__;

malblk_t *malblk_split(malblk_t *blk, size_t size)
{
    /*
     * Split a block into 2.
     *
     * Blocks that are being split will generally be not in use and therefore
     * their flag bits are assumed to be clear.
     *
     * In fact we will assert that they are clear
     */

    malblk_t *rem_blk;
    size_t rem_size;
    size_t curr_size;
    size_t requ_size;

    /* Size of the requested split block */
    requ_size = size + 2 * sizeof(size_t);
    /* Size of the block we are splitting */
    curr_size = blk_size_get(blk);

    malloc_debug("splitting %d bytes, size=%d, split_size=%d\n", curr_size, size, requ_size);

    /* Check that the split is worth doing */
    if ((requ_size + 2 * sizeof(void *)) >= curr_size)
    {
        malloc_debug("no split needed\n");
        /* It's only just big enough so return NULL to indicate no split */
        return NULL;
    }

    /* We'll split off from the front of the blk. Firstly we figure out where
     * the remaining block will start
     */
    rem_blk = (malblk_t *)((char *)blk + requ_size);
    rem_size = blk_size_get(blk) - requ_size;
    malloc_debug("rem_size=%d\n", rem_size);
    blk_size_and_flags_set(rem_blk, rem_size);
    blk_size_and_flags_set(blk, requ_size);

    return rem_blk;
}

size_t malblk_round_up(size_t val)
{
    /*
     * The minimum allocable size is 2 ptrs. In fact this is the granularity
     * of the allocator.
     *
     * Each block must also contain 2 size_t fields. Therefore roundup as
     * follows.
     *
     *  0 -  8 bytes >> 4 ptrs >> 16 bytes
     *  9 - 16 bytes >> 6 ptrs >> 24 bytes
     * 17 - 24 bytes >> 8 ptrs >> 32 bytes
     * etc.
     *
     * For 64 bit libs the values double.
     *
     *  0 - 16 bytes >> 4 ptrs >> 32 bytes
     * 17 - 32 bytes >> 6 ptrs >> 48 bytes
     * 33 - 48 bytes >> 8 ptrs >> 64 bytes
     * etc.
     */

    if (val == 0)
    {
        val = 1;
    }

    val += MALLOC_ALIGN_MASK;
    val &= ~MALLOC_ALIGN_MASK;
    val += 2 * sizeof(void *);

    /* Check that we didn't overflow */
    if (val > (SIZE_MAX - 4 * sizeof(void*)))
    {
        malloc_debug("OFLOW: size=%d\n", val);
        return 0;
    }

    return val;
}


malblk_t *malblk_try_join_prev(malblk_t *blk)
{
    /*
     * Join blk with the prev block but only if the prev block is not
     * the end and not busy
     */
    malblk_t *prev;
    size_t total;

    malloc_debug("malblk_try_join_prev - ");

    prev = blk_prev(blk);

    if (!prev || blk_used_get(prev) || blk_end_get(prev))
    {
        malloc_debug("fail\n");
        malloc_debug("prev=%08lx\n", prev);
        if (prev)
        {
            malloc_debug("used=%d\n", blk_used_get(prev));
            malloc_debug("end=%d\n", blk_end_get(prev));
        }
        return blk;
    }

    /* Take the prev from the free list */
    freelist_remove(prev);

    /* Now join them */
    total = blk_size_get(prev) + blk_size_get(blk);
    malloc_debug("done, join %d %d ", blk_size_get(prev), blk_size_get(blk));
    blk_size_and_flags_set(prev, total);
    malloc_debug("got %d\n", total);

    return prev;
}

malblk_t *malblk_try_join_next(malblk_t *blk)
{
    /* Join blk with the next block but only if the next block is not
     * the end and not busy
     */
    malblk_t *next;
    size_t total;

    malloc_debug("malblk_try_join_next - ");

    next = blk_next(blk);

    if (!next || blk_used_get(next) || blk_end_get(next))
    {
        malloc_debug("fail\n");
        return blk;
    }

    malloc_debug("used=%d\n", blk_used_get(next));
    malloc_debug("end=%d\n", blk_end_get(next));

    /* Take the next from the free list */
    freelist_remove(next);

    /* Now join them */
    total = blk_size_get(blk) + blk_size_get(next);
    malloc_debug("done, join %d %d ", blk_size_get(blk), blk_size_get(next));
    blk_size_and_flags_set(blk, total);
    malloc_debug("got %d\n", total);

    return blk;
}

/*
 *
 * Moving forward or back through a 'zone' of blocks
 *
 *
 */

static malblk_t *blk_prev(malblk_t *pblk)
{
    /* We just need to look at the size_t sized word right before the start
     * of the current block. As long as it's not an 'end' we can just subtract
     */
    size_t prev_size;

    prev_size = *((size_t *)pblk - 1);

    if ((prev_size & BLK_END) == BLK_END)
    {
        /* It's an end so ... */
        return NULL;
    }
    prev_size &= ~BLK_BITMASK;

    return  (malblk_t *)((char *)pblk - prev_size);
}

static malblk_t *blk_next(malblk_t *pblk)
{
    /* We just need to look at the size_t sized word right after the end
     * of the current block. As long as it's not an 'end' we can just add
     */
    malblk_t *pnext;

    pnext = (malblk_t *)((char *)pblk + blk_size_get(pblk));

    if ((pnext->size_this & BLK_END) == BLK_END)
    {
        /* It's an end so ... */
        return NULL;
    }

    return pnext;
}

/*
 *
 * Access functions for the members of malblk_t
 *
 *
 */

#define blk_size_this2(__blk) \
        (*((size_t *)((char *)(__blk) + (size_and_flags & ~BLK_BITMASK) - sizeof(size_t))))

void blk_size_and_flags_set(malblk_t *pblk, size_t size_and_flags)
{
    pblk->size_this = size_and_flags;
    blk_size_this2(pblk) = size_and_flags;
}

size_t blk_size_get(malblk_t *pblk)
{
    return pblk->size_this & ~BLK_BITMASK;
}

int blk_used_get(malblk_t *pblk)
{
    return (pblk->size_this & BLK_USED) == BLK_USED;
}

void blk_used_set(malblk_t *pblk)
{
    pblk->size_this |= BLK_USED;
}

void blk_used_clr(malblk_t *pblk)
{
    pblk->size_this &= ~BLK_USED;
}

int blk_end_get(malblk_t *pblk)
{
    return (pblk->size_this & BLK_END) == BLK_END;
}


/* Each chunk looks like this
 *
 *
 *     chunk *next
 *     size_t chunk size | BLK_END
 *         ...  the mem between the 2 size_ts is used by malloc
 *         ...  chunk size - 2 size_t bytes of usable data
 *         ...
 *     size_t chunk size | BLK_END
 *
 */

malblk_t *_Sysmem_get(size_t size) {

    malblk_t *retp = NULL;
    size = size;

    if (!_Core) {
        malloc_debug("getting system memory %08lx\n", size);
        malloc_debug("__ebss__ = %08x\n", &__ebss__);
        malloc_debug("__erom__ = %08x\n", &__eram__);

        size_t more_core_size = &__eram__ - &__ebss__;
        // Leave space for the main stack
        more_core_size -= 1024;

        malloc_debug("size = %d\n", more_core_size);
        _Core = (malblk_t *)&__ebss__;
        //memset(_Core, 0, more_core_size);
        blk_size_and_flags_set(_Core, more_core_size | BLK_END);
        /* Make this into a free block */
        retp = (malblk_t *)((char *)_Core + sizeof(size_t));
        blk_size_and_flags_set(retp, more_core_size - 2 * sizeof(size_t));
        malloc_debug("getting system memory %08lx\n", _Core);
    }

    return retp;
}

#if defined (_MALLOC_DEBUG)
int _SysmemCheck()
{
    malblk_t *p;
    malblk_t *item;

    p = _Core;

    malloc_debug("Core was %x\n", p->size_this);

    /* Move to the first item */
    item = (malblk_t *)((char *)p + sizeof(size_t));

    while (1)
    {
        size_t  size;
        int     flags;

        size  = item->size_this & ~0x3;
        flags = item->size_this & 0x3;
        malloc_debug("%08lx %d(%x) f=%x\n", &item->data, size, size, flags);

        /* Move to the next item */
        item = (malblk_t *)((char *)item + size);

        if (item->size_this & 0x2)
        {
            break;
        }
    }

    return 0;
}
#endif

#define MAX_BUCKETS (65)

#define MALBLK_TO_BUCKET(__p) ((bucket_t *)(MALBLK_TO_PTR(__p)))
#define BUCKET_TO_MALLOC_BLOCK(__p) PTR_TO_MALLOC_BLOCK(__p)

struct bucket
{
    struct bucket *prev;
    struct bucket *next;
};
typedef struct bucket bucket_t;

#define _B(i) { &Bucket[(i)], &Bucket[(i)] }
static bucket_t Bucket[MAX_BUCKETS] = {
    _B( 0), _B( 1), _B( 2), _B( 3), _B( 4), _B( 5), _B( 6), _B( 7),
    _B( 8), _B( 9), _B(10), _B(11), _B(12), _B(13), _B(14), _B(15),
    _B(16), _B(17), _B(18), _B(19), _B(20), _B(21), _B(22), _B(23),
    _B(24), _B(25), _B(26), _B(27), _B(28), _B(29), _B(30), _B(31),
    _B(32), _B(33), _B(34), _B(35), _B(36), _B(37), _B(38), _B(39),
    _B(40), _B(41), _B(42), _B(43), _B(44), _B(45), _B(46), _B(47),
    _B(48), _B(49), _B(50), _B(51), _B(52), _B(53), _B(54), _B(55),
    _B(56), _B(57), _B(58), _B(59), _B(60), _B(61), _B(62), _B(63),
    _B(64),
};
#undef _B

static int get_bkt_num(size_t size)
{
    if (size <= (1 << (MALLOC_ALIGN_LOG + 5))) /* <= 256 */
    {
        return (size >> MALLOC_ALIGN_LOG) - 1; /* 0 - 31 */
    }
    if (size <= (1 << (MALLOC_ALIGN_LOG + 5 + 5))) /* <= 8192 */
    {
        return (1 << 5) + (size >> (MALLOC_ALIGN_LOG + 5)) - 1; /* 32 - 63 */
    }
    return (1 << 5) + (1 << 5); /* 64 */
}

static int bktlist_is_empty(bucket_t *list)
{
    return list == list->next;
}

static bucket_t *bktlist_peek_head(bucket_t *list)
{
    if (bktlist_is_empty(list))
    {
        return NULL;
    }
    return list->next;
}

static void bktlist_rem_item(bucket_t *item)
{
    item->next->prev = item->prev;
    item->prev->next = item->next;
}

static int bktlist_is_tail(bucket_t *list, bucket_t *item)
{
    return item->next == list;
}

static bucket_t *bktlist_peek_next(bucket_t *list, bucket_t *item)
{
    if (bktlist_is_tail(list, item))
    {
        return NULL;
    }
    return item->next;
}

static void bktlist_add_head(bucket_t *list, bucket_t *item)
{
    /* Add item between list and list->next */
    bucket_t *first_item = list->next;
    first_item->prev = item;
    item->next = first_item;
    item->prev = list;
    list->next = item;
}

malblk_t *freelist_get(size_t size)
{
    bucket_t *pbkt;
    bucket_t *item;
    int bkt_num;
    int b;

    bkt_num = get_bkt_num(size);
    malloc_debug("bkt_num %d\n", bkt_num);

    for (b=bkt_num; b<MAX_BUCKETS; ++b)
    {
        pbkt = &Bucket[b];
        item = bktlist_peek_head(pbkt);
        while (item)
        {
            size_t  item_size;
            item_size = blk_size_get((BUCKET_TO_MALLOC_BLOCK(item)));
            if (item_size > size)
            {
                bktlist_rem_item(item);
                malloc_debug("using freelist bkt %d\n", b);
                return BUCKET_TO_MALLOC_BLOCK(item);
            }
            item = bktlist_peek_next(pbkt, item);
        }
    }

    malloc_debug("free list empty\n");
    return NULL;
}

void freelist_put(malblk_t *blk)
{
    bucket_t *pbkt;
    size_t size;
    int bkt_num;

    size = blk_size_get(blk);
    bkt_num = get_bkt_num(size);

    malloc_debug("adding %d bytes to freelist bkt %d\n", size, bkt_num);

    pbkt = &Bucket[bkt_num];

    bktlist_add_head(pbkt, MALBLK_TO_BUCKET(blk));
}

void freelist_remove(malblk_t *blk)
{
    bucket_t *bkt;
    bkt = MALBLK_TO_BUCKET(blk);
    bktlist_rem_item(bkt);
}

void *_Anvil_malloc(size_t size)
{
    malblk_t *rem_blk;
    malblk_t *p;
    void *pmem;

    malloc_debug("---------------------------------\n", size);
    malloc_debug("malloc %d bytes\n", size);

    /* The function will return 0 if an error occurs */
    if ((size = malblk_round_up(size)) == 0)
    {
        return NULL;
    }
    malloc_debug("size %d bytes\n", size);

    /* Search for some memory in the free list */
    if ((p = freelist_get(size)) == NULL)
    {
        /* Free list is empty, get some system memory */
        if ((p = _Sysmem_get(size)) == NULL)
        {
            /* That's it, no more memory */
            return NULL;
        }
    }

    /* Check whether it's too big and split it if it is */
    if ((rem_blk = malblk_split(p, size)) != NULL)
    {
        /* Free up the bit we don't need */
        freelist_put(rem_blk);
    }
    blk_used_set(p);
    pmem = MALBLK_TO_PTR(p);

#if defined (_MALLOC_DEBUG)
    malloc_debug("********* Mallocing %08lx\n", pmem);
    _SysmemCheck();
    malloc_debug("********* Mallocing %08lx\n", pmem);
#endif

    return pmem;
}

void _Anvil_free(void *ptr)
{
    malblk_t *blk;

    malloc_debug("---------------------------------\n");
    malloc_debug("free %08lx\n", ptr);

    /* If it's a NULL pointer don't do anything */
    if (ptr == NULL)
    {
        return;
    }

    blk = PTR_TO_MALLOC_BLOCK(ptr);

    /* Are the adjacent blocks free ? */
    blk = malblk_try_join_prev(blk);
    blk = malblk_try_join_next(blk);

    /* Add the block to the bucket list for its size */
    blk_used_clr(blk);

    freelist_put(blk);

#if defined (_MALLOC_DEBUG)
    malloc_debug("********* Freeing %08lx\n", ptr);
    _SysmemCheck();
    malloc_debug("********* Freeing %08lx\n", ptr);
#endif
}

void *_Anvil_realloc(void *__ptr, __SIZE_TYPE__ __size)
{
    return NULL;
}
