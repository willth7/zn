//   Copyright 2022 Will Thomas
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0;
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.

//   gloria in excelsis deo

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void avr_rel(uint8_t* bin, uint64_t addr, uint64_t rddr, uint8_t typ) {
	if (typ == 1) {
		bin[addr + 2] = rddr;
		bin[addr + 3] = rddr >> 8;
	}
	else if (typ == 2) {
		rddr /= 2;
		bin[addr + 2] = rddr;
		bin[addr + 3] = rddr >> 8;
	}
	else if (typ == 3) {
		rddr = ((rddr - addr) / 2) - 1;
		rddr |= (rddr >> 1) & 2048;
		bin[addr] |= rddr;
		bin[addr + 1] |= rddr >> 8;
	}
	else if (typ == 4) {
		rddr = ((rddr - addr) / 2) - 1;
		rddr |= (rddr >> 1) & 64;
		bin[addr] |= rddr << 3;
		bin[addr + 1] |= rddr >> 5;
	}
	else if (typ == 5) {
		bin[addr] |= rddr & 15;
		bin[addr + 1] |= (rddr >> 4) & 15;
	}
	else if (typ == 133) {
		bin[addr] |= (rddr >> 8) & 15;
		bin[addr + 1] |= (rddr >> 12) & 15;
	}
	else if (typ == 6) {
		bin[addr] |= rddr & 15;
		bin[addr] |= (rddr << 2) & 192;
	}
	else if (typ == 134) {
		bin[addr] |= (rddr >> 8) & 15;
		bin[addr + 1] |= (rddr >> 6) & 192;
	}
}
