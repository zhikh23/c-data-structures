/**
 * hmap.h - улучшенная реализация хэш-таблицы.
 *
 * В сравнении с shmap.h улучшенная производительность достигается за счёт:
 * - развёрнутого списка (buckets) хранения;
 * - хранения high-order bytes (HOB) хэша для проверки отсутствия элемента в таблице;
 * - использование побитовых операций с хэшем вместо арифметических.
 *
 * Аналогично shmap.h используются:
 * - метод деления для вычисления позиции элемента;
 * - открытая адресация (метод цепочек) для решения коллизий.
 */
#ifndef HMAP_H
#define HMAP_H

#include "map.h"

extern const imap_t HashMapClass;
// map_new(HashMap, hash_function)
static const imap_t *HashMap = &HashMapClass;

#endif // HMAP_H
