#include <stdio.h>
#include <cflush.h>
#include <stdlib.h>

int bytes_per_thread;

#define U32 *(volatile unsigned int *)
#define MEMORY_MEM_ADDR 0x80000000UL

void bench(int cid, void *x) {
    //printf("Core %d got 0x%x\n", cid , x);

    int bytes = bytes_per_thread;

    uint8_t z[bytes];
    void *y = z;
    // dirty each line
    for (int i = 0; i < bytes; i+=64) {
        U32(y+i) = i; 
    }

    // //flush
    //uint64_t start = read_csr(cycle);
    
    for (int i = 0; i < bytes; i += 64) {
        //printf("Core %d flushing 0x%x\n", cid, x+i);
        CBO_FLUSH_FN(y+i);
    }

    asm volatile ("fence rw, rw");
    
    //uint64_t end = read_csr(cycle);
    //uint64_t elapsed = end - start;
    //printf("CORE %d ELAPSED, %d cycles\n", cid, elapsed);

}

void thread_entry(int cid, int nc) {

    int reps = 10;
    int numThreads = 2;
    void *x = (void *)MEMORY_MEM_ADDR;
    for(int bytes=128;bytes<=16384;bytes*=2) {
        bytes_per_thread = bytes/2;
        //printf("CORE %d START %d long flushes\n", cid, bytes);
        for(int i=0;i<reps;i++) {
            //printf("CORE %d: Rep %d START\n", cid, i);
            if(cid == 0)
                bench(cid, x);
            else
                bench(cid, x+(bytes/2));
            //printf("CORE %d: Rep %d END\n", cid, i);
        }
        //printf("CORE %d FINISHED %d long flushes\n", cid, bytes);
    }

    //printf("DONE\n");
    //exit(0);
}