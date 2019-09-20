
#undef TARGET_OS_CPP_BUILTINS
#define TARGET_OS_CPP_BUILTINS()      \
do {                                  \
    builtin_define_std ("anvil");      \
    builtin_define_std ("unix");      \
    builtin_assert ("system=anvil");   \
    builtin_assert ("system=unix");   \
    builtin_define ("__ELF__");     \
    builtin_define ("__ANVIL__");     \
    TARGET_BPABI_CPP_BUILTINS();        \
} while(0);

/* Don't assume anything about the header files.  */
/*#define NO_IMPLICIT_EXTERN_C*/
