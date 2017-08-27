#include <dlfcn.h>
#include <stddef.h>
#include <stdio.h>
#include <time.h>
#include <sys/timeb.h>
#include <sys/time.h>
#include "interpose.h"
#include "time_hook.h"

extern "C" {
    static int offset = 0;
    void set_time_offset(int val) {
        offset = val;
    }
}

INTERPOSE(time)(time_t *time_tptr) {
    fprintf(stderr, "interpose time: %d\n", offset);
    fflush(stderr);
    auto result = real::time(time_tptr);
    if (time_tptr != NULL) {
        *time_tptr = *time_tptr + offset;
     fprintf(stderr, "interpose time modified %ld\n", *time_tptr);
        fflush(stderr);
    }
    result += offset;
     fprintf(stderr, "interpose time modified %ld\n", result);
        fflush(stderr);
    return result;
}
INTERPOSE(ftime)(struct timeb *tb) {
    fprintf(stderr, "interpose ftime: %d\n", offset);
    fflush(stderr);
    auto result = real::ftime(tb);
    if (tb != NULL) {
        tb->time = tb->time + offset;
     fprintf(stderr, "interpose ftime modified %ld\n", tb->time);
        fflush(stderr);
    }
    return result;
}
#ifdef __APPLE__
INTERPOSE(gettimeofday)(struct timeval *tv, void *tz) {
#else
INTERPOSE(gettimeofday)(struct timeval *tv, struct timezone *tz) {
#endif
    fprintf(stderr, "interpose gettimeofday: %d\n", offset);
    fflush(stderr);
    auto result = real::gettimeofday(tv, tz);
    if (tv != NULL) {
        tv->tv_sec = tv->tv_sec + offset;
     fprintf(stderr, "interpose gettimeofday modified %ld\n", tv->tv_sec);
        fflush(stderr);
    }
    return result;
}
#ifdef __APPLE__
#include <mach/clock.h>
#include <mach/mach.h>
INTERPOSE(clock_get_time)(clock_serv_t clock_serv, mach_timespec_t *cur_timeclockid_t){
    fprintf(stderr, "interpose clock_get_time: %d\n", offset);
    fflush(stderr);
    auto result = real::clock_get_time(clock_serv, cur_timeclockid_t);
    if (cur_timeclockid_t != NULL) {
        cur_timeclockid_t->tv_sec = cur_timeclockid_t->tv_sec + offset;
        fprintf(stderr, "interpose clock_get_time modified %d\n", cur_timeclockid_t->tv_sec);
        fflush(stderr);
    }
    return result;
}
#else
INTERPOSE(clock_gettime)(clockid_t clk_id, struct timespec *tp) {
    fprintf(stderr, "interpose clock_gettime: %d\n", offset);
    fflush(stderr);
    auto result = real::clock_gettime(clk_id, tp);
    if (tp != NULL) {
        tp->tv_sec = tp->tv_sec + offset;
        fprintf(stderr, "interpose clock_gettime modified %ld\n", tp->tv_sec);
        fflush(stderr);
    }
    return result;
}
#endif