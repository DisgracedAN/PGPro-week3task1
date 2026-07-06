#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "process.h"
#include <stdlib.h>
#include <string.h>

//Функции симуляции планирования
//procs: массив процессов (может быть изменён)
//count: количество процессов
//time_quantum: квант времени
//gantt: выходной массив, в который будут записаны pid процессов,
//выполнявшихся в каждый момент времени (по размеру = общее время выполнения)
//gantt_size: максимальный размер gantt (после заполнения содержит реальный размер)
//Возвращает 0 при успехе, -1 при ошибке
int schedule_fifo(Process* procs, int count, int* gantt, int* gantt_size);
int schedule_rr(Process* procs, int count, int time_quantum, int* gantt, int* gantt_size);
int schedule_mlfq(Process* procs, int count, int* gantt, int* gantt_size);

//вспомогательные функции для копирования процессов и сравнения времени прибытия
int cmp_arrival(void* a, void* b);
Process* copy_processes(Process* src, int count);


#endif 
