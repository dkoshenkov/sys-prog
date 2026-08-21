#pragma once

#include <sys/types.h>

/**
 * User-defined in-memory filesystem. It is as simple as possible.
 * Each file lies in the memory as an array of blocks. A file
 * has an unique file name, and there are no directories, so the
 * FS is a monolithic flat contiguous folder.
 *
 * Пользовательская файловая система в памяти. Она максимально проста.
 * Каждый файл хранится в памяти как массив блоков. У файла уникальное
 * имя, каталогов нет, поэтому ФС представляет собой единый плоский
 * непрерывный каталог.
 */

/**
 * Here you should specify which features do you want to implement
 * via macros: NEED_OPEN_FLAGS and NEED_RESIZE. If you want to
 * allow advanced flags, do this here:
 *
 *     #define NEED_OPEN_FLAGS 1
 *
 * To allow resize() functions define this:
 *
 *     #define NEED_RESIZE 1
 *
 * It is important to define these macros here, in the header,
 * because it is used by tests.
 *
 * Здесь через макросы задаются возможности, которые требуется
 * реализовать: NEED_OPEN_FLAGS и NEED_RESIZE. Чтобы разрешить
 * расширенные флаги, добавьте:
 *
 *     #define NEED_OPEN_FLAGS 1
 *
 * Чтобы разрешить функции resize(), добавьте:
 *
 *     #define NEED_RESIZE 1
 *
 * Эти макросы важно определить здесь, в заголовочном файле, поскольку
 * их используют тесты.
 */
#define NEED_OPEN_FLAGS 0
#define NEED_RESIZE 0

/**
 * Flags for ufs_open call.
 *
 * Флаги для вызова ufs_open.
 */
enum open_flags {
    /**
     * If the flag specified and a file does not exist -
     * create it.
     *
     * Если флаг указан, а файла не существует — создать его.
     */
    UFS_CREATE = 0b0001,

#if NEED_OPEN_FLAGS
    /**
     * With this flag it is allowed to only read the file.
     *
     * С этим флагом разрешено только читать файл.
     */
    UFS_READ_ONLY=0b0010,
    /**
     * With this flag it is allowed to only write into the
     * file.
     *
     * С этим флагом разрешено только записывать в файл.
     */
    UFS_WRITE_ONLY=0b0100,
    /**
     * With this flag it is allowed to both read and write
     * into the file.
     *
     * С этим флагом разрешено и читать, и записывать в файл.
     */
    UFS_READ_WRITE= UFS_READ_ONLY| UFS_WRITE_ONLY,
#endif
};

/**
 * Possible errors from all functions.
 *
 * Возможные ошибки всех функций.
 */
enum ufs_error_code {
    UFS_ERR_NO_ERR = 0,
    UFS_ERR_NO_FILE,
    UFS_ERR_NO_MEM,
    UFS_ERR_NOT_IMPLEMENTED,

#if NEED_OPEN_FLAGS
    UFS_ERR_NO_PERMISSION,
#endif
};

/**
 * Get code of the last error.
 *
 * Получить код последней ошибки.
 */
ufs_error_code ufs_errno();

/**
 * Open a file by filename.
 * @param filename Name of a file to open.
 * @param flags Bitwise combination of open_flags.
 *
 * @retval > 0 File descriptor.
 * @retval -1 Error occurred. Check ufs_errno() for a code.
 *     - UFS_ERR_NO_FILE - no such file, and UFS_CREATE flag is
 *       not specified.
 *
 * Открыть файл по имени.
 * @param filename Имя открываемого файла.
 * @param flags Побитовая комбинация open_flags.
 *
 * @retval > 0 Файловый дескриптор.
 * @retval -1 Произошла ошибка. Код можно получить через ufs_errno().
 *     - UFS_ERR_NO_FILE — файла не существует и флаг UFS_CREATE не
 *       указан.
 */
int ufs_open(const char* filename, int flags);

/**
 * Write data to the file.
 * @param fd File descriptor from ufs_open().
 * @param buf Buffer to write.
 * @param size Size of @a buf.
 *
 * @retval > 0 How many bytes were written.
 * @retval -1 Error occurred. Check ufs_errno() for a code.
 *     - UFS_ERR_NO_FILE - invalid file descriptor.
 *     - UFS_ERR_NO_MEM - not enough memory.
 *
 * Записать данные в файл.
 * @param fd Файловый дескриптор из ufs_open().
 * @param buf Буфер с записываемыми данными.
 * @param size Размер @a buf.
 *
 * @retval > 0 Число записанных байтов.
 * @retval -1 Произошла ошибка. Код можно получить через ufs_errno().
 *     - UFS_ERR_NO_FILE — некорректный файловый дескриптор.
 *     - UFS_ERR_NO_MEM — недостаточно памяти.
 */
ssize_t ufs_write(int fd, const char* buf, size_t size);

/**
 * Read data from the file.
 * @param fd File descriptor from ufs_open().
 * @param buf Buffer to read into.
 * @param size Maximum bytes to read.
 *
 * @retval > 0 How many bytes were read.
 * @retval 0 EOF.
 * @retval -1 Error occurred. Check ufs_errno() for a code.
 *     - UFS_ERR_NO_FILE - invalid file descriptor.
 *
 * Прочитать данные из файла.
 * @param fd Файловый дескриптор из ufs_open().
 * @param buf Буфер для чтения.
 * @param size Максимальное число читаемых байтов.
 *
 * @retval > 0 Число прочитанных байтов.
 * @retval 0 EOF.
 * @retval -1 Произошла ошибка. Код можно получить через ufs_errno().
 *     - UFS_ERR_NO_FILE — некорректный файловый дескриптор.
 */
ssize_t ufs_read(int fd, char* buf, size_t size);

/**
 * Close a file.
 * @param fd File descriptor from ufs_open().
 * @retval 0 Success.
 * @retval -1 Error occurred. Check ufs_errno() for a code.
 *     - UFS_ERR_NO_FILE - invalid file descriptor.
 *
 * Закрыть файл.
 * @param fd Файловый дескриптор из ufs_open().
 * @retval 0 Успешное выполнение.
 * @retval -1 Произошла ошибка. Код можно получить через ufs_errno().
 *     - UFS_ERR_NO_FILE — некорректный файловый дескриптор.
 */
int ufs_close(int fd);

/**
 * Delete a file by its name. Note, that it is allowed to drop the
 * file even if there are opened descriptors. In such a case the
 * file content will live until the last descriptor is closed. If
 * the file is deleted, it is allowed to create a new one with the
 * same name immediately and it should not affect existing opened
 * descriptors of the deleted file.
 *
 * @param filename Name of a file to delete.
 * @retval -1 Error occurred. Check ufs_errno() for a code.
 *     - UFS_ERR_NO_FILE - no such file.
 *
 * Удалить файл по имени. Файл разрешено удалить, даже если существуют
 * открытые дескрипторы. В этом случае содержимое файла существует до
 * закрытия последнего дескриптора. После удаления разрешено сразу создать
 * новый файл с тем же именем: это не должно повлиять на существующие
 * открытые дескрипторы удалённого файла.
 *
 * @param filename Имя удаляемого файла.
 * @retval -1 Произошла ошибка. Код можно получить через ufs_errno().
 *     - UFS_ERR_NO_FILE — файла не существует.
 */
int ufs_delete(const char* filename);

#if NEED_RESIZE

/**
 * Resize a file opened by the file descriptor @a fd. If current
 * file size is less than @a new_size, then new empty blocks are
 * created and positions of opened file descriptors are not
 * changed. If the current size is bigger than @a new_size, then
 * the blocks are truncated. Opened file descriptors behind the
 * new file size should proceed from the new file end.
 *
 * @param fd File descriptor from ufs_open().
 * @param new_size New file size.
 * @retval 0 Success.
 * @retval -1 Error occurred.
 *     - UFS_ERR_NO_FILE - invalid file descriptor.
 *     - UFS_ERR_NO_PERMISSION - descriptor should have been opened with
 *       UFS_WRITE_ONLY or UFS_READ_WRITE permissions.
 *     - UFS_ERR_NO_MEM - not enough memory. Can appear only when
 *       @a new_size is bigger than the current size.
 *
 * Изменить размер файла, открытого по файловому дескриптору @a fd. Если
 * текущий размер файла меньше @a new_size, создаются новые пустые блоки, а
 * позиции открытых файловых дескрипторов не меняются. Если текущий размер
 * больше @a new_size, блоки обрезаются. Открытые файловые дескрипторы,
 * находящиеся за новой границей файла, должны продолжить работу с нового
 * конца файла.
 *
 * @param fd Файловый дескриптор из ufs_open().
 * @param new_size Новый размер файла.
 * @retval 0 Успешное выполнение.
 * @retval -1 Произошла ошибка.
 *     - UFS_ERR_NO_FILE — некорректный файловый дескриптор.
 *     - UFS_ERR_NO_PERMISSION — дескриптор должен быть открыт с правами
 *       UFS_WRITE_ONLY или UFS_READ_WRITE.
 *     - UFS_ERR_NO_MEM — недостаточно памяти. Может возникнуть только
 *       когда @a new_size больше текущего размера.
 */
int ufs_resize(int fd, size_t new_size);

#endif

/**
 * Destroy all the global variables, free all the memory, close and delete all
 * the files. After the destruction neither of the ufs functions are supposed to
 * be used. Purpose of the destruction is to reclaim all the dynamic memory.
 *
 * Уничтожить все глобальные переменные, освободить всю память, закрыть и
 * удалить все файлы. После уничтожения ни одну из функций ufs использовать
 * нельзя. Цель уничтожения — освободить всю динамически выделенную память.
 */
void ufs_destroy(void);
