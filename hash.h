#include "uthash.h"

struct Hentry {
    char *key;
    void *value;
    UT_hash_handle hh;
};

void hset(struct Hentry **hash, char *key, void *value);
void *hget(struct Hentry *hash, char *key);
