#include <stdio.h>
#include <stdlib.h>
#include "metrics.h"
#include "shared.h"

void get_msg(char *buffer, size_t buffer_size) {
    FILE *fp = fopen("/proc/loadavg", "r");
    if (fp == NULL) {
        perror("Error opening /proc/loadavg");
        exit(1);
    }

    fgets(buffer, buffer_size, fp);
    fclose(fp);
}
