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

#ifndef GB_H
#define GB_H

#include "includes.h"

/* a bank in global buffer */
class Bank
{
public:
    /* constructor */
    Bank();

    /* destructor */
    ~Bank();

    /* read an entire entry */
    void read(int entry, Packet *p);

    /* read a block of an entry */
    void read(int entry, int offset, Packet *p);

    /* write an entire entry */
    void write(int entry, Packet *p);

    /* write a block of an entry */
    void write(int entry, int offset, Packet *p);

    /* print the content of one entry for debug */
    void debug_print_entry(int entry);

private:
    vector<Packet *> entries;  // the actual data, GB_BANK_ENTRIES entries, each entry has GB_DATA_WIDTH bytes
};

/////////////////////////////////////////////////////////////////////

/* which region in global buffer */
enum GB_REGION
{
    GB_NONE,    // invalid region

    GB_IFMAP,   // ifmap region
    GB_FILTER,  // filter region
    GB_PSUM     // psum region
};

/* global buffer */
class GB
{
public:
    /* constructor */
    GB();

    /* destructor */
    ~GB();

    /* configure global buffer */
    void configure(int num_ifmap_banks, int num_filter_banks, int num_psum_banks);

    /* get FIFOs */
    FIFO *ifmap_fifo_in();
    FIFO *filter_fifo_in();
    FIFO *psum_fifo_out();

    /* read an entire entry */
    void read(GB_REGION region, int bank, int entry, Packet *p);

    /* read a block of an entry */
    void read(GB_REGION region, int bank, int entry, int offset, Packet *p);

    /* write an entire entry */
    void write(GB_REGION region, int bank, int entry, Packet *p);

    /* write a block of an entry */
    void write(GB_REGION region, int bank, int entry, int offset, Packet *p);

    /* print the content of one bank for debug */
    void debug_print_bank(GB_REGION region, int bank);

    /* print the content of FIFOs for debug */
    void debug_print_fifos();

private:
    vector<Bank *> banks_;      // all banks

    FIFO *ifmap_fifo_in_;       // ifmap in FIFO
    FIFO *filter_fifo_in_;      // filter in FIFO
    FIFO *psum_fifo_out_;       // psum out FIFO

    int num_ifmap_banks_;       // number of banks assigned to ifmap
    int ifmap_bank_begin_;      // the first bank ID assigned to ifmap
    int ifmap_bank_end_;        // the last bank ID assigned to ifmap
    int num_filter_banks_;      // number of banks assigned to filter
    int filter_bank_begin_;     // the first bank ID assigned to filter
    int filter_bank_end_;       // the last bank ID assigned to filter
    int num_psum_banks_;        // number of banks assigned to psum
    int psum_bank_begin_;       // the first bank ID assigned to psum
    int psum_bank_end_;         // the last bank ID assigned to psum
};

#endif
