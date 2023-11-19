#include <mosquitto.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "shared.h"
#include "metrics.h"


void on_publish(struct mosquitto *mosq, void *obj, int mid) {
    static char tsbuf[27];

    struct timeval tv;
    gettimeofday(&tv, NULL);
    timestamp(&tv, tsbuf, sizeof(tsbuf));
    
    printf("[%s] Published (mid: %d)\n", tsbuf, mid);
}

int main(int argc, const char *argv[]) {
    srand(time(NULL));

    struct args args;
    bool err = args_init(&args, argc, argv);
    if (!err) {
        fprintf(stderr, "Usage: %s <host:port>", argv[0]);
        return 1;
    }

    const char *host = args.host;
    const u16 port = args.port;

    struct mosquitto *mosq = NULL;
    int rc = 0;

    mosquitto_lib_init();

    mosq = mosquitto_new(NULL, true, NULL);
    
    if (mosq == NULL) {
        switch (errno) {
            case ENOMEM:
                fprintf(stderr, "Error: Out of memory.");
                return 1;
            case EINVAL:
                fprintf(stderr, "Error: Invalid id and/or clean_session.");
                return 1;
        }
    }

    mosquitto_publish_callback_set(mosq, on_publish);

    rc = mosquitto_connect(mosq, host, port, 60);
    if (rc != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "Error: %s", mosquitto_strerror(rc));
        return 1;
    }
 
    rc = mosquitto_loop_start(mosq);
    if (rc != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "Error: %s", mosquitto_strerror(rc));
        return 1;
    }

    
    char msg[256];
    for (;;) {
        get_msg(msg, sizeof(msg));
        rc = mosquitto_publish(
            mosq, NULL, 
            "metrics/loadavg", sizeof(msg), msg, 
            0, false
        );
        if (rc != MOSQ_ERR_SUCCESS) {
            fprintf(stderr, "Error: %s", mosquitto_strerror(rc));
            return 1;
        }
        usleep(5000000); 
    }


    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

    return 0;
} 
