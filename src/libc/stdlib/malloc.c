
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

//#define MALLOC_DEBUG
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

// These are stored in the low bits of size_this - room for 3 (or 4)
#define BLK_PREV_IN_USE       (1<<0)
#define BLK_BITMASK         (BLK_PREV_IN_USE)

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

#define MAX_BUCKETS (65)

struct bucket
{
    struct bucket *prev;
    struct bucket *next;
};
typedef struct bucket bucket_t;

// The malloc context
struct mal_context
{
    bucket_t *bucket;
    malblk_t *first_blk;
    size_t free;
    size_t used_blocks;
};

struct mal_context mal_ctx;

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
        pbkt = &mal_ctx.bucket[b];
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

    pbkt = &mal_ctx.bucket[bkt_num];

    bktlist_add_head(pbkt, blk_to_ptr(blk));
}

static void freelist_remove(malblk_t *blk)
{
    bucket_t *bkt;
    bkt = blk_to_ptr(blk);
    bktlist_rem_item(bkt);
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

    if (requested_size == 0)
    {
        requested_size = 1;
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

extern char __ebss__;
extern char __eram__;

int heap_check()
{
    malblk_t *item;
    int item_is_free = -1;
    size_t total_free = 0;
    size_t total_used = 0;

    // Todo: Add checks for

    /* Move to the first item */
    item = (malblk_t *)mal_ctx.first_blk;

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

        if (flags & 0x1)
        {
            total_used += size;
        }
        else
        {
            total_free += size;
        }

        if (item_is_free != -1 && !flags && item_is_free)
        {
            // This item and the previous are both free
            malloc_debug("CONSECUTIVE FREE ITEMS\n");
            return -1;
        }

        // Check that the 2nd size is correct if the item is free
        if (!flags)
        {
            if ((item->size_this & ~0x7) != next_item->size_prev)
            {
                malloc_debug("Bad 2nd size %08x %08x!!\n", item->size_this, next_item->size_prev);
                return -1;
            }
            item_is_free = 1;
        }
        else
        {
            item_is_free = 0;
        }

        // Print info for item
        malloc_debug(" %08lx %4d(%4x)   %s\n", item, size, size, flags?"*":"");

        item = next_item;
    }

    malloc_debug("Free %u %u\n", total_free, mal_ctx.free);
    malloc_debug("Used %u\n", total_used);
    if (total_free != mal_ctx.free)
    {
        malloc_debug("WRONG FREE AMOUNT\n");
        return -1;
    }
    malloc_debug("=========================\n");

    return 0;
}

static int initialised = 0;

void initialise()
{
    char *heap_start;
    size_t heap_len;
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

    mal_ctx.bucket = (bucket_t *)heap_start;
    heap_start = (char *)(mal_ctx.bucket + MAX_BUCKETS);
    malloc_debug("heap_start  = %08x\n", heap_start);
    heap_len -= MAX_BUCKETS * sizeof(bucket_t);

    for (i=0; i<MAX_BUCKETS; ++i)
    {
        mal_ctx.bucket[i].prev = mal_ctx.bucket[i].next = &mal_ctx.bucket[i];
    }

    mal_ctx.first_blk = (malblk_t *)heap_start;
    heap_len -= 1024;

    memset(mal_ctx.first_blk, 0, heap_len);

    blk_size_and_flags_set(mal_ctx.first_blk, heap_len | BLK_PREV_IN_USE);

    // XXX: Todo: Souldn't heap_len be reduced enough to fit the header of the next block??
    malblk_t *next = (malblk_t *)(((char *)mal_ctx.first_blk) + heap_len);
    next->size_prev = heap_len;
    next->size_this = 0;

    /* Make this into a free block */
    malloc_debug("getting system memory %08lx %d\n", mal_ctx.first_blk, mal_ctx.first_blk->size_this);
    malloc_debug("getting system memory %08lx %d\n", next, next->size_this);

    malloc_debug("%08x %08x\n", mal_ctx.first_blk, ((char *)mal_ctx.first_blk) + mal_ctx.first_blk->size_this);

    freelist_put(mal_ctx.first_blk);
    mal_ctx.free = heap_len;

#if defined (MALLOC_DEBUG)
    heap_check();
#endif
}

void *malloc(size_t requested_size)
{
    malblk_t *new_blk;
    size_t req_blk_size;
    size_t rem_size;

    malloc_debug("---------------------------------\n", requested_size);
    malloc_debug("malloc %d bytes\n", requested_size);

    if (!initialised)
    {
        initialise();
        initialised = 1;
    }

    // The function will return 0 if an error occurs
    if ((req_blk_size = malblk_size(requested_size)) == 0)
    {
        return NULL;
    }
    malloc_debug("size %d bytes\n", req_blk_size);

    // Search for some memory in the free list
    if ((new_blk = freelist_get(req_blk_size)) == NULL)
    {
        // That's it, no more memory
        return NULL;
    }

    malloc_debug("Found in freelist\n");

    // Check whether it's too big and split it if it is
    rem_size = blk_size_get(new_blk) - req_blk_size;
    if (rem_size >= s_min_blk_size)
    {
        // Split the block and return the split off piece to the free list
        malblk_t *rem_blk = (malblk_t *)((char *)new_blk + req_blk_size);
        blk_size_set(rem_blk, rem_size);
        blk_size_set(new_blk, req_blk_size);
        // If we were able to split some off, free it now
        blk_used_clr(rem_blk);
        freelist_put(rem_blk);
    }

    // Now we have our new block and its the right size
    blk_used_set(new_blk);

    mal_ctx.free -= blk_size_get(new_blk);

#if defined (MALLOC_DEBUG)
    heap_check();
#endif

    return blk_to_ptr(new_blk);
}

void free(void *ptr)
{
    malblk_t *blk;
    malblk_t *adj;

    malloc_debug("---------------------------------\n");
    malloc_debug("free %08lx\n", ptr);

    // If it's a NULL pointer don't do anything
    if (ptr == NULL)
    {
        return;
    }

    blk = ptr_to_blk(ptr);

    mal_ctx.free += blk_size_get(blk);

    // Join with the prev block but only if the prev block is not busy
    adj = blk_prev_if_free(blk);
    if (adj)
    {
        // Take the prev from the free list
        freelist_remove(adj);
        // Now join them
        blk_size_set(adj, blk_size_get(adj) + blk_size_get(blk));
        blk = adj;
    }

    // Join with the next block
    adj = blk_next(blk);
    if (adj && !blk_used_get(adj))
    {
        // Take the next from the free list
        freelist_remove(adj);
        // Now join them
        blk_size_set(blk, blk_size_get(blk) + blk_size_get(adj));
    }

    // Add the block to the bucket list for its size
    blk_used_clr(blk);
    freelist_put(blk);

#if defined (MALLOC_DEBUG)
    heap_check();
#endif
}

void *realloc(void *old_ptr, size_t requested_size)
{
    void *new_ptr = NULL;
    size_t orig_size;
    malblk_t *orig_blk;
    size_t req_blk_size;
    malblk_t *next_blk;

    malloc_debug("---------------------------------\n", requested_size);
    malloc_debug("realloc %x to %d bytes\n", old_ptr, requested_size);

    if (!old_ptr)
    {
        // Just do a malloc
        return malloc(requested_size);
    }

    if ((req_blk_size = malblk_size(requested_size)) == 0)
    {
        return NULL;
    }

    malloc_debug("size %d bytes\n", req_blk_size);

    orig_blk = ptr_to_blk(old_ptr);
    orig_size = blk_size_get(orig_blk);
    next_blk = blk_next(orig_blk);

    // Join the original block with the next block if the next block is free and either
    //  a) the original block is too big and we are going to free some and
    //     coalesce it with the next block later anyway
    //  b) the original block is too small and joining will make it big enough
    if (next_blk && !blk_used_get(next_blk) &&
         ((orig_size > req_blk_size) || ((req_blk_size > orig_size) && (blk_size_get(next_blk) >= req_blk_size - orig_size)))
       )
    {
		// Take the next from the free list
		freelist_remove(next_blk);
	    mal_ctx.free -= blk_size_get(next_blk);
		// Now join them
		blk_size_set(orig_blk, orig_size + blk_size_get(next_blk));
        blk_used_set(orig_blk);
	}

    // Is it big enough now?
    if (blk_size_get(orig_blk) >= req_blk_size)
    {
        // The only ways that orig_blk is now bigger than the request are
        // a) it was already bigger in which case we would have coalesced above
        // b) it became bigger because we coalesced above
        // In either case the next block if there is one will never be free
    	// Todo: Assert that here
    	size_t rem_size = blk_size_get(orig_blk) - req_blk_size;
        if (rem_size >= s_min_blk_size)
        {
        	// Split the block and return the split off piece to the free list
        	malblk_t *rem_blk = (malblk_t *)((char *)orig_blk + req_blk_size);
        	blk_size_set(rem_blk, rem_size);
        	blk_size_set(orig_blk, req_blk_size);
        	// If we were able to split some off, free it now
        	blk_used_clr(rem_blk);
            mal_ctx.free += blk_size_get(rem_blk);
        	freelist_put(rem_blk);
            blk_used_set(orig_blk);
        }
#if defined (MALLOC_DEBUG)
        heap_check();
#endif
        return old_ptr;
    }

    // If we get here it all failed so we need to do it the slow way
    new_ptr = malloc(requested_size);
    if (!new_ptr)
    {
        return NULL;
    }

    memcpy(new_ptr, old_ptr, orig_size);
    free(old_ptr);

#if defined (MALLOC_DEBUG)
    heap_check();
#endif

    return new_ptr;
}
