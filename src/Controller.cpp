#include "includes.h"

/* constructor */
Controller::Controller()
{
    file_opened_ = false;

    instQueue_ = new InstQueue(CONTROLLER_INST_Q_SIZE);

    for (int i = 0; i < CONTROLLER_BASE_REGS; i++)
    {
        regs_[i] = 0;
    }
}

/* destructor */
Controller::~Controller()
{
    if (file_opened_)
    {
        file_.close();
    }
    delete instQueue_;
}

/* open the file that contains instructions */
void Controller::open_file(char *filename)
{
    if (file_opened_)
    {
        file_.close();
    }
    file_.open(filename);
    file_opened_ = true;
}

/* read, decode an instruction, then put it in the instruction queue */
bool Controller::read_instruction()
{
    if (instQueue_->full())
    {
        return true;
    }

    string line;
    getline(file_, line);
    if(file_.eof())
    {
        return false;
    }

    Instruction inst;

    char *token = strtok(const_cast<char*>(line.c_str()), " ");
    if (strcmp(token, "nop") == 0)
    {
        inst.opcode_ = OP_NOP;
    }
    else if (strcmp(token, "ld.reg") == 0)
    {
        inst.opcode_ = OP_LD_REG;

        token = strtok(NULL, " ");
        inst.reg_addr_ = atoi(token);
        assert(inst.reg_addr_ < CONTROLLER_BASE_REGS);

        token = strtok(NULL, " ");
        inst.mem_addr_ = atoi(token) << GB_DATA_WIDTH_LOG2;
        assert(inst.mem_addr_ < MEM_SIZE);
    }
    else if (strcmp(token, "gb.ld.ifmap") == 0)
    {
        inst.opcode_ = OP_GB_LD_IFMAP;

        token = strtok(NULL, " ");
        int gb_addr = atoi(token) << GB_DATA_WIDTH_LOG2;
        inst.gb_region_ = GB_IFMAP;
        inst.gb_bank_ = gb_addr / (GB_DATA_WIDTH*GB_BANK_ENTRIES);
        inst.gb_entry_ = gb_addr / GB_DATA_WIDTH % GB_BANK_ENTRIES;
        inst.gb_offset_ = 0;

        token = strtok(NULL, " ");
        inst.reg_addr_ = atoi(token);
        assert(inst.reg_addr_ < CONTROLLER_BASE_REGS);

        token = strtok(NULL, " ");
        inst.mem_addr_ = regs_[inst.reg_addr_] + (atoi(token) << GB_DATA_WIDTH_LOG2);
        assert(inst.mem_addr_ < MEM_SIZE);
    }
    else if (strcmp(token, "gb.ld.filter") == 0)
    {
        inst.opcode_ = OP_GB_LD_FILTER;

        token = strtok(NULL, " ");
        int gb_addr = atoi(token) << GB_DATA_WIDTH_LOG2;
        inst.gb_region_ = GB_FILTER;
        inst.gb_bank_ = gb_addr / (GB_DATA_WIDTH*GB_BANK_ENTRIES);
        inst.gb_entry_ = gb_addr / GB_DATA_WIDTH % GB_BANK_ENTRIES;
        inst.gb_offset_ = 0;

        token = strtok(NULL, " ");
        inst.reg_addr_ = atoi(token);
        assert(inst.reg_addr_ < CONTROLLER_BASE_REGS);

        token = strtok(NULL, " ");
        inst.mem_addr_ = regs_[inst.reg_addr_] + (atoi(token) << GB_DATA_WIDTH_LOG2);
        assert(inst.mem_addr_ < MEM_SIZE);
    }
    else if (strcmp(token, "gb.st.psum") == 0)
    {
        inst.opcode_ = OP_GB_ST_PSUM;

        token = strtok(NULL, " ");
        int gb_addr = atoi(token) << GB_DATA_WIDTH_LOG2;
        inst.gb_region_ = GB_PSUM;
        inst.gb_bank_ = gb_addr / (GB_DATA_WIDTH*GB_BANK_ENTRIES);
        inst.gb_entry_ = gb_addr / GB_DATA_WIDTH % GB_BANK_ENTRIES;
        inst.gb_offset_ = 0;

        token = strtok(NULL, " ");
        inst.reg_addr_ = atoi(token);
        assert(inst.reg_addr_ < CONTROLLER_BASE_REGS);

        token = strtok(NULL, " ");
        inst.mem_addr_ = regs_[inst.reg_addr_] + (atoi(token) << GB_DATA_WIDTH_LOG2);
        assert(inst.mem_addr_ < MEM_SIZE);
    }
    else if (strcmp(token, "pe.ld.ifmap") == 0)
    {
        inst.opcode_ = OP_PE_LD_IFMAP;

        token = strtok(NULL, " ");
        int gb_addr = atoi(token) << PE_DATA_WIDTH_LOG2;
        inst.gb_region_ = GB_IFMAP;
        inst.gb_bank_ = gb_addr / (GB_DATA_WIDTH*GB_BANK_ENTRIES);
        inst.gb_entry_ = gb_addr / GB_DATA_WIDTH % GB_BANK_ENTRIES;
        inst.gb_offset_ = gb_addr / PE_DATA_WIDTH % (GB_DATA_WIDTH/PE_DATA_WIDTH);

        token = strtok(NULL, " ");
        inst.row_id_ = atoi(token);
        assert(inst.row_id_ < 16);

        token = strtok(NULL, " ");
        inst.col_id_ = atoi(token);
        assert(inst.col_id_ < 32);

        inst.ifmap_read_en_ = 0;
        inst.ifmap_read_addr_ = -1;

        token = strtok(NULL, " ");
        inst.ifmap_write_en_ = 1;
        inst.ifmap_write_addr_ = atoi(token);
        assert(inst.ifmap_write_addr_ < PE_IFMAP_SPAD_ENTRIES);

        inst.filter_read_en_ = 0;
        inst.filter_read_addr_ = -1;

        inst.filter_write_en_ = 0;
        inst.filter_write_addr_ = -1;

        inst.psum_read_en_ = 0;
        inst.psum_read_addr_ = -1;

        inst.psum_write_en_ = -1;
        inst.psum_write_addr_ = 0;

        inst.add_mux_ = 0;
        inst.psum_read_mux_ = 0;
        inst.send_psum_out_ = 0;
    }
    else if (strcmp(token, "pe.ld.filter") == 0)
    {
        inst.opcode_ = OP_PE_LD_FILTER;

        token = strtok(NULL, " ");
        int gb_addr = atoi(token) << PE_DATA_WIDTH_LOG2;
        inst.gb_region_ = GB_FILTER;
        inst.gb_bank_ = gb_addr / (GB_DATA_WIDTH*GB_BANK_ENTRIES);
        inst.gb_entry_ = gb_addr / GB_DATA_WIDTH % GB_BANK_ENTRIES;
        inst.gb_offset_ = gb_addr / PE_DATA_WIDTH % (GB_DATA_WIDTH/PE_DATA_WIDTH);

        token = strtok(NULL, " ");
        inst.row_id_ = atoi(token);
        assert(inst.row_id_ < 16);

        token = strtok(NULL, " ");
        inst.col_id_ = atoi(token);
        assert(inst.col_id_ < 32);

        inst.ifmap_read_en_ = 0;
        inst.ifmap_read_addr_ = -1;

        inst.ifmap_write_en_ = 0;
        inst.ifmap_write_addr_ = -1;

        inst.filter_read_en_ = 0;
        inst.filter_read_addr_ = -1;

        token = strtok(NULL, " ");
        inst.filter_write_en_ = 1;
        inst.filter_write_addr_ = atoi(token);
        assert(inst.filter_write_addr_ < PE_FILTER_SPAD_ENTRIES);

        inst.psum_read_en_ = 0;
        inst.psum_read_addr_ = -1;

        inst.psum_write_en_ = -1;
        inst.psum_write_addr_ = 0;

        inst.add_mux_ = 0;
        inst.psum_read_mux_ = 0;
        inst.send_psum_out_ = 0;
    }
    else if (strcmp(token, "pe.ld.psum") == 0)
    {
        inst.opcode_ = OP_PE_LD_PSUM;

        token = strtok(NULL, " ");
        int gb_addr = atoi(token) << PE_DATA_WIDTH_LOG2;
        inst.gb_region_ = GB_PSUM;
        inst.gb_bank_ = gb_addr / (GB_DATA_WIDTH*GB_BANK_ENTRIES);
        inst.gb_entry_ = gb_addr / GB_DATA_WIDTH % GB_BANK_ENTRIES;
        inst.gb_offset_ = gb_addr / PE_DATA_WIDTH % (GB_DATA_WIDTH/PE_DATA_WIDTH);

        token = strtok(NULL, " ");
        inst.row_id_ = atoi(token);
        assert(inst.row_id_ < 16);

        token = strtok(NULL, " ");
        inst.col_id_ = atoi(token);
        assert(inst.col_id_ < 32);

        inst.ifmap_read_en_ = 0;
        inst.ifmap_read_addr_ = -1;

        inst.ifmap_write_en_ = 0;
        inst.ifmap_write_addr_ = -1;

        inst.filter_read_en_ = 0;
        inst.filter_read_addr_ = -1;

        inst.filter_write_en_ = 0;
        inst.filter_write_addr_ = -1;

        inst.psum_read_en_ = 0;
        inst.psum_read_addr_ = -1;

        token = strtok(NULL, " ");
        inst.psum_write_en_ = 1;
        inst.psum_write_addr_ = atoi(token);
        assert(inst.psum_write_addr_ < PE_PSUM_SPAD_ENTRIES);

        inst.add_mux_ = 1;
        inst.psum_read_mux_ = 0;
        inst.send_psum_out_ = 0;
    }
    else if (strcmp(token, "pe.st.psum") == 0)
    {
        inst.opcode_ = OP_PE_ST_PSUM;

        token = strtok(NULL, " ");
        int gb_addr = atoi(token) << PE_DATA_WIDTH_LOG2;
        inst.gb_region_ = GB_PSUM;
        inst.gb_bank_ = gb_addr / (GB_DATA_WIDTH*GB_BANK_ENTRIES);
        inst.gb_entry_ = gb_addr / GB_DATA_WIDTH % GB_BANK_ENTRIES;
        inst.gb_offset_ = gb_addr / PE_DATA_WIDTH % (GB_DATA_WIDTH/PE_DATA_WIDTH);

        token = strtok(NULL, " ");
        inst.row_id_ = atoi(token);
        assert(inst.row_id_ < 16);

        token = strtok(NULL, " ");
        inst.col_id_ = atoi(token);
        assert(inst.col_id_ < 32);

        inst.ifmap_read_en_ = 0;
        inst.ifmap_read_addr_ = -1;

        inst.ifmap_write_en_ = 0;
        inst.ifmap_write_addr_ = -1;

        inst.filter_read_en_ = 0;
        inst.filter_read_addr_ = -1;

        inst.filter_write_en_ = 0;
        inst.filter_write_addr_ = -1;

        inst.psum_read_en_ = 0;
        inst.psum_read_addr_ = -1;

        inst.psum_write_en_ = -1;
        inst.psum_write_addr_ = 0;

        inst.add_mux_ = 0;
        inst.psum_read_mux_ = 0;
        inst.send_psum_out_ = 0;
    }
    else if (strcmp(token, "pe.mult.s") == 0)
    {
        inst.opcode_ = OP_PE_COMPUTE;

        token = strtok(NULL, " ");
        inst.ifmap_read_en_ = 1;
        inst.ifmap_read_addr_ = atoi(token);
        assert(inst.ifmap_read_addr_ < PE_IFMAP_SPAD_ENTRIES);

        inst.ifmap_write_en_ = 0;
        inst.ifmap_write_addr_ = -1;

        token = strtok(NULL, " ");
        inst.filter_read_en_ = 1;
        inst.filter_read_addr_ = atoi(token);
        assert(inst.filter_read_addr_ < PE_FILTER_SPAD_ENTRIES);

        inst.filter_write_en_ = 0;
        inst.filter_write_addr_ = -1;

        inst.psum_read_en_ = 0;
        inst.psum_read_addr_ = -1;

        token = strtok(NULL, " ");
        inst.psum_write_en_ = 1;
        inst.psum_write_addr_ = atoi(token);
        assert(inst.psum_write_addr_ < PE_PSUM_SPAD_ENTRIES);

        inst.add_mux_ = 0;
        inst.psum_read_mux_ = 0;
        inst.send_psum_out_ = 0;
    }
    else if (strcmp(token, "pe.mult.o") == 0)
    {
        inst.opcode_ = OP_PE_COMPUTE;

        token = strtok(NULL, " ");
        inst.ifmap_read_en_ = 1;
        inst.ifmap_read_addr_ = atoi(token);
        assert(inst.ifmap_read_addr_ < PE_IFMAP_SPAD_ENTRIES);

        inst.ifmap_write_en_ = 0;
        inst.ifmap_write_addr_ = -1;

        token = strtok(NULL, " ");
        inst.filter_read_en_ = 1;
        inst.filter_read_addr_ = atoi(token);
        assert(inst.filter_read_addr_ < PE_FILTER_SPAD_ENTRIES);

        inst.filter_write_en_ = 0;
        inst.filter_write_addr_ = -1;

        inst.psum_read_en_ = 0;
        inst.psum_read_addr_ = -1;

        inst.psum_write_en_ = 0;
        inst.psum_write_addr_ = -1;

        inst.add_mux_ = 0;
        inst.psum_read_mux_ = 0;
        inst.send_psum_out_ = 1;
    }
    else if (strcmp(token, "pe.mult.so") == 0)
    {
        inst.opcode_ = OP_PE_COMPUTE;

        token = strtok(NULL, " ");
        inst.ifmap_read_en_ = 1;
        inst.ifmap_read_addr_ = atoi(token);
        assert(inst.ifmap_read_addr_ < PE_IFMAP_SPAD_ENTRIES);

        inst.ifmap_write_en_ = 0;
        inst.ifmap_write_addr_ = -1;

        token = strtok(NULL, " ");
        inst.filter_read_en_ = 1;
        inst.filter_read_addr_ = atoi(token);
        assert(inst.filter_read_addr_ < PE_FILTER_SPAD_ENTRIES);

        inst.filter_write_en_ = 0;
        inst.filter_write_addr_ = -1;

        inst.psum_read_en_ = 0;
        inst.psum_read_addr_ = -1;

        token = strtok(NULL, " ");
        inst.psum_write_en_ = 1;
        inst.psum_write_addr_ = atoi(token);
        assert(inst.psum_write_addr_ < PE_PSUM_SPAD_ENTRIES);

        inst.add_mux_ = 0;
        inst.psum_read_mux_ = 0;
        inst.send_psum_out_ = 1;
    }
    else if (strcmp(token, "pe.madd.s") == 0)
    {
        inst.opcode_ = OP_PE_COMPUTE;

        token = strtok(NULL, " ");
        inst.ifmap_read_en_ = 1;
        inst.ifmap_read_addr_ = atoi(token);
        assert(inst.ifmap_read_addr_ < PE_IFMAP_SPAD_ENTRIES);

        inst.ifmap_write_en_ = 0;
        inst.ifmap_write_addr_ = -1;

        token = strtok(NULL, " ");
        inst.filter_read_en_ = 1;
        inst.filter_read_addr_ = atoi(token);
        assert(inst.filter_read_addr_ < PE_FILTER_SPAD_ENTRIES);

        inst.filter_write_en_ = 0;
        inst.filter_write_addr_ = -1;

        token = strtok(NULL, " ");
        inst.psum_read_en_ = 1;
        inst.psum_read_addr_ = atoi(token);
        assert(inst.psum_read_addr_ < PE_PSUM_SPAD_ENTRIES);

        token = strtok(NULL, " ");
        inst.psum_write_en_ = 1;
        inst.psum_write_addr_ = atoi(token);
        assert(inst.psum_write_addr_ < PE_PSUM_SPAD_ENTRIES);

        inst.add_mux_ = 0;
        inst.psum_read_mux_ = 1;
        inst.send_psum_out_ = 0;
    }
    else if (strcmp(token, "pe.madd.o") == 0)
    {
        inst.opcode_ = OP_PE_COMPUTE;

        token = strtok(NULL, " ");
        inst.ifmap_read_en_ = 1;
        inst.ifmap_read_addr_ = atoi(token);
        assert(inst.ifmap_read_addr_ < PE_IFMAP_SPAD_ENTRIES);

        inst.ifmap_write_en_ = 0;
        inst.ifmap_write_addr_ = -1;

        token = strtok(NULL, " ");
        inst.filter_read_en_ = 1;
        inst.filter_read_addr_ = atoi(token);
        assert(inst.filter_read_addr_ < PE_FILTER_SPAD_ENTRIES);

        inst.filter_write_en_ = 0;
        inst.filter_write_addr_ = -1;

        token = strtok(NULL, " ");
        inst.psum_read_en_ = 1;
        inst.psum_read_addr_ = atoi(token);
        assert(inst.psum_read_addr_ < PE_PSUM_SPAD_ENTRIES);

        inst.psum_write_en_ = 0;
        inst.psum_write_addr_ = -1;

        inst.add_mux_ = 0;
        inst.psum_read_mux_ = 1;
        inst.send_psum_out_ = 1;
    }
    else if (strcmp(token, "pe.madd.so") == 0)
    {
        inst.opcode_ = OP_PE_COMPUTE;

        token = strtok(NULL, " ");
        inst.ifmap_read_en_ = 1;
        inst.ifmap_read_addr_ = atoi(token);
        assert(inst.ifmap_read_addr_ < PE_IFMAP_SPAD_ENTRIES);

        inst.ifmap_write_en_ = 0;
        inst.ifmap_write_addr_ = -1;

        token = strtok(NULL, " ");
        inst.filter_read_en_ = 1;
        inst.filter_read_addr_ = atoi(token);
        assert(inst.filter_read_addr_ < PE_FILTER_SPAD_ENTRIES);

        inst.filter_write_en_ = 0;
        inst.filter_write_addr_ = -1;

        token = strtok(NULL, " ");
        inst.psum_read_en_ = 1;
        inst.psum_read_addr_ = atoi(token);
        assert(inst.psum_read_addr_ < PE_PSUM_SPAD_ENTRIES);

        token = strtok(NULL, " ");
        inst.psum_write_en_ = 1;
        inst.psum_write_addr_ = atoi(token);
        assert(inst.psum_write_addr_ < PE_PSUM_SPAD_ENTRIES);

        inst.add_mux_ = 0;
        inst.psum_read_mux_ = 1;
        inst.send_psum_out_ = 1;
    }
    else if (strcmp(token, "pe.add.s") == 0)
    {
        inst.opcode_ = OP_PE_COMPUTE;

        inst.ifmap_read_en_ = 0;
        inst.ifmap_read_addr_ = -1;

        inst.ifmap_write_en_ = 0;
        inst.ifmap_write_addr_ = -1;

        inst.filter_read_en_ = 0;
        inst.filter_read_addr_ = -1;

        inst.filter_write_en_ = 0;
        inst.filter_write_addr_ = -1;

        token = strtok(NULL, " ");
        inst.psum_read_en_ = 1;
        inst.psum_read_addr_ = atoi(token);
        assert(inst.psum_read_addr_ < PE_PSUM_SPAD_ENTRIES);

        token = strtok(NULL, " ");
        inst.psum_write_en_ = 1;
        inst.psum_write_addr_ = atoi(token);
        assert(inst.psum_write_addr_ < PE_PSUM_SPAD_ENTRIES);

        inst.add_mux_ = 1;
        inst.psum_read_mux_ = 1;
        inst.send_psum_out_ = 0;
    }
    else if (strcmp(token, "pe.add.o") == 0)
    {
        inst.opcode_ = OP_PE_COMPUTE;

        inst.ifmap_read_en_ = 0;
        inst.ifmap_read_addr_ = -1;

        inst.ifmap_write_en_ = 0;
        inst.ifmap_write_addr_ = -1;

        inst.filter_read_en_ = 0;
        inst.filter_read_addr_ = -1;

        inst.filter_write_en_ = 0;
        inst.filter_write_addr_ = -1;

        token = strtok(NULL, " ");
        inst.psum_read_en_ = 1;
        inst.psum_read_addr_ = atoi(token);
        assert(inst.psum_read_addr_ < PE_PSUM_SPAD_ENTRIES);

        inst.psum_write_en_ = 0;
        inst.psum_write_addr_ = -1;

        inst.add_mux_ = 1;
        inst.psum_read_mux_ = 1;
        inst.send_psum_out_ = 1;
    }
    else if (strcmp(token, "pe.add.so") == 0)
    {
        inst.opcode_ = OP_PE_COMPUTE;

        inst.ifmap_read_en_ = 0;
        inst.ifmap_read_addr_ = -1;

        inst.ifmap_write_en_ = 0;
        inst.ifmap_write_addr_ = -1;

        inst.filter_read_en_ = 0;
        inst.filter_read_addr_ = -1;

        inst.filter_write_en_ = 0;
        inst.filter_write_addr_ = -1;

        token = strtok(NULL, " ");
        inst.psum_read_en_ = 1;
        inst.psum_read_addr_ = atoi(token);
        assert(inst.psum_read_addr_ < PE_PSUM_SPAD_ENTRIES);

        token = strtok(NULL, " ");
        inst.psum_write_en_ = 1;
        inst.psum_write_addr_ = atoi(token);
        assert(inst.psum_write_addr_ < PE_PSUM_SPAD_ENTRIES);

        inst.add_mux_ = 1;
        inst.psum_read_mux_ = 1;
        inst.send_psum_out_ = 1;
    }
    else if (strcmp(token, "up") == 0)
    {
        inst.opcode_ = OP_UP;

        inst.ifmap_read_en_ = 0;
        inst.ifmap_read_addr_ = -1;

        inst.ifmap_write_en_ = 0;
        inst.ifmap_write_addr_ = -1;

        inst.filter_read_en_ = 0;
        inst.filter_read_addr_ = -1;

        inst.filter_write_en_ = 0;
        inst.filter_write_addr_ = -1;

        inst.psum_read_en_ = 0;
        inst.psum_read_addr_ = -1;

        inst.psum_write_en_ = 0;
        inst.psum_write_addr_ = -1;

        inst.add_mux_ = 0;
        inst.psum_read_mux_ = 0;
        inst.send_psum_out_ = 2;
    }
    instQueue_->push(&inst);

    return true;
}

/* issue instructions */
bool Controller::issue_nop()
{
    if (instQueue_->empty())
    {
        return false;
    }

    Instruction *inst = instQueue_->front();
    if (inst->opcode_ == OP_NOP)
    {
        instQueue_->pop();
        return true;
    }

    return false;
}
bool Controller::issue_ld_reg()
{
    if (instQueue_->empty())
    {
        return false;
    }

    Instruction *inst = instQueue_->front();
    if (inst->opcode_ == OP_LD_REG)
    {
        regs_[inst->reg_addr_] = inst->mem_addr_;
        instQueue_->pop();
        return true;
    }

    return false;
}
bool Controller::issue_gb_ld(Memory *mem, GB *gb)
{
    if (instQueue_->empty())
    {
        return false;
    }

    Instruction *inst = instQueue_->front();
    if (inst->opcode_ == OP_GB_LD_IFMAP)
    {
        if (gb->ifmap_fifo_in()->full())
        {
            return false;
        }

        Packet *p = new Packet(GB_DATA_WIDTH);
        mem->read(inst->mem_addr_, p);
        p->set_gb_region(inst->gb_region_);
        p->set_gb_bank(inst->gb_bank_);
        p->set_gb_entry(inst->gb_entry_);
        p->set_gb_offset(inst->gb_offset_);
        gb->ifmap_fifo_in()->push(p);
        delete p;
        instQueue_->pop();
        return true;
    }
    else if (inst->opcode_ == OP_GB_LD_FILTER)
    {
        if (gb->filter_fifo_in()->full())
        {
            return false;
        }

        Packet *p = new Packet(GB_DATA_WIDTH);
        mem->read(inst->mem_addr_, p);
        p->set_gb_region(inst->gb_region_);
        p->set_gb_bank(inst->gb_bank_);
        p->set_gb_entry(inst->gb_entry_);
        p->set_gb_offset(inst->gb_offset_);
        gb->filter_fifo_in()->push(p);
        delete p;
        instQueue_->pop();
        return true;
    }

    return false;
}
bool Controller::issue_gb_st(GB *gb)
{
    if (instQueue_->empty())
    {
        return false;
    }

    Instruction *inst = instQueue_->front();
    if (inst->opcode_ == OP_GB_ST_PSUM)
    {
        if (gb->psum_fifo_out()->full())
        {
            return false;
        }

        Packet *p = new Packet(GB_DATA_WIDTH);
        gb->read(inst->gb_region_, inst->gb_bank_, inst->gb_entry_, p);
        p->set_mem_addr(inst->mem_addr_);
        gb->psum_fifo_out()->push(p);
        delete p;
        instQueue_->pop();
        return true;
    }

    return false;
}
bool Controller::issue_pe_ld(GB *gb, Array *array)
{
    if (instQueue_->empty())
    {
        return false;
    }

    Instruction *inst = instQueue_->front();
    if (inst->opcode_ == OP_PE_LD_IFMAP)
    {
        if (array->ifmap_fifo_in()->full())
        {
            return false;
        }

        if (array->issue_ld_ready(NOC_TYPE_IFMAP_IN, inst->row_id_, inst->col_id_) == false)
        {
            return false;
        }

        Packet *p = new Packet(PE_DATA_WIDTH);
        gb->read(inst->gb_region_, inst->gb_bank_, inst->gb_entry_, inst->gb_offset_, p);
        p->set_row_id(inst->row_id_);
        p->set_col_id(inst->col_id_);
        array->ifmap_fifo_in()->push(p);
        delete p;
        array->issue_ld(NOC_TYPE_IFMAP_IN, inst, inst->row_id_, inst->col_id_);
        instQueue_->pop();
        return true;
    }
    else if (inst->opcode_ == OP_PE_LD_FILTER)
    {
        if (array->filter_fifo_in()->full())
        {
            return false;
        }

        if (array->issue_ld_ready(NOC_TYPE_FILTER_IN, inst->row_id_, inst->col_id_) == false)
        {
            return false;
        }

        Packet *p = new Packet(PE_DATA_WIDTH);
        gb->read(inst->gb_region_, inst->gb_bank_, inst->gb_entry_, inst->gb_offset_, p);
        p->set_row_id(inst->row_id_);
        p->set_col_id(inst->col_id_);
        array->filter_fifo_in()->push(p);
        delete p;
        array->issue_ld(NOC_TYPE_FILTER_IN, inst, inst->row_id_, inst->col_id_);
        instQueue_->pop();
        return true;
    }
    else if (inst->opcode_ == OP_PE_LD_PSUM)
    {
        if (array->psum_fifo_in()->full())
        {
            return false;
        }

        if (array->issue_ld_ready(NOC_TYPE_PSUM_IN, inst->row_id_, inst->col_id_) == false)
        {
            return false;
        }

        Packet *p = new Packet(PE_DATA_WIDTH);
        gb->read(inst->gb_region_, inst->gb_bank_, inst->gb_entry_, inst->gb_offset_, p);
        p->set_row_id(inst->row_id_);
        p->set_col_id(inst->col_id_);
        array->psum_fifo_in()->push(p);
        delete p;
        array->issue_ld(NOC_TYPE_PSUM_IN, inst, inst->row_id_, inst->col_id_);
        instQueue_->pop();
        return true;
    }

    return false;
}
bool Controller::issue_pe_st(Array *array)
{
    if (instQueue_->empty())
    {
        return false;
    }

    Instruction *inst = instQueue_->front();
    if (inst->opcode_ == OP_PE_ST_PSUM)
    {
        if (array->psum_fifo_out()->full())
        {
            return false;
        }

        if (array->get_PE(inst->row_id_, inst->col_id_)->psum_fifo_out()->empty())
        {
            return false;
        }

        Packet *p = array->get_PE(inst->row_id_, inst->col_id_)->psum_fifo_out()->front();
        array->psum_fifo_out()->push(p);
        array->psum_fifo_out()->end()->set_gb_region(inst->gb_region_);
        array->psum_fifo_out()->end()->set_gb_bank(inst->gb_bank_);
        array->psum_fifo_out()->end()->set_gb_entry(inst->gb_entry_);
        array->psum_fifo_out()->end()->set_gb_offset(inst->gb_offset_);
        array->get_PE(inst->row_id_, inst->col_id_)->psum_fifo_out()->pop();
        instQueue_->pop();
        return true;
    }

    return false;
}
bool Controller::issue_pe_compute(Array *array)
{
    if (instQueue_->empty())
    {
        return false;
    }

    Instruction *inst = instQueue_->front();
    if (inst->opcode_ == OP_PE_COMPUTE)
    {
        if (array->issue_compute_ready() == false)
        {
            return false;
        }

        array->issue_compute(inst);
        instQueue_->pop();
        return true;
    }

    return false;
}
bool Controller::issue_up(Array *array)
{
    if (instQueue_->empty())
    {
        return false;
    }

    Instruction *inst = instQueue_->front();
    if (inst->opcode_ == OP_UP)
    {
        if (array->issue_compute_ready() == false)
        {
            return false;
        }

        array->issue_compute(inst);
        instQueue_->pop();
        return true;
    }

    return false;
}
