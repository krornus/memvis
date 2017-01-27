#include <stdio.h>
#include "uthash.h"
#include "hash.h"

void hset(struct Hentry **hash, char *key, void *value);
void *hget(struct Hentry *hash, char *key);

void hset(struct Hentry **hash, char *key, void *value) 
{
    struct Hentry *ent;

    HASH_FIND_INT(*hash, &key, ent); 
    if(0!=key[0] && NULL == ent) /* disallow "" keys */
    {
        ent = malloc(sizeof(struct Hentry));
        ent->key = key;
        ent->value = value;

        HASH_ADD_STR(*hash, key, ent); 
    }
}


void *hget(struct Hentry *hash, char *key)
{
    struct Hentry *ent;

    HASH_FIND_STR(hash, key, ent); 

    if(NULL==ent)
    {
        return NULL;
    }
    return ent->value;
}
