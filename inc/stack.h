/**
 * stack.h - структура данных стек, организующая хранение по принципу FIFO
 * - first in, first out.
 *
 * Заголовочный файл является интерфейсом для реализации стека.
 */
#ifndef STACK_H
#define STACK_H

#include <stddef.h>
#include <stdarg.h>

// Синоним типа значения.
typedef int sval_t;

// Дескриптор стека.
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

    int (*is_empty)(const void *);
    int (*push)(void *, sval_t val);
    sval_t (*peek)(const void *);
    sval_t (*pop)(void *);
} istack_t;

/**
 * Создаёт объект указанного класса.
 * Сначала выделяет память для объекта, затем вызывает конструктор класса.
 * @param  class класс, реализующий интерфейс istack_t.
 * @param  ...   параметры конструктора класса.
 * @return Проинициализированный объект класса или ошибку (err.h).
 */
void *stack_new(const istack_t *class, ...);

/**
 * Освобождает память, использованную для объекта.
 * Сначала вызывает деструктор класса, если таковой имеется, затем
 * освобождает аллоцированную память в map_new.
 * @param self объект класса, реализующего интерфейс istack_t.
 */
void stack_destroy(void *self);

/**
 * Проверяет есть ли значения в стеке.
 * @param  self объект класса, реализующего интерфейс istack_t.
 * @return 1 если стек пустой; 0 иначе.
 */
int stack_empty(const void *self);

/**
 * Добавляет значение на верх стека.
 * @param  self объект класса, реализующего интерфейс istack_t.
 * @param  val  значение.
 * @return 0 если успешно; иначе код ошибки (err.h).
 */
int stack_push(void *self, sval_t val);

/**
 * Возвращает верхнее значение стека.
 * @attention Стек должен быть не пустым.
 * @param     self объект класса, реализующего интерфейс istack_t.
 * @return    Верхнее значение стека.
 */
sval_t stack_peek(const void *self);

/**
 * Удаляет верхнее значение стека и возвращает его.
 * @attention Стек должен быть не пустым.
 * @param     self объект класса, реализующего интерфейс istack_t.
 * @return    Верхнее значение стека.
 */
sval_t stack_pop(void *self);

#endif // STACK_H
