#pragma once

/**
 * Here you should specify which features do you want to implement via macros:
 * If you want to enable author name support, do:
 *
 *     #define NEED_AUTHOR 1
 *
 * To enable server-feed from admin do:
 *
 *     #define NEED_SERVER_FEED 1
 *
 * It is important to define these macros here, in the header, because it is
 * used by tests.
 *
 * Здесь макросами укажите реализуемые функции. Чтобы включить поддержку имени автора,
 * определите NEED_AUTHOR в 1; чтобы включить отправку сервером сообщений администратора,
 * определите NEED_SERVER_FEED в 1. Важно определить макросы в заголовке: их используют
 * тесты.
 */
#define NEED_AUTHOR 0
#define NEED_SERVER_FEED 0

#include <string>

enum chat_errcode {
    CHAT_ERR_INVALID_ARGUMENT = 1,
    CHAT_ERR_TIMEOUT,
    CHAT_ERR_PORT_BUSY,
    CHAT_ERR_NO_ADDR,
    CHAT_ERR_ALREADY_STARTED,
    CHAT_ERR_NOT_IMPLEMENTED,
    CHAT_ERR_NOT_STARTED,
    CHAT_ERR_SYS,
};

enum chat_events {
    CHAT_EVENT_INPUT = 1,
    CHAT_EVENT_OUTPUT = 2,
};

struct chat_message {
#if NEED_AUTHOR
    /** Author's name.
     *
     * Имя автора.
     */
    std::string author;
#endif
    /** 0-terminate text.
     *
     * Текст с завершающим нулевым байтом.
     */
    std::string data;

    /* PUT HERE OTHER MEMBERS */
};

/** Convert chat_events mask to events suitable for poll().
 *
 * Преобразовать маску chat_events в события, подходящие для poll().
 */
int chat_events_to_poll_events(int mask);
