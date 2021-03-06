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

#ifndef MC_H
#define MC_H

#include "includes.h"

/* NOC type */
enum NOC_TYPE
{
    NOC_TYPE_IFMAP_IN,        // this MC belongs to ifmap in NOC
    NOC_TYPE_FILTER_IN,       // this MC belongs to filter in NOC
    NOC_TYPE_PSUM_IN,         // this MC belongs to psum in NOC
    NOC_TYPE_PSUM_OUT,        // this MC belongs to psum out NOC
};

/* type of MC */
enum MC_TYPE
{
    MC_TYPE_ROW,          // row MC
    MC_TYPE_COLUMN        // column MC
};

/* multicast controller */
class MC
{
public:
    /* constructor for row MC */
    MC(NOC_TYPE noc_type, MC_TYPE mc_type, vector<PE *> *pes);

    /* constructor for column MC */
    MC(NOC_TYPE noc_type, MC_TYPE mc_type, PE *pe);

    /* destructor */
    ~MC();

    /* get the ID of this MC */
    int get_id();

    /* set the ID of this MC */
    void set_id(int id);

    /* get a specific column MC, only valid for row MCs */
    MC *get_column_MC(int col);

    /* get the associated PE, only valid for column MCs */
    PE *get_PE();

    /* check if all destination PEs are ready to accept new data, only used for NOC_TYPE_IFMAP_IN, NOC_TYPE_FILTER_IN and NOC_TYPE_PSUM_IN */
    bool multicast_ready(int row_id, int col_id);

    /* multicast a packet to destination PEs, only used for NOC_TYPE_IFMAP_IN, NOC_TYPE_FILTER_IN and NOC_TYPE_PSUM_IN */
    void multicast(Packet *p, int row_id, int col_id);

    /* check if all destination PEs are ready to accept new instruction, only used for NOC_TYPE_IFMAP_IN, NOC_TYPE_FILTER_IN and NOC_TYPE_PSUM_IN */
    bool issue_to_some_pes_ready(int row_id, int col_id);

    /* issue an instruction to destination PEs, only used for NOC_TYPE_IFMAP_IN, NOC_TYPE_FILTER_IN and NOC_TYPE_PSUM_IN */
    void issue_to_some_pes(Instruction *inst, int row_id, int col_id);

    /* check if it is ready to collect data from a specific PE, only used for NOC_TYPE_PSUM_OUT */
    bool collect_from_pe_ready(int row_id, int col_id);

    /* collect data from a specific PE, only used for NOC_TYPE_PSUM_OUT */
    void collect_from_pe(Packet *p, int row_id, int col_id);

private:
    NOC_TYPE noc_type_;     // type of NOC
    MC_TYPE mc_type_;       // row MC or column MC
    int id_;                // for row MC this is the row ID, for colum MC this is the column ID

    vector<MC *> col_mcs_;  // only valid for row MC, all associated column MCs
    PE *pe_;                // only valid for column MC, the associated PE
};

#endif
