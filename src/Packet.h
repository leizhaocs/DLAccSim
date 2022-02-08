#ifndef PACKET_H
#define PACKET_H

#include "includes.h"

/* one data packet */
class Packet
{
public:
    /* constructor */
    Packet(int size);

    /* destructor */
    ~Packet();

    /* copy */
    void copy(Packet *p);

    /* copy from/into an offset of the data */
    void copy(int offset, Packet *p);

    /* 0: select p1, 1: select p2 */
    void select(Packet *p1, Packet *p2, int sel);

    /* add p1 and p2 */
    void add(Packet *p1, Packet *p2);

    /* multiply p1 and p2 (stage 1) */
    void mult1(Packet *p1, Packet *p2);

    /* multiply p1 and p2 (stage 2) */
    void mult2(Packet *p);

    /* quantize p */
    void quantize(Packet *p);

    /* get a specific byte of the data */
    unsigned char data(int byte);

    /* set a specific byte of the data */
    void data(int byte, unsigned char c);

    /* getters */
    int get_size();
    unsigned char *get_data();
    int get_row_id();
    int get_col_id();
    GB_REGION get_gb_region();
    int get_gb_bank();
    int get_gb_entry();
    int get_gb_offset();
    int get_mem_addr();

    /* setters */
    void set_size(int size);
    void set_data(unsigned char *data);
    void set_row_id(int row_id);
    void set_col_id(int col_id);
    void set_gb_region(GB_REGION gb_region);
    void set_gb_bank(int gb_bank);
    void set_gb_entry(int gb_entry);
    void set_gb_offset(int gb_offset);
    void set_mem_addr(int mem_addr);

    /* print the content of this packet for debug */
    void debug_print();

private:
    int size_;               // size of the packet in bytes
    unsigned char *data_;    // the actual data in the packet

    int row_id_;             // destination of this packet to PE
    int col_id_;             // destination of this packet to PE

    GB_REGION gb_region_;    // destination of this packet to global buffer, which region of the global memory
    int gb_bank_;            // destination of this packet to global buffer, which bank
    int gb_entry_;           // destination of this packet to global buffer, which entry in the bank
    int gb_offset_;          // destination of this packet to global buffer, which offset in the entry, offset is based on PE_DATA_WIDTH not byte

    int mem_addr_;           // destination of this packet to off-chip memory
};

#endif
