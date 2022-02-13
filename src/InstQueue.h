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

#ifndef INSTQUEUE_H
#define INSTQUEUE_H

#include "includes.h"

/* an instruction queue */
class InstQueue
{
public:
    /* constructor */
    InstQueue(int num_entries);

    /* destructor */
    ~InstQueue();

    /* get the current size of the instruction queue */
    int get_size();

    /* get the front instruction */
    Instruction *front();

    /* pop out the front instruction */
    void pop();

    /* push an instruction at the end */
    void push(Instruction *inst);

    /* return true if the instruction queque is full */
    bool full();

    /* return true if the instruction queque is empty */
    bool empty();

    /* initialize the traverse */
    void traverse_init();

    /* get the next instruction in the queue when traversing it */
    Instruction *traverse_next();

private:
    int num_entries_;               // total capacity of instruction queque
    vector<Instruction *> queue_;   // the queque of instructions
    int size_;                      // current number instructions in instruction queque
    int front_;                     // index of the first instruction
    int end_;                       // index of the next empty instruction slot
    int curr_;                      // this is used to traverse the instruction queue in 
};

#endif
