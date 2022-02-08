#include "includes.h"

/* constructor */
Array::Array()
{
    for (int i = 0; i < ARRAY_ROWS; i++)
    {
        pes_.push_back(vector<PE *>());
        for (int j = 0; j < ARRAY_COLUMNS; j++)
        {
            pes_[i].push_back(new PE(i, j));
        }
    }

    for (int i = 0; i < ARRAY_ROWS; i++)
    {
        row_mcs_ifmap_in_.push_back(new MC(NOC_TYPE_IFMAP_IN, MC_TYPE_ROW, &(pes_[i])));
        row_mcs_filter_in_.push_back(new MC(NOC_TYPE_FILTER_IN, MC_TYPE_ROW, &(pes_[i])));
        row_mcs_psum_in_.push_back(new MC(NOC_TYPE_PSUM_IN, MC_TYPE_ROW, &(pes_[i])));
        row_mcs_psum_out_.push_back(new MC(NOC_TYPE_PSUM_OUT, MC_TYPE_ROW, &(pes_[i])));
    }

    ifmap_fifo_in_ = new FIFO(ARRAY_IFMAP_FIFO_IN_SIZE, PE_DATA_WIDTH);
    filter_fifo_in_ = new FIFO(ARRAY_FILTER_FIFO_IN_SIZE, PE_DATA_WIDTH);
    psum_fifo_in_ = new FIFO(ARRAY_PSUM_FIFO_IN_SIZE, PE_DATA_WIDTH);
    psum_fifo_out_ = new FIFO(ARRAY_PSUM_FIFO_OUT_SIZE, PE_DATA_WIDTH);
}

/* Destructor */
Array::~Array()
{
    for (int i = 0; i < ARRAY_ROWS; i++)
    {
        for (int j = 0; j < ARRAY_COLUMNS; j++)
        {
            delete pes_[i][j];
        }
    }
    for (int i = 0; i < ARRAY_ROWS; i++)
    {
        delete row_mcs_ifmap_in_[i];
        delete row_mcs_filter_in_[i];
        delete row_mcs_psum_in_[i];
        delete row_mcs_psum_out_[i];
    }
    delete ifmap_fifo_in_;
    delete filter_fifo_in_;
    delete psum_fifo_in_;
    delete psum_fifo_out_;
}

/* get a specific PE */
PE *Array::get_PE(int row, int col)
{
    return pes_[row][col];
}

/* get a specific row MC */
MC *Array::get_row_MC(NOC_TYPE noc_type, int row)
{
    if (noc_type == NOC_TYPE_IFMAP_IN)
    {
        return row_mcs_ifmap_in_[row];
    }
    else if (noc_type == NOC_TYPE_FILTER_IN)
    {
        return row_mcs_filter_in_[row];
    }
    else if (noc_type == NOC_TYPE_PSUM_IN)
    {
        return row_mcs_psum_in_[row];
    }
    else if (noc_type == NOC_TYPE_PSUM_OUT)
    {
        return row_mcs_psum_out_[row];
    }
    return NULL;
}

/* get FIFOs */
FIFO *Array::ifmap_fifo_in()
{
    return ifmap_fifo_in_;
}
FIFO *Array::filter_fifo_in()
{
    return filter_fifo_in_;
}
FIFO *Array::psum_fifo_in()
{
    return psum_fifo_in_;
}
FIFO *Array::psum_fifo_out()
{
    return psum_fifo_out_;
}

/* check if all desination PEs are ready to accept new data for a specific NOC, only used for NOC_TYPE_IFMAP_IN, NOC_TYPE_FILTER_IN and NOC_TYPE_PSUM_IN */
bool Array::multicast_ready(NOC_TYPE noc_type, int row_id, int col_id)
{
    if (noc_type == NOC_TYPE_IFMAP_IN)
    {
        for (int i = 0; i < ARRAY_ROWS; i++)
        {
            if (row_mcs_ifmap_in_[i]->multicast_ready(row_id, col_id) == false)
            {
                return false;
            }
        }
        return true;
    }
    else if (noc_type == NOC_TYPE_FILTER_IN)
    {
        for (int i = 0; i < ARRAY_ROWS; i++)
        {
            if (row_mcs_filter_in_[i]->multicast_ready(row_id, col_id) == false)
            {
                return false;
            }
        }
        return true;
    }
    else if (noc_type == NOC_TYPE_PSUM_IN)
    {
        for (int i = 0; i < ARRAY_ROWS; i++)
        {
            if (row_mcs_psum_in_[i]->multicast_ready(row_id, col_id) == false)
            {
                return false;
            }
        }
        return true;
    }
    return false;
}

/* multicast a packet from FIFO to destination PEs through a specific NOC, only used for NOC_TYPE_IFMAP_IN, NOC_TYPE_FILTER_IN and NOC_TYPE_PSUM_IN */
void Array::multicast(NOC_TYPE noc_type, Packet *p, int row_id, int col_id)
{
    if (noc_type == NOC_TYPE_IFMAP_IN)
    {
        for (int i = 0; i < ARRAY_ROWS; i++)
        {
            row_mcs_ifmap_in_[i]->multicast(p, row_id, col_id);
        }
    }
    else if (noc_type == NOC_TYPE_FILTER_IN)
    {
        for (int i = 0; i < ARRAY_ROWS; i++)
        {
            row_mcs_filter_in_[i]->multicast(p, row_id, col_id);
        }
    }
    else if (noc_type == NOC_TYPE_PSUM_IN)
    {
        for (int i = 0; i < ARRAY_ROWS; i++)
        {
            row_mcs_psum_in_[i]->multicast(p, row_id, col_id);
        }
    }
}

/* check if all destination PEs are ready to accept new ld instruction, only used for NOC_TYPE_IFMAP_IN, NOC_TYPE_FILTER_IN and NOC_TYPE_PSUM_IN */
bool Array::issue_ld_ready(NOC_TYPE noc_type, int row_id, int col_id)
{
    if (noc_type == NOC_TYPE_IFMAP_IN)
    {
        for (int i = 0; i < ARRAY_ROWS; i++)
        {
            if (row_mcs_ifmap_in_[i]->issue_ready(row_id, col_id) == false)
            {
                return false;
            }
        }
        return true;
    }
    else if (noc_type == NOC_TYPE_FILTER_IN)
    {
        for (int i = 0; i < ARRAY_ROWS; i++)
        {
            if (row_mcs_filter_in_[i]->issue_ready(row_id, col_id) == false)
            {
                return false;
            }
        }
        return true;
    }
    else if (noc_type == NOC_TYPE_PSUM_IN)
    {
        for (int i = 0; i < ARRAY_ROWS; i++)
        {
            if (row_mcs_psum_in_[i]->issue_ready(row_id, col_id) == false)
            {
                return false;
            }
        }
        return true;
    }
    return false;
}

/* issue a new ld instruction to all destination PEs, only used for NOC_TYPE_IFMAP_IN, NOC_TYPE_FILTER_IN and NOC_TYPE_PSUM_IN */
void Array::issue_ld(NOC_TYPE noc_type, Instruction *inst, int row_id, int col_id)
{
    if (noc_type == NOC_TYPE_IFMAP_IN)
    {
        for (int i = 0; i < ARRAY_ROWS; i++)
        {
            row_mcs_ifmap_in_[i]->issue(inst, row_id, col_id);
        }
    }
    else if (noc_type == NOC_TYPE_FILTER_IN)
    {
        for (int i = 0; i < ARRAY_ROWS; i++)
        {
            row_mcs_filter_in_[i]->issue(inst, row_id, col_id);
        }
    }
    else if (noc_type == NOC_TYPE_PSUM_IN)
    {
        for (int i = 0; i < ARRAY_ROWS; i++)
        {
            row_mcs_psum_in_[i]->issue(inst, row_id, col_id);
        }
    }
}

/* check if all active PEs are ready to accept new compute instruction */
bool Array::issue_compute_ready()
{
    for (int i = 0; i < ARRAY_ROWS; i++)
    {
        for (int j = 0; j < ARRAY_COLUMNS; j++)
        {
            if (pes_[i][j]->get_active())
            {
                if (pes_[i][j]->instQueue()->full())
                {
                    return false;
                }
            }
        }
    }
    return true;
}

/* issue a new compute instruction to all active PEs */
void Array::issue_compute(Instruction *inst)
{
    for (int i = 0; i < ARRAY_ROWS; i++)
    {
        for (int j = 0; j < ARRAY_COLUMNS; j++)
        {
            if (pes_[i][j]->get_active())
            {
                pes_[i][j]->instQueue()->push(inst);
            }
        }
    }
}

/* check if all destination PEs are ready to send data to adjacent PE above it */
bool Array::issue_up_ready()
{
    for (int i = 0; i < ARRAY_ROWS; i++)
    {
        for (int j = 0; j < ARRAY_COLUMNS; j++)
        {
            if (pes_[i][j]->get_active())
            {
                if (pes_[i][j]->get_is_bottom_of_set())
                {
                    if (pes_[i][j]->psum_fifo_out()->empty())
                    {
                        return false;
                    }
                }
                else
                {
                    if (pes_[i][j]->psum_fifo_in()->full())
                    {
                        return false;
                    }
                }
            }
        }
    }
    return true;
}

/* send data to adjacent PE above it */
void Array::issue_up()
{
    for (int i = 0; i < ARRAY_ROWS; i++)
    {
        for (int j = 0; j < ARRAY_COLUMNS; j++)
        {
            if (pes_[i][j]->get_active())
            {
                if (pes_[i][j]->get_is_bottom_of_set() == false)
                {
                    Packet *p = pes_[i+1][j]->psum_fifo_in()->front();
                    pes_[i][j]->psum_fifo_in()->push(p);
                    pes_[i+1][j]->psum_fifo_out()->pop();
                }
            }
        }
    }
}

/* print the IDs of MCs for debug */
void Array::debug_print_noc(NOC_TYPE noc_type)
{
    if (noc_type == NOC_TYPE_IFMAP_IN)
    {
        for (int i = 0; i < ARRAY_ROWS; i++)
        {
            cout << setw(2) << row_mcs_ifmap_in_[i]->get_id() << " =";
            for (int j = 0; j < ARRAY_COLUMNS; j++)
            {
                cout << setw(2) << row_mcs_ifmap_in_[i]->get_column_MC(j)->get_id() << " ";
            }
            cout << endl;
        }
    }
    else if (noc_type == NOC_TYPE_FILTER_IN)
    {
        for (int i = 0; i < ARRAY_ROWS; i++)
        {
            cout << setw(2) << row_mcs_filter_in_[i]->get_id() << " =";
            for (int j = 0; j < ARRAY_COLUMNS; j++)
            {
                cout << setw(2) << row_mcs_filter_in_[i]->get_column_MC(j)->get_id() << " ";
            }
            cout << endl;
        }
    }
    else if (noc_type == NOC_TYPE_PSUM_IN)
    {
        for (int i = 0; i < ARRAY_ROWS; i++)
        {
            cout << setw(2) << row_mcs_psum_in_[i]->get_id() << " =";
            for (int j = 0; j < ARRAY_COLUMNS; j++)
            {
                cout << setw(2) << row_mcs_psum_in_[i]->get_column_MC(j)->get_id() << " ";
            }
            cout << endl;
        }
    }
    else if (noc_type == NOC_TYPE_PSUM_OUT)
    {
        for (int i = 0; i < ARRAY_ROWS; i++)
        {
            cout << setw(2) << row_mcs_psum_out_[i]->get_id() << " =";
            for (int j = 0; j < ARRAY_COLUMNS; j++)
            {
                cout << setw(2) << row_mcs_psum_out_[i]->get_column_MC(j)->get_id() << " ";
            }
            cout << endl;
        }
    }
}

/* print the active PEs for debug */
void Array::debug_print_active()
{
    for (int i = 0; i < ARRAY_ROWS; i++)
    {
        for (int j = 0; j < ARRAY_COLUMNS; j++)
        {
            cout  << pes_[i][j]->get_active() << " ";
        }
        cout << endl;
    }
}

/* print the bottom of set PEs for debug */
void Array::debug_print_bottom_of_set()
{
    for (int i = 0; i < ARRAY_ROWS; i++)
    {
        for (int j = 0; j < ARRAY_COLUMNS; j++)
        {
            cout  << pes_[i][j]->get_is_bottom_of_set() << " ";
        }
        cout << endl;
    }
}

/* print the content of FIFOs for debug */
void Array::debug_print_fifos()
{
    cout << "array ifmap fifo in : ";
    ifmap_fifo_in_->debug_print();
    cout << "array filter fifo in: ";
    filter_fifo_in_->debug_print();
    cout << "array psum fifo in  : ";
    psum_fifo_in_->debug_print();
    cout << "array psum fifo out : ";
    psum_fifo_out_->debug_print();
}