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

uint8_t* zn_rlct_32(uint8_t* bits, uint32_t* bn, elf_st32_t* sym, uint16_t* symn, elf_r32_t* rel, uint16_t* reln, uint8_t* data, elf_sh32_t* sh, uint16_t shn) {
	uint32_t nbn = *bn;
	uint16_t nymn = *symn;
	
	for (uint16_t shi = 0; shi < shn; shi++) {
		if (sh[shi].type == 1) { //progbits
			memcpy(bits + *bn, data + sh[shi].offset, sh[shi].size);
			nbn += sh[shi].size;
		}
		else if (sh[shi].type == 2) { //symtab
			memcpy(sym + *symn, data + sh[shi].offset, sh[shi].size);
			for (uint16_t i = *symn; i < *symn + (sh[shi].size / sh[shi].entsize); i++) {
				sym[i].value += *bn;
			}
			nymn += sh[shi].size / sh[shi].entsize;
		}
		else if (sh[shi].type == 9) { //rel
			memcpy(rel + *reln, data + sh[shi].offset, sh[shi].size);
			for (uint16_t i = *reln; i < *reln + (sh[shi].size / sh[shi].entsize); i++) {
				rel[i].offset += *bn;
				rel[i].info = (rel[i].info & 255) | (((rel[i].info >> 8) + *symn) << 8);
			}
			*reln += sh[shi].size / sh[shi].entsize;
		}
	}
	*bn = nbn;
	*symn = nymn;
}

int8_t main(int32_t argc, int8_t** argv) {
	if (argc < 3) {
		return -1;
	}
	
	uint8_t bits[65535];
	uint32_t bn = 0;
	elf_st32_t sym[65535];
	uint16_t symn = 0;
	elf_r32_t rel[65535];
	uint16_t reln = 0;
	
	for (uint8_t ai = 1; ai < argc - 1; ai ++) {
		FILE* f = fopen(argv[ai], "r");
		if (f == 0) {
			printf("error: file %s doesn't exist'\n", argv[1]);
			return -1;
		}
		fseek(f, 0, SEEK_END);
		uint64_t fn = ftell(f);
		uint8_t* data = malloc(fn);
		fseek(f, 0, SEEK_SET);
		fread(data, fn, 1, f);
		fclose(f);
		
		if (data[4] == 1) {
			elf_e32_t* eh = data + 16;
			elf_p32_t* ph = data + eh->phoff;
			elf_sh32_t* sh = data + eh->shoff;
			
			if (eh->type == 1) {
				zn_rlct_32(bits, &bn, sym, &symn, rel, &reln, data, sh, eh->shnum);
			}
		}
		else if (data[4] == 2) {
			elf_e64_t* eh = data + 16;
			elf_p64_t* ph = data + eh->phoff;
			elf_sh64_t* sh = data + eh->shoff;
		}
		
		free(data);
	}
	
	for (uint8_t i = 0; i < symn; i++) {
		printf("sym value: %i\n", sym[i].value);
	}
	for (uint8_t i = 0; i < reln; i++) {
		printf("rel offset: %i\n", rel[i].offset);
		printf("rel value: %i\n", sym[rel[i].info >> 8].value);
	}
	
	return 0;
}
