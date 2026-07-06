#define _CRT_SECURE_NO_WARNINGS
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#define Sleep(x) usleep((x)*1000)
#endif
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "scheduler.h"

//Сравнение двух массивов gantt 
static int compare_gantt(const int* g1, int sz1, const int* g2, int sz2) {
    if (sz1 != sz2) return 0;
    for (int i = 0; i < sz1; i++)
        if (g1[i] != g2[i]) return 0;
    return 1;
}

// Автоматические тесты 
void run_automatic_tests(void) {
    printf("Автоматические тесты\n");

    // Тест 1: FIFO с простоем (процесс прибывает позже)
    {
        Process procs[] = {
            {1, 2, 3, 0, 3},   // прибытие в 2
            {2, 5, 2, 0, 2}
        };
        int gantt[10];
        int sz = 10;
        int res = schedule_fifo(procs, 2, gantt, &sz);
        assert(res == 0);
        int expected[] = { -1, -1, 1, 1, 1, 2, 2 };
        assert(compare_gantt(gantt, sz, expected, 7));
        printf("  [OK] FIFO с простоем\n");
    }

    // Тест 2: Round Robin с простоем
    {
        Process procs[] = {
            {1, 0, 2, 0, 2},
            {2, 3, 2, 0, 2}
        };
        int gantt[10];
        int sz = 10;
        int res = schedule_rr(procs, 2, 2, gantt, &sz);
        assert(res == 0);
        // P1 выполняется 0,1 (завершён), затем простой в t=2, потом P2 в t=3,4
        int expected[] = { 1, 1, -1, 2, 2 };
        assert(compare_gantt(gantt, sz, expected, 5));
        printf("  [OK] RR с простоем\n");
    }

    // Тест 3: MLFQ с простоем
    {
        Process procs[] = {
            {1, 4, 3, 0, 3}
        };
        int gantt[10];
        int sz = 10;
        int res = schedule_mlfq(procs, 1, gantt, &sz);
        assert(res == 0);
        // Простой 0..3, затем P1 4..6
        int expected[] = { -1, -1, -1, -1, 1, 1, 1 };
        assert(compare_gantt(gantt, sz, expected, 7));
        printf("  [OK] MLFQ с простоем\n");
    }

    // Тест 4: Базовый FIFO без простоев
    {
        Process procs[] = {
            {1, 0, 5, 0, 5},
            {2, 1, 3, 0, 3},
            {3, 2, 2, 0, 2}
        };
        int gantt[15];
        int sz = 15;
        schedule_fifo(procs, 3, gantt, &sz);
        int expected[] = { 1,1,1,1,1,2,2,2,3,3 };
        assert(compare_gantt(gantt, sz, expected, 10));
        printf("  [OK] FIFO базовый\n");
    }

    printf("Все автоматические тесты пройдены.\n\n");
}

//Вспомогательная функция вывода диаграммы Ганта
static void print_gantt(const int* gantt, int size, const char* algo_name) {
    printf("Диаграмма Ганта (%s):\n", algo_name);
    printf("|");
    for (int i = 0; i < size; i++) {
        if (gantt[i] == -1)
            printf("--");
        else
            printf("%2d", gantt[i]);
        if (i < size - 1) printf("|");
    }
    printf("|\n ");
    for (int i = 0; i < size; i++) {
        printf("%2d ", i);
    }
    printf("\n\n");
}

//Демонстрация
void run_demo(void) {
    printf("Демонстрация алгоритмов планирования\n");

    // Демо-набор процессов
    Process procs[] = {
        {1, 0, 6, 2, 6},
        {2, 2, 4, 1, 4},
        {3, 4, 2, 0, 2},
        {4, 6, 3, 1, 3},
        {5, 8, 5, 2, 5}
    };
    int count = sizeof(procs) / sizeof(procs[0]);
    int total_burst = 0;
    for (int i = 0; i < count; i++) total_burst += procs[i].burst_time;
    int max_gantt = total_burst + 20;   // запас на простои
    int* gantt = (int*)malloc(max_gantt * sizeof(int));
    int gantt_size;

    // FIFO
    gantt_size = max_gantt;
    schedule_fifo(procs, count, gantt, &gantt_size);
    print_gantt(gantt, gantt_size, "FIFO");

    // Round Robin (квант = 3)
    gantt_size = max_gantt;
    schedule_rr(procs, count, 3, gantt, &gantt_size);
    print_gantt(gantt, gantt_size, "Round Robin (q=3)");

    // MLFQ
    gantt_size = max_gantt;
    schedule_mlfq(procs, count, gantt, &gantt_size);
    print_gantt(gantt, gantt_size, "MLFQ");

    free(gantt);
}

#define ANIMATION_DELAY_MS 500

static void animate_gantt(const int* gantt, int size, const char* algo_name) {
    printf("Анимация выполнения (%s). Каждый такт с задержкой %d мс.\n", algo_name, ANIMATION_DELAY_MS);
    printf("Нажмите Ctrl+C для остановки.\n");

    for (int i = 0; i < size; i++) {
        printf("\rТакт %2d: [", i);
        if (gantt[i] == -1)
            printf("ПРОСТОЙ");
        else
            printf("PID %2d", gantt[i]);
        printf("]");
        fflush(stdout);
        Sleep(ANIMATION_DELAY_MS);
    }
    printf("\n");
    // После анимации выводим полную диаграмму
    print_gantt(gantt, size, algo_name);
}

void run_animated_demo(void) {
    printf("Анимированная демонстрация алгоритмов планирования\n");

    Process procs[] = {
        {1, 0, 6, 2, 6},
        {2, 2, 4, 1, 4},
        {3, 4, 2, 0, 2},
        {4, 6, 3, 1, 3},
        {5, 8, 5, 2, 5}
    };
    int count = sizeof(procs) / sizeof(procs[0]);
    int total_burst = 0;
    for (int i = 0; i < count; i++) total_burst += procs[i].burst_time;
    int max_gantt = total_burst + 20;
    int* gantt = (int*)malloc(max_gantt * sizeof(int));
    int gantt_size;

    printf("Выберите алгоритм:\n");
    printf("1. FIFO\n");
    printf("2. Round Robin (q=3)\n");
    printf("3. MLFQ\n");
    printf("Ваш выбор: ");
    int choice;
    scanf("%d", &choice);

    switch (choice) {
    case 1:
        gantt_size = max_gantt;
        schedule_fifo(procs, count, gantt, &gantt_size);
        animate_gantt(gantt, gantt_size, "FIFO");
        break;
    case 2:
        gantt_size = max_gantt;
        schedule_rr(procs, count, 3, gantt, &gantt_size);
        animate_gantt(gantt, gantt_size, "Round Robin (q=3)");
        break;
    case 3:
        gantt_size = max_gantt;
        schedule_mlfq(procs, count, gantt, &gantt_size);
        animate_gantt(gantt, gantt_size, "MLFQ");
        break;
    default:
        printf("Неверный выбор.\n");
    }
    free(gantt);
}