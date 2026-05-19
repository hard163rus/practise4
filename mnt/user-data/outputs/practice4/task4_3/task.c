/*
 * Задание 4-3: Демоны и фоновые процессы
 * Вариант 1: Программа запускается как демон и каждые 5 секунд
 * записывает текущее время в daemon.log.
 *
 * Компиляция: gcc -Wall -Wextra -o task task.c
 * Запуск:     ./task
 * Остановка:  kill <PID>   (PID выводится при запуске)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

#define LOG_FILE "/tmp/daemon.log"
#define INTERVAL 5   /* секунд между записями */

/* Флаг завершения — выставляется обработчиком сигнала */
static volatile sig_atomic_t running = 1;

/* Обработчик сигналов SIGTERM и SIGINT */
static void handle_signal(int sig) {
    (void)sig;
    running = 0;
}

/* Записать строку в лог-файл */
static void log_write(const char *msg) {
    int fd;
    fd = open(LOG_FILE, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd == -1) return;
    write(fd, msg, strlen(msg));
    close(fd);
}

int main(void) {
    pid_t pid, sid;
    time_t now;
    struct tm *tm_info;
    char buf[128];

    /* ============================================================
     * ДЕМОНИЗАЦИЯ — классическая двойная вилка
     * ============================================================ */

    /* Шаг 1: первый fork — отсоединяемся от терминала */
    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        /* Родитель завершается — дочерний продолжает */
        printf("[parent] Демон запущен с PID %d\n", (int)pid);
        printf("[parent] Лог-файл: %s\n", LOG_FILE);
        printf("[parent] Остановить: kill %d\n", (int)pid);
        exit(EXIT_SUCCESS);
    }

    /* Шаг 2: создаём новую сессию — процесс становится лидером */
    sid = setsid();
    if (sid == -1) {
        perror("setsid");
        exit(EXIT_FAILURE);
    }

    /* Шаг 3: второй fork — теперь процесс не может получить tty */
    pid = fork();
    if (pid == -1) {
        perror("fork 2");
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        exit(EXIT_SUCCESS);  /* второй родитель завершается */
    }

    /* Шаг 4: сменяем рабочую директорию на корень */
    if (chdir("/") == -1) {
        perror("chdir");
        exit(EXIT_FAILURE);
    }

    /* Шаг 5: сбрасываем маску прав доступа */
    umask(0);

    /* Шаг 6: закрываем стандартные дескрипторы */
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    /* Перенаправляем stdin/stdout/stderr в /dev/null */
    open("/dev/null", O_RDONLY);   /* stdin  */
    open("/dev/null", O_WRONLY);   /* stdout */
    open("/dev/null", O_WRONLY);   /* stderr */

    /* ============================================================
     * ОСНОВНОЙ ЦИКЛ ДЕМОНА
     * ============================================================ */

    /* Устанавливаем обработчики сигналов */
    signal(SIGTERM, handle_signal);
    signal(SIGINT,  handle_signal);

    log_write("=== Демон запущен ===\n");

    while (running) {
        /* Получаем текущее время */
        now = time(NULL);
        tm_info = localtime(&now);
        strftime(buf, sizeof(buf), "[%Y-%m-%d %H:%M:%S] Демон работает\n", tm_info);

        log_write(buf);
        sleep(INTERVAL);
    }

    log_write("=== Демон остановлен ===\n");
    return 0;
}
