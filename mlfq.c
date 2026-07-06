#include "scheduler.h"
#include <stdlib.h>
#include <string.h>

#define NUM_QUEUES 3 // число уровней приоритета
#define QUANTUM_0 4 // квант для высшего приоритета
#define QUANTUM_1 8
#define QUANTUM_2 16

//MLFQ – упрощённая многоуровневая очередь с обратной связью
//Правила
//	Если процесс не завершается за время кванта на текущем уровне, он понижается в следующий
//	Процесс с высшим приоритетом (0) всегда выбирается первым (абсолютный приоритет)
//	Внутри очереди используется Round Robin с заданным квантом
//Все новые процессы попадают в очередь 0
int schedule_mlfq(Process* procs, int count, int* gantt, int* gantt_size) {
    if (!procs || count <= 0 || !gantt || !gantt_size)
        return -1;

    Process* p = copy_processes(procs, count);
    if (!p) return -1;

    // Инициализация оставшегося времени
    for (int i = 0; i < count; i++) {
        p[i].remaining_time = p[i].burst_time;
    }

    // Три очереди (кольцевые буферы)
    int* queues[NUM_QUEUES];
    int head[NUM_QUEUES], tail[NUM_QUEUES], size[NUM_QUEUES];
    for (int q = 0; q < NUM_QUEUES; q++) {
        queues[q] = (int*)malloc(count * sizeof(int));
        if (!queues[q]) {
            for (int j = 0; j < q; j++) free(queues[j]);
            free(p);
            return -1;
        }
        head[q] = tail[q] = size[q] = 0;
    }

    int* added = (int*)calloc(count, sizeof(int));
    if (!added) {
        for (int q = 0; q < NUM_QUEUES; q++) free(queues[q]);
        free(p);
        return -1;
    }

    int current_time = 0;
    int gantt_idx = 0;
    int done = 0;

    while (done < count) {
        //Все новые процессы помещаем в очередь 0 (высший приоритет)
        for (int i = 0; i < count; i++) {
            if (!added[i] && p[i].arrival_time <= current_time) {
                queues[0][tail[0]] = i;
                tail[0] = (tail[0] + 1) % count;
                size[0]++;
                added[i] = 1;
            }
        }

        //Ищем наивысшую непустую очередь
        int active_q = -1;
        for (int q = 0; q < NUM_QUEUES; q++) {
            if (size[q] > 0) {
                active_q = q;
                break;
            }
        }

        //Если все очереди пусты – простой до следующего прибытия
        if (active_q == -1) {
            int next_arrival = -1;
            for (int i = 0; i < count; i++) {
                if (!added[i] && (next_arrival == -1 || p[i].arrival_time < next_arrival))
                    next_arrival = p[i].arrival_time;
            }
            if (next_arrival != -1) {
                while (gantt_idx < *gantt_size && current_time < next_arrival) {
                    gantt[gantt_idx++] = -1;
                    current_time++;
                }
            }
            continue;
        }

        //Извлекаем процесс из выбранной очереди
        int idx = queues[active_q][head[active_q]];
        head[active_q] = (head[active_q] + 1) % count;
        size[active_q]--;

        //Определяем квант для текущего уровня
        int quantum = (active_q == 0) ? QUANTUM_0 :
            (active_q == 1) ? QUANTUM_1 : QUANTUM_2;

        int exec_time = (p[idx].remaining_time < quantum)
            ? p[idx].remaining_time : quantum;

        for (int t = 0; t < exec_time && gantt_idx < *gantt_size; t++) {
            gantt[gantt_idx++] = p[idx].pid;
        }
        current_time += exec_time;
        p[idx].remaining_time -= exec_time;

        //Добавляем процессы, прибывшие за это время, в очередь 0
        for (int i = 0; i < count; i++) {
            if (!added[i] && p[i].arrival_time <= current_time) {
                queues[0][tail[0]] = i;
                tail[0] = (tail[0] + 1) % count;
                size[0]++;
                added[i] = 1;
            }
        }

        //Если процесс не завершён – понижаем в следующую очередь (или оставляем в той же, если это последняя)
        if (p[idx].remaining_time > 0) {
            int next_q = (active_q + 1 < NUM_QUEUES) ? active_q + 1 : active_q;
            queues[next_q][tail[next_q]] = idx;
            tail[next_q] = (tail[next_q] + 1) % count;
            size[next_q]++;
        }
        else {
            done++;
        }
    }

    *gantt_size = gantt_idx;

    for (int q = 0; q < NUM_QUEUES; q++) free(queues[q]);
    free(added);
    free(p);
    return 0;
}