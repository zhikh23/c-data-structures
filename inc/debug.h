#ifndef DEBUG_H
#define DEBUG_H

#define DEBUG

#ifdef DEBUG
#define log_errorf(...) do { fprintf(stderr, __VA_ARGS__); fputc('\n', stderr); } while (0)
#else
#define log_errorf(...) ((void) 0)
#endif

#endif // DEBUG_H
