#ifndef __DEGUG_UTIL_H__
#define __DEGUG_UTIL_H__


#define PRINT_DEBUG(...) do {\
    struct timeval val; \
    gettimeofday(&val, NULL); \
    fprintf(stderr, "\n"); \
    fprintf(stderr, "[[%ld.%06ld]%s:%s:%d]", val.tv_sec, val.tv_usec, __FILE__, __FUNCTION__, __LINE__); \
    fprintf(stderr, ##__VA_ARGS__);\
} while(0)

#endif