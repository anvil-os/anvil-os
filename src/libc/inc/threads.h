
#ifndef _THREADS_H_INCLUDED
#define _THREADS_H_INCLUDED

#ifndef _SYS_ANVIL_INTERNAL_H_INCLUDED
#include <sys/anvil_internal.h>
#endif

#define thread_local _Thread_local
#define ONCE_FLAG_INIT
#define TSS_DTOR_ITERATIONS

typedef int cnd_t;
typedef int thrd_t;
typedef int tss_t;
typedef int mtx_t;
typedef void (*tss_dtor_t)(void *);
typedef int (*thrd_start_t)(void *);
typedef int once_flag;

enum __mtx_type
{
    mtx_plain,
    mtx_recursive,
    mtx_timed,
};

enum __thrd_result
{   thrd_timedout,
    thrd_success,
    thrd_busy,
    thrd_error,
    thrd_nomem,
};

void call_once(once_flag *__flag, void (*__func)(void));

int cnd_broadcast(cnd_t *__cond);
void cnd_destroy(cnd_t *__cond);
int cnd_init(cnd_t *__cond);
int cnd_signal(cnd_t *__cond);
//int cnd_timedwait(cnd_t *restrict __cond, mtx_t *restrict __mtx, const struct timespec *restrict __ts);
int cnd_wait(cnd_t *__cond, mtx_t *__mtx);

void mtx_destroy(mtx_t *__mtx);
int mtx_init(mtx_t *__mtx, int __type);
int mtx_lock(mtx_t *__mtx);
//int mtx_timedlock(mtx_t *restrict __mtx, const struct timespec *restrict __ts);
int mtx_trylock(mtx_t *__mtx);
int mtx_unlock(mtx_t *__mtx);

int thrd_create(thrd_t *__thr, thrd_start_t __func, void *__arg);
thrd_t thrd_current(void);
int thrd_detach(thrd_t __thr);
int thrd_equal(thrd_t __thr0, thrd_t __thr1);
_Noreturn void thrd_exit(int __res);
int thrd_join(thrd_t __thr, int *__res);
//int thrd_sleep(const struct timespec *__duration, struct timespec *__remaining);
void thrd_yield(void);

int tss_create(tss_t *__key, tss_dtor_t __dtor);
void tss_delete(tss_t __key);
void *tss_get(tss_t __key);
int tss_set(tss_t __key, void *__val);

#endif /* _THREADS_H_INCLUDED */
