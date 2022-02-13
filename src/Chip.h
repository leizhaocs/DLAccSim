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

#ifndef CHIP_H
#define CHIP_H

#include "includes.h"

/* the entire chip */
class Chip
{
public:
    /* constructor */
    Chip(char *memory_filename);

    /* destructor */
    ~Chip();

    /* get global buffer */
    GB *get_gb();

    /* get array */
    Array *get_array();

    /* map the NOC IDs */
    void map(NOC_TYPE noc_type, char *filename);

    /* configure the global buffer */
    void configure_gb(int num_ifmap_banks, int num_filter_banks, int num_psum_banks);

    /* configure the PEs */
    void configure_pes();

    /* open instruction file */
    void open_instruction_file(char *filename);

    /* run one clock, return false if finished */
    bool tick();

private:
    Controller *controller_;              // the controller

    GB *gb_;                              // the global buffer

    Array *array_;                        // the PE array

    Memory *mem_;                         // off-chip memory
};

#endif
