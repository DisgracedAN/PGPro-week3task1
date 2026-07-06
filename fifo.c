#include "scheduler.h"
#include <stdlib.h>
#include <string.h>

//FIFO (First In, First Out) – невытесняющий алгоритм
//Процессы выполняются в порядке поступления (arrival_time)
//Если несколько процессов поступают одновременно, порядок задаётся их исходным положением в массиве
int schedule_fifo(Process* procs, int count, int* gantt, int* gantt_size) {
    if (!procs || count <= 0 || !gantt || !gantt_size) return -1;

    // Копия массива, чтобы не портить исходный
    Process* p = copy_processes(procs, count);
    if (!p) return -1;

    // Сортируем по времени прибытия
    qsort(p, count, sizeof(Process), cmp_arrival);

    int current_time = 0;
    int gantt_idx = 0;

    for (int i = 0; i < count; i++) {
        // Заполняем простои, если процесс прибывает позже
        while (current_time < p[i].arrival_time && gantt_idx < *gantt_size) {
            gantt[gantt_idx++] = -1;   // -1 обозначает простой
            current_time++;
        }

        // Выполняем процесс
        for (int t = 0; t < p[i].burst_time && gantt_idx < *gantt_size; t++) {
            gantt[gantt_idx++] = p[i].pid;
            current_time++;
        }
    }

    *gantt_size = gantt_idx;
    free(p);
    return 0;
}