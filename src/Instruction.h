#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "includes.h"

/*************************************************************
 * instruction format:
 * -----------------------------------------------------------
 *
 * OP_NOP
 * | 4 bits | 28 bits |
 * | opcode |    0    |
 *
 * nop
 *
 * -----------------------------------------------------------
 *
 * OP_LD_REG
 * | 4 bits |     2bits     |         26 bits          |
 * | opcode | base register |   base memory address    |
 *
 *   1. Base register address be must within CONTROLLER_BASE_REGS
 *   2. Base memory address will be left-shifted by GB_DATA_WIDTH_LOG2 bits to be aligned with an entry of the global buffer
 *
 * ld.reg <base_reg> <memory_addr>
 *
 * -----------------------------------------------------------
 *
 * OP_GB_LD_IFMAP/OP_GB_LD_FILTER/OP_GB_ST_PSUM
 * | 4 bits |       14 bits         |     2bits     |         12 bits          |
 * | opcode | global buffer address | base register |      memory offset       |
 *
 *   1. Global buffer address will be left-shifted by GB_DATA_WIDTH_LOG2 bits to be aligned with an entry of the global buffer
 *   2. Base register address be must within CONTROLLER_BASE_REGS
 *   3. Base memory address will be left-shifted by GB_DATA_WIDTH_LOG2 bits to be aligned with an entry of the global buffer
 *
 * gb.ld.ifmap   <global_buffer_write_addr> <base_reg> <memory_read_addr>
 * gb.ld.filter  <global_buffer_write_addr> <base_reg> <memory_read_addr>
 * gb.st.psum    <global_buffer_read_addr>  <base_reg> <memory_write_addr>
 *
 * -----------------------------------------------------------
 *
 * OP_PE_LD_IFMAP/OP_PE_LD_FILTER/OP_PE_LD_PSUM/OP_PE_ST_PSUM
 * | 4 bits |       17 bits         |     4 bits    |    5 bits     |    2 bits   |
 * | opcode | global buffer address |     row ID    |   column ID   |      0      |
 *
 *   1. Global buffer address will be left-shifted by PE_DATA_WIDTH_LOG2 bits to be aligned with an entry of PE word
 *   2. Row ID must be within 16, as indicated in the paper
 *   3. Column ID must be with 32, as indicated in the paper
 *
 * pe.ld.ifmap  <global_buffer_read_addr>  <row_id> <col_id> <ifmap_write_addr>
 * pe.ld.filter <global_buffer_read_addr>  <row_id> <col_id> <filter_write_addr>
 * pe.ld.psum   <global_buffer_read_addr>  <row_id> <col_id> <psum_write_addr>
 * pe.st.psum   <global_buffer_write_addr> <row_id> <col_id> <psum_read_addr>
 *
 * -----------------------------------------------------------
 *
 * OP_PE_COMPUTE
 * | 4 bits |        4 bits           |         8 bits           |          5 bits        |        5 bits           |  6 bits |
 * | opcode | ifmap_spad read address | filter_spad read address | psum_spad read address | psum_spad write address |    0    |
 *
 * pe.mult.s   <ifmap_read_addr> <filter_read_addr> <psum_write_addr>
 * pe.mult.o   <ifmap_read_addr> <filter_read_addr>
 * pe.mult.so  <ifmap_read_addr> <filter_read_addr> <psum_write_addr>
 * pe.madd.s   <ifmap_read_addr> <filter_read_addr> <psum_read_addr> <psum_write_addr>
 * pe.madd.o   <ifmap_read_addr> <filter_read_addr> <psum_read_addr>
 * pe.madd.so  <ifmap_read_addr> <filter_read_addr> <psum_read_addr> <psum_write_addr>
 * pe.add.s    <psum_read_addr>  <psum_write_addr>
 * pe.add.o    <psum_read_addr>
 * pe.add.so   <psum_read_addr>  <psum_write_addr>
 *
 * -----------------------------------------------------------
 *
 * OP_UP
 * | 4 bits | 28 bits |
 * | opcode |    0    |
 *
 * up <psum_read_addr>
 *
 *************************************************************/

/* opcode */
#define OP_NOP            0x0
#define OP_LD_REG         0x1
#define OP_GB_LD_IFMAP    0x2
#define OP_GB_LD_FILTER   0x3
#define OP_GB_ST_PSUM     0x4
#define OP_PE_LD_IFMAP    0x5
#define OP_PE_LD_FILTER   0x6
#define OP_PE_LD_PSUM     0x7
#define OP_PE_ST_PSUM     0x8
#define OP_PE_COMPUTE     0x9
#define OP_UP             0xA

/* a decoded instruction */
typedef struct Instruction
{
    int opcode_;                // opcode of the instruction

    int reg_addr_;              // the base register that is going to be modified

    int mem_addr_;              // off-chip memory address

    GB_REGION gb_region_;       // global buffer address, which region of global buffer
    int gb_bank_;               // global buffer address, which bank
    int gb_entry_;              // global buffer address, which entry in the bank
    int gb_offset_;             // global buffer address, which offset in the entry, offset is based on PE_DATA_WIDTH not byte

    int row_id_;                // row ID
    int col_id_;                // column ID

    int ifmap_read_en_;         // need to read from ifmap sratchpad memory
    int ifmap_read_addr_;       // ifmap sratchpad memory read address
    int ifmap_write_en_;        // need to write into ifmap sratchpad memory
    int ifmap_write_addr_;      // ifmap sratchpad memory write address
    int filter_read_en_;        // need to read from filter sratchpad memory
    int filter_read_addr_;      // filter sratchpad memory read address
    int filter_write_en_;       // need to write into filter sratchpad memory
    int filter_write_addr_;     // filter sratchpad memory write address
    int psum_read_en_;          // need to read from psum sratchpad memory
    int psum_read_addr_;        // psum sratchpad memory read address
    int psum_write_en_;         // need to write into psum sratchpad memory
    int psum_write_addr_;       // psum sratchpad memory write address
    int add_mux_;               // 0: choose multiplication results of ifmap and filter; 1: choose data read from psum fifo in
    int psum_read_mux_;         // 0: choose 0; 1: choose data read from psum scratchpad memory
    int send_psum_out_;         // 0: do not write result into psum fifo out; 1: write result into psum fifo out    
} Instruction;

/* print decoded instruction for debug */
void debug_print_instruction(Instruction *inst);

#endif
