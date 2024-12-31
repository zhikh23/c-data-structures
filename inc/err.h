#ifndef ERR_H
#define ERR_H

#define MAX_ERRNO 4095

#define EIO    5    // Ошибка I/O.
#define ENOMEM 12   // Ошибка выделения памяти.
#define EINVAL 22   // Некорректные данные.

// Возвращает 1, если указатель является кодом ошибки.
#define IS_ERR_VALUE(x) ((unsigned long)(void *)(x) >= (unsigned long)-MAX_ERRNO)

// Преобразует код ошибки в указатель.
#define ERR_PTR(error) ((void *) error)

// Преобразует указатель в код ошибки.
#define PTR_ERR(ptr) (-(long) ptr)

// Возвращает 1, если указатель является кодом ошибки; 0 иначе.
#define IS_ERR(ptr) (IS_ERR_VALUE((unsigned long) ptr))

// Возвращает 1, если указатель является кодом ошибки; NULL иначе.
#define IS_ERR_OR_NULL(ptr) (!(ptr) || IS_ERR_VALUE((unsigned long)ptr))

// Преобразует указатель к типу void*.
#define ERR_CAST(ptr) ((void *) ptr)

// Возвращает код ошибки или 0.
#define PTR_ERR_OR_ZERO(ptr) (IS_ERR(ptr) ? PTR_ERR(ptr) : 0)

#endif // ERR_H
