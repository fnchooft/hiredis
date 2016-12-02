#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include <hiredis.h>
#include <async.h>
#include <adapters/libev.h>

void onMessage(redisAsyncContext *c, void *reply, void *privdata) {
    redisReply *r = reply;
    if (reply == NULL) return;

    if (r->type == REDIS_REPLY_ARRAY) {
        unsigned int j = 0;
        for (j = 0; j < r->elements; j++) {
            printf("%u) %s\n", j, r->element[j]->str);
        }
    }
}

//int main (int argc, char **argv) {
int main (void) {
    signal(SIGPIPE, SIG_IGN);

    redisAsyncContext *c = redisAsyncConnect("127.0.0.1", 6379);
    if (c->err) {
        printf("error: %s\n", c->errstr);
        return 1;
    }
    printf("Subscribing to test-database             [%d]\n",9);
    printf("Subscribing to key-space-events on prefix[%s]\n","abckey");
    printf("To test:\n\tredis-cli\n\tselect 9\n\tset abckey:1 somevalue\n-----------------------------\n");
    redisLibevAttach(EV_DEFAULT_ c);
    redisAsyncCommand(c, onMessage, NULL, "CONFIG SET notify-keyspace-events KEA");
    redisAsyncCommand(c, onMessage, NULL, "PSUBSCRIBE __key*@9__:abckey*");
    ev_loop(EV_DEFAULT_ 0);
    return 0;
}
