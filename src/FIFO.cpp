#include "includes.h"

/* constructor */
FIFO::FIFO(int num_entries, int data_size)
{
    num_entries_ = num_entries;
    data_size_ = data_size;
    for (int i = 0; i < num_entries_; i++)
    {
        entries_.push_back(new Packet(data_size_));
    }
    size_ = 0;
    front_ = 0;
    end_ = 0;
}

/* destructor */
FIFO::~FIFO()
{
    for (int i = 0; i < num_entries_; i++)
    {
        delete entries_[i];
    }
}

/* read the front element */
Packet *FIFO::front()
{
    if (empty())
    {
        return NULL;
    }

    return entries_[front_];
}

/* pop out the front element */
void FIFO::pop()
{
    front_++;
    if (front_ == num_entries_)
    {
        front_ = 0;
    }
    size_--;
}

/* push an element at the end */
void FIFO::push(Packet *p)
{
    entries_[end_]->copy(p);

    end_++; 
    if (end_ == num_entries_)
    {
        end_ = 0;
    }
    size_++;
}

/* get the end element */
Packet *FIFO::end()
{
    if (empty())
    {
        return NULL;
    }

    int end = end_ - 1;
    if (end < 0)
    {
        end = num_entries_ - 1;
    }
    return entries_[end];
}

/* return true if the FIFO is full */
bool FIFO::full()
{
    return (size_ == num_entries_);
}

/* return true if the FIFO is empty */
bool FIFO::empty()
{
    return (size_ == 0);
}

/* print the content of this FIFO for debug */
void FIFO::debug_print()
{
    cout << "FIFO: ";
    for (int i = 0, index = front_; i < size_; i++)
    {
        entries_[index++]->debug_print();
        cout << " ";
        if (index == num_entries_)
        {
            index = 0;
        }
    }
    cout <<endl;
}
