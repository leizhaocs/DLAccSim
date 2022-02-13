import json
import sys

# this class stores all information of the accelerator and the NN
# this class also does all the compilation works
class Compiler:

    # constructor
    def __init__(self, acc_config, input_config, output_filename):

        # read accelerator configuration file
        with open(acc_config) as file:
            config = json.load(file)
            self.GB_DATA_WIDTH = config["GB"].get("GB_DATA_WIDTH", None)
            self.GB_DATA_WIDTH_LOG2 = config["GB"].get("GB_DATA_WIDTH_LOG2", None)
            self.GB_BANKS = config["GB"].get("GB_BANKS", None)
            self.GB_BANK_ENTRIES = config["GB"].get("GB_BANK_ENTRIES", None)
            self.ARRAY_ROWS = config["ARRAY"].get("ARRAY_ROWS", None)
            self.ARRAY_COLUMNS = config["ARRAY"].get("ARRAY_COLUMNS", None)
            self.PE_DATA_WIDTH = config["PE"].get("PE_DATA_WIDTH", None)
            self.PE_IFMAP_SPAD_ENTRIES = config["PE"].get("PE_IFMAP_SPAD_ENTRIES", None)
            self.PE_FILTER_SPAD_ENTRIES = config["PE"].get("PE_FILTER_SPAD_ENTRIES", None)
            self.PE_PSUM_SPAD_ENTRIES = config["PE"].get("PE_PSUM_SPAD_ENTRIES", None)
            self.MEM_SIZE = config["MEMORY"].get("MEM_SIZE", None)

        # read input configuration file
        with open(input_config) as file:
            config = json.load(file)
            self.N = config["INPUT"].get("N", None)
            self.C = config["INPUT"].get("C", None)
            self.H = config["INPUT"].get("H", None)
            self.W = config["INPUT"].get("W", None)
            self.M = config["FILTER"].get("M", None)
            self.R = config["FILTER"].get("R", None)
            self.S = config["FILTER"].get("S", None)
            self.n = config["PE"].get("n", None)
            self.q = config["PE"].get("q", None)
            self.p = config["PE"].get("p", None)

        # calcualte the remaining arguments
        self.E = self.H - (self.R - 1) # output height
        self.F = self.W - (self.S - 1) # output width
        self.r = self.C // self.q      # replicates for channel dimension (C)
        self.t = self.M // self.p      # replicates for filter dimention (M)
        self.s = self.N // self.n      # replicates for input dimention (N)

        # replicates for input dimention (N) need to be guaranteed not exceeds the total number of rows in ARRAY
        while self.s * self.r * self.t * self.R > self.ARRAY_ROWS:
            self.s = self.s - 1

        # ifmap scratchpad
        self.next_ifmap_spad_entry = 0 # the entry for next load from global buffer
        self.curr_ifmap_spad_start = 0 # the start entry to perform MAC with the filters

        self.output = open(output_filename, "w")

    # destructor
    def __del__(self):

        self.output.close()

    # print each instruction
    def nop(self):
        self.output.write("nop\n")
    def ld_reg(self, base_reg, memory_addr):
        self.output.write("ld.reg "+str(base_reg)+" "+str(memory_addr)+"\n")
    def gb_ld_ifmap(self, global_buffer_write_addr, base_reg, memory_read_addr):
        self.output.write("gb.ld.ifmap "+str(global_buffer_write_addr)+" "+str(base_reg)+" "+str(memory_read_addr)+"\n")
    def gb_ld_filter(self, global_buffer_write_addr, base_reg, memory_read_addr):
        self.output.write("gb.ld.filter "+str(global_buffer_write_addr)+" "+str(base_reg)+" "+str(memory_read_addr)+"\n")
    def gb_st_psum(self, global_buffer_read_addr, base_reg, memory_write_addr):
        self.output.write("gb.st.psum "+str(global_buffer_read_addr)+" "+str(base_reg)+" "+str(memory_write_addr)+"\n")
    def pe_ld_ifmap(self, global_buffer_read_addr, row_id, col_id, ifmap_write_addr):
        self.output.write("pe.ld.ifmap "+str(global_buffer_read_addr)+" "+str(row_id)+" "+str(col_id)+" "+str(ifmap_write_addr)+"\n")
    def pe_ld_filter(self, global_buffer_read_addr, row_id, col_id, filter_write_addr):
        self.output.write("pe.ld.filter "+str(global_buffer_read_addr)+" "+str(row_id)+" "+str(col_id)+" "+str(filter_write_addr)+"\n")
    def pe_ld_psum(self, global_buffer_read_addr, row_id, col_id, psum_write_addr):
        self.output.write("pe.ld.psum "+str(global_buffer_read_addr)+" "+str(row_id)+" "+str(col_id)+" "+str(psum_write_addr)+"\n")
    def pe_st_psum(self, global_buffer_write_addr, row_id, col_id):
        self.output.write("pe.st.psum "+str(global_buffer_write_addr)+" "+str(row_id)+" "+str(col_id)+"\n")
    def pe_mult_s(self, ifmap_read_addr, filter_read_addr, psum_write_addr):
        self.output.write("pe.mult.s "+str(ifmap_read_addr)+" "+str(filter_read_addr)+" "+str(psum_write_addr)+"\n")
    def pe_mult_o(self, ifmap_read_addr, filter_read_addr):
        self.output.write("pe.mult.o "+str(ifmap_read_addr)+" "+str(filter_read_addr)+"\n")
    def pe_mult_so(self, ifmap_read_addr, filter_read_addr, psum_write_addr):
        self.output.write("pe.mult.so "+str(ifmap_read_addr)+" "+str(filter_read_addr)+" "+str(psum_write_addr)+"\n")
    def pe_madd_s(self, ifmap_read_addr, filter_read_addr, psum_read_addr, psum_write_addr):
        self.output.write("pe.madd.s "+str(ifmap_read_addr)+" "+str(filter_read_addr)+" "+str(psum_read_addr)+" "+str(psum_write_addr)+"\n")
    def pe_madd_o(self, ifmap_read_addr, filter_read_addr, psum_read_addr):
        self.output.write("pe.madd.o "+str(ifmap_read_addr)+" "+str(filter_read_addr)+" "+str(psum_read_addr)+"\n")
    def pe_madd_so(self, ifmap_read_addr, filter_read_addr, psum_read_addr, psum_write_addr):
        self.output.write("pe.madd.so "+str(ifmap_read_addr)+" "+str(filter_read_addr)+" "+str(psum_read_addr)+" "+str(psum_write_addr)+"\n")
    def pe_add_s(self, psum_read_addr, psum_write_addr):
        self.output.write("pe.add.s "+str(psum_read_addr)+" "+str(psum_write_addr)+"\n")
    def pe_add_o(self, psum_read_addr):
        self.output.write("pe.add.o "+str(psum_read_addr)+"\n")
    def pe_add_so(self, psum_read_addr, psum_write_addr):
        self.output.write("pe.add.so "+str(psum_read_addr)+" "+str(psum_write_addr)+"\n")
    def pe_acc_o(self, psum_read_addr):
        self.output.write("pe.acc.o "+str(psum_read_addr)+"\n")
    def pe_acc_so(self, psum_read_addr, psum_write_addr):
        self.output.write("pe.acc.so "+str(psum_read_addr)+" "+str(psum_write_addr)+"\n")
    def pe_up(self):
        self.output.write("pe.up\n")

    # initialize the base registers
    def initialize_base_registers(self):
    
        # ifmaps start from address 0 in memory
        self.ifmap_start_mem_addr = 0
        # filters follow ifmaps
        self.filter_start_mem_addr = self.ifmap_start_mem_addr + self.N*self.C*self.H*self.W*self.PE_DATA_WIDTH//self.GB_DATA_WIDTH;
        # psums follow filters
        self.psum_start_mem_addr = self.filter_start_mem_addr + self.M*self.C*self.R*self.S*self.PE_DATA_WIDTH//self.GB_DATA_WIDTH;
    
        # generate and write instructions into the output file
        self.ld_reg(0, self.ifmap_start_mem_addr)
        self.ld_reg(1, self.filter_start_mem_addr)
        self.ld_reg(2, self.psum_start_mem_addr)

    # load all ifmaps and filters from memory to global buffer
    def load_data_from_to_gb(self):

        # load ifmaps from memory to global buffer
        for ifmap_addr in range(self.ifmap_start_mem_addr, self.filter_start_mem_addr):
            self.gb_ld_ifmap(ifmap_addr, 0, ifmap_addr)

        # load filters from memory to global buffer
        for filter_addr in range(self.filter_start_mem_addr, self.psum_start_mem_addr):
            self.gb_ld_filter(filter_addr-self.filter_start_mem_addr, 1, filter_addr-self.filter_start_mem_addr)

    # multicast all filter data to all the PEs
    def multicast_all_filters(self):

        num_row_ids = self.r * self.t # total number of row IDs in FILTER IN NOC
        num_col_ids = self.R          # total number of column IDs in FILTER IN NOC

        # record each PE's filter scratchpad address for the next arriving data
        filter_write_addr = [[0 for j in range(0, num_col_ids)] for i in range(0, num_row_ids)]

        # these nested loops will go through every data in all the filters
        for tt in range(0, self.t):                     # -- each filter
            for pp in range(0, self.p):                 # -/
                for rr in range(0, self.r):             # -- each channel in the filter
                    for qq in range(0, self.q):         # -/
                        for RR in range(0, self.R):     # each row in the filter
                            for SS in range(0, self.S): # each column in the filter

                                # calculate which PEs this data will be multicasted to
                                row_id = tt*self.r + rr  # the row ID indicates the filter and channel
                                col_id = RR              # the column ID indicates the which row in the filter
        
                                # calulate this data's address in global buffer
                                cur_filter = tt*self.p + pp
                                cur_channel = rr*self.q + qq
                                cur_row = RR
                                cur_col = SS;
                                global_buffer_read_addr = cur_filter*self.C*self.R*self.S+cur_channel*self.R*self.S+cur_row*self.S+cur_col
        
                                # generate and write the instruction into the output file
                                self.pe_ld_filter(global_buffer_read_addr, row_id, col_id, filter_write_addr[row_id][col_id])

                                # increase the filter scratchpad address for next load
                                filter_write_addr[row_id][col_id] = filter_write_addr[row_id][col_id] + 1

    # multicast one data to all the PEs
    def multicast_one_ifmap(self, curr_W):

        num_row_ids = self.r # total number of row IDs in IFMAP IN NOC
        num_col_ids = self.H # total number of column IDs in IFMAP IN NOC

        # for each row ID and column ID
        for row_id in range(0, num_row_ids):
            for col_id in range(0, num_col_ids):

                # go through all n inputs and q channels, the height is indicated by column ID, the width is indicated by current load (curr_W)
                for nn in range(0, self.n):     # each input
                    for qq in range(0, self.q): # each channel

                        # calulate this data position (N,C,H,W) in the input
                        this_N = row_id//self.r + nn
                        this_C = row_id*self.q + qq
                        this_H = col_id
                        this_W = curr_W

                        # calculate this data's address in global buffer according to its position
                        global_buffer_read_addr = this_N*self.C*self.H*self.W+this_C*self.H*self.W+this_H*self.W+this_W

                        # calulate the target address in ifmap scratchpad
                        ifmap_addr = (self.next_ifmap_spad_entry+nn*self.q+qq) % self.PE_IFMAP_SPAD_ENTRIES

                        # generate and write the instruction into the output file
                        self.pe_ld_ifmap(global_buffer_read_addr, row_id, col_id, ifmap_addr)

        # increase the ifmap scratchpad entry address for next load (need to wrap around)
        self.next_ifmap_spad_entry = (self.next_ifmap_spad_entry+self.n*self.q) % self.PE_IFMAP_SPAD_ENTRIES

        # increase the start ifmap scratchpad entry address for MAC in the PEs
        self.curr_ifmap_spad_start = (self.curr_ifmap_spad_start+self.n*self.q) % self.PE_IFMAP_SPAD_ENTRIES

    # perform MAC in the PEs
    def compute(self):

        # the psum addr to store
        psum_addr = -1
        
        # all multiplications
        for nn in range(0, self.n):              # each input
            for pp in range(0, self.p):          # each filter
                for qq in range(0, self.q):      # each channel
                    for SS in range(0, self.S):  # each width
        
                        # ifmap scratchpad address
                        ifmap_read_addr = (self.curr_ifmap_spad_start + SS*self.n*self.q+nn*self.q+qq) % self.PE_IFMAP_SPAD_ENTRIES

                        # filter scratchpad address
                        filter_read_addr = pp*self.q*self.S+qq*self.S+SS

                        # compute
                        if (qq == 0) and (SS == 0):
                            psum_addr = psum_addr + 1
                            self.pe_mult_s(ifmap_read_addr, filter_read_addr, psum_addr)
                        else:
                            self.pe_madd_s(ifmap_read_addr, filter_read_addr, psum_addr, psum_addr)

    # accumulate psums vertically
    def accumulate(self):

        # number of psums in each PE
        num_psum = self.n * self.p

        # accumulate each of the psums
        for i in range(0, num_psum):
            self.pe_acc_so(i, num_psum+i)
            self.pe_up()

    # send data back from PEs to global buffer
    def writeback(self, curr_F):

        num_row_ids = self.t * self.s # total number of row IDs in PSUM OUT NOC
        num_col_ids = self.E          # total number of column IDs in PSUM OUT NOC

        # for each PEs in PSUM OUT NOC
        for row_id in range(0, num_row_ids):
            for col_id in range(0, num_col_ids):

                # go through all accumulated psums
                for nn in range(0, self.n):
                    for pp in range(0, self.p):

                        # calulate this data position (N,M,E,F) in the output
                        this_N = row_id//self.r + nn
                        this_M = row_id * self.p + pp
                        this_E = col_id
                        this_F = curr_F

                        # calculate this data's address in global buffer according to its position
                        global_buffer_write_addr = this_N*self.M*self.E*self.F + this_M*self.E*self.F + this_E*self.F + this_F

                        # generate and write the instruction into the output file
                        self.pe_st_psum(global_buffer_write_addr, row_id, col_id)

# main function
def main():

    if len(sys.argv) < 4:
        print("Usage: python compiler.py <accelerator config> <input config> <output file name>")

    # construct the compiler
    compiler = Compiler(sys.argv[1], sys.argv[2], sys.argv[3])

    # initialize the base registers
    compiler.initialize_base_registers()

    # load data onto the chip
    compiler.load_data_from_to_gb()

    # load all filters into PEs, all filters must fit into the PEs
    compiler.multicast_all_filters()

    # first, load the data for one compute
    for i in range(0, compiler.S):
        compiler.multicast_one_ifmap(i)

    # do the first compute
    compiler.curr_ifmap_spad_start = 0
    compiler.compute()
    compiler.accumulate()
    compiler.writeback(0)

    # then, load and do the remaining computes
    for i in range(0, compiler.W-compiler.S):
        compiler.multicast_one_ifmap(i+compiler.S)
        compiler.compute()
        compiler.accumulate()
        compiler.writeback(i+1)

    # some nop at the end to empty pipeline
    for i in range(100):
        compiler.nop()

if __name__ == "__main__":
    main()
