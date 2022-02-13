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
    cout << "==================" << endl;
    chip->get_gb()->debug_print_bank(GB_PSUM, 0);

    chip->get_gb()->debug_print_fifos();
    chip->get_array()->debug_print_fifos();
    chip->get_array()->get_PE(0, 0)->debug_print_fifos();
    chip->get_array()->get_PE(0, 0)->debug_print_spads(12);
    chip->get_array()->get_PE(1, 0)->debug_print_fifos();
    chip->get_array()->get_PE(1, 0)->debug_print_spads(12);
    chip->get_array()->get_PE(2, 0)->debug_print_fifos();
    chip->get_array()->get_PE(2, 0)->debug_print_spads(12);
    chip->get_array()->get_PE(3, 0)->debug_print_fifos();
    chip->get_array()->get_PE(3, 0)->debug_print_spads(12);
    chip->get_array()->get_PE(4, 0)->debug_print_fifos();
    chip->get_array()->get_PE(4, 0)->debug_print_spads(12);
    chip->get_array()->get_PE(5, 0)->debug_print_fifos();
    chip->get_array()->get_PE(5, 0)->debug_print_spads(12);
    chip->get_array()->get_PE(6, 0)->debug_print_fifos();
    chip->get_array()->get_PE(6, 0)->debug_print_spads(12);
    chip->get_array()->get_PE(7, 0)->debug_print_fifos();
    chip->get_array()->get_PE(7, 0)->debug_print_spads(12);
    chip->get_array()->get_PE(8, 0)->debug_print_fifos();
    chip->get_array()->get_PE(8, 0)->debug_print_spads(12);

    delete chip;
    
    return 0;
}
