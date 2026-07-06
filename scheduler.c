#include "scheduler.h"
#include <stdlib.h>
#include <string.h>

int cmp_arrival(const void* a, const void* b) {
    const Process* pa = (const Process*)a;
    const Process* pb = (const Process*)b;
    return pa->arrival_time - pb->arrival_time;
}

Process* copy_processes(Process* src, int count) {
    Process* dst = (Process*)malloc(count * sizeof(Process));
    if (dst)
        memcpy(dst, src, count * sizeof(Process));
    return dst;
}