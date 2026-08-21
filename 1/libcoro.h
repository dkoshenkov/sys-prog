#pragma once

#include <stdbool.h>

struct coro;
typedef void*(*coro_f)(void*);

/**
 * Initialize the coroutines engine.
 *
 * Инициализировать движок сопрограмм.
 */
void coro_sched_init(void);

/**
 * Run the coroutines processing while there are any runnable
 * ones.
 *
 * Выполнять обработку сопрограмм, пока есть готовые к запуску.
 */
void coro_sched_run(void);

/**
 * Destroy the coroutines engine. All coros must be finished by
 * now.
 *
 * Уничтожить движок сопрограмм. К этому моменту все сопрограммы
 * должны завершиться.
 */
void coro_sched_destroy(void);

/**
 * Get the currently working coroutine.
 *
 * Получить текущую выполняемую сопрограмму.
 */
struct coro* coro_this(void);

/**
 * Create a new coroutine. The function won't yield. The coroutine
 * will start execution automatically on the next iteration of the
 * scheduler.
 *
 * Whatever the callback function returns, will be returned from
 * coro_join().
 *
 * Создать сопрограмму. Функция не передаёт управление. Сопрограмма
 * автоматически начнёт выполняться на следующей итерации планировщика.
 *
 * Значение, возвращённое функцией обратного вызова, вернёт coro_join().
 */
struct coro* coro_new(coro_f func, void* func_arg);

/**
 * Join a coroutine. When joined, its resources are freed, and the
 * result of its callback function is returned. Each coroutine
 * must be joined. Otherwise it leaks.
 *
 * Присоединиться к сопрограмме. При присоединении её ресурсы освобождаются,
 * а возвращается результат функции обратного вызова. Каждую сопрограмму
 * необходимо присоединить, иначе возникнет утечка.
 */
void* coro_join(struct coro* coro);

/**
 * Pause the current coroutine until its explicitly woken up with
 * coro_wakeup(). Can be used to wait for some event, which will
 * wakeup this coro when happens.
 *
 * Приостановить текущую сопрограмму до явного пробуждения через
 * coro_wakeup(). Можно использовать для ожидания события, которое
 * разбудит сопрограмму.
 */
void coro_suspend(void);

/**
 * Pause the current coroutine until the next iteration of the
 * scheduler. Can be used to let the other coroutines work for a
 * bit.
 *
 * Приостановить текущую сопрограмму до следующей итерации планировщика.
 * Это позволяет другим сопрограммам немного поработать.
 */
void coro_yield(void);

/**
 * Wakeup a coroutine. If it was suspended, then it is going to be
 * continued on the next iteration of the scheduler. Otherwise
 * this function is a nop.
 *
 * Разбудить сопрограмму. Если она была приостановлена, её выполнение
 * продолжится на следующей итерации планировщика. Иначе функция ничего
 * не делает.
 */
void coro_wakeup(struct coro* coro);
