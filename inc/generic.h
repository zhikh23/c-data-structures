/**
 * Заголовочный файл макросов для создания псевдо-обобщённых
 * типов данных.
 *
 * Псевдо-обобщённым типом данных в Си будем называть коллекцию, тип элемента
 * которой определяется на этапе препроцессирования.
 *
 * ВАЖНО! Для указания конкретного типа псевдо-обобщённого типа данных
 *        допускается использование только ИДЕНТИФИКАТОРОВ типа.
 *
 * Не допускается использование указателей, структур:
 *
 *     GENERIC_TYPE(arr, void*)   // -> arr_void*_t
 *
 * Вместо этого необходимо объявить идентификатор-синоним типа:
 *
 *     typedef void *pvoid;
 *     GENERIC_TYPE(arr, pvoid)   // -> arr_pvoid_t
 */

#ifndef GENERIC_H
#define GENERIC_H

/**
 * Идентификатор структуры обобщённого типа данных.
 * @param base Префикс псевдо-обобщённого типа данных.
 * @param type Идентификатор типа элемента коллекции (T). НЕ допускается
 *             использование указателей, структур и других указаний типов
 *             данных, не являющихся идентификатором.
 */
#define GENERIC_STRUCT(base, type) struct base ## _ ## type

/**
 * Пример использования.
 *
 * #define ARR_STRUCT(type) GENERIC_STRUCT(arr, type)
 * ARR_STRUCT(T) {
 *     size_t len;
 *     T      data[];
 * };
 *
 * Для T = int:
 *
 * struct arr_int {
 *     size_t len;
 *     int    data[];
 * };
 */

/**
 * Идентификатор обобщённого типа данных.
 * @param base Префикс псевдо-обобщённого типа данных.
 * @param type Идентификатор типа элемента коллекции (T). НЕ допускается
 *             использование указателей, структур и других указаний типов
 *             данных, не являющихся идентификатором.
 */
#define GENERIC_TYPE(base, type) base ## _ ## type ## _t

/**
 * Пример использования.
 *
 * #define ARR(type) GENERIC_TYPE(arr, type)
 * typedef struct {
 *     size_t len;
 *     T      data[];
 * } ARR(T);
 *
 * Для T = int:
 *
 * typedef struct {
 *     size_t len;
 *     int    data[];
 * } arr_int_t;

/**
 * Идентификатор метода обобщённого типа данных.
 * @param base   Префикс обобщённого типа данных.
 * @param method Название метода.
 * @param type   Идентификатор типа элемента коллекции (T). НЕ допускается
 *               использование указателей, структур и других указаний типов
 *               данных, не являющихся идентификатором.
 */
#define GENERIC_METHOD(base, method, type) base ## _ ## type ## _ ## method

/**
 * Пример использования.
 *
 * #define ARR_APPEND(type) GENERIC_METHOD(arr, append, type)
 * ARR(T) *ARR_APPEND(T) (ARR(T) *self, T value);
 * ...
 * array = ARR_APPEND(T) (array, value);
 *
 * Для T = int:
 *
 * arr_int_t *arr_int_append(arr_int_t *self, int value);
 * ...
 * array = arr_int_append(array, value);
 */

#endif // GENERIC_H
