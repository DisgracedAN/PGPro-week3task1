#include "scheduler.h"
#include <stdlib.h>
#include <string.h>

//Round Robin – вытесняющий алгоритм с квантом времени
//Все процессы находятся в одной круговой очереди
//Если процесс не завершился за квант, он помещается в конец очереди

int schedule_rr(Process* procs, int count, int time_quantum, int* gantt, int* gantt_size) {
    if (!procs || count <= 0 || time_quantum <= 0 || !gantt || !gantt_size)
        return -1;

    Process* p = copy_processes(procs, count);
    if (!p) return -1;

    // Инициализация оставшегося времени
    for (int i = 0; i < count; i++) {
        p[i].remaining_time = p[i].burst_time;
    }

    // Очередь готовых процессов (кольцевой буфер индексов)
    int* queue = (int*)malloc(count * sizeof(int));
    int q_head = 0, q_tail = 0, q_size = 0;
    if (!queue) { free(p); return -1; }

    // Флаги добавления в очередь
    int* added = (int*)calloc(count, sizeof(int));
    if (!added) { free(queue); free(p); return -1; }

    int current_time = 0;
    int gantt_idx = 0;
    int done = 0;

    while (done < count) {
        //Добавляем все процессы, прибывшие к текущему моменту
        for (int i = 0; i < count; i++) {
            if (!added[i] && p[i].arrival_time <= current_time) {
                queue[q_tail] = i;
                q_tail = (q_tail + 1) % count;
                q_size++;
                added[i] = 1;
            }
        }

        //Если очередь пуста – заполняем простой до следующего прибытия
        if (q_size == 0) {
            int next_arrival = -1;
            for (int i = 0; i < count; i++) {
                if (!added[i] && (next_arrival == -1 || p[i].arrival_time < next_arrival))
                    next_arrival = p[i].arrival_time;
            }
            if (next_arrival != -1) {
                while (gantt_idx < *gantt_size && current_time < next_arrival) {
                    gantt[gantt_idx++] = -1;   // процессор простаивает
                    current_time++;
                }
            }
            continue;
        }

        //Извлекаем процесс из головы очереди
        int idx = queue[q_head];
        q_head = (q_head + 1) % count;
        q_size--;

        //Выполняем процесс не более кванта
        int exec_time = (p[idx].remaining_time < time_quantum)
            ? p[idx].remaining_time : time_quantum;

        for (int t = 0; t < exec_time && gantt_idx < *gantt_size; t++) {
            gantt[gantt_idx++] = p[idx].pid;
        }
        current_time += exec_time;
        p[idx].remaining_time -= exec_time;

        //Добавляем процессы, прибывшие за время выполнения
        for (int i = 0; i < count; i++) {
            if (!added[i] && p[i].arrival_time <= current_time) {
                queue[q_tail] = i;
                q_tail = (q_tail + 1) % count;
                q_size++;
                added[i] = 1;
            }
        }

        //Если процесс не завершён – возвращаем в конец очереди
        if (p[idx].remaining_time > 0) {
            queue[q_tail] = idx;
            q_tail = (q_tail + 1) % count;
            q_size++;
        }
        else {
            done++;
        }
    }

    *gantt_size = gantt_idx;
    free(queue);
    free(added);
    free(p);
    return 0;
}