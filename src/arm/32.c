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

void arm_32_rel(uint8_t* bin, uint64_t addr, uint64_t rddr, uint8_t typ, uint64_t* sym) {
	if (typ == 1) {
		if (((rddr - addr) - 4) % 2) {
			printf("error: symbol '%s' out of alignment\n", sym);
		}
		else if ((((int64_t) (rddr - addr) - 4) / 2) > 127 || (((int64_t) (rddr - addr) - 4) / 2) < -128) {
			printf("error: symbol '%s' out of range\n", sym);
		}
		
		rddr = ((rddr - addr) - 4) / 2;
		bin[addr] |= rddr;
	}
	else if (typ == 2) {
		if (((rddr - addr) - 2) % 2) {
			printf("error: symbol '%s' out of alignment\n", sym);
		}
		else if ((((rddr - addr) - 2) / 4) > 255) {
			printf("error: symbol '%s' out of range\n", sym);
		}
		
		rddr = ((rddr - addr) - 2) / 4;
		bin[addr] |= rddr;
	}
	else if (typ == 3) {
		if (((rddr - addr) - 4) % 2) {
			printf("error: symbol '%s' out of alignment\n", sym);
		}
		else if ((((int64_t) (rddr - addr) - 4) / 2) > 1023 || (((int64_t) (rddr - addr) - 4) / 2) < -1024) {
			printf("error: symbol '%s' out of range\n", sym);
		}
		
		int16_t nddr = ((rddr - addr) - 4) / 2;
		nddr |= (nddr >> 5) & 1024;
		bin[addr] |= nddr;
		bin[addr + 1] |= (nddr >> 8) & 7;
	}
	else if (typ == 4) {
		int32_t nddr = ((rddr - addr) - 4) / 2;
		nddr |= (nddr >> 8) & 8388608;
		bin[addr + 2] |= nddr;
		bin[addr + 3] |= (nddr >> 8) & 7;
		bin[addr] |= nddr >> 11;
		bin[addr + 1] |= (nddr >> 19) & 3;
		bin[addr + 1] |= (nddr >> 23) & 4;
	}
	else if (typ == 5) {
		int32_t nddr = ((rddr - addr) - 8) / 4;
		nddr |= (nddr >> 8) & 8388608;
		bin[addr] |= nddr;
		bin[addr + 1] |= (nddr >> 8);
		bin[addr + 2] |= (nddr >> 16);
	}
	else if (typ == 6) {
		int16_t nddr = (rddr - addr) - 8;
		bin[addr + 2] |= (~nddr >> 8) & 128;
		if (nddr < 0) {
			nddr *= -1;
		}
		bin[addr] |= nddr;
		bin[addr + 1] |= (nddr >> 8) & 15;
	}
	else if (typ == 7) {
		int16_t nddr = (rddr - addr) - 8;
		bin[addr + 2] |= (~nddr >> 8) & 128;
		if (nddr < 0) {
			nddr *= -1;
		}
		bin[addr] |= nddr & 15;
		bin[addr + 1] |= (nddr >> 4) & 15;
	}
	else if (typ == 8) {
		int16_t nddr = (rddr - addr) - 8;
		if (nddr < 0) {
			nddr *= -1;
			bin[addr + 2] |= 64;
		}
		else {
			bin[addr + 2] |= 128;
		}
		bin[addr] |= nddr;
		bin[addr + 1] |= (nddr >> 8) & 15;
	}
}
