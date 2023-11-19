#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <sys/time.h>

typedef uint16_t u16;

struct args {
    char host[256];
    u16 port; 
};


bool args_init(struct args *me, int argc, const char *argv[]);

size_t timestamp(struct timeval *tv, char *buf, size_t buf_size);
