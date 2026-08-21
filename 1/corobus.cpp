#include "corobus.h"

#include "libcoro.h"
#include "rlist.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

/**
 * One coroutine waiting to be woken up in a list of other
 * suspended coros.
 *
 * Одна сопрограмма ожидает пробуждения в списке других приостановленных
 * сопрограмм.
*/
struct wakeup_entry {
    struct rlist base;
    struct coro* coro;
};

/*
 * A queue of suspended coros waiting to be woken up.
 *
 * Очередь приостановленных сопрограмм, ожидающих пробуждения.
*/
struct wakeup_queue {
    struct rlist coros;
};

#if 0 /* Uncomment this if want to use */

/*
 * Suspend the current coroutine until it is woken up.
 *
 * Приостановить текущую сопрограмму до пробуждения.
*/
static void wakeup_queue_suspend_this(struct wakeup_queue* queue) {
    struct wakeup_entry entry;
    entry.coro = coro_this();
    rlist_add_tail_entry(&queue->coros, &entry, base);
    coro_suspend();
    rlist_del_entry(&entry, base);
}

/*
 * Wakeup the first coroutine in the queue.
 *
 * Разбудить первую сопрограмму в очереди.
*/
static void wakeup_queue_wakeup_first(struct wakeup_queue* queue) {
    if (rlist_empty(&queue->coros)) return;
    struct wakeup_entry* entry = rlist_first_entry(&queue->coros,
		struct wakeup_entry, base);
    coro_wakeup(entry->coro);
}

#endif

struct coro_bus_channel {
    /*
     * Channel max capacity.
     *
     * Максимальная вместимость канала.
    */
    size_t size_limit;
    /*
     * Coroutines waiting until the channel is not full.
     *
     * Сопрограммы, ожидающие, пока канал перестанет быть заполненным.
    */
    struct wakeup_queue send_queue;
    /*
     * Coroutines waiting until the channel is not empty.
     *
     * Сопрограммы, ожидающие, пока канал перестанет быть пустым.
    */
    struct wakeup_queue recv_queue;
    /*
     * Message queue.
     *
     * Очередь сообщений.
    */
    /* std::vector/queue/deque/list/...<unsigned> data; */
};

struct coro_bus {
    struct coro_bus_channel** channels;
    int channel_count;
};

static enum coro_bus_error_code global_error = CORO_BUS_ERR_NONE;

enum coro_bus_error_code coro_bus_errno(void) {
    return global_error;
}

void coro_bus_errno_set(enum coro_bus_error_code err) {
    global_error = err;
}

struct coro_bus* coro_bus_new(void) {
    /* IMPLEMENT THIS FUNCTION */
    coro_bus_errno_set(CORO_BUS_ERR_NOT_IMPLEMENTED);
    return NULL;
}

void coro_bus_delete(struct coro_bus* bus) {
    /* IMPLEMENT THIS FUNCTION */
    (void)bus;
}

int coro_bus_channel_open(struct coro_bus* bus, size_t size_limit) {
    /* IMPLEMENT THIS FUNCTION */
    (void)bus;
    (void)size_limit;
    coro_bus_errno_set(CORO_BUS_ERR_NOT_IMPLEMENTED);
    /*
     * One of the tests will force you to reuse the channel
     * descriptors. It means, that if your maximal channel
     * descriptor is N, and you have any free descriptor in
     * the range 0-N, then you should open the new channel on
     * that old descriptor.
     *
     * A more precise instruction - check if any of the
     * bus->channels[i] with i = 0 -> bus->channel_count is
     * free (== NULL). If yes - reuse the slot. Don't grow the
     * bus->channels array, when have space in it.
     *
     *
     *  Один из тестов потребует повторного использования
     * дескрипторов каналов. Это означает, что если
     * максимальный индекс дескриптора канала равен N,
     * и в диапазоне 0–N имеется свободный дескриптор,
     * то новый канал следует открывать, используя этот
     * старый дескриптор.
     *  Более точное указание: проверяйте, свободен ли
     * (равен ли NULL) какой-либо элемент bus->channels[i]
     * в диапазоне от 0 до bus->channel_count. Если да —
     * используйте этот слот повторно. Не увеличивайте
     * размер массива bus->channels, пока в нем есть
     * свободное место.
     */
    return -1;
}

void coro_bus_channel_close(struct coro_bus* bus, int channel) {
    /* IMPLEMENT THIS FUNCTION */
    (void)bus;
    (void)channel;
    /*
     * Be very attentive here. What happens, if the channel is
     * closed while there are coroutines waiting on it? For
     * example, the channel was empty, and some coros were
     * waiting on its recv_queue.
     *
     * If you wakeup those coroutines and just delete the
     * channel right away, then those waiting coroutines might
     * on wakeup try to reference invalid memory.
     *
     * Can happen, for example, if you use an intrusive list
     * (rlist), delete the list itself (by deleting the
     * channel), and then the coroutines on wakeup would try
     * to remove themselves from the already destroyed list.
     *
     * Think how you could address that. Remove all the
     * waiters from the list before freeing it? Yield this
     * coroutine after waking up the waiters but before
     * freeing the channel, so the waiters could safely leave?
     *
     *
     *  Будьте здесь очень внимательны. Что произойдет, если канал
     * будет закрыт в тот момент, когда его ожидают какие-либо
     * сопрограммы? Например, канал был пуст, и некоторые
     * сопрограммы находились в очереди ожидания (`recv_queue`).
     *  Если разбудить эти сопрограммы и сразу же удалить канал,
     * то после пробуждения они могут попытаться обратиться
     * к невалидной памяти.
     *  Такое может случиться, например, при использовании
     * интрузивного списка (`rlist`): если удалить сам список
     * (вместе с каналом), то сопрограммы при пробуждении
     * попытаются исключить себя из уже уничтоженного списка.
     *  Подумайте, как можно решить эту проблему. Удалить всех
     * ожидающих из списка перед его освобождением? Или
     * приостановить выполнение текущей сопрограммы (сделать
     * `yield`) после пробуждения ожидающих, но до освобождения
     * канала, чтобы они могли безопасно выйти?
     */
}

int coro_bus_send(struct coro_bus* bus, int channel, unsigned data) {
    /* IMPLEMENT THIS FUNCTION */
    (void)bus;
    (void)channel;
    (void)data;
    /*
     * Try sending in a loop, until success. If error, then
     * check which one is that. If 'wouldblock', then suspend
     * this coroutine and try again when woken up.
     *
     * If see the channel has space, then wakeup the first
     * coro in the send-queue. That is needed so when there is
     * enough space for many messages, and many coroutines are
     * waiting, they would then wake each other up one by one
     * as lone as there is still space.
     *
     *
     *  Попробуйте выполнить отправку в цикле до успешного завершения. В случае ошибки:
     * проверьте её тип. Если это 'wouldblock', приостановите
     * выполнение текущей сопрограммы и повторите попытку после возобновления работы.
     *  Если в канале появилось свободное место, разбудите первую
     * сопрограмму из очереди на отправку. Это необходимо для того, чтобы
     * при наличии места для нескольких сообщений и ожидании множества
     * сопрограмм они могли «будить» друг друга по очереди —
     * до тех пор, пока в канале остается свободное пространство.
     */
    coro_bus_errno_set(CORO_BUS_ERR_NOT_IMPLEMENTED);
    return -1;
}

int coro_bus_try_send(struct coro_bus* bus, int channel, unsigned data) {
    /* IMPLEMENT THIS FUNCTION */
    (void)bus;
    (void)channel;
    (void)data;
    /*
     * Append data if has space. Otherwise 'wouldblock' error.
     * Wakeup the first coro in the recv-queue! To let it know
     * there is data.
     *
     *
     *  Добавить данные, если есть свободное место.
     * В противном случае возникнет ошибка 'wouldblock'.
     * Разбудить первый цикл в очереди recv-queue!
     * Чтобы он знал, что есть данные.
     */
    coro_bus_errno_set(CORO_BUS_ERR_NOT_IMPLEMENTED);
    return -1;
}

int coro_bus_recv(struct coro_bus* bus, int channel, unsigned* data) {
    /* IMPLEMENT THIS FUNCTION */
    (void)bus;
    (void)channel;
    (void)data;
    coro_bus_errno_set(CORO_BUS_ERR_NOT_IMPLEMENTED);
    return -1;
}

int coro_bus_try_recv(struct coro_bus* bus, int channel, unsigned* data) {
    /* IMPLEMENT THIS FUNCTION */
    (void)bus;
    (void)channel;
    (void)data;
    coro_bus_errno_set(CORO_BUS_ERR_NOT_IMPLEMENTED);
    return -1;
}


#if NEED_BROADCAST

int coro_bus_broadcast(struct coro_bus* bus, unsigned data) {
    /* IMPLEMENT THIS FUNCTION */
    (void)bus;
    (void)data;
    coro_bus_errno_set(CORO_BUS_ERR_NOT_IMPLEMENTED);
    return -1;
}

int coro_bus_try_broadcast(struct coro_bus* bus, unsigned data) {
    /* IMPLEMENT THIS FUNCTION */
    (void)bus;
    (void)data;
    coro_bus_errno_set(CORO_BUS_ERR_NOT_IMPLEMENTED);
    return -1;
}

#endif

#if NEED_BATCH

int coro_bus_send_v(struct coro_bus* bus, int channel, const unsigned* data, unsigned count) {
    /* IMPLEMENT THIS FUNCTION */
    (void)bus;
    (void)channel;
    (void)data;
    (void)count;
    coro_bus_errno_set(CORO_BUS_ERR_NOT_IMPLEMENTED);
    return -1;
}

int coro_bus_try_send_v(struct coro_bus* bus, int channel, const unsigned* data, unsigned count) {
    /* IMPLEMENT THIS FUNCTION */
    (void)bus;
    (void)channel;
    (void)data;
    (void)count;
    coro_bus_errno_set(CORO_BUS_ERR_NOT_IMPLEMENTED);
    return -1;
}

int coro_bus_recv_v(struct coro_bus* bus, int channel, unsigned* data, unsigned capacity) {
    /* IMPLEMENT THIS FUNCTION */
    (void)bus;
    (void)channel;
    (void)data;
    (void)capacity;
    coro_bus_errno_set(CORO_BUS_ERR_NOT_IMPLEMENTED);
    return -1;
}

int coro_bus_try_recv_v(struct coro_bus* bus, int channel, unsigned* data, unsigned capacity) {
    /* IMPLEMENT THIS FUNCTION */
    (void)bus;
    (void)channel;
    (void)data;
    (void)capacity;
    coro_bus_errno_set(CORO_BUS_ERR_NOT_IMPLEMENTED);
    return -1;
}

#endif
