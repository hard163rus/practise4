/*
 * Задание 4-2: Отображение файлов в память mmap()
 * Вариант 1: Отобразить текстовый файл в память и подсчитать
 * количество символов 'a' и 'A'.
 *
 * Компиляция: gcc -Wall -Wextra -o task task.c
 * Запуск:     ./task input.txt
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int fd;
    struct stat sb;
    char *mapped;
    size_t i;
    long count_lower = 0;  /* 'a' */
    long count_upper = 0;  /* 'A' */

    if (argc != 2) {
        fprintf(stderr, "Использование: %s <файл>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* 1. Открываем файл */
    fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    /* 2. Получаем размер файла через fstat */
    if (fstat(fd, &sb) == -1) {
        perror("fstat");
        close(fd);
        exit(EXIT_FAILURE);
    }

    if (sb.st_size == 0) {
        printf("Файл пустой.\n");
        close(fd);
        return 0;
    }

    printf("[open]  Файл: %s, размер: %lld байт\n",
           argv[1], (long long)sb.st_size);

    /* 3. Отображаем файл в память */
    mapped = mmap(NULL,               /* ядро выбирает адрес */
                  (size_t)sb.st_size, /* размер отображения  */
                  PROT_READ,          /* только чтение       */
                  MAP_PRIVATE,        /* приватная копия     */
                  fd,                 /* файловый дескриптор */
                  0);                 /* смещение 0          */
    if (mapped == MAP_FAILED) {
        perror("mmap");
        close(fd);
        exit(EXIT_FAILURE);
    }

    printf("[mmap]  Файл отображён в память по адресу %p\n", (void *)mapped);

    /* 4. Закрываем fd — отображение остаётся активным */
    if (close(fd) == -1) {
        perror("close");
    }

    /* 5. Считаем символы 'a' и 'A' */
    for (i = 0; i < (size_t)sb.st_size; i++) {
        if (mapped[i] == 'a') count_lower++;
        else if (mapped[i] == 'A') count_upper++;
    }

    printf("[result] Символов 'a' (строчных): %ld\n", count_lower);
    printf("[result] Символов 'A' (заглавных): %ld\n", count_upper);
    printf("[result] Итого 'a'+'A': %ld\n", count_lower + count_upper);

    /* 6. Снимаем отображение */
    if (munmap(mapped, (size_t)sb.st_size) == -1) {
        perror("munmap");
        exit(EXIT_FAILURE);
    }
    printf("[munmap] Отображение снято\n");

    return 0;
}
