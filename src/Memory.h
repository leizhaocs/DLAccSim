#ifndef MEMORY_H
#define MEMORY_H

#include "includes.h"

/* off-chip memory */
class Memory
{
public:
    /* constructor */
    Memory(char *memory_filename);

    /* destructor */
    ~Memory();

    /* read from memory */
    void read(int addr, Packet *p);

    /* write into memory */
    void write(int addr, Packet *p);

private:
    unsigned char mem_[MEM_SIZE];  // the actual data in memory
};

#endif
