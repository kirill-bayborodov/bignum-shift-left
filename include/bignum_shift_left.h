/**
 * @file    bignum_shift_left.h
 * @author  git@bayborodov.com
 * @version 1.0.0
 * @date    03.10.2025
 *
 * @brief   Публичный API для логического сдвига bignum_t влево.
 *
 * @details
 *   Выполняет in-place (на месте) логический сдвиг большого числа на
 *   заданное количество бит. Нормализация (удаление ведущих нулей)
 *   выполняется автоматически.
 *
 *   Функция является потокобезопасной при условии, что разные потоки
 *   работают с разными, не пересекающимися объектами `bignum_t`.
 *
 *   **Алгоритм:**
 *    1. Проверка аргументов.
 *    2. Нулевой сдвиг — быстрый выход.
 *    3. Разбиение `shift_amount` на сдвиг по словам (`word_shift`) и битам (`bit_shift`).
 *    4. Проверка на переполнение (при выходе старшего бита за BIGNUM_CAPACITY).
 *    5. Сдвиг по словам, затем побитовый сдвиг с переносами между словами.
 *    6. Обновление `len` и нормализация результата.
 *
 * @see     bignum.h
 * @since   1.0.0
 *
 * @history
 *   - rev. 1 (02.08.2025): Первоначальное создание API.
 *   - rev. 2 (02.08.2025): API улучшен по результатам аудита: добавлены
 *                         макросы версий, `restrict`, `size_t`, улучшены
 *                         Doxygen-комментарии и include guards.
 */

#ifndef BIGNUM_SHIFT_LEFT_H
#define BIGNUM_SHIFT_LEFT_H

#include <bignum.h>
#include <stddef.h>
#include <stdint.h>

// Проверка на наличие определения BIGNUM_CAPACITY из общего заголовка
#ifndef BIGNUM_CAPACITY
#  error "bignum.h must define BIGNUM_CAPACITY"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Макросы семантического версионирования.
 */
#define BIGNUM_SHIFT_LEFT_VER_MAJOR  1
#define BIGNUM_SHIFT_LEFT_VER_MINOR  0
#define BIGNUM_SHIFT_LEFT_VER_PATCH  0

/**
 * @brief Коды состояния для функции bignum_shift_left.
 */
typedef enum {
    BIGNUM_SHIFT_SUCCESS         =  0, /**< Успех. Сдвиг выполнен. */
    BIGNUM_SHIFT_ERROR_NULL_ARG  = -1, /**< Указатель `num` равен NULL. */
    BIGNUM_SHIFT_ERROR_OVERFLOW  = -2  /**< Сдвиг привел к потере значащих бит (переполнению). */
} bignum_shift_status_t;

/**
 * @brief      Выполняет логический сдвиг большого числа влево.
 *
 * @param[in,out] num           Указатель на число для модификации. Размер внутреннего
 *                              буфера определяется BIGNUM_CAPACITY.
 * @param[in]     shift_amount  Количество бит для сдвига влево.
 *
 * @return
 *   - `BIGNUM_SHIFT_SUCCESS` (0) – сдвиг выполнен успешно.
 *   - `BIGNUM_SHIFT_ERROR_NULL_ARG` (-1) – передан NULL вместо числа.
 *   - `BIGNUM_SHIFT_ERROR_OVERFLOW` (-2) – сдвиг привёл к переполнению ёмкости.
 *
 * @details
 *   **Алгоритм:**
 *   1.  Проверка аргументов на NULL.
 *   2.  Если `shift_amount` равен 0, немедленно вернуть успех.
 *   3.  Вычислить сдвиг в целых словах (`word_shift`) и в битах внутри слова (`bit_shift`).
 *   4.  Проверить, не приведет ли сдвиг к переполнению (самый старший бит
 *       сдвигается за пределы емкости `BIGNUM_CAPACITY`).
 *   5.  Выполнить сдвиг по словам, перемещая данные в старшие позиции.
 *   6.  Выполнить побитовый сдвиг внутри слов, распространяя биты-переносы
 *       из младших слов в старшие.
 *   7.  Обновить поле `len` и нормализовать результат (удалить ведущие нули).
 *
 * @param[in,out] num           Указатель на число для модификации.
 * @param[in]     shift_amount  Количество бит для сдвига влево.
 *
 * @return     Код состояния `bignum_shift_status_t`.
 */
bignum_shift_status_t bignum_shift_left(bignum_t* restrict num, size_t shift_amount);

/**
 * @brief      Возвращает строковое представление версии библиотеки.
 * @return     Указатель на статическую строку с версией "MAJOR.MINOR.PATCH".
 */
const char* bignum_shift_left_get_version_string(void);

/**
 * @brief      Возвращает числовое представление версии библиотеки.
 * @details    Формат: `0xMMmmpp` (MAJOR<<16 | MINOR<<8 | PATCH).
 * @return     Числовое представление версии.
 */
uint32_t bignum_shift_left_get_version_number(void);

#ifdef __cplusplus
}
#endif

#endif /* BIGNUM_SHIFT_LEFT_H */
