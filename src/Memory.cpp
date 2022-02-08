#include "includes.h"

/* constructor */
Memory::Memory(char *memory_filename)
{
    ifstream file(memory_filename);
    char c;
    int i = 0;
    while (!file.eof())
    {
        file.get(c);
        mem_[i++] = (unsigned char)(c-'0');
    }
    file.close();
}

/* destructor */
Memory::~Memory()
{
}

/* read from memory */
void Memory::read(int addr, Packet *p)
{
    for (int i = 0; i < p->get_size(); i++)
    {
        p->data(i, mem_[addr+i]);
    }
}

/* write into memory */
void Memory::write(int addr, Packet *p)
{
    for (int i = 0; i < p->get_size(); i++)
    {
        mem_[addr+i] = p->data(i);
    }
}
