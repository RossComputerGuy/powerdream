#pragma once

#ifdef SIGILL
#undef SIGILL
#endif
#ifdef SIGFPE
#undef SIGFPE
#endif
#ifdef SIGKILL
#undef SIGKILL
#endif
#ifdef SIGCHLD
#undef SIGCHLD
#endif

#define SIGILL 0
#define SIGFPE 1
#define SIGKILL 2
#define SIGCHLD 3

#define SIGNAL_MAX 4
