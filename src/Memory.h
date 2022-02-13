/**********************************************************************
 *
 * Copyright Lei Zhao.
 * contact: leizhao0403@gmail.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 **********************************************************************/

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
