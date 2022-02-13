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
