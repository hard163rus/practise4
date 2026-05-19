/*
 * Задание 4-1: Динамическая память
 * Вариант 1: Выделить память под массив из 100 целых чисел,
 * заполнить числами от 1 до 100, найти сумму.
 *
 * Компиляция: gcc -Wall -Wextra -o task task.c
 * Запуск:     ./task
 */

#include <stdio.h>
#include <stdlib.h>

#define N 100

int main(void) {
    int *arr;
    int i;
    long sum = 0;

    /* 1. Выделяем память под 100 int через malloc */
    arr = malloc(N * sizeof(int));
    if (arr == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    printf("[malloc]  Выделено %zu байт под %d элементов\n",
           N * sizeof(int), N);

    /* 2. Заполняем числами от 1 до 100 */
    for (i = 0; i < N; i++) {
        arr[i] = i + 1;
    }

    /* 3. Считаем сумму */
    for (i = 0; i < N; i++) {
        sum += arr[i];
    }
    printf("[result]  Сумма чисел от 1 до %d = %ld\n", N, sum);

    /* 4. Освобождаем память */
    free(arr);
    printf("[free]    Память освобождена\n");

    /* --- Демонстрация calloc и realloc --- */

    /* calloc: выделяет и обнуляет память */
    int *arr2 = calloc(N, sizeof(int));
    if (arr2 == NULL) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }
    printf("[calloc]  Выделено и обнулено %zu байт\n", N * sizeof(int));
    printf("[calloc]  arr2[0] = %d (должен быть 0)\n", arr2[0]);

    /* realloc: расширяем массив до 200 элементов */
    int *arr3 = realloc(arr2, 200 * sizeof(int));
    if (arr3 == NULL) {
        perror("realloc");
        free(arr2);
        exit(EXIT_FAILURE);
    }
    arr2 = arr3;
    printf("[realloc] Массив расширен до 200 элементов\n");

    free(arr2);
    printf("[free]    Память освобождена\n");

    return 0;
}
