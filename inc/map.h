#ifndef MAP_H
#define MAP_H

#include <stddef.h>

#include "str.h"

typedef string_t *const mkey_t;
typedef string_t *mut_mkey_t;
typedef int mval_t;

// Тип возвращаемого значения функции lookup.
typedef struct {
    mval_t data;   // Найденное значение или нулевое значение типа (все байты равны 0).
    int     ok;    // 1, если значение найдено.
} map_res_t;

// Интерфейс мапы (словаря).
// Мапа (словарь) - структура данных, предназначенная для хранения
// данных в формате (ключ, значение).
typedef struct {
    // size указывает на объём памяти, требуемый для выделения
    // объекту класса перед вызовом конструктора.
    // Обычно:
    //     .size = sizeof(MapImplementation);
    size_t size;

    // Конструктор класса.
    void *(*ctor)(void *, va_list *);

    // Деструктор класса.
    void (*dtor)(void *);

    // Методы мапы.

    void (*insert)(void *, mkey_t, mval_t);
    map_res_t (*lookup)(const void *, mkey_t);
    int (*remove)(void *, mkey_t);
} imap_t;


/**
 * Создаёт объект указанного класса.
 * Сначала выделяет память для объекта, затем вызывает конструктор класса.
 * @param  class класс, реализующий интерфейс imap_t.
 * @param  ...   параметры конструктора класса.
 * @return Проинициализированный объект класса или ошибку (err.h).
 */
void *map_new(const imap_t *class, ...);

/**
 * Освобождает память, использованную для объекта.
 * Сначала вызывает деструктор класса, если таковой имеется, затем
 * освобождает аллоцированную память в map_new.
 * @param self объект класса, реализующего интерфейс imap_t.
 */
void map_destroy(void *self);

/**
 * Сопоставляет ключу указанное значение.
 * Если по данному ключу уже есть значение, обновляет его.
 * @param self  объект класса, реализующего интерфейс imap_t.
 * @param key   ключ.
 * @param value значение.
 */
void map_insert(void *self, mkey_t key, mval_t value);

/**
 * Находит значение по ключу.
 *
 * Пример использования:
 *     res_int_t ret = map_lookup(map, "some key");
 *     if (!ret.ok)
 *         return NOT_FOUND;
 *     V res = ret.data;
 *
 * Если проверка на наличие значения в мапе не требуется, то можно
 * преобразовать возвращаемое значение к типу V.
 *     V res = map_lookup(map, "some key");
 *
 * @param  self объект класса, реализующего интерфейс imap_t.
 * @param  key  ключ.
 * @return Структура MAP_RES(V), где
 *           data - найденное значение или значение по умолчанию (все биты 0);
 *           ok   - 1 если найдено, иначе 0.
 */
map_res_t map_lookup(const void *self, mkey_t key);

/**
 * Удаляет значение по ключу, если существует.
 * @param  self объект класса, реализующего интерфейс imap_t.
 * @param  key  ключ
 * @return 1 если значение по ключу было удалено;
 *         0 если по ключу нет значения.
 */
int map_remove(void *self, mkey_t key);

#endif // MAP_H
