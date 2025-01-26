#include "hmap.h"

#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>

#include "err.h"
#include "hash.h"


#define HMAP_BUCKET_SIZE 8
#define HMAP_INITIAL_B 4    // 2^4 = 16
#define HMAP_MAX_LOAD_FACTOR 0.75

#define TOP_HASH_MASK(B) ((1 << (B)) - 1)
#define LOW_HASH_MASK(B) (~((1 << (B)) - 1))
#define HMAP_BUCKETS(B) (1 << (B))


struct hmap_bucket;
typedef struct hmap_bucket hmap_bucket_t;

struct hmap_bucket {
    // HOB - High Order Bytes, старшие байты хэша.
    hash_t hob[HMAP_BUCKET_SIZE];

    // Количество элементов в бакете.
    unsigned char len;

    // Ключи, причём i-ый ключ имеет i-ое значение из vals.
    mut_mkey_t keys[HMAP_BUCKET_SIZE];

    // Значения, причём i-ое значение имеет i-ый ключ из keys.
    mval_t vals[HMAP_BUCKET_SIZE];

    // Следующий бакет в случае переполнения текущего.
    hmap_bucket_t *next;
};

typedef struct {
    // Реализация интерфейса imap_t.
    const imap_t *class;

    // log2 от количества бакетов.
    // Использование логарифма от числа бакетов
    // необходимо для побитовых операций с хэшем.
    unsigned char B;

    // Массив бакетов количеством 2^B,
    hmap_bucket_t *buckets;

    // Хэш-функция.
    hash_func_t hash;

    // Зерно для криптографической устойчивости хэш-функции.
    // Генерируется случайно при создании мапы.
    hash_t seed;
} hmap_t;

// Необходимо для валидной реализации интерфейса.
_Static_assert(offsetof(hmap_t, class) == 0);


void *hmap_ctor(void *_class, va_list *ap) {
    hmap_t *self = _class;

    self->B = HMAP_INITIAL_B;
    self->buckets = calloc(HMAP_BUCKETS(self->B), sizeof(hmap_bucket_t));
    if (self->buckets == NULL)
        return ERR_PTR(-ENOMEM);

    self->seed = rand();
    self->hash = va_arg(*ap, hash_func_t);
    assert(self->hash != NULL);

    return self;
}

void hmap_dtor(void *_self) {
    hmap_t *self = _self;

    // Бакеты 0-го уровня вложенности хранятся единым участком памяти,
    // поэтому они освобождаются free(old_buckets).
    // Бакеты, которые соединены в цепочку, аллоцированы отдельно,
    // поэтому необходимо отдельно их освободить.
    for (size_t i = 0; i < HMAP_BUCKETS(self->B); i++) {
        hmap_bucket_t *bucket = self->buckets[i].next;
        while (bucket) {
            for (unsigned char j = 0; j < bucket->len; j++) {
                free(bucket->keys[j]);
                bucket->keys[j] = NULL;
            }
            hmap_bucket_t *next = bucket->next;
            free(bucket);
            bucket = next;
        }
        bucket = &self->buckets[i];
        for (unsigned char j = 0; j < bucket->len; j++) {
            free(bucket->keys[j]);
            bucket->keys[j] = NULL;
        }
    }

    free(self->buckets);
    self->buckets = NULL;
}

static inline hash_t hmap_lob_hash(const hmap_t *self, mkey_t key) {
    return self->hash(self->seed, key) & TOP_HASH_MASK(self->B);
}

static inline hash_t hmap_hob_hash(const hmap_t *self, mkey_t key) {
    return self->hash(self->seed, key) & LOW_HASH_MASK(self->B);
}

static double hmap_load_factor(const hmap_t *self) {
    unsigned elems = 0;
    unsigned buckets = 0;

    for (unsigned char i = 0; i < HMAP_BUCKETS(self->B); i++) {
        const hmap_bucket_t *bucket = &self->buckets[i];
        while (bucket) {
            elems += bucket->len;
            buckets++;
            bucket = bucket->next;
        }
    }

    // guaranteed at least one bucket (B = 0, 2^0 = 1).
    return (double) elems / ((double) buckets * HMAP_BUCKET_SIZE);
}

void hmap_insert(void *_self, mkey_t key, mval_t value);

int hmap_grow(hmap_t *self) {
    hmap_bucket_t *old_buckets = self->buckets;
    const size_t old_capacity = HMAP_BUCKETS(self->B);

    self->B++;
    void *tmp = malloc(HMAP_BUCKETS(self->B) * sizeof(void *));
    if (tmp == NULL)
        return ENOMEM;
    self->buckets = tmp;

    for (size_t i = 0; i < old_capacity; i++) {
        hmap_bucket_t *bucket = &old_buckets[i];
        while (bucket) {
            for (unsigned char j = 0; j < bucket->len; j++) {
                hmap_insert(self, bucket->keys[j], bucket->vals[j]);
                free(bucket->keys[j]); // hmap_insert вызывает strdup для ключа
                bucket->keys[j] = NULL;
            }
            bucket = bucket->next;
        }
    }

    // Бакеты 0-го уровня вложенности хранятся единым участком памяти,
    // поэтому они освобождаются free(old_buckets).
    // Бакеты, которые соединены в цепочку, аллоцированы отдельно,
    // поэтому необходимо отдельно их освободить.
    for (size_t i = 0; i < old_capacity; i++) {
        hmap_bucket_t *bucket = old_buckets[i].next;
        while (bucket) {
            hmap_bucket_t *next = bucket->next;
            free(bucket); // вызываем только для бакета, так как ключи уже освобождены.
            bucket = next;
        }
    }

    free(old_buckets);

    return 0;
}

void hmap_insert(void *_self, mkey_t key, const mval_t value) {
    hmap_t *self = _self;

    const hash_t lob = hmap_lob_hash(self, key);
    hmap_bucket_t *bucket = &self->buckets[lob];

    const hash_t hob = hmap_hob_hash(self, key);
    unsigned char i;
    for (i = 0; i < bucket->len; i++) {
        if (bucket->hob[i] == hob && STR_EQ(bucket->keys[i], key)) {
            bucket->vals[i] = value;    // update existing value.
            goto done;
        }
    }

    if (i == HMAP_BUCKET_SIZE) {
        if (bucket->next == NULL) {
            bucket->next = calloc(1, sizeof(hmap_bucket_t));
            bucket = bucket->next;
            i = 0;
        }
    }

    bucket->hob[i] = hob;
    bucket->len++;
    bucket->keys[i] = strdup(key);
    bucket->vals[i] = value;

done:
    while (hmap_load_factor(self) > HMAP_MAX_LOAD_FACTOR) {
        hmap_grow(self);
    }
}

map_res_t hmap_lookup(const void *_self, mkey_t key) {
    const hmap_t *self = _self;

    const hash_t lob = hmap_lob_hash(self, key);
    const hmap_bucket_t *bucket = &self->buckets[lob];

    const hash_t hob = hmap_hob_hash(self, key);
again:
    for (unsigned char i = 0; i < bucket->len; i++) {
        if (hob == bucket->hob[i] && STR_EQ(bucket->keys[i], key)) {
            return (map_res_t){
                .data = bucket->vals[i],
                .ok   = 1,
            };
        }
    }

    if (bucket->next == NULL)
        return (map_res_t){0};

    bucket = bucket->next;
    goto again;
}

int hmap_remove(void *_self, mkey_t key) {
    hmap_t *self = _self;

    const hash_t lob = hmap_lob_hash(self, key);
    hmap_bucket_t *bucket = &self->buckets[lob];

    const hash_t hob = hmap_hob_hash(self, key);
again:
    for (unsigned char i = 0; i < bucket->len; i++) {
        if (bucket->hob[i] == hob && STR_EQ(bucket->keys[i], key)) {
            free(bucket->keys[i]);
            memmove(bucket->hob  + i, bucket->hob  + i + 1, sizeof(hash_t) * (bucket->len - i - 1));
            memmove(bucket->keys + i, bucket->keys + i + 1, sizeof(mkey_t) * (bucket->len - i - 1));
            memmove(bucket->vals + i, bucket->vals + i + 1, sizeof(mval_t) * (bucket->len - i - 1));
            bucket->len--;
            return 1;
        }
    }

    if (bucket->next == NULL)
        return 0;

    bucket = bucket->next;
    goto again;
}

const imap_t HashMapClass = {
    .size   = sizeof(hmap_t),
    .ctor   = hmap_ctor,
    .dtor   = hmap_dtor,
    .insert = hmap_insert,
    .lookup = hmap_lookup,
    .remove = hmap_remove,
};
