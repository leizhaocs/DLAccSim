#include "includes.h"

/* constructor */
PE::PE(int row, int col)
{
    row_ = row;
    col_ = col;

    active_ = false;
    is_bottom_of_set_ = false;

    instQueue_ = new InstQueue(PE_INST_Q_SIZE);

    ifmap_spad_ = new SPad(PE_IFMAP_SPAD_ENTRIES);
    filter_spad_ = new SPad(PE_FILTER_SPAD_ENTRIES);
    psum_spad_ = new SPad(PE_PSUM_SPAD_ENTRIES);

    ifmap_fifo_in_ = new FIFO(PE_IFMAP_FIFO_IN_SIZE, PE_DATA_WIDTH);
    filter_fifo_in_ = new FIFO(PE_FILTER_FIFO_IN_SIZE, PE_DATA_WIDTH);
    psum_fifo_in_ = new FIFO(PE_PSUM_FIFO_IN_SIZE, PE_DATA_WIDTH);
    psum_fifo_out_ = new FIFO(PE_PSUM_FIFO_OUT_SIZE, PE_DATA_WIDTH);

    packet_ifmap_read_ = new Packet(PE_DATA_WIDTH);
    packet_filter_read_ = new Packet(PE_DATA_WIDTH);
    packet_psum_read_ = new Packet(PE_DATA_WIDTH);
    packet_mult1_ = new Packet(2*PE_DATA_WIDTH);
    packet_mult2_ = new Packet(2*PE_DATA_WIDTH);
    packet_quant_ = new Packet(PE_DATA_WIDTH);
    packet_add_mux_ = new Packet(PE_DATA_WIDTH);
    packet_psum_read_mux_ = new Packet(PE_DATA_WIDTH);
    packet_add_ = new Packet(PE_DATA_WIDTH);
    packet_zero_ = new Packet(PE_DATA_WIDTH);

    ifmap_read_en_1_ = 0;
    ifmap_read_addr_1_ = -1;
    ifmap_write_en_1_ = 0;
    ifmap_write_addr_1_ = -1;
    filter_read_en_1_ = 0;
    filter_read_addr_1_ = -1;
    filter_write_en_1_ = 0;
    filter_write_addr_1_ = -1;
    psum_read_en_1_ = 0;
    psum_read_addr_1_ = -1;
    psum_write_en_1_ = 0;
    psum_write_en_2_ = 0;
    psum_write_en_3_ = 0;
    psum_write_en_4_ = 0;
    psum_write_addr_1_ = -1;
    psum_write_addr_2_ = -1;
    psum_write_addr_3_ = -1;
    psum_write_addr_4_ = -1;
    add_mux_1_ = 0;
    add_mux_2_ = 0;
    add_mux_3_ = 0;
    psum_read_mux_1_ = 0;
    psum_read_mux_2_ = 0;
    send_psum_out_1_ = 0;
    send_psum_out_2_ = 0;
    send_psum_out_3_ = 0;
    send_psum_out_4_ = 0;
}

/* destructor */
PE::~PE()
{
    delete instQueue_;

    delete ifmap_spad_;
    delete filter_spad_;
    delete psum_spad_;

    delete ifmap_fifo_in_;
    delete filter_fifo_in_;
    delete psum_fifo_in_;
    delete psum_fifo_out_;

    delete packet_ifmap_read_;
    delete packet_filter_read_;
    delete packet_psum_read_;
    delete packet_mult1_;
    delete packet_mult2_;
    delete packet_quant_;
    delete packet_add_mux_;
    delete packet_psum_read_mux_;
    delete packet_add_;
    delete packet_zero_;
}

/* get active */
bool PE::get_active()
{
    return active_;
}

/* set active */
void PE::set_active(bool active)
{
    active_ = active;
}

/* get is_bottom_of_set_ */
bool PE::get_is_bottom_of_set()
{
    return is_bottom_of_set_;
}

/* set is_bottom_of_set_ */
void PE::set_is_bottom_of_set(bool is_bottom_of_set)
{
    is_bottom_of_set_ = is_bottom_of_set;
}

/* get instruction queue */
InstQueue *PE::instQueue()
{
    return instQueue_;
}

/* get FIFOs */
FIFO *PE::ifmap_fifo_in()
{
    return ifmap_fifo_in_;
}
FIFO *PE::filter_fifo_in()
{
    return filter_fifo_in_;
}
FIFO *PE::psum_fifo_in()
{
    return psum_fifo_in_;
}
FIFO *PE::psum_fifo_out()
{
    return psum_fifo_out_;
}

/* issue an instruction from the instruction queue */
void PE::issue_inst()
{
    if (instQueue_->empty())
    {
        ifmap_read_en_1_ = 0;
        ifmap_read_addr_1_ = -1;
        ifmap_write_en_1_ = 0;
        ifmap_write_addr_1_ = -1;
        filter_read_en_1_ = 0;
        filter_read_addr_1_ = -1;
        filter_write_en_1_ = 0;
        filter_write_addr_1_ = -1;
        psum_read_en_1_ = 0;
        psum_read_addr_1_ = -1;
        psum_write_en_1_ = 0;
        psum_write_addr_1_ = -1;
        add_mux_1_ = 0;
        psum_read_mux_1_ = 0;
        send_psum_out_1_ = 0;
        return;
    }

    Instruction *inst = instQueue_->front();
    ifmap_read_en_1_ = inst->ifmap_read_en_;
    ifmap_read_addr_1_ = inst->ifmap_read_addr_;
    ifmap_write_en_1_ = inst->ifmap_write_en_;
    ifmap_write_addr_1_ = inst->ifmap_write_addr_;
    filter_read_en_1_ = inst->filter_read_en_;
    filter_read_addr_1_ = inst->filter_read_addr_;
    filter_write_en_1_ = inst->filter_write_en_;
    filter_write_addr_1_ = inst->filter_write_addr_;
    psum_read_en_1_ = inst->psum_read_en_;
    psum_read_addr_1_ = inst->psum_read_addr_;
    psum_write_en_1_ = inst->psum_write_en_;
    psum_write_addr_1_ = inst->psum_write_addr_;
    add_mux_1_ = inst->add_mux_;
    psum_read_mux_1_ = inst->psum_read_mux_;
    send_psum_out_1_ = inst->send_psum_out_;
    instQueue_->pop();
}

/* run one clock */
void PE::tick()
{
    // stage 4: write result

    if (send_psum_out_4_)
    {
        if (psum_fifo_out_->full())
        {
            return;
        }
        psum_fifo_out_->push(packet_add_);
    }
    if (psum_write_en_4_)
    {
        psum_spad_->write(psum_write_addr_4_, packet_add_);
    }

    // stage 3: second half of execution

    packet_mult2_->mult2(packet_mult1_);
    packet_quant_->quantize(packet_mult2_);
    if (add_mux_3_ == 1)
    {
        if (psum_fifo_in_->empty())
        {
            psum_write_en_4_ = 0;
            psum_write_addr_4_ = -1;
            send_psum_out_4_ = 0;
            return;
        }
    }
    packet_add_mux_->select(packet_quant_, psum_fifo_in_->front(), add_mux_3_);
    if (add_mux_3_ == 1)
    {
        psum_fifo_in_->pop();
    }
    packet_add_->add(packet_add_mux_, packet_psum_read_mux_);

    psum_write_en_4_ = psum_write_en_3_;
    psum_write_addr_4_ = psum_write_addr_3_;
    send_psum_out_4_ = send_psum_out_3_;

    // stage 2: first half of execution, reset accumulation

    packet_mult1_->mult1(packet_ifmap_read_, packet_filter_read_);
    packet_psum_read_mux_->select(packet_zero_, packet_psum_read_, psum_read_mux_2_);

    psum_write_en_3_ = psum_write_en_2_;
    psum_write_addr_3_ = psum_write_addr_2_;
    add_mux_3_ = add_mux_2_;
    send_psum_out_3_ = send_psum_out_2_;

    // stage 1: read/write ifmap_spad, read/write filter_spad, read psum_spad

    if (ifmap_read_en_1_)
    {
        ifmap_spad_->read(ifmap_read_addr_1_, packet_ifmap_read_);
    }
    if (ifmap_write_en_1_)
    {
        if (ifmap_fifo_in_->empty())
        {
            psum_write_en_2_ = 0;
            psum_write_addr_2_ = -1;
            add_mux_2_ = 0;
            psum_read_mux_2_ = 0;
            send_psum_out_2_ = 0;
            return;
        }
        else
        {
            ifmap_spad_->write(ifmap_write_addr_1_, ifmap_fifo_in_->front());
            ifmap_fifo_in_->pop();
        }
    }
    if (filter_read_en_1_)
    {
        filter_spad_->read(filter_read_addr_1_, packet_filter_read_);
    }
    if (filter_write_en_1_)
    {
        if (filter_fifo_in_->empty())
        {
            psum_write_en_2_ = 0;
            psum_write_addr_2_ = -1;
            add_mux_2_ = 0;
            psum_read_mux_2_ = 0;
            send_psum_out_2_ = 0;
            return;
        }
        else
        {
            filter_spad_->write(filter_write_addr_1_, filter_fifo_in_->front());
            filter_fifo_in_->pop();
        }
    }
    if (psum_read_en_1_)
    {
        if (psum_write_en_2_ && (psum_read_addr_1_ == psum_write_addr_2_))
        {
            psum_write_en_2_ = 0;
            psum_write_addr_2_ = -1;
            add_mux_2_ = 0;
            psum_read_mux_2_ = 0;
            send_psum_out_2_ = 0;
            return;
        }
        else if (psum_write_en_3_ && (psum_read_addr_1_ == psum_write_addr_3_))
        {
            psum_write_en_2_ = 0;
            psum_write_addr_2_ = -1;
            add_mux_2_ = 0;
            psum_read_mux_2_ = 0;
            send_psum_out_2_ = 0;
            return;
        }
        else if (psum_write_en_4_ && (psum_read_addr_1_ == psum_write_addr_4_))
        {
            packet_psum_read_->copy(packet_add_);
        }
        else
        {
            psum_spad_->read(psum_read_addr_1_, packet_psum_read_);
        }
    }

    psum_write_en_2_ = psum_write_en_1_;
    psum_write_addr_2_ = psum_write_addr_1_;
    add_mux_2_ = add_mux_1_;
    psum_read_mux_2_ = psum_read_mux_1_;
    send_psum_out_2_ = send_psum_out_1_;

    issue_inst();
}

/* print an entry of all scratchpads for debug */
void PE::debug_print_spads(int num_entries)
{
    Packet *p = new Packet(PE_DATA_WIDTH);
    cout << "pe ifmap spad      : ";
    for (int i = 0; i < num_entries; i++)
    {
        if (i < PE_IFMAP_SPAD_ENTRIES)
        {
            ifmap_spad_->read(i, p);
            p->debug_print();
            cout << " ";
        }
    }
    cout << endl;
    cout << "pe filter spad     : ";
    for (int i = 0; i < num_entries; i++)
    {
        if (i < PE_FILTER_SPAD_ENTRIES)
        {
            filter_spad_->read(i, p);
            p->debug_print();
            cout << " ";
        }
    }
    cout << endl;
    cout << "pe psum spad       : ";
    for (int i = 0; i < num_entries; i++)
    {
        if (i < PE_PSUM_SPAD_ENTRIES)
        {
            psum_spad_->read(i, p);
            p->debug_print();
            cout << " ";
        }
    }
    cout << endl;
    delete p;
}

/* print the content of FIFOs for debug */
void PE::debug_print_fifos()
{
    cout << "PE ifmap fifo in : ";
    ifmap_fifo_in_->debug_print();
    cout << "PE filter fifo in: ";
    filter_fifo_in_->debug_print();
    cout << "PE psum fifo in  : ";
    psum_fifo_in_->debug_print();
    cout << "PE psum fifo out : ";
    psum_fifo_out_->debug_print();
}
