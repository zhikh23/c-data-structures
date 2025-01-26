/**
 * shmap.h - Simple Hash MAP, простейшая реализация
 * хэш-таблицы из курса Типов и структур данных.
 *
 * Используется:
 * - метод деления для вычисления позиции элемента.
 * - метод цепочек (открытая адресация) для решения коллизий.
 */
#ifndef SHMAP_H
#define SHMAP_H

#include "map.h"

typedef unsigned hash_t;
typedef hash_t (*hash_func_t)(hash_t seed, const void *key);


extern const imap_t SimpleHashMapClass;
// map_new(SimpleHashMap, capacity, hash_function)
static const imap_t *SimpleHashMap = &SimpleHashMapClass;

#endif // SHMAP_H
