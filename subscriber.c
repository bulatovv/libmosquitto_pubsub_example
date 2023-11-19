#include <mosquitto.h>
#include <stdio.h>
#include <errno.h>
#include "shared.h"

void on_subscribe(struct mosquitto *mosq, void *obj, int mid, int qos_count, const int *granted_qos) {
    printf("Subscribed (mid: %d): %d\n", mid, granted_qos[0]);
}


void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg) {
    static char tsbuf[27];

    struct timeval tv;
    gettimeofday(&tv, NULL);
    timestamp(&tv, tsbuf, sizeof(tsbuf));

    printf(
        "[%s] New message %s: %s\n",
        tsbuf,
        msg->topic,
        (char *)msg->payload
    );
}

int main(int argc, const char *argv[]) {
    struct args args;
    bool err = args_init(&args, argc, argv);
    if (!err) {
        fprintf(stderr, "Usage: %s <host:port>\n", argv[0]);
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
                fprintf(stderr, "Error: Out of memory");
                return 1;
            case EINVAL:
                fprintf(stderr, "Error: Invalid id and/or clean_session");
                return 1;
        }
    }

    mosquitto_subscribe_callback_set(mosq, on_subscribe);

    rc = mosquitto_connect(mosq, host, port, 60);
    if (rc != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "Error: %s\n", mosquitto_strerror(rc));
        return 1;
    }

    mosquitto_subscribe(mosq, NULL, "metrics/loadavg", 0);

    mosquitto_message_callback_set(mosq, on_message);

    mosquitto_loop_forever(mosq, -1, 1);

    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

    return 0;
}
