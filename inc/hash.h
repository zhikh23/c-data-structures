#ifndef HASH_H
#define HASH_H

typedef unsigned hash_t;
typedef hash_t (*hash_func_t)(hash_t seed, const char *key);

hash_t djb2(hash_t salt, const char *key);

#endif // HASH_H
