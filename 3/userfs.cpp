#include "userfs.h"

#include "rlist.h"

#include <stddef.h>
#include <string>
#include <vector>

enum {
    BLOCK_SIZE = 512,
    MAX_FILE_SIZE = 1024 * 1024 * 100,
};

/**
 * Global error code. Set from any function on any error.
 *
 * Глобальный код ошибки. Устанавливается любой функцией при ошибке.
 */
static ufs_error_code ufs_error_code = UFS_ERR_NO_ERR;

struct block {
    /**
     * Block memory.
     *
     * Память блока.
     */
    char memory[BLOCK_SIZE];
    /**
     * A link in the block list of the owner-file.
     *
     * Связь в списке блоков файла-владельца.
     */
    rlist in_block_list = RLIST_LINK_INITIALIZER;

    /* PUT HERE OTHER MEMBERS */
};

struct file {
    /**
     * Doubly-linked intrusive list of file blocks. Intrusiveness of the
     * list gives you the full control over the lifetime of the items in the
     * list without having to use double pointers with performance penalty.
     *
     * Двусвязный интрузивный список блоков файла. Интрузивность даёт полный
     * контроль времени жизни элементов без двойных указателей и их потерь
     * производительности.
     */
    rlist blocks = RLIST_HEAD_INITIALIZER(blocks);
    /**
     * How many file descriptors are opened on the file.
     *
     * Число открытых дескрипторов файла.
     */
    int refs = 0;
    /**
     * File name.
     *
     * Имя файла.
     */
    std::string name;
    /**
     * A link in the global file list.
     *
     * Связь в глобальном списке файлов.
     */
    rlist in_file_list = RLIST_LINK_INITIALIZER;

    /* PUT HERE OTHER MEMBERS */
};

/**
 * Intrusive list of all files. In this case the intrusiveness of the list also
 * grants the ability to remove items from any position in O(1) complexity
 * without having to know their iterator.
 *
 * Интрузивный список всех файлов. Он также позволяет удалять элемент из
 * произвольной позиции за O(1), не зная его итератора.
 */
static rlist file_list = RLIST_HEAD_INITIALIZER(file_list);

struct filedesc {
    file* atfile;

    /* PUT HERE OTHER MEMBERS */
};

/**
 * An array of file descriptors. When a file descriptor is
 * created, its pointer drops here. When a file descriptor is
 * closed, its place in this array is set to NULL and can be
 * taken by next ufs_open() call.
 *
 * Массив файловых дескрипторов. При создании дескриптора его указатель
 * попадает сюда. При закрытии соответствующий элемент становится NULL
 * и может быть использован следующим вызовом ufs_open().
 */
static std::vector<filedesc*> file_descriptors;

enum ufs_error_code ufs_errno() {
    return ufs_error_code;
}

int ufs_open(const char* filename, int flags) {
    /* IMPLEMENT THIS FUNCTION */
    (void)filename;
    (void)flags;
    (void)file_list;
    (void)file_descriptors;
    ufs_error_code = UFS_ERR_NOT_IMPLEMENTED;
    return -1;
}

ssize_t ufs_write(int fd, const char* buf, size_t size) {
    /* IMPLEMENT THIS FUNCTION */
    (void)fd;
    (void)buf;
    (void)size;
    ufs_error_code = UFS_ERR_NOT_IMPLEMENTED;
    return -1;
}

ssize_t ufs_read(int fd, char* buf, size_t size) {
    /* IMPLEMENT THIS FUNCTION */
    (void)fd;
    (void)buf;
    (void)size;
    ufs_error_code = UFS_ERR_NOT_IMPLEMENTED;
    return -1;
}

int ufs_close(int fd) {
    /* IMPLEMENT THIS FUNCTION */
    (void)fd;
    ufs_error_code = UFS_ERR_NOT_IMPLEMENTED;
    return -1;
}

int ufs_delete(const char* filename) {
    /* IMPLEMENT THIS FUNCTION */
    (void)filename;
    ufs_error_code = UFS_ERR_NOT_IMPLEMENTED;
    return -1;
}

#if NEED_RESIZE

int ufs_resize(int fd, size_t new_size) {
    /* IMPLEMENT THIS FUNCTION */
    (void)fd;
    (void)new_size;
    ufs_error_code = UFS_ERR_NOT_IMPLEMENTED;
    return -1;
}

#endif

void ufs_destroy(void) {
    /*
     * The file_descriptors array is likely to leak even if
     * you resize it to zero or call clear(). This is because
     * the vector keeps memory reserved in case more elements
     * would be added.
     *
     * The recommended way of freeing the memory is to swap()
     * the vector with a temporary empty vector.
     *
     * Массив file_descriptors может утечь даже при уменьшении до нуля или
     * вызове clear(), потому что vector сохраняет резерв памяти для будущих
     * элементов. Рекомендуется освободить память, обменяв vector через swap()
     * с временным пустым vector.
     */
}
