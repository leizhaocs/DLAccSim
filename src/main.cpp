#include "includes.h"

int main(int argc, char **argv)
{
    // step 1: construct the chip
    Chip *chip = new Chip(argv[1]);

    // step 2: map the NOCs
    chip->map(NOC_TYPE_IFMAP_IN, argv[2]);
    chip->map(NOC_TYPE_FILTER_IN, argv[3]);
    chip->map(NOC_TYPE_PSUM_IN, argv[4]);
    chip->map(NOC_TYPE_PSUM_OUT, argv[5]);

    // step 3: configure the global buffer
    chip->configure_gb(12, 2, 12);

    // step 4: configure the PEs
    chip->configure_pes();

    cout << "==================================" <<endl;
    cout << "IFMAP IN" <<endl;
    chip->get_array()->debug_print_noc(NOC_TYPE_IFMAP_IN);
    cout << "==================================" <<endl;
    cout << "FILTER IN" <<endl;
    chip->get_array()->debug_print_noc(NOC_TYPE_FILTER_IN);
    cout << "==================================" <<endl;
    cout << "PSUM IN" <<endl;
    chip->get_array()->debug_print_noc(NOC_TYPE_PSUM_IN);
    cout << "==================================" <<endl;
    cout << "PSUM OUT" <<endl;
    chip->get_array()->debug_print_noc(NOC_TYPE_PSUM_OUT);
    cout << "==================================" <<endl;
    cout << "ACTIVE" <<endl;
    chip->get_array()->debug_print_active();
    cout << "==================================" <<endl;
    cout << "TOP OF SET" <<endl;
    chip->get_array()->debug_print_top_of_set();
    cout << "==================================" <<endl;
    cout << "BOTTOM OF SET" <<endl;
    chip->get_array()->debug_print_bottom_of_set();

    // step 5: read instructions
    chip->open_instruction_file(argv[6]);

    // step 6: run
    for (int i = 0; i < 5000; i++)
    {
        bool running = chip->tick();
        if (running == false)
        {
            break;
        }
    }

    //chip->get_gb()->debug_print_bank(GB_IFMAP, 0);
    cout << "==================" << endl;
    //chip->get_gb()->debug_print_bank(GB_FILTER, 0);

    chip->get_gb()->debug_print_fifos();
    chip->get_array()->debug_print_fifos();
    chip->get_array()->get_PE(0, 0)->debug_print_fifos();
    chip->get_array()->get_PE(0, 0)->debug_print_spads(12);
    chip->get_array()->get_PE(0, 1)->debug_print_fifos();
    chip->get_array()->get_PE(0, 1)->debug_print_spads(12);
    chip->get_array()->get_PE(0, 2)->debug_print_fifos();
    chip->get_array()->get_PE(0, 2)->debug_print_spads(12);
    chip->get_array()->get_PE(0, 3)->debug_print_fifos();
    chip->get_array()->get_PE(0, 3)->debug_print_spads(12);
    chip->get_array()->get_PE(0, 4)->debug_print_fifos();
    chip->get_array()->get_PE(0, 4)->debug_print_spads(12);
    chip->get_array()->get_PE(0, 5)->debug_print_fifos();
    chip->get_array()->get_PE(0, 5)->debug_print_spads(12);
    chip->get_array()->get_PE(0, 6)->debug_print_fifos();
    chip->get_array()->get_PE(0, 6)->debug_print_spads(12);
    chip->get_array()->get_PE(0, 7)->debug_print_fifos();
    chip->get_array()->get_PE(0, 7)->debug_print_spads(12);
    chip->get_array()->get_PE(0, 8)->debug_print_fifos();
    chip->get_array()->get_PE(0, 8)->debug_print_spads(12);
    chip->get_array()->get_PE(0, 9)->debug_print_fifos();
    chip->get_array()->get_PE(0, 9)->debug_print_spads(12);

    delete chip;
    
    return 0;
}
