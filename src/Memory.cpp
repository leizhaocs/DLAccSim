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
