#ifndef ARRAY_H
#define ARRAY_H

#include "includes.h"

/* the array of PEs */
class Array
{
public:
    /* constructor */
    Array();

    /* Destructor */
    ~Array();

    /* get a specific PE */
    PE *get_PE(int row, int col);

    /* get a specific row MC */
    MC *get_row_MC(NOC_TYPE noc_type, int row);

    /* get FIFOs */
    FIFO *ifmap_fifo_in();
    FIFO *filter_fifo_in();
    FIFO *psum_fifo_in();
    FIFO *psum_fifo_out();

    /* check if all desination PEs are ready to accept new data for a specific NOC, only used for NOC_TYPE_IFMAP_IN, NOC_TYPE_FILTER_IN and NOC_TYPE_PSUM_IN */
    bool multicast_ready(NOC_TYPE noc_type, int row_id, int col_id);

    /* multicast a packet from FIFO to destination PEs through a specific NOC, only used for NOC_TYPE_IFMAP_IN, NOC_TYPE_FILTER_IN and NOC_TYPE_PSUM_IN */
    void multicast(NOC_TYPE noc_type, Packet *p, int row_id, int col_id);

    /* check if all destination PEs are ready to accept new ld instruction, only used for NOC_TYPE_IFMAP_IN, NOC_TYPE_FILTER_IN and NOC_TYPE_PSUM_IN */
    bool issue_ld_ready(NOC_TYPE noc_type, int row_id, int col_id);

    /* issue a new ld instruction to all destination PEs, only used for NOC_TYPE_IFMAP_IN, NOC_TYPE_FILTER_IN and NOC_TYPE_PSUM_IN */
    void issue_ld(NOC_TYPE noc_type, Instruction *inst, int row_id, int col_id);

    /* check if all active PEs are ready to accept new compute instruction */
    bool issue_compute_ready();

    /* issue a new compute instruction to all active PEs */
    void issue_compute(Instruction *inst);

    /* check if all destination PEs are ready to send data to adjacent PE above it */
    bool issue_up_ready();

    /* send data to adjacent PE above it */
    void issue_up();

    /* print the IDs of MCs for debug */
    void debug_print_noc(NOC_TYPE noc_type);

    /* print the active PEs for debug */
    void debug_print_active();

    /* print the bottom of set PEs for debug */
    void debug_print_bottom_of_set();

    /* print the content of FIFOs for debug */
    void debug_print_fifos();

private:
    vector<vector<PE *> > pes_;           // all PEs in the array

    vector<MC *> row_mcs_ifmap_in_;       // the ifmap in NOC
    vector<MC *> row_mcs_filter_in_;      // the filter in NOC
    vector<MC *> row_mcs_psum_in_;        // the psum in NOC
    vector<MC *> row_mcs_psum_out_;       // the psum out NOC

    FIFO *ifmap_fifo_in_;                 // ifmap in FIFO
    FIFO *filter_fifo_in_;                // filter in FIFO
    FIFO *psum_fifo_in_;                  // psum in FIFO
    FIFO *psum_fifo_out_;                 // psum out FIFO
};

#endif
