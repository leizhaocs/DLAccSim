#ifndef PE_H
#define PE_H

#include "includes.h"

/* processing element */
class PE
{
public:
    /* constructor */
    PE(int row, int col, PE *above_pe);

    /* destructor */
    ~PE();

    /* get active */
    bool get_active();

    /* set active */
    void set_active(bool active);

    /* get is_top_of_set_ */
    bool get_is_top_of_set();

    /* set is_top_of_set_ */
    void set_is_top_of_set(bool is_top_of_set);

    /* get is_bottom_of_set_ */
    bool get_is_bottom_of_set();

    /* set is_bottom_of_set_ */
    void set_is_bottom_of_set(bool is_bottom_of_set);

    /* get instruction queue */
    InstQueue *instQueue();

    /* get FIFOs */
    FIFO *ifmap_fifo_in();
    FIFO *filter_fifo_in();
    FIFO *psum_fifo_in();
    FIFO *psum_fifo_out();

    /* check if this PE is ready to send data back to the global buffer */
    bool ready_send_back_to_gb();

    /* check if this PE is ready to accept data from below PE */
    bool can_accept_from_below();

    /* issue an instruction from the instruction queue */
    void issue_inst();

    /* run one clock */
    void tick();

    /* print an entry of all scratchpads for debug */
    void debug_print_spads(int num_entries);

    /* print the content of FIFOs for debug */
    void debug_print_fifos();

private:
    int row_;                        // the row of this PE in the array, this is the physcial position, not the same as NOC's reconfigurable row ID
    int col_;                        // the column of this PE in the array, this is the physcial position, not the same as NOC's reconfigurable column ID

    PE *above_pe_;                   // the above PE that will accept data from this PE for accumulation

    bool active_;                    // whether if this PE is activate
    bool is_top_of_set_;             // whether if this PE is in the first row of a set
    bool is_bottom_of_set_;          // whether if this PE is in the last row of a set

    InstQueue *instQueue_;           // the instruction queue

    SPad *ifmap_spad_;               // ifmap scratchpad
    SPad *filter_spad_;              // filter scratchpad
    SPad *psum_spad_;                // psum scratchpad

    FIFO *ifmap_fifo_in_;            // ifmap input FIFO
    FIFO *filter_fifo_in_;           // filter input FIFO
    FIFO *psum_fifo_in_;             // psum input FIFO
    FIFO *psum_fifo_out_;            // psum output FIFO

    Packet *packet_ifmap_read_;      // data read from ifmap scratchpad
    Packet *packet_filter_read_;     // data read from filter scratchpad
    Packet *packet_psum_read_;       // data read from psum scratchpad
    Packet *packet_mult1_;           // result of first stage of multiplication
    Packet *packet_mult2_;           // result of second stage of multiplication
    Packet *packet_quant_;           // result of quantization
    Packet *packet_add_mux_;         // after selection between multiplication result and psum by add_mux_
    Packet *packet_psum_read_mux_;   // after selection between data read from psum scratchpad and 0 by psum_read_mux_
    Packet *packet_add_;             // result of addition
    Packet *packet_zero_;            // a packet of PE_DATA_WIDTH bytes of zeros

    int opcode1_;                    // opcode
    int opcode2_;                    // opcode
    int opcode3_;                    // opcode
    int opcode4_;                    // opcode
    int ifmap_read_en_1_;            // ifmap scratchpad read enable
    int ifmap_read_addr_1_;          // ifmap scratchpad read address
    int ifmap_write_en_1_;           // ifmap scratchpad write enable
    int ifmap_write_addr_1_;         // ifmap scratchpad write address
    int filter_read_en_1_;           // filter scratchpad read enable
    int filter_read_addr_1_;         // filter scratchpad read address
    int filter_write_en_1_;          // filter scratchpad write enable
    int filter_write_addr_1_;        // filter scratchpad write address
    int psum_read_en_1_;             // psum scratchpad read enable
    int psum_read_addr_1_;           // psum scratchpad read address
    int psum_write_en_1_;            // psum scratchpad write enable
    int psum_write_en_2_;            // psum scratchpad write enable
    int psum_write_en_3_;            // psum scratchpad write enable
    int psum_write_en_4_;            // psum scratchpad write enable
    int psum_write_addr_1_;          // psum scratchpad write address
    int psum_write_addr_2_;          // psum scratchpad write address
    int psum_write_addr_3_;          // psum scratchpad write address
    int psum_write_addr_4_;          // psum scratchpad write address
    int add_mux_1_;                  // 0: 0; 1: multiplication results of ifmap and filter; 2: data read from psum in FIFO
    int add_mux_2_;                  // 0: 0; 1: multiplication results of ifmap and filter; 2: data read from psum in FIFO
    int add_mux_3_;                  // 0: 0; 1: multiplication results of ifmap and filter; 2: data read from psum in FIFO
    int psum_read_mux_1_;            // 0: 0; 1: data read from psum scratchpad memory
    int psum_read_mux_2_;            // 0: 0; 1: data read from psum scratchpad memory
    int send_psum_out_1_;            // 0: do not save result; 1: write result into psum out FIFO; 2: send data to above PE
    int send_psum_out_2_;            // 0: do not save result; 1: write result into psum out FIFO; 2: send data to above PE
    int send_psum_out_3_;            // 0: do not save result; 1: write result into psum out FIFO; 2: send data to above PE
    int send_psum_out_4_;            // 0: do not save result; 1: write result into psum out FIFO; 2: send data to above PE
};

#endif
