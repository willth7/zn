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

void x86_rel(uint8_t* bin, uint64_t addr, uint64_t rddr, uint8_t typ) {
	if (typ == 1) { //rel imm
		uint8_t bn = addr;
		if (bin[bn] == 102) { //leg op
			bn += 1;
		}
		if (bin[bn] == 103) { //leg addr
			bn += 1;
		}
		if ((bin[bn] & 240) == 64) { //rex
			bn += 1;
		}
		if (bin[bn] == 15) {
			bn += 2;
		}
		else {
			bn += 1;
		}
		int8_t nddr = rddr - (bn + 1);
		bin[bn] = nddr;
	}
	else if (typ == 2) {
		uint8_t bn = addr;
		if (bin[bn] == 102) { //leg op
			bn += 1;
		}
		if (bin[bn] == 103) { //leg addr
			bn += 1;
		}
		if ((bin[bn] & 240) == 64) { //rex
			bn += 1;
		}
		if (bin[bn] == 15) {
			bn += 2;
		}
		else {
			bn += 1;
		}
		int16_t nddr = rddr - (bn + 2);
		bin[bn] = nddr;
		bin[bn + 1] = nddr >> 8;
	}
	else if (typ == 3) {
		uint8_t bn = addr;
		if (bin[bn] == 102) { //leg op
			bn += 1;
		}
		if (bin[bn] == 103) { //leg addr
			bn += 1;
		}
		if ((bin[bn] & 240) == 64) { //rex
			bn += 1;
		}
		if (bin[bn] == 15) {
			bn += 2;
		}
		else {
			bn += 1;
		}
		int32_t nddr = rddr - (bn + 4);
		bin[bn] = nddr;
		bin[bn + 1] = nddr >> 8;
		bin[bn + 2] = nddr >> 16;
		bin[bn + 3] = nddr >> 24;
	}
	else if (typ == 4) { //rel ip
		uint8_t bn = addr;
		if (bin[bn] == 102) { //leg op
			bn += 1;
		}
		if (bin[bn] == 103) { //leg addr
			bn += 1;
		}
		if ((bin[bn] & 240) == 64) { //rex
			bn += 1;
		}
		if (bin[bn] == 15) {
			bn += 3;
		}
		else {
			bn += 2;
		}
		int32_t nddr = rddr - (bn + 4);
		bin[bn] = nddr;
		bin[bn + 1] = nddr >> 8;
		bin[bn + 2] = nddr >> 16;
		bin[bn + 3] = nddr >> 24;
	}
}
