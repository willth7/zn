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

void arm_32_rel(uint8_t* bin, uint64_t addr, uint64_t rddr, uint8_t typ) {
	if (typ == 1) {
		rddr = ((rddr - addr) - 4) / 2;
		bin[addr] |= rddr;
	}
	else if (typ == 2) {
		rddr = ((rddr - addr) - 2) / 4;
		bin[addr] |= rddr;
	}
	else if (typ == 3) {
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
}