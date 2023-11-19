#include <memory.h>
#include <errno.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include "shared.h"

bool strtou16(const char *str, u16 *res) {
    char *end;
    errno = 0;
    intmax_t val = strtoimax(str, &end, 10);
    if (errno == ERANGE || val < 0 || val > UINT16_MAX || end == str || *end != '\0') {
        return false;
    }

    *res = (u16) val;
  
    return true;
}

bool parse_host_port(struct args *result, const char* argv_host_port) {
    char *colon = strchr(argv_host_port, ':');
    if (colon == NULL) {
        return false;
    }

    size_t host_len = colon - argv_host_port;
    if (host_len > sizeof(result->host) - 1) {
        return false;
    }

    memcpy(result->host, argv_host_port, host_len);
    result->host[host_len] = '\0';

    char *endptr;
    bool err = strtou16(colon + 1, &result->port);
    if (!err) {
        return false;
    }

    return true;
}

bool args_init(struct args *result, int argc, const char *argv[]) { 
    if (argc != 2) {
        return false;
    }

    return parse_host_port(result, argv[1]);
}


size_t timestamp(struct timeval *tv, char *buf, size_t buf_size) {
    struct tm *tm = localtime(&tv->tv_sec);
    size_t len = strftime(buf, buf_size, "%Y-%m-%d %H:%M:%S", tm);
    if (len == 0) {
        return 0;
    }

    size_t written = len;
    len = snprintf(buf + written, buf_size - written, ".%06ld", tv->tv_usec);
    if (len == 0) {
        return 0;
    }

    written += len;
    return written;
}
