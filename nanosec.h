#ifndef NANOSEC_H
#define NANOSEC_H

#include <stdint.h>

uint64_t time_in_secs(uint64_t time);
uint64_t time_get();
float time_diff(uint64_t end, uint64_t start);

#endif
