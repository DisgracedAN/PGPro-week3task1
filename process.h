#ifndef PROCESS_H
#define PROCESS_H
//структура процесса
typedef struct {
	int pid;//идентификатор процесса
	int arrival_time;//время прибытия в систему
	int burst_time;//требуемое время процессора (исходное)
	int priotity;//статический приоритет (чем больше число, тем ниже приоритет)
	int remaining_time;//оставшееся время выполнения (используется во время симуляции)

}Process;

#endif 
