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

/* constructor for row MC */
MC::MC(NOC_TYPE noc_type, MC_TYPE mc_type, vector<PE *> *pes)
{
    assert(mc_type == MC_TYPE_ROW);

    noc_type_ = noc_type;
    mc_type_ = mc_type;
    id_ = -1;

    for (int i = 0; i < ARRAY_COLUMNS; i++)
    {
        col_mcs_.push_back(new MC(noc_type_, MC_TYPE_COLUMN, (*pes)[i]));
    }
}

/* constructor for column MC */
MC::MC(NOC_TYPE noc_type, MC_TYPE mc_type, PE *pe)
{
    assert(mc_type == MC_TYPE_COLUMN);

    noc_type_ = noc_type;
    mc_type_ = mc_type;
    id_ = -1;

    pe_ = pe;
}

/* destructor */
MC::~MC()
{
    if (mc_type_ == MC_TYPE_ROW)
    {
        for (int i = 0; i < ARRAY_COLUMNS; i++)
        {
            delete col_mcs_[i];
        }
    }
}

/* get the ID of this MC */
int MC::get_id()
{
    return id_;
}

/* set the ID of this MC */
void MC::set_id(int id)
{
    id_ = id;
}

/* get a specific column MC, only valid for row MCs */
MC *MC::get_column_MC(int col)
{
    assert(mc_type_ == MC_TYPE_ROW);

    return col_mcs_[col];
}

/* get a specific PE, only valid for column MCs */
PE *MC::get_PE()
{
    assert(mc_type_ == MC_TYPE_COLUMN);

    return pe_;
}

/* check if all destination PEs are ready to accept new data, only used for NOC_TYPE_IFMAP_IN, NOC_TYPE_FILTER_IN and NOC_TYPE_PSUM_IN */
bool MC::multicast_ready(int row_id, int col_id)
{
    assert((noc_type_ == NOC_TYPE_IFMAP_IN) || (noc_type_ == NOC_TYPE_FILTER_IN) || (noc_type_ == NOC_TYPE_PSUM_IN));

    if (mc_type_ == MC_TYPE_ROW)
    {
        if (row_id != id_)
        {
            return true;
        }
        for (int i = 0; i < ARRAY_COLUMNS; i++)
        {
            if (col_mcs_[i]->multicast_ready(row_id, col_id) == false)
            {
                return false;
            }
        }
        return true;
    }
    else
    {
        if (col_id != id_)
        {
            return true;
        }
        switch(noc_type_)
        {
            case NOC_TYPE_IFMAP_IN:
                return !(pe_->ifmap_fifo_in()->full());
            case NOC_TYPE_FILTER_IN:
                return !(pe_->filter_fifo_in()->full());
            case NOC_TYPE_PSUM_IN:
                return!(pe_->psum_fifo_in()->full());
            default:
                return true;
        }
    }
}

/* multicast a packet to destination PEs, only used for NOC_TYPE_IFMAP_IN, NOC_TYPE_FILTER_IN and NOC_TYPE_PSUM_IN */
void MC::multicast(Packet *p, int row_id, int col_id)
{
    assert((noc_type_ == NOC_TYPE_IFMAP_IN) || (noc_type_ == NOC_TYPE_FILTER_IN) || (noc_type_ == NOC_TYPE_PSUM_IN));

    if (mc_type_ == MC_TYPE_ROW)
    {
        if (row_id != id_)
        {
            return;
        }
        for (int i = 0; i < ARRAY_COLUMNS; i++)
        {
            col_mcs_[i]->multicast(p, row_id, col_id);
        }
    }
    else
    {
        if (col_id != id_)
        {
            return;
        }
        switch(noc_type_)
        {
            case NOC_TYPE_IFMAP_IN:
                pe_->ifmap_fifo_in()->push(p);
                break;
            case NOC_TYPE_FILTER_IN:
                pe_->filter_fifo_in()->push(p);
                break;
            case NOC_TYPE_PSUM_IN:
                pe_->psum_fifo_in()->push(p);
                break;
            default:
                break;
        }
    }
}

/* check if all destination PEs are ready to accept new instruction, only used for NOC_TYPE_IFMAP_IN, NOC_TYPE_FILTER_IN and NOC_TYPE_PSUM_IN */
bool MC::issue_to_some_pes_ready(int row_id, int col_id)
{
    assert((noc_type_ == NOC_TYPE_IFMAP_IN) || (noc_type_ == NOC_TYPE_FILTER_IN) || (noc_type_ == NOC_TYPE_PSUM_IN));

    if (mc_type_ == MC_TYPE_ROW)
    {
        if (row_id != id_)
        {
            return true;
        }
        for (int i = 0; i < ARRAY_COLUMNS; i++)
        {
            if (col_mcs_[i]->issue_to_some_pes_ready(row_id, col_id) == false)
            {
                return false;
            }
        }
        return true;
    }
    else
    {
        if (col_id != id_)
        {
            return true;
        }
        return !(pe_->instQueue()->full());
    }
}

/* issue an instruction to destination PEs, only used for NOC_TYPE_IFMAP_IN, NOC_TYPE_FILTER_IN and NOC_TYPE_PSUM_IN */
void MC::issue_to_some_pes(Instruction *inst, int row_id, int col_id)
{
    assert((noc_type_ == NOC_TYPE_IFMAP_IN) || (noc_type_ == NOC_TYPE_FILTER_IN) || (noc_type_ == NOC_TYPE_PSUM_IN));

    if (mc_type_ == MC_TYPE_ROW)
    {
        if (row_id != id_)
        {
            return;
        }
        for (int i = 0; i < ARRAY_COLUMNS; i++)
        {
            col_mcs_[i]->issue_to_some_pes(inst, row_id, col_id);
        }
    }
    else
    {
        if (col_id != id_)
        {
            return;
        }
        pe_->instQueue()->push(inst);
    }
}

/* check if it is ready to collect data from a specific PE, only used for NOC_TYPE_PSUM_OUT */
bool MC::collect_from_pe_ready(int row_id, int col_id)
{
    assert(noc_type_ == NOC_TYPE_PSUM_OUT);

    if (mc_type_ == MC_TYPE_ROW)
    {
        if (row_id != id_)
        {
            return true;
        }
        for (int i = 0; i < ARRAY_COLUMNS; i++)
        {
            if (col_mcs_[i]->collect_from_pe_ready(row_id, col_id) == false)
            {
                return false;
            }
        }
        return true;
    }
    else
    {
        if (col_id != id_)
        {
            return true;
        }
        return pe_->ready_send_back_to_gb();
    }
}

/* collect data from a specific PE, only used for NOC_TYPE_PSUM_OUT */
void MC::collect_from_pe(Packet *p, int row_id, int col_id)
{
    assert(noc_type_ == NOC_TYPE_PSUM_OUT);

    if (mc_type_ == MC_TYPE_ROW)
    {
        if (row_id != id_)
        {
            return;
        }
        for (int i = 0; i < ARRAY_COLUMNS; i++)
        {
            col_mcs_[i]->collect_from_pe(p, row_id, col_id);
        }
    }
    else
    {
        if (col_id != id_)
        {
            return;
        }
        p->copy(pe_->psum_fifo_out()->front());
        pe_->psum_fifo_out()->pop();
    }
}
