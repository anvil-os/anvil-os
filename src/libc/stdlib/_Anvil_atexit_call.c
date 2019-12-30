
#include "_Anvil_atexit.h"

void _Anvil_atexit_call(struct _Anvil_atexit_record **list)
{
    while (*list)
    {
        // We need to be a little careful because it's possible for atexit
        // functions to add more atexit functions. These will be added to the
        // head of the atexit list. As we go through the list we need to
        // remove items before calling the func() so this works in the
        // expected way.
        // If we do it this way we conform to the odd wording in the C17 spec.
        // "First, all functions registered by the atexit function are called,
        // in the reverse order of their registration except that a function
        // is called after any previously registered functions that had
        // already been called at the time it was registered.
        struct _Anvil_atexit_record *p = *list;
        *list = p->pnext;
        // During this func() other funcs may be added to the list
        p->func();
        free(p);
    }
}
