#ifndef SPAD_H
#define SPAD_H

#include "includes.h"

/* scratchpad memory */
class SPad
{
public:
    /* constructor */
    SPad(int num_entries);

    /* destructor */
    ~SPad();

    /* read from scratchpad */
    void read(int addr, Packet *p);

    /* wrtie to scratchpad */
    void write(int addr, Packet *p);

private:
    int num_entries_;           // total number of entries in the scratchpad, each entry has PE_DATA_WIDTH bytes
    vector<Packet *> entries_;  // the actual data
};

#endif
