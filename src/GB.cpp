#include "includes.h"

/* constructor */
Bank::Bank()
{
    for (int i = 0; i < GB_BANK_ENTRIES; i++)
    {
        entries.push_back(new Packet(GB_DATA_WIDTH));
    }
}

/* destructor */
Bank::~Bank()
{
    for (int i = 0; i < GB_BANK_ENTRIES; i++)
    {
        delete entries[i];
    }
}

/* read an entire entry */
void Bank::read(int entry, Packet *p)
{
    p->copy(entries[entry]);
}

/* read a block of an entry */
void Bank::read(int entry, int offset, Packet *p)
{
    p->copy(offset*PE_DATA_WIDTH, entries[entry]);
}

/* write an entire entry */
void Bank::write(int entry, Packet *p)
{
    entries[entry]->copy(p);
}

/* write a block of an entry */
void Bank::write(int entry, int offset, Packet *p)
{
    entries[entry]->copy(offset*PE_DATA_WIDTH, p);
}

/* print the content of one entry for debug */
void Bank::debug_print_entry(int entry)
{
    entries[entry]->debug_print();
}

/////////////////////////////////////////////////////////////////////

/* constructor */
GB::GB()
{
    for (int i = 0; i < GB_BANKS; i++)
    {
        banks_.push_back(new Bank);
    }

    ifmap_fifo_in_ = new FIFO(GB_IFMAP_FIFO_IN_SIZE, GB_DATA_WIDTH);
    filter_fifo_in_ = new FIFO(GB_FILTER_FIFO_IN_SIZE, GB_DATA_WIDTH);
    psum_fifo_out_ = new FIFO(GB_PSUM_FIFO_OUT_SIZE, GB_DATA_WIDTH);

    num_ifmap_banks_ = 12;
    ifmap_bank_begin_ = 0;
    ifmap_bank_end_ = 11;
    num_filter_banks_ = 2;
    filter_bank_begin_ = 24;
    filter_bank_end_ = 25;
    num_psum_banks_ = 12;
    psum_bank_begin_ = 12;
    psum_bank_end_ = 23;
}

/* destructor */
GB::~GB()
{
    for (int i = 0; i < GB_BANKS; i++)
    {
        delete banks_[i];
    }
    delete ifmap_fifo_in_;
    delete filter_fifo_in_;
    delete psum_fifo_out_;
}

/* configure global buffer */
void GB::configure(int num_ifmap_banks, int num_filter_banks, int num_psum_banks)
{
    assert(num_ifmap_banks + num_filter_banks + num_psum_banks <= GB_BANKS);

    num_ifmap_banks_ = num_ifmap_banks;
    ifmap_bank_begin_ = 0;
    ifmap_bank_end_ = ifmap_bank_begin_ + num_ifmap_banks - 1;
    num_filter_banks_ = num_filter_banks;
    filter_bank_begin_ = num_ifmap_banks;
    filter_bank_end_ = filter_bank_begin_ + num_filter_banks - 1;
    num_psum_banks_ = num_psum_banks;
    psum_bank_begin_ = num_ifmap_banks + num_filter_banks;
    psum_bank_end_ = psum_bank_begin_ + num_psum_banks - 1;
}

/* get FIFOs */
FIFO *GB::ifmap_fifo_in()
{
    return ifmap_fifo_in_;
}
FIFO *GB::filter_fifo_in()
{
    return filter_fifo_in_;
}
FIFO *GB::psum_fifo_out()
{
    return psum_fifo_out_;
}

/* read an entire entry */
void GB::read(GB_REGION region, int bank, int entry, Packet *p)
{
    assert(region != GB_NONE);

    if (region == GB_IFMAP)
    {
        banks_[bank+ifmap_bank_begin_]->read(entry, p);
    }
    else if (region == GB_FILTER)
    {
        banks_[bank+filter_bank_begin_]->read(entry, p);
    }
    else if (region == GB_PSUM)
    {
        banks_[bank+psum_bank_begin_]->read(entry, p);
    }
}

/* read a block of an entry */
void GB::read(GB_REGION region, int bank, int entry, int offset, Packet *p)
{
    assert(region != GB_NONE);

    if (region == GB_IFMAP)
    {
        banks_[bank+ifmap_bank_begin_]->read(entry, offset, p);
    }
    else if (region == GB_FILTER)
    {
        banks_[bank+filter_bank_begin_]->read(entry, offset, p);
    }
    else if (region == GB_PSUM)
    {
        banks_[bank+psum_bank_begin_]->read(entry, offset, p);
    }
}

/* write an entire entry */
void GB::write(GB_REGION region, int bank, int entry, Packet *p)
{
    assert(region != GB_NONE);

    if (region == GB_IFMAP)
    {
        banks_[bank+ifmap_bank_begin_]->write(entry, p);
    }
    else if (region == GB_FILTER)
    {
        banks_[bank+filter_bank_begin_]->write(entry, p);
    }
    else if (region == GB_PSUM)
    {
        banks_[bank+psum_bank_begin_]->write(entry, p);
    }
}

/* write a block of an entry */
void GB::write(GB_REGION region, int bank, int entry, int offset, Packet *p)
{
    assert(region != GB_NONE);

    if (region == GB_IFMAP)
    {
        banks_[bank+ifmap_bank_begin_]->write(entry, offset, p);
    }
    else if (region == GB_FILTER)
    {
        banks_[bank+filter_bank_begin_]->write(entry, offset, p);
    }
    else if (region == GB_PSUM)
    {
        banks_[bank+psum_bank_begin_]->write(entry, offset, p);
    }
}

/* print the content of one bank for debug */
void GB::debug_print_bank(GB_REGION region, int bank)
{
    assert(region != GB_NONE);

    if (region == GB_IFMAP)
    {
        for (int i = 0; i < GB_BANK_ENTRIES; i++)
        {
            banks_[bank+ifmap_bank_begin_]->debug_print_entry(i);
            cout << endl;
        }
    }
    else if (region == GB_FILTER)
    {
        for (int i = 0; i < GB_BANK_ENTRIES; i++)
        {
            banks_[bank+filter_bank_begin_]->debug_print_entry(i);
            cout << endl;
        }
    }
    else if (region == GB_PSUM)
    {
        for (int i = 0; i < GB_BANK_ENTRIES; i++)
        {
            banks_[bank+psum_bank_begin_]->debug_print_entry(i);
            cout << endl;
        }
    }
}

/* print the content of FIFOs for debug */
void GB::debug_print_fifos()
{
    cout << "global buffer ifmap fifo in : ";
    ifmap_fifo_in_->debug_print();
    cout << "global buffer filter fifo in: ";
    filter_fifo_in_->debug_print();
    cout << "global buffer psum fifo out : ";
    psum_fifo_out_->debug_print();
}
