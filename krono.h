/* krono.h — “chrono-ish” timing utilities for C (Windows/macOS/Linux)
*
 * Features:
 *  - Monotonic time in nanoseconds: krono_now_ns()
 *  - Wall-clock (realtime) time in nanoseconds: krono_now_realtime_ns()  (best-effort)
 *  - Sleep for N nanoseconds: krono_sleep_ns()
 *  - Sleep until a monotonic deadline (ns): krono_sleep_until_ns()
 *
 * Notes:
 *  - Monotonic time is for intervals/profiling/frame pacing; it never goes backward.
 *  - Realtime can jump due to NTP/manual changes; use only for timestamps/logging.
 *  - Actual timer resolution depends on OS/hardware.
 */
//
// Created by devinrcohen on 2/12/26.
//

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* =========================
 *  Platform detection macros
 * ========================= */

#if defined(_WIN32) || defined(_WIN64)
  #define KRONO_OS_WINDOWS 1
#else
  #define KRONO_OS_WINDOWS 0
#endif

#if defined(__APPLE__) && defined(__MACH__)
  #define KRONO_OS_MACOS 1
#else
  #define KRONO_OS_MACOS 0
#endif

#if defined(__linux__)
  #define KRONO_OS_LINUX 1
#else
  #define KRONO_OS_LINUX 0
#endif

#if !KRONO_OS_WINDOWS && !KRONO_OS_MACOS && !KRONO_OS_LINUX
  #define KRONO_OS_UNKNOWN 1
#else
  #define KRONO_OS_UNKNOWN 0
#endif

/* =========================
 *  Includes
 * ========================= */

#include <stdint.h>

#if KRONO_OS_WINDOWS
  #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
  #endif
  #include <windows.h>
#else
  #include <time.h>
  #include <errno.h>
  #if KRONO_OS_MACOS
    #include <AvailabilityMacros.h>
    #include <TargetConditionals.h>
    #include <mach/mach_time.h>
  #endif
#endif

/* =========================
 *  Helpers
 * ========================= */

#ifndef KRONO_INLINE
  #if defined(_MSC_VER)
    #define KRONO_INLINE __forceinline
  #elif defined(__GNUC__) || defined(__clang__)
    #define KRONO_INLINE __attribute__((always_inline)) inline
  #else
    #define KRONO_INLINE inline
  #endif
#endif

/* Saturating add for deadlines (optional convenience) */
KRONO_INLINE uint64_t krono_add_ns(uint64_t t, uint64_t delta_ns) {
    uint64_t r = t + delta_ns;
    if (r < t) return UINT64_MAX;
    return r;
}

/* =========================
 *  Monotonic time (ns)
 * ========================= */

#if KRONO_OS_WINDOWS

KRONO_INLINE uint64_t krono_now_ns(void) {
    LARGE_INTEGER c, f;
    QueryPerformanceCounter(&c);
    QueryPerformanceFrequency(&f);

    /* Convert ticks -> ns using integer math:
       ns = (ticks * 1e9) / freq
       (This can overflow if you multiply first; use 128-bit if available, else split.) */
#if defined(__SIZEOF_INT128__)
    __int128 num = (__int128)c.QuadPart * 1000000000ll;
    return (uint64_t)(num / f.QuadPart);
#else
    /* Fallback: reduce risk of overflow by dividing first (slightly less precise). */
    uint64_t ticks = (uint64_t)c.QuadPart;
    uint64_t freq  = (uint64_t)f.QuadPart;
    uint64_t sec   = ticks / freq;
    uint64_t rem   = ticks % freq;
    uint64_t ns    = sec * 1000000000ull + (rem * 1000000000ull) / freq;
    return ns;
#endif
}

#else /* POSIX-ish */

KRONO_INLINE uint64_t krono_now_ns(void) {
#if defined(CLOCK_MONOTONIC)
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ull + (uint64_t)ts.tv_nsec;
#elif KRONO_OS_MACOS
    /* Older macOS fallback: mach_absolute_time() */
    static mach_timebase_info_data_t tb;
    if (tb.denom == 0) {
        (void)mach_timebase_info(&tb);
    }
    uint64_t t = mach_absolute_time();
#if defined(__SIZEOF_INT128__)
    __int128 ns = (__int128)t * tb.numer / tb.denom;
    return (uint64_t)ns;
#else
    /* Fallback without 128-bit: may overflow on very long uptimes, but generally OK. */
    return (t * (uint64_t)tb.numer) / (uint64_t)tb.denom;
#endif
#else
    /* Unknown platform: best effort using time() seconds. */
    return (uint64_t)time(NULL) * 1000000000ull;
#endif
}

#endif

/* =========================
 *  Realtime / wall-clock time (ns) — best effort
 * ========================= */

#if KRONO_OS_WINDOWS

KRONO_INLINE uint64_t krono_now_realtime_ns(void) {
    /* FILETIME: 100-ns intervals since Jan 1, 1601 (UTC) */
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    ULARGE_INTEGER u;
    u.LowPart  = ft.dwLowDateTime;
    u.HighPart = ft.dwHighDateTime;
    uint64_t t100ns = (uint64_t)u.QuadPart;
    return t100ns * 100ull; /* -> ns */
}

#else

KRONO_INLINE uint64_t krono_now_realtime_ns(void) {
#if defined(CLOCK_REALTIME)
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ull + (uint64_t)ts.tv_nsec;
#else
    /* C11 timespec_get is another option; keep it simple here. */
    return (uint64_t)time(NULL) * 1000000000ull;
#endif
}

#endif

/* =========================
 *  Sleep utilities
 * ========================= */

#if KRONO_OS_WINDOWS

KRONO_INLINE void krono_sleep_ns(uint64_t ns) {
    if (ns == 0) return;

    /* Use a waitable timer for sub-ms-ish waits (still OS dependent). */
    HANDLE timer = CreateWaitableTimer(NULL, TRUE, NULL);
    if (!timer) {
        /* Fallback: Sleep() uses milliseconds */
        DWORD ms = (DWORD)((ns + 999999ull) / 1000000ull);
        Sleep(ms);
        return;
    }

    /* Relative time in 100-ns units; negative indicates relative */
    LARGE_INTEGER due;
    uint64_t t100ns = (ns + 99ull) / 100ull; /* round up */
    due.QuadPart = -(LONGLONG)t100ns;

    if (!SetWaitableTimer(timer, &due, 0, NULL, NULL, FALSE)) {
        CloseHandle(timer);
        DWORD ms = (DWORD)((ns + 999999ull) / 1000000ull);
        Sleep(ms);
        return;
    }

    WaitForSingleObject(timer, INFINITE);
    CloseHandle(timer);
}

#else /* POSIX */

KRONO_INLINE void krono_sleep_ns(uint64_t ns) {
    if (ns == 0) return;

    struct timespec req;
    req.tv_sec  = (time_t)(ns / 1000000000ull);
    req.tv_nsec = (long)(ns % 1000000000ull);

    /* nanosleep can be interrupted; loop until complete */
    while (nanosleep(&req, &req) == -1 && errno == EINTR) {
        /* keep sleeping */
    }
}

#endif

KRONO_INLINE void krono_sleep_until_ns(uint64_t deadline_ns) {
    for (;;) {
        uint64_t now = krono_now_ns();
        if (now >= deadline_ns) return;

        uint64_t remaining = deadline_ns - now;

        /* Avoid oversleeping for very small remaining times. */
        if (remaining > 2000000ull) {               /* > 2 ms */
            krono_sleep_ns(remaining - 1000000ull); /* leave ~1 ms margin */
        } else if (remaining > 100000ull) {         /* 0.1 ms ... 2 ms */
            krono_sleep_ns(remaining / 2);
        } else {
            /* Busy-wait for the last ~100 µs */
            while (krono_now_ns() < deadline_ns) { /* spin */ }
            return;
        }
    }
}

/* =========================
 *  Convenience conversions
 * ========================= */

KRONO_INLINE double krono_ns_to_ms(uint64_t ns) { return (double)ns / 1e6; }
KRONO_INLINE double krono_ns_to_s (uint64_t ns) { return (double)ns / 1e9; }

#ifdef __cplusplus
} /* extern "C" */
#endif