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

/* print decoded instruction for debug */
void debug_print_instruction(Instruction *inst)
{
    if (inst->opcode_ == OP_NOP)
    {
        cout << "nop" << endl;
    }
    else if (inst->opcode_ == OP_LD_REG)
    {
        cout << "ld.reg" << endl;
    }
    else if (inst->opcode_ == OP_GB_LD_IFMAP)
    {
        cout << "gb.ld.ifmap" << endl;
    }
    else if (inst->opcode_ == OP_GB_LD_FILTER)
    {
        cout << "gb.ld.filter" << endl;
    }
    else if (inst->opcode_ == OP_GB_ST_PSUM)
    {
        cout << "gb.st.psum" << endl;
    }
    else if (inst->opcode_ == OP_PE_LD_IFMAP)
    {
        cout << "pe.ld.ifmap" << endl;
    }
    else if (inst->opcode_ == OP_PE_LD_FILTER)
    {
        cout << "pe.ld.filter" << endl;
    }
    else if (inst->opcode_ == OP_PE_LD_PSUM)
    {
        cout << "pe.ld.psum" << endl;
    }
    else if (inst->opcode_ == OP_PE_ST_PSUM)
    {
        cout << "pe.st.psum" << endl;
    }
    else if (inst->opcode_ == OP_PE_COMPUTE)
    {
        cout << "pe.compute" << endl;
    }
    else if (inst->opcode_ == OP_PE_ACC)
    {
        cout << "pe.acc" << endl;
    }
    else if (inst->opcode_ == OP_PE_UP)
    {
        cout << "pe.up" << endl;
    }
}
