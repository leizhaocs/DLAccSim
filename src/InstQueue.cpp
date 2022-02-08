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
