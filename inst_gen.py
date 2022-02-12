# global buffer
GB_DATA_WIDTH           =     8
GB_DATA_WIDTH_LOG2      =     3
GB_BANKS                =     26
GB_BANK_ENTRIES         =     512

# array
ARRAY_ROWS              =     12
ARRAY_COLUMNS           =     14

# PE
PE_DATA_WIDTH           =     2
PE_IFMAP_SPAD_ENTRIES   =     12
PE_FILTER_SPAD_ENTRIES  =     224
PE_PSUM_SPAD_ENTRIES    =     24

# off-chip memory
MEM_SIZE                =     106496

N = 4
C = 4
H = 10
W = 10
M = 4
R = 2
S = 2

E = H - (R - 1)
F = W - (S - 1)

n = 2;  # n inputs can be put in the same PE
q = 2;  # q channles can be put in the same PE
p = 2;  # p filters can be put in the same PE

r = 2;  # r replecates for channel dimension (C//q)
t = 2;  # t replicates for filter dimention (M//p)

def nop(f):
    f.write("nop\n")
def ld_reg(f, base_reg, memory_addr):
    f.write("ld.reg "+str(base_reg)+" "+str(memory_addr)+"\n")
def gb_ld_ifmap(f, global_buffer_write_addr, base_reg, memory_read_addr):
    f.write("gb.ld.ifmap "+str(global_buffer_write_addr)+" "+str(base_reg)+" "+str(memory_read_addr)+"\n")
def gb_ld_filter(f, global_buffer_write_addr, base_reg, memory_read_addr):
    f.write("gb.ld.filter "+str(global_buffer_write_addr)+" "+str(base_reg)+" "+str(memory_read_addr)+"\n")
def gb_st_psum(f, global_buffer_read_addr, base_reg, memory_write_addr):
    f.write("gb.st.psum "+str(global_buffer_read_addr)+" "+str(base_reg)+" "+str(memory_write_addr)+"\n")
def pe_ld_ifmap(f, global_buffer_read_addr, row_id, col_id, ifmap_write_addr):
    f.write("pe.ld.ifmap "+str(global_buffer_read_addr)+" "+str(row_id)+" "+str(col_id)+" "+str(ifmap_write_addr)+"\n")
def pe_ld_filter(f, global_buffer_read_addr, row_id, col_id, filter_write_addr):
    f.write("pe.ld.filter "+str(global_buffer_read_addr)+" "+str(row_id)+" "+str(col_id)+" "+str(filter_write_addr)+"\n")
def pe_ld_psum(f, global_buffer_read_addr, row_id, col_id, psum_write_addr):
    f.write("pe.ld.psum "+str(global_buffer_read_addr)+" "+str(row_id)+" "+str(col_id)+" "+str(psum_write_addr)+"\n")
def pe_st_psum(f, global_buffer_write_addr, row_id, col_id):
    f.write("pe.st.psum "+str(global_buffer_write_addr)+" "+str(row_id)+" "+str(col_id)+"\n")
def pe_mult_s(f, ifmap_read_addr, filter_read_addr, psum_write_addr):
    f.write("pe.mult.s "+str(ifmap_read_addr)+" "+str(filter_read_addr)+" "+str(psum_write_addr)+"\n")
def pe_mult_o(f, ifmap_read_addr, filter_read_addr):
    f.write("pe.mult.o "+str(ifmap_read_addr)+" "+str(filter_read_addr)+"\n")
def pe_mult_so(f, ifmap_read_addr, filter_read_addr, psum_write_addr):
    f.write("pe.mult.so "+str(ifmap_read_addr)+" "+str(filter_read_addr)+" "+str(psum_write_addr)+"\n")
def pe_madd_s(f, ifmap_read_addr, filter_read_addr, psum_read_addr, psum_write_addr):
    f.write("pe.madd.s "+str(ifmap_read_addr)+" "+str(filter_read_addr)+" "+str(psum_read_addr)+" "+str(psum_write_addr)+"\n")
def pe_madd_o(f, ifmap_read_addr, filter_read_addr, psum_read_addr):
    f.write("pe.madd.o "+str(ifmap_read_addr)+" "+str(filter_read_addr)+" "+str(psum_read_addr)+"\n")
def pe_madd_so(f, ifmap_read_addr, filter_read_addr, psum_read_addr, psum_write_addr):
    f.write("pe.madd.so "+str(ifmap_read_addr)+" "+str(filter_read_addr)+" "+str(psum_read_addr)+" "+str(psum_write_addr)+"\n")
def pe_add_s(f, psum_read_addr, psum_write_addr):
    f.write("pe.add.s "+str(psum_read_addr)+" "+str(psum_write_addr)+"\n")
def pe_add_o(f, psum_read_addr):
    f.write("pe.add.o "+str(psum_read_addr)+"\n")
def pe_add_so(f, psum_read_addr, psum_write_addr):
    f.write("pe.add.so "+str(psum_read_addr)+" "+str(psum_write_addr)+"\n")
def pe_acc_o(f, psum_read_addr):
    f.write("pe.acc.o "+str(psum_read_addr)+"\n")
def pe_acc_so(f, psum_read_addr, psum_write_addr):
    f.write("pe.acc.so "+str(psum_read_addr)+" "+str(psum_write_addr)+"\n")
def pe_up(f):
    f.write("pe.up\n")

f = open("ins.txt", "w")

# write base registers

ifmap_start_addr = 0
filter_start_addr = ifmap_start_addr + N*C*H*W*PE_DATA_WIDTH//GB_DATA_WIDTH;
psum_start_addr = filter_start_addr + M*C*R*S*PE_DATA_WIDTH//GB_DATA_WIDTH;

ld_reg(f, 0, ifmap_start_addr)
ld_reg(f, 1, filter_start_addr)
ld_reg(f, 2, psum_start_addr)

# load ifmap from memory to global buffer

for ifmap_addr in range(0, filter_start_addr):
    gb_ld_ifmap(f, ifmap_addr, 0, ifmap_addr)

# load filter from memory to global buffer

for filter_addr in range(filter_start_addr, psum_start_addr):
    gb_ld_filter(f, filter_addr-filter_start_addr, 1, filter_addr-filter_start_addr)

# multicast filter to PEs

num_row_ids = r*t
num_col_ids = R

filter_write_addr = [[0 for j in range(0, num_col_ids)] for i in range(0, num_row_ids)]

for tt in range(0, t):
    for pp in range(0, p):
        for rr in range(0, r):
            for qq in range(0, q):
                for RR in range(0, R):
                    for SS in range(0, S):
                        row_id = tt*r + rr
                        col_id = RR

                        cur_filter = tt*p + pp
                        cur_channel = rr*q + qq
                        cur_row = RR
                        cur_col = SS;
                        global_buffer_read_addr = cur_filter*C*R*S+cur_channel*R*S+cur_row*S+cur_col

                        pe_ld_filter(f, global_buffer_read_addr, row_id, col_id, filter_write_addr[row_id][col_id])

                        filter_write_addr[row_id][col_id] = filter_write_addr[row_id][col_id] + 1

# multicast ifmap to PEs

num_row_ids = r
num_col_ids = H

ifmap_write_addr = [[0 for j in range(0, num_col_ids)] for i in range(0, num_row_ids)]

for WW in range(0, W):
    for nn in range(0, n):
        for qq in range (0, q):
            for rr in range (0, r):
                for HH in range(0, H):
                    row_id = rr;
                    col_id = HH;

                    cur_input = nn;
                    cur_channel = rr*q + qq
                    cur_row = HH
                    cur_col = WW
                    global_buffer_read_addr = cur_input*C*H*W+cur_channel*H*W+cur_row*W+cur_col

                    if ifmap_write_addr[row_id][col_id] >= PE_IFMAP_SPAD_ENTRIES:
                        continue

                    pe_ld_ifmap(f, global_buffer_read_addr, row_id, col_id, ifmap_write_addr[row_id][col_id])

                    ifmap_write_addr[row_id][col_id] = ifmap_write_addr[row_id][col_id] + 1

# compute

psum_addr = -1

for nn in range(0, n):
    for pp in range(0, p):
        for qq in range(0, q):
            for SS in range(0, S):

                ifmap_read_addr = SS*n*q+nn*q+qq

                filter_read_addr = pp*q*S+qq*S+SS

                if (qq == 0) and (SS == 0):
                    psum_addr = psum_addr + 1
                    pe_mult_s(f, ifmap_read_addr, filter_read_addr, psum_addr)
                else:
                    pe_madd_s(f, ifmap_read_addr, filter_read_addr, psum_addr, psum_addr)

# accumulate

num_output = n * p

for i in range(0, num_output):
    pe_acc_so(f, i, num_output+i)
    pe_up(f)

# send data back to global buffer

num_row_ids = t
num_col_ids = E

for i in range(0, num_row_ids):
    for j in range(0, num_col_ids):
        for k in range(0, n):
            for l in range(0, p):
                out = k
                channel = i * p + l
                height = j
                width = 0
                addr = out*M*E*F + channel*E*F + height*F + width
                pe_st_psum(f, addr, i, j)

# some nop at the end to empty pipeline

for i in range(100):
    nop(f)

f.close()
