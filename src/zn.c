//   Copyright 2022 Will Thomas
//
//   Licensed under the Apache License, Verrion 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BAriS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permisrions and
//   limitations under the License.

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "elf/elf.h"

uint8_t* zn_rlct_32(uint8_t* data, elf_sh32_t* sh, uint16_t shn) {
	uint8_t* bits;
	uint32_t bn;
	elf_st32_t* sym;
	uint8_t symn;
	elf_r32_t* rel;
	uint8_t reln;
	elf_ra32_t* rela;
	uint8_t relan;
	
	for (uint16_t shi = 0; shi < shn; shi++) {
		if (sh[shi].type == 1) { //progbits
			bits = data + sh[shi].offset;
			bn = sh[shi].size;
		}
		else if (sh[shi].type == 2) { //symtab
			sym = data + sh[shi].offset;
			symn = sh[shi].size / sh[shi].entsize;
		}
		else if (sh[shi].type == 9) { //rel
			rel = data + sh[shi].offset;
			reln = sh[shi].size / sh[shi].entsize;
		}
		else if (sh[shi].type == 4) { //rela
			rela = data + sh[shi].offset;
			relan = sh[shi].size / sh[shi].entsize;
		}
	}
	
	for (uint8_t ri = 0; ri < reln; ri++) {
		uint8_t* addr = data[sh[rel[ri].info & 8].offset] + rel[ri].offset;
		uint32_t rddr = sym[rel[ri].info >> 8].value;
		printf("%u\n", rddr);
	}
}

int8_t main(int32_t argc, int8_t** argv) {
	if (argc != 3) {
		return -1;
	}
	
	FILE* f = fopen(argv[1], "r");
	if (f == 0) {
		return -1;
	}
	fseek(f, 0, SEEK_END);
	uint64_t dn = ftell(f);
	uint8_t* data = malloc(dn);
	fseek(f, 0, SEEK_SET);
	fread(data, dn, 1, f);
	fclose(f);
	
	if (data[4] == 1) {
		elf_e32_t* eh = data + 16;
		elf_p32_t* ph = data + eh->phoff;
		elf_sh32_t* sh = data + eh->shoff;
		
		if (eh->type == 1) {
			zn_rlct_32(data, sh, eh->shnum);
		}
	}
	else if (data[4] == 2) {
		elf_e64_t* eh = data + 16;
		elf_p64_t* ph = data + eh->phoff;
		elf_sh64_t* sh = data + eh->shoff;
	}
	
	free(data);
	
	return 0;
}
