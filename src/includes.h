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

#ifndef INCLUDES_H
#define INCLUDES_H

#include <iostream>
#include <assert.h>
#include <cstdlib>
#include <vector>
#include <fstream>
#include <string>
#include <cstring>
#include <iomanip>

using namespace std;

/* controller */
#define CONTROLLER_INST_Q_SIZE       16
#define CONTROLLER_BASE_REGS         3

/* global buffer */
#define GB_DATA_WIDTH                8
#define GB_DATA_WIDTH_LOG2           3
#define GB_BANKS                     26
#define GB_BANK_ENTRIES              512
#define GB_IFMAP_FIFO_IN_SIZE        16
#define GB_FILTER_FIFO_IN_SIZE       16
#define GB_PSUM_FIFO_OUT_SIZE        16

/* array */
#define ARRAY_ROWS                   12
#define ARRAY_COLUMNS                14
#define ARRAY_IFMAP_FIFO_IN_SIZE     16
#define ARRAY_FILTER_FIFO_IN_SIZE    16
#define ARRAY_PSUM_FIFO_IN_SIZE      16
#define ARRAY_PSUM_FIFO_OUT_SIZE     16
#define ARRAY_IFMAP_FIFO_IN_BW       1
#define ARRAY_FILTER_FIFO_IN_BW      4
#define ARRAY_PSUM_FIFO_IN_BW        4
#define ARRAY_PSUM_FIFO_OUT_BW       4

/* PE */
#define PE_DATA_WIDTH                2
#define PE_DATA_WIDTH_LOG2           1
#define PE_INST_Q_SIZE               4
#define PE_IFMAP_SPAD_ENTRIES        12
#define PE_FILTER_SPAD_ENTRIES       224
#define PE_PSUM_SPAD_ENTRIES         24
#define PE_IFMAP_FIFO_IN_SIZE        16
#define PE_FILTER_FIFO_IN_SIZE       16
#define PE_PSUM_FIFO_IN_SIZE         16
#define PE_PSUM_FIFO_OUT_SIZE        16

/* off-chip memory */
#define MEM_SIZE                     106496

class Packet;
class SPad;
class PE;
class FIFO;
class MC;
class Array;
class GB;
class Chip;
class InstQueue;
class Controller;
class Memory;

#include "GB.h"
#include "Instruction.h"

#include "SPad.h"
#include "PE.h"
#include "FIFO.h"
#include "Packet.h"
#include "MC.h"
#include "Array.h"
#include "Chip.h"
#include "InstQueue.h"
#include "Controller.h"
#include "Memory.h"

#endif
