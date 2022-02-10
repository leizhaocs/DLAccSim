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
