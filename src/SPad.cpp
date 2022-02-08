#include "includes.h"

SPad::SPad(int num_entries)
{
    num_entries_ = num_entries;
    for (int i = 0; i < num_entries_; i++)
    {
        entries_.push_back(new Packet(PE_DATA_WIDTH));
    }
}

/* destructor */
SPad::~SPad()
{
    for (int i = 0; i < num_entries_; i++)
    {
        delete entries_[i];
    }
}

/* read from scratchpad */
void SPad::read(int addr, Packet *p)
{
    p->copy(entries_[addr]);
}

/* wrtie to scratchpad */
void SPad::write(int addr, Packet *p)
{
    entries_[addr]->copy(p);
}
