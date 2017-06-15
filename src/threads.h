// glibc doesn't have C11 threads.h yet. So write a crude replacement, for just
// what is needed, but exposing a C11 API to make the client code future-proof.
#pragma once
#include <pthread.h>

#define thread_local _Thread_local
typedef pthread_t thrd_t;
typedef int (*thrd_start_t)(void*);

enum {mtx_plain/*, mtx_recursive, mtx_timed*/};
enum {thrd_success/*, thrd_timedout, thrd_busy, thrd_error, thrd_nomem*/};

/* Return thrd_success if ok */
#ifdef _WIN64
// Win32 API Critical Section (workaround mingw bug)
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

typedef CRITICAL_SECTION mtx_t;

#define mtx_init(m, t) InitializeCriticalSection(m)
#define mtx_destroy(m) DeleteCriticalSection(m)
#define mtx_lock(m) EnterCriticalSection(m)
#define mtx_unlock(m) LeaveCriticalSection(m)
#else
// POSIX mutex
typedef pthread_mutex_t mtx_t;

#define mtx_init(m, t) pthread_mutex_init(m, NULL)
#define mtx_destroy(m) pthread_mutex_destroy(m)
#define mtx_lock(m) pthread_mutex_lock(m)
#define mtx_unlock(m) pthread_mutex_unlock(m)
#endif

#define thrd_create(thrd, func, args) pthread_create(thrd, 0, (void*(*)(void*))func, args)
#define thrd_join(thrd, dummy) pthread_join(thrd, NULL)
#define thrd_sleep nanosleep
