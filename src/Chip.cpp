#include "includes.h"

/* constructor */
Chip::Chip(char *memory_filename)
{
    controller_ = new Controller();

    gb_ = new GB;

    array_ = new Array;

    mem_ = new Memory(memory_filename);
}

/* destructor */
Chip::~Chip()
{
    delete controller_;
    delete gb_;
    delete array_;
    delete mem_;
}

/* get global buffer */
GB *Chip::get_gb()
{
    return gb_;
}

/* get array */
Array *Chip::get_array()
{
    return array_;
}

/* map the NOC IDs */
void Chip::map(NOC_TYPE noc_type, char *filename)
{
    ifstream file(filename);
    string line;

    for (int i = 0; i < ARRAY_ROWS; i++)
    {
        getline(file, line);
        char *token = strtok(const_cast<char*>(line.c_str()), " ");
        int row = atoi(token);

        array_->get_row_MC(noc_type, i)->set_id(row);

        token = strtok(NULL, " "); 
        for (int j = 0; j < ARRAY_COLUMNS; j++)
        {
            token = strtok(NULL, " ");
            int col = atoi(token);

            array_->get_row_MC(noc_type, i)->get_column_MC(j)->set_id(col);
        }
    }
    file.close();
}

/* configure the global buffer */
void Chip::configure_gb(int num_ifmap_banks, int num_filter_banks, int num_psum_banks)
{
    gb_->configure(num_ifmap_banks, num_filter_banks, num_psum_banks);
}

/* configure the PEs */
void Chip::configure_pes()
{
    for (int i = 0; i < ARRAY_ROWS; i++)
    {
        for (int j = 0; j < ARRAY_COLUMNS; j++)
        {
            MC *row = array_->get_row_MC(NOC_TYPE_FILTER_IN, i);
            MC *col = row->get_column_MC(j);
            if ((row->get_id() != -1) && (col->get_id() != -1))
            {
                array_->get_PE(i, j)->set_active(true);
            }
            else
            {
                array_->get_PE(i, j)->set_active(false);
            }
        }
    }

    int prev_row_id = -1;
    int set_width = 0;
    for (int i = 0; i < ARRAY_ROWS; i++)
    {
        if (array_->get_row_MC(NOC_TYPE_PSUM_OUT, i)->get_id() != prev_row_id)
        {
            if (array_->get_row_MC(NOC_TYPE_PSUM_OUT, i)->get_id() != -1)
            {
                set_width = 0;
                for (int j = 0; j < ARRAY_COLUMNS; j++)
                {
                    if (array_->get_row_MC(NOC_TYPE_PSUM_OUT, i)->get_column_MC(j)->get_id() != -1)
                    {
                        set_width++;
                    }
                }
            }
            prev_row_id = array_->get_row_MC(NOC_TYPE_PSUM_OUT, i)->get_id();
        }

        bool top_of_set = false;
        bool bottom_of_set = false;
        if (array_->get_row_MC(NOC_TYPE_PSUM_OUT, i)->get_id() != -1)
        {
            if (i == 0)
            {
                top_of_set = true;
            }
            else if (array_->get_row_MC(NOC_TYPE_PSUM_OUT, i)->get_id() != array_->get_row_MC(NOC_TYPE_PSUM_OUT, i-1)->get_id())
            {
                top_of_set = true;
            }

            if (i == ARRAY_ROWS-1)
            {
                bottom_of_set = true;
            }
            else if (array_->get_row_MC(NOC_TYPE_PSUM_OUT, i)->get_id() != array_->get_row_MC(NOC_TYPE_PSUM_OUT, i+1)->get_id())
            {
                bottom_of_set = true;
            }
        }

        if (top_of_set == true)
        {
            for (int j = 0; j < ARRAY_COLUMNS; j++)
            {
                if (j < set_width)
                {
                    array_->get_PE(i, j)->set_is_top_of_set(true);
                }
                else
                {
                    array_->get_PE(i, j)->set_is_top_of_set(false);
                }
            }
        }
        else
        {
            for (int j = 0; j < ARRAY_COLUMNS; j++)
            {
                array_->get_PE(i, j)->set_is_top_of_set(false);
            }
        }

        if (bottom_of_set == true)
        {
            for (int j = 0; j < ARRAY_COLUMNS; j++)
            {
                if (j < set_width)
                {
                    array_->get_PE(i, j)->set_is_bottom_of_set(true);
                }
                else
                {
                    array_->get_PE(i, j)->set_is_bottom_of_set(false);
                }
            }
        }
        else
        {
            for (int j = 0; j < ARRAY_COLUMNS; j++)
            {
                array_->get_PE(i, j)->set_is_bottom_of_set(false);
            }
        }
    }
}

/* open instruction file */
void Chip::open_instruction_file(char *filename)
{
    controller_->open_file(filename);
}

/* run one clock */
bool Chip::tick()
{
    bool issued = false;

    // GB psum out FIFO -> memory

    if (gb_->psum_fifo_out()->empty() == false)
    {
        Packet *p = gb_->psum_fifo_out()->front();
        mem_->write(p->get_mem_addr(), p);
        gb_->psum_fifo_out()->pop();
    }

    // GB -> GB psum out FIFO

    if (!issued)
    {
        issued = controller_->issue_gb_st(gb_);
    }

    // array psum out FIFO -> GB

    for (int i = 0; i < ARRAY_PSUM_FIFO_OUT_BW; i++)
    {
        if (array_->psum_fifo_out()->empty())
        {
            break;
        }

        Packet *p = array_->psum_fifo_out()->front();
        gb_->write(p->get_gb_region(), p->get_gb_bank(), p->get_gb_entry(), p->get_gb_offset(), p);
        array_->psum_fifo_out()->pop();
    }

    // PE psum out FIFO -> array psum out FIFO

    if (!issued)
    {
        issued = controller_->issue_pe_st(array_);
    }

    // PE ifmap in FIFO -> PE, PE filter in FIFO -> PE, PE psum in FIFO -> PE, PE -> PE psum out FIFO

    for (int i = 0; i < ARRAY_ROWS; i++)
    {
        for (int j = 0; j < ARRAY_COLUMNS; j++)
        {
            if (array_->get_PE(i, j)->get_active())
            {
                array_->get_PE(i, j)->tick();
            }
        }
    }

    // array ifmap in FIFO -> PE ifmap in FIFO, array filter in FIFO -> PE filter in FIFO, array psum in FIFO -> PE psum in FIFO

    for (int i = 0; i < ARRAY_IFMAP_FIFO_IN_BW; i++)
    {
        if (array_->ifmap_fifo_in()->empty())
        {
            break;
        }

        Packet *p = array_->ifmap_fifo_in()->front();
        int row_id = p->get_row_id();
        int col_id = p->get_col_id();

        if (array_->multicast_ready(NOC_TYPE_IFMAP_IN, row_id, col_id))
        {
            array_->multicast(NOC_TYPE_IFMAP_IN, p, row_id, col_id);
            array_->ifmap_fifo_in()->pop();
        }
        else
        {
            break;
        }
    }
    for (int i = 0; i < ARRAY_FILTER_FIFO_IN_BW; i++)
    {
        if (array_->filter_fifo_in()->empty())
        {
            break;
        }

        Packet *p = array_->filter_fifo_in()->front();
        int row_id = p->get_row_id();
        int col_id = p->get_col_id();

        if (array_->multicast_ready(NOC_TYPE_FILTER_IN, row_id, col_id))
        {
            array_->multicast(NOC_TYPE_FILTER_IN, p, row_id, col_id);
            array_->filter_fifo_in()->pop();
        }
        else
        {
            break;
        }
    }
    for (int i = 0; i < ARRAY_PSUM_FIFO_IN_BW; i++)
    {
        if (array_->psum_fifo_in()->empty())
        {
            break;
        }
        Packet *p = array_->psum_fifo_in()->front();

        int row_id = p->get_row_id();
        int col_id = p->get_col_id();

        if (array_->multicast_ready(NOC_TYPE_PSUM_IN, row_id, col_id))
        {
            array_->multicast(NOC_TYPE_PSUM_IN, p, row_id, col_id);
            array_->psum_fifo_in()->pop();
        }
        else
        {
            break;
        }
    }

    // GB -> array ifmap in FIFO, GB -> array filter in FIFO, GB -> array psum in FIFO

    if (!issued)
    {
        issued = controller_->issue_pe_ld(gb_, array_);
    }

    // GB ifmap in FIFO - > GB, GB filter in FIFO -> GB

    if (gb_->ifmap_fifo_in()->empty() == false)
    {
        Packet *p = gb_->ifmap_fifo_in()->front();
        gb_->write(p->get_gb_region(), p->get_gb_bank(), p->get_gb_entry(), p);
        gb_->ifmap_fifo_in()->pop();
    }
    if (gb_->filter_fifo_in()->empty() == false)
    {
        Packet *p = gb_->filter_fifo_in()->front();
        gb_->write(p->get_gb_region(), p->get_gb_bank(), p->get_gb_entry(), p);
        gb_->filter_fifo_in()->pop();
    }

    // memory -> GB ifmap in FIFO, memory -> GB filter in FIFO

    if (!issued)
    {
        issued = controller_->issue_gb_ld(mem_, gb_);
    }

    // other instructions

    if (!issued)
    {
        issued = controller_->issue_nop();
    }
    if (!issued)
    {
        issued = controller_->issue_ld_reg();
    }
    if (!issued)
    {
        issued = controller_->issue_pe_compute(array_);
    }
    if (!issued)
    {
        issued = controller_->issue_pe_acc(array_);
    }
    if (!issued)
    {
        issued = controller_->issue_up(array_);
    }

    return controller_->read_instruction();
}
