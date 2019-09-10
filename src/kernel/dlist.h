
#ifndef DLIST_H_INCLUDED
#define DLIST_H_INCLUDED

#include <stdlib.h>

struct dlist_node
{
    struct dlist_node *next;
    struct dlist_node *prev;
};

typedef struct dlist_node dlist_t;
typedef struct dlist_node dlist_item_t;
typedef struct dlist_node dlist_chain_t;

static __inline__ void dlist_init(dlist_t *list)
{
    list->next = list->prev = list;
}

static __inline__ void dlist_add_head(dlist_t *list, dlist_item_t *item)
{
    /* Add item between list and the first item (list->next) */
    item->next = list->next;
    item->prev = list;
    list->next = item;
    item->next->prev = item;
}

static __inline__ void dlist_add_tail(dlist_t *list, dlist_item_t *item)
{
    /* Add item after the last item (list->prev) */
    item->next = list;
    item->prev = list->prev;
    list->prev->next = item;
    list->prev = item;
}

static __inline__ int dlist_is_empty(dlist_t *list)
{
    return list == list->next;
}

static __inline__ dlist_item_t *dlist_peek_head(dlist_t *list)
{
    if (dlist_is_empty(list))
    {
        return NULL;
    }
    return list->next;
}

static __inline__ void dlist_rem_item(dlist_item_t *item)
{
    item->next->prev = item->prev;
    item->prev->next = item->next;
}

static __inline__ dlist_item_t *dlist_rem_head(dlist_t *list)
{
    dlist_item_t *item;
    if ((item = dlist_peek_head(list)) != NULL)
    {
        dlist_rem_item(item);
    }
    return item;
}

#endif /* DLIST_H_INCLUDED */
