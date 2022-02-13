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
InstQueue::InstQueue(int num_entries)
{
    num_entries_ = num_entries;
    for (int i = 0; i < num_entries_; i++)
    {
        Instruction *inst = (Instruction *)malloc(sizeof(Instruction));
        queue_.push_back(inst);
    }
    size_ = 0;
    front_ = 0;
    end_ = 0;
}

/* destructor */
InstQueue::~InstQueue()
{
    for (int i = 0; i < num_entries_; i++)
    {
        free(queue_[i]);
    }
}

/* get the current size of the instruction queue */
int InstQueue::get_size()
{
    return size_;
}

/* get the front instruction */
Instruction *InstQueue::front()
{
    return queue_[front_];
}

/* pop out the front instruction */
void InstQueue::pop()
{
    front_++;
    if (front_ == num_entries_)
    {
        front_ = 0;
    }
    size_--;
}

/* push an instruction at the end */
void InstQueue::push(Instruction *inst)
{
    memcpy(queue_[end_], inst, sizeof(Instruction));

    end_++; 
    if (end_ == num_entries_)
    {
        end_ = 0;
    }
    size_++;
}

/* return true if the instruction queque is full */
bool InstQueue::full()
{
    return (size_ == num_entries_);
}

/* return true if the instruction queque is empty */
bool InstQueue::empty()
{
    return (size_ == 0);
}

/* initialize the traverse */
void InstQueue::traverse_init()
{
    curr_ = front_;
}

/* get the next instruction in the queue druing traverse */
Instruction *InstQueue::traverse_next()
{
    if (curr_ == end_)
    {
        return NULL;
    }

    Instruction *inst = queue_[curr_];
    curr_++; 
    if (curr_ == num_entries_)
    {
        curr_ = 0;
    }
    return inst;
}
