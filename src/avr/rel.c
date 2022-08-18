//   Copyright 2022 Will Thomas
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.

#include <stdint.h>

void avr_rel(uint8_t* bits, uint16_t addr, uint16_t rddr, uint8_t type) {
	if (type == 2) {
		bits[0] &= 7;
		bits[0] |= (rddr - addr) >> 4;
		bits[1] &= 252;
		bits[1] |= ((rddr - addr) >> 6) & 3;
	}
	else if (type == 3) { //todo
		bits[0] = 0;
		bits[0] |= ((rddr - addr) / 2);
		bits[1] &= 240;
		bits[1] |= (((rddr - addr) / 2) >> 8 ) & 15;
	}
	else if (type == 4) {
		//todo
	}
	else if (type == 18) {
		bits[2] = rddr;
		bits[3] = rddr >> 8;
	}
	else if (type == 19) {
		bits[0] &= 240;
		bits[0] |= rddr & 15;
		bits[1] &= 240;
		bits[1] |= (rddr >> 4) & 15;
	}
	else if (type == 21) {
		bits[0] &= 48;
		bits[0] |= rddr & 15;
		bits[0] |= (rddr << 2) & 192;
	}
}
