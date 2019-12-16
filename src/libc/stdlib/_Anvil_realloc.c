
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define MALLOC_DEBUG
#if !defined (MALLOC_DEBUG)
#define malloc_debug(...)
#else
#define MALLOC_DEBUG
#include <stdio.h>
#define malloc_debug(...) printf(__VA_ARGS__)
#endif

#define ALIGNMENT (2 * sizeof(size_t))

#if (__SIZEOF_SIZE_T__ == 4)
#define MALLOC_ALIGN_LOG    (3)
#else
#define MALLOC_ALIGN_LOG    (4)
#endif

/* These are stored in the low bits of size_this - room for 3 (or 4) */
#define BLK_PREV_IN_USE       (1<<0)
#define BLK_BITMASK         (BLK_PREV_IN_USE)

//#define MALBLK_TO_PTR(__p) ((char *)__p + 2 * sizeof(size_t))
//#define PTR_TO_MALLOC_BLOCK(__p) ((malblk_t *)((char *)__p - 2 * sizeof(size_t)))


static const size_t s_min_blk_size = 2 * sizeof(size_t) + 2 * sizeof(void *);

struct malblk
{
    // This is the size of the previous block. It is only valid if the
    // previous block is free. If the previous block is in use user data
    // will creep over these bytes
    size_t size_prev;

    // This is the entire size of the this malblk. This includes the 2
    // size_t and the user data
    size_t size_this;

    // The user area starts here. When the block is free these pointers are
    // used to hold the data in a free list
    struct malblk *prev;
    struct malblk *next;
};
typedef struct malblk malblk_t;

////////////////////////////////////////////////
/// Access functions for malblk_t
////////////////////////////////////////////////

static void blk_size_and_flags_set(malblk_t *pblk, size_t size_and_flags)
{
    pblk->size_this = size_and_flags;
}

static size_t blk_size_get(malblk_t *pblk)
{
    return pblk->size_this & ~BLK_BITMASK;
}

static void blk_size_set(malblk_t *pblk, size_t size)
{
    pblk->size_this = (pblk->size_this & BLK_BITMASK) | size;
}

static int blk_used_get(malblk_t *pblk)
{
    malblk_t *next = (malblk_t *)(((char *)pblk) + blk_size_get(pblk));
    return (next->size_this & BLK_PREV_IN_USE) == BLK_PREV_IN_USE;
}

static void blk_used_set(malblk_t *pblk)
{
    malblk_t *next = (malblk_t *)(((char *)pblk) + blk_size_get(pblk));
    next->size_this |= BLK_PREV_IN_USE;
}

static void blk_used_clr(malblk_t *pblk)
{
    malblk_t *next = (malblk_t *)(((char *)pblk) + blk_size_get(pblk));
    next->size_prev = pblk->size_this & ~BLK_PREV_IN_USE;
    next->size_this &= ~BLK_PREV_IN_USE;
}

static void *blk_to_ptr(malblk_t *blk)
{
    return ((char *)blk + 2 * sizeof(size_t));
}

static malblk_t *ptr_to_blk(void *p)
{
    return ((malblk_t *)((char *)p - 2 * sizeof(size_t)));
}

////////////////////////////////////////////////
/// Functions for navigating blocks
////////////////////////////////////////////////

static malblk_t *blk_prev_if_free(malblk_t *pblk)
{
    // This function returns the previous block BUT ONLY IF THE BLOCK IS FREE
    if (pblk->size_this & BLK_PREV_IN_USE)
    {
        return NULL;
    }
    return (malblk_t *)((char *)pblk - pblk->size_prev);
}

static malblk_t *blk_next(malblk_t *pblk)
{
    // We just need to look at the size_this of the next block. If it is
    // zero we are at the end
    malblk_t *next = (malblk_t *)(((char *)pblk) + blk_size_get(pblk));
    if (blk_size_get(next) == 0)
    {
        return NULL;
    }
    return next;
}

////////////////////////////////////////////////
/// Functions for managing the free lists
////////////////////////////////////////////////

#define MAX_BUCKETS (65)

struct bucket
{
    struct bucket *prev;
    struct bucket *next;
};
typedef struct bucket bucket_t;

bucket_t *Bucket;

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

static bucket_t *bktlist_get_head(bucket_t *list)
{
    if (list == list->next)
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

static bucket_t *bktlist_get_next(bucket_t *list, bucket_t *item)
{
    if (item->next == list)
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

static malblk_t *freelist_get(size_t size)
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
        item = bktlist_get_head(pbkt);
        while (item)
        {
            size_t  item_size;
            item_size = blk_size_get((ptr_to_blk(item)));
            if (item_size > size)
            {
                bktlist_rem_item(item);
                malloc_debug("using freelist bkt %d\n", b);
                return ptr_to_blk(item);
            }
            item = bktlist_get_next(pbkt, item);
        }
    }

    malloc_debug("free list empty\n");
    return NULL;
}

static void freelist_put(malblk_t *blk)
{
    bucket_t *pbkt;
    size_t size;
    int bkt_num;

    size = blk_size_get(blk);
    bkt_num = get_bkt_num(size);

    malloc_debug("adding %d bytes to freelist bkt %d\n", size, bkt_num);

    pbkt = &Bucket[bkt_num];

    bktlist_add_head(pbkt, blk_to_ptr(blk));
}

static void freelist_remove(malblk_t *blk)
{
    bucket_t *bkt;
    bkt = blk_to_ptr(blk);
    bktlist_rem_item(bkt);
}

////////////////////////////////////////////////
/// Functions for splitting and joining blocks
////////////////////////////////////////////////

static malblk_t *malblk_split(malblk_t *blk, size_t new_size)
{
    // Split a block into 2. This function is used to split both free and
    // used blocks
    malblk_t *rem_blk;
    size_t rem_size;
    size_t curr_size;

    /* Size of the block we are splitting */
    curr_size = blk_size_get(blk);

    malloc_debug("splitting %d bytes, split_size=%d\n", curr_size, new_size);

    /* Check that the split is possible. Both parts must end up > (2 size_t + 2 void *) */
    if (new_size < s_min_blk_size || new_size + s_min_blk_size > curr_size)
    {
        malloc_debug("Split not possible %u %u %u\n", curr_size, s_min_blk_size, new_size);
        /* It's only just big enough so return NULL to indicate no split */
        return NULL;
    }

    /* We'll split off from the front of the blk. Firstly we figure out where
     * the remaining block will start
     */
    rem_blk = (malblk_t *)((char *)blk + new_size);
    rem_size = curr_size - new_size;
    malloc_debug("rem_size=%d\n", rem_size);
    blk_size_set(rem_blk, rem_size);
    blk_size_set(blk, new_size);

    return rem_blk;
}

static malblk_t *malblk_try_join_prev(malblk_t *blk)
{
    /* Join blk with the prev block but only if the prev block is not busy */
    malblk_t *prev;
    size_t total;

    malloc_debug("%08x malblk_try_join_prev - ", blk);

    prev = blk_prev_if_free(blk);
    if (prev == NULL)
    {
        malloc_debug("prev in use\n");
        return blk;
    }

    /* Take the prev from the free list */
    freelist_remove(prev);

    /* Now join them */
    total = blk_size_get(prev) + blk_size_get(blk);
    malloc_debug("done, join %d %d ", blk_size_get(prev), blk_size_get(blk));
    blk_size_set(prev, total);
    malloc_debug("got %d\n", total);

    return prev;
}

static malblk_t *malblk_try_join_next(malblk_t *blk, size_t extra_req)
{
    /* Join blk with the next block but only if the next block is not busy */
    malblk_t *next;
    size_t total;

    malloc_debug("malblk_try_join_next - ");

    next = blk_next(blk);

    if (!next || (blk_size_get(next) < extra_req) || blk_used_get(next))
    {
        malloc_debug("fail\n");
        return blk;
    }

    malloc_debug("used=%d\n", blk_used_get(next));

    /* Take the next from the free list */
    freelist_remove(next);

    /* Now join them */
    total = blk_size_get(blk) + blk_size_get(next);
    malloc_debug("done, join %d %d ", blk_size_get(blk), blk_size_get(next));
    blk_size_set(blk, total);
    malloc_debug("got %d\n", total);

    return blk;
}

////////////////////////////////////////////////
/// Convert requested bytes to block size
////////////////////////////////////////////////

static size_t malblk_size(size_t requested_size)
{
    size_t block_size;

    // Check that we won't overflow
    if (requested_size > SIZE_MAX - 4 * sizeof(size_t))
    {
        malloc_debug("malloc: requested_size %d too big\n", requested_size);
        return 0;
    }

    //
    // We need to add room for the block header. 2 * size_t to store the size
    // of the previous block and this one.
    //
    // The first few bytes of a block contain the size of the previous block.
    // However, by making a rule that size_prev is only valid if the previous
    // block is free we get to use the first size_t bytes of the next block.
    //
    // So a block in use looks like this
    //
    //   size_t size_prev
    //   size_t size_this
    //   data [  ]  ------------------------|
    //   ..                                 |--- total size >= requested_size
    //   sizeof(size_t) from next block ----|
    //

    // As above, add 2 size_t but subtract 1
    block_size = requested_size + sizeof(size_t);

    // To ensure that each block starts on a 2 x size_t boundary we must
    // round up the block size to that alignment
    block_size += (ALIGNMENT - 1);
    block_size &= ~(ALIGNMENT - 1);

    // To ensure that blocks have enough room for next and prev pointers for
    // the free lists, our minimum block size is 2 size_t + 2 void*
    if (block_size < (2 * sizeof(size_t) + 2 * sizeof(void *)))
    {
        block_size = 2 * sizeof(size_t) + 2 * sizeof(void *);
    }

    //  For 32bit size_t
    //    16 bytes - can hold 1 to 12 bytes
    //    24 bytes - can hold 13 to 20 bytes
    //    32 bytes - can hold 21 to 28 bytes
    //    40 bytes - can hold 29 to 36 bytes
    //    48 bytes - can hold 37 to 44 bytes
    //    etc.
    //
    //  For 64 bit libs the values double.
    //
    //    32 bytes - can hold 1 to 24 bytes
    //    48 bytes - can hold 25 to 40 bytes
    //    64 bytes - can hold 41 to 56 bytes
    //    etc.
    //

    return block_size;
}


#if defined (MALLOC_DEBUG)
static int heap_check(void);
#endif

static malblk_t *_Core;

extern char __ebss__;
extern char __eram__;



#if defined (MALLOC_DEBUG)
int heap_check()
{
    malblk_t *item;

    // Todo: Add checks for
    // 1. that there are never 2 free blocks in succession

    /* Move to the first item */
    item = (malblk_t *)_Core;

    malloc_debug("=========================\n");

    while (1)
    {
        size_t  size;
        int     flags;

        // Record the size of this item
        size = item->size_this & ~0x7;

        // Move to the next item so we can see the in-use flag
        malblk_t *next_item = (malblk_t *)((char *)item + size);
        flags = next_item->size_this & 0x7;

        if (blk_size_get(item) == 0)
        {
            malloc_debug(" %08lx %4d(%4x)   %s\n", item, size, size, flags?"*":"");
            break;
        }

        // Check that the 2nd size is correct if the item is free
        if (!flags)
        {
            if ((item->size_this & ~0x7) != next_item->size_prev)
            {
                malloc_debug("Bad 2nd size %08x %08x!!\n", item->size_this, next_item->size_prev);
            }
        }

        // Print info for item
        malloc_debug(" %08lx %4d(%4x)   %s\n", item, size, size, flags?"*":"");

        item = next_item;
    }

    malloc_debug("=========================\n");

    return 0;
}
#endif

static int initialised = 0;

void initialise()
{
    char *heap_start;
    size_t heap_len;
    size_t buckets_len;
    int i;

    // In this version of malloc we take all available memory and use it to create the heap
    malloc_debug("Malloc initialise\n");
    malloc_debug("__ebss__ = %08x\n", &__ebss__);
    malloc_debug("__erom__ = %08x\n", &__eram__);
    malloc_debug("s_min_blk_size = %d\n", s_min_blk_size);

//    int bkt_num = 0;
//    for (i=0; i<8192; ++i)
//    {
//        int new_bkt = get_bkt_num(malblk_size(i));
//        if (new_bkt != bkt_num)
//        {
//            malloc_debug("% 5d: % 5d % 5d\n", i, malblk_size(i), get_bkt_num(malblk_size(i)));
//            bkt_num = new_bkt;
//        }
//    }

    // We need to align to 2 x sizeof(size_t)
    heap_start = &__ebss__;
    while ((uintptr_t)heap_start & (ALIGNMENT - 1))
    {
        ++heap_start;
    }
    malloc_debug("heap_start  = %08x\n", heap_start);

    // How much heap have we got?
    heap_len = &__eram__ - heap_start;
    malloc_debug("heap_len    = %08x\n", heap_len);

    // Carve off some for our bucket list
    buckets_len = MAX_BUCKETS * sizeof(bucket_t);
    malloc_debug("buckets_len = %08x\n", buckets_len);

    Bucket = (bucket_t *)heap_start;
    heap_start = (char *)(Bucket + MAX_BUCKETS);
    malloc_debug("heap_start  = %08x\n", heap_start);
    heap_len -= MAX_BUCKETS * sizeof(bucket_t);

    for (i=0; i<MAX_BUCKETS; ++i)
    {
        Bucket[i].prev = Bucket[i].next = &Bucket[i];
    }

    _Core = (malblk_t *)heap_start;
    heap_len -= 1024;

    memset(_Core, 0, heap_len);

    blk_size_and_flags_set(_Core, heap_len | BLK_PREV_IN_USE);

    // XXX: Todo: Souldn't heap_len be reduced enough to fit the header of the next block??
    malblk_t *next = (malblk_t *)(((char *)_Core) + heap_len);
    next->size_prev = heap_len;
    next->size_this = 0;

    /* Make this into a free block */
    malloc_debug("getting system memory %08lx %d\n", _Core, _Core->size_this);
    malloc_debug("getting system memory %08lx %d\n", next, next->size_this);

    malloc_debug("%08x %08x\n", _Core, ((char *)_Core) + _Core->size_this);

    freelist_put(_Core);

#if defined (MALLOC_DEBUG)
    heap_check();
#endif

//    return _Core;
}

void *_Anvil_malloc(size_t size)
{
    malblk_t *rem_blk;
    malblk_t *new_blk;
    size_t new_size;

    malloc_debug("---------------------------------\n", size);
    malloc_debug("malloc %d bytes\n", size);

    if (!initialised)
    {
        initialise();
        initialised = 1;
    }

    /* The function will return 0 if an error occurs */
    if ((new_size = malblk_size(size)) == 0)
    {
        return NULL;
    }
    malloc_debug("size %d bytes\n", new_size);

    // Search for some memory in the free list
    if ((new_blk = freelist_get(new_size)) == NULL)
    {
        // That's it, no more memory
        return NULL;
    }

    malloc_debug("Found in freelist\n");

    // Check whether it's too big and split it if it is
    if ((rem_blk = malblk_split(new_blk, new_size)) != NULL)
    {
        // Free up the bit we don't need
        freelist_put(rem_blk);
        blk_used_clr(rem_blk);
        malloc_debug("Put to freelist\n");
    }

    // Now we have our new block and its the right size
    blk_used_set(new_blk);

#if defined (MALLOC_DEBUG)
    heap_check();
#endif

    return blk_to_ptr(new_blk);
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

    if (!initialised)
    {
        initialise();
        initialised = 1;
    }

    blk = ptr_to_blk(ptr);

    // Are the adjacent blocks free?
    blk = malblk_try_join_prev(blk);
    blk = malblk_try_join_next(blk, 0);

    // Add the block to the bucket list for its size
    blk_used_clr(blk);
    freelist_put(blk);

#if defined (MALLOC_DEBUG)
    heap_check();
#endif
}

void *_Anvil_realloc(void *old_ptr, size_t requested_size)
{
    void *new_ptr = NULL;
    size_t old_size;
    malblk_t *old_blk;
    malblk_t *next_blk;
    size_t new_size;

    malloc_debug("---------------------------------\n", requested_size);
    malloc_debug("realloc %x to %d bytes\n", old_ptr, requested_size);

    if (!initialised)
    {
        initialise();
        initialised = 1;
    }

    if (requested_size == 0)
    {
        // In the past it was assumed that if size were 0 then realloc would
        // simply free p and return NULL.
        // This is bad because a NULL return was supposed to indicate that the
        // ptr was not freed. We do nothing and indicate that by returning NULL
        return NULL;
    }

    if (!old_ptr)
    {
        // Just do a malloc
        return _Anvil_malloc(requested_size);
    }

    if ((new_size = malblk_size(requested_size)) == 0)
    {
        return NULL;
    }

    malloc_debug("size %d bytes\n", new_size);

    old_blk = ptr_to_blk(old_ptr);
    old_size = blk_size_get(old_blk);

    // realloc to a new size
    if (new_size <= old_size)
    {
        // Trim to the new size if it's worth it. The split will fail if we are only
        // trimming by a few bytes but that's okay
        malloc_debug("Trimming from %d to %d bytes\n", blk_size_get(old_blk), new_size);
        malblk_t *rem_blk;
        if ((rem_blk = malblk_split(old_blk, new_size)) != NULL)
        {
            // If we were able to split some off, free it now
            rem_blk = malblk_try_join_next(rem_blk, 0);
            blk_used_clr(rem_blk);
            freelist_put(rem_blk);
        }
        return old_ptr;
    }

    // If we are here new_size > old_size so try to extend the old block to the new size

    // Check the next block
    next_blk = blk_next(old_blk);

    if (next_blk && blk_size(next_blk) >= new_size - old_size)
    {
        old_blk = malblk_try_join_next(old_blk, new_size - old_size);
        old_size = blk_size_get(old_blk);

        // Trim if necessary
        if (blk_size_get(old_blk) >= new_size)
        {
            // Trim the block
            malloc_debug("Trimming from %d to %d bytes\n", blk_size_get(old_blk), new_size);
            malblk_t *rem_blk;
            if ((rem_blk = malblk_split(old_blk, new_size)) != NULL)
            {
                /* Free up the bit we don't need */
                rem_blk = malblk_try_join_next(rem_blk, 0);
                blk_used_clr(rem_blk);
                freelist_put(rem_blk);
            }
            return old_ptr;
        }
    }

    // If we get here it all failed so we need to do it the slow way
    new_ptr = _Anvil_malloc(new_size);
    if (!new_ptr)
    {
        return NULL;
    }

    memcpy(new_ptr, old_ptr, old_size);
    _Anvil_free(old_ptr);

#if defined (MALLOC_DEBUG)
    heap_check();
#endif

    return new_ptr;
}
