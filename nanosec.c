#include <stdint.h>
#include <time.h>

#include "nanosec.h"

uint64_t time_in_secs(uint64_t time) {
    return time / 1e9;
}

#ifdef MAC

#include <mach/mach_time.h>  

uint64_t time_get()
{
    return mach_absolute_time(); 
}

float time_diff(uint64_t end,
                uint64_t start) {  
        uint64_t difference = end - start;  
        static mach_timebase_info_data_t info = {0,0};  

        if (info.denom == 0)  
                mach_timebase_info(&info);  
  
        uint64_t elapsednano = difference * (info.numer / info.denom);  

        return elapsednano * 1e-9;
}  

#else

uint64_t time_get()
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec * 1e9 + t.tv_nsec;
}

float time_diff(uint64_t end,
                uint64_t start)
{
    float elapsednano = end - start;

    return elapsednano * 1e-9;
}
#endif
