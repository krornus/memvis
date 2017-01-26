struct Hentry {
    char *key;
    void *value;
    UT_hash_handle hh;
};

void hset(char *key, void *value);
void *hget(char *key);
