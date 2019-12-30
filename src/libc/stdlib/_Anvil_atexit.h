
#include <stdlib.h>

struct _Anvil_atexit_record
{
    void (*func)(void);
    struct _Anvil_atexit_record *pnext;
};

void _Anvil_atexit_call(struct _Anvil_atexit_record **list);
int _Anvil_atexit_reg(struct _Anvil_atexit_record **list, void (*func)(void));

extern struct _Anvil_atexit_record *_Anvil_atexit_list;
extern struct _Anvil_atexit_record *_Anvil_at_quick_exit_list;
