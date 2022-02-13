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

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "includes.h"

/* the controller of the accelerator */
class Controller
{
public:
    /* constructor */
    Controller();

    /* destructor */
    ~Controller();

    /* open the file that contains instructions */
    void open_file(char *filename);

    /* read, decode an instruction, then put it in the instruction queue */
    bool read_instruction();

    /* issue instructions */
    bool issue_nop();
    bool issue_ld_reg();
    bool issue_gb_ld(Memory *mem, GB *gb);
    bool issue_gb_st(GB *gb);
    bool issue_pe_ld(GB *gb, Array *array);
    bool issue_pe_st(Array *array);
    bool issue_pe_compute(Array *array);
    bool issue_pe_acc(Array *array);
    bool issue_up(Array *array);

private:
    bool file_opened_;                         // true if the file that contains the instructions has already been opened
    ifstream file_;                            // the file that contains the instructions

    InstQueue *instQueue_;                     // the instruction queue

    unsigned int regs_[CONTROLLER_BASE_REGS];  // base registers to access in off-chip memory

};

#endif
