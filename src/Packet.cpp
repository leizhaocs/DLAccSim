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
Packet::Packet(int size)
{
    size_ = size;
    data_ = (unsigned char *)calloc(1, size_);

    row_id_ = -1;
    col_id_ = -1;

    gb_region_ = GB_NONE;
    gb_bank_ = -1;
    gb_entry_ = -1;
    gb_offset_ = -1;

    mem_addr_ = -1;
}

/* destructor */
Packet::~Packet()
{
    free(data_);
}

/* copy */
void Packet::copy(Packet *p)
{
    assert(size_ == p->get_size());

    for (int i = 0; i < size_; i++)
    {
        data_[i] = p->data(i);
    }

    row_id_ = p->get_row_id();
    col_id_ = p->get_col_id();
    gb_region_ = p->get_gb_region();
    gb_bank_ = p->get_gb_bank();
    gb_entry_ = p->get_gb_entry();
    gb_offset_ = p->get_gb_offset();
    mem_addr_ = p->get_mem_addr();
}

/* copy from/into an offset of the data */
void Packet::copy(int offset, Packet *p)
{
    if (size_ > p->get_size())
    {
        assert(size_ >= (p->get_size() + offset));
        for (int i = 0; i < p->get_size(); i++)
        {
            data_[i+offset] = p->data(i);
        }
    }
    else if (size_ < p->get_size())
    {
        assert((size_ + offset) <= p->get_size());
        for (int i = 0; i < size_; i++)
        {
            data_[i] = p->data(i+offset);
        }
    }
    else
    {
        copy(p);
    }

    row_id_ = p->get_row_id();
    col_id_ = p->get_col_id();
    gb_region_ = p->get_gb_region();
    gb_bank_ = p->get_gb_bank();
    gb_entry_ = p->get_gb_entry();
    gb_offset_ = p->get_gb_offset();
    mem_addr_ = p->get_mem_addr();
}

/* 0: select p1, 1: select p2 */
void Packet::mux2(Packet *p1, Packet *p2, int sel)
{
    if (sel == 0)
    {
        copy(p1);
    }
    else if (sel == 1)
    {
        copy(p2);
    }
}

/* 0: select p1, 1: select p2, 2: p3 */
void Packet::mux3(Packet *p1, Packet *p2, Packet *p3, int sel)
{
    if (sel == 0)
    {
        copy(p1);
    }
    else if (sel == 1)
    {
        copy(p2);
    }
    else if (sel == 2)
    {
        copy(p3);
    }
}

/* add p1 and p2 */
void Packet::add(Packet *p1, Packet *p2)
{
    assert(size_ == p1->get_size());
    assert(size_ == p2->get_size());

    (*((unsigned int*)data_)) = (*((unsigned int*)(p1->get_data()))) + (*((unsigned int*)(p2->get_data())));

    row_id_ = p1->get_row_id();
    col_id_ = p1->get_col_id();
    gb_region_ = p1->get_gb_region();
    gb_bank_ = p1->get_gb_bank();
    gb_entry_ = p1->get_gb_entry();
    gb_offset_ = p1->get_gb_offset();
    mem_addr_ = p1->get_mem_addr();
}

/* multiply p1 and p2 (stage 1) */
void Packet::mult1(Packet *p1, Packet *p2)
{
    assert(p1->get_size() == p2->get_size());
    assert(size_ == 2*(p2->get_size()));

    (*((unsigned int*)data_)) = (*((unsigned int*)(p1->get_data()))) * (*((unsigned int*)(p2->get_data())));

    row_id_ = p1->get_row_id();
    col_id_ = p1->get_col_id();
    gb_region_ = p1->get_gb_region();
    gb_bank_ = p1->get_gb_bank();
    gb_entry_ = p1->get_gb_entry();
    gb_offset_ = p1->get_gb_offset();
    mem_addr_ = p1->get_mem_addr();
}

/* multiply p1 and p2 (stage 2) */
void Packet::mult2(Packet *p)
{
    assert(size_ == p->get_size());

    for (int i = 0; i < size_; i++)
    {
        data_[i] = p->data(i);
    }

    row_id_ = p->get_row_id();
    col_id_ = p->get_col_id();
    gb_region_ = p->get_gb_region();
    gb_bank_ = p->get_gb_bank();
    gb_entry_ = p->get_gb_entry();
    gb_offset_ = p->get_gb_offset();
    mem_addr_ = p->get_mem_addr();
}

/* quantize p */
void Packet::quantize(Packet *p)
{
    assert(2*size_ == p->get_size());

    // FIXME perform quantization
    for (int i = 0; i < size_; i++)
    {
        data_[i] = p->data(i);
    }

    row_id_ = p->get_row_id();
    col_id_ = p->get_col_id();
    gb_region_ = p->get_gb_region();
    gb_bank_ = p->get_gb_bank();
    gb_entry_ = p->get_gb_entry();
    gb_offset_ = p->get_gb_offset();
    mem_addr_ = p->get_mem_addr();
}

/* get a specific byte of the data */
unsigned char Packet::data(int byte)
{
    return data_[byte];
}

/* set a specific byte of the data */
void Packet::data(int byte, unsigned char c)
{
    data_[byte] = c;
}

/* getters */
int Packet::get_size()
{
    return size_;
}
unsigned char *Packet::get_data()
{
    return data_;
}
int Packet::get_row_id()
{
    return row_id_;
}
int Packet::get_col_id()
{
    return col_id_;
}
GB_REGION Packet::get_gb_region()
{
    return gb_region_;
}
int Packet::get_gb_bank()
{
    return gb_bank_;
}
int Packet::get_gb_entry()
{
    return gb_entry_;
}
int Packet::get_gb_offset()
{
    return gb_offset_;
}
int Packet::get_mem_addr()
{
    return mem_addr_;
}

/* setters */
void Packet::set_size(int size)
{
    size_ = size;
}
void Packet::set_data(unsigned char *data)
{
    data_ = data;
}
void Packet::set_row_id(int row_id)
{
    row_id_ = row_id;
}
void Packet::set_col_id(int col_id)
{
    col_id_ = col_id;
}
void Packet::set_gb_region(GB_REGION gb_region)
{
    gb_region_ = gb_region;
}
void Packet::set_gb_bank(int gb_bank)
{
    gb_bank_ = gb_bank;
}
void Packet::set_gb_entry(int gb_entry)
{
    gb_entry_ = gb_entry;
}
void Packet::set_gb_offset(int gb_offset)
{
    gb_offset_ = gb_offset;
}
void Packet::set_mem_addr(int mem_addr)
{
    mem_addr_ = mem_addr;
}

/* print the content of this packet for debug */
void Packet::debug_print()
{
    int num_words = size_ / PE_DATA_WIDTH;
    cout << "[";
    for (int i = 0; i < num_words; i++)
    {
        // FIXME add support when PE_DATA_WIDTH is not 2 bytes
        cout << *((unsigned short*)&(data_[i*PE_DATA_WIDTH]));
        if (i != num_words-1)
        {
            cout << " ";
        }
    }
    cout << "]";
}
