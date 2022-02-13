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

#ifndef FIFO_H
#define FIFO_H

#include "includes.h"

/* FIFO */
class FIFO
{
public:
    /* constructor */
    FIFO(int num_entries, int data_size);

    /* destructor */
    ~FIFO();

    /* get the front element */
    Packet *front();

    /* pop out the front element */
    void pop();

    /* push an element at the end */
    void push(Packet *p);

    /* get the end element */
    Packet *end();

    /* return true if the FIFO is full */
    bool full();

    /* return true if the FIFO is empty */
    bool empty();

    /* print the content of this FIFO for debug */
    void debug_print();

private:
    int num_entries_;           // total number of entries in the scratchpad
    int data_size_;             // size of each element in bytes
    vector<Packet *> entries_;  // the actual data
    int size_;                  // current number of elements in the FIFO
    int front_;                 // index of the first element
    int end_;                   // index of the next empty entry
};

#endif
