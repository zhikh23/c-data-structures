#include "hash.h"

hash_t djb2(const hash_t salt, const char *key) {
    hash_t hash = salt;

    unsigned char c;
    while ((c = *key++))
        hash = (hash << 5) + hash + c;  // hash * 33 + c

    return hash;
}
