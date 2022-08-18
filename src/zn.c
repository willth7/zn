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

#include "avr/rel.h"
#include "elf/elf.h"

void zn_prse_32(uint8_t* bits, uint32_t* bn, int8_t* str, uint32_t* strn, elf_st32_t* sym, uint16_t* symn, elf_r32_t* rel, uint16_t* reln, uint8_t* data, elf_sh32_t* sh, uint16_t shn, uint16_t shndx) {
	uint32_t nbn = *bn;
	uint32_t ntrn = *strn;
	uint16_t nymn = *symn;
	
	for (uint16_t shi = 0; shi < shn; shi++) {
		if (sh[shi].type == 1) { //progbits
			memcpy(bits + *bn, data + sh[shi].offset, sh[shi].size);
			nbn += sh[shi].size;
		}
		else if (sh[shi].type == 2) { //symtab
			memcpy(sym + *symn, data + sh[shi].offset, sh[shi].size);
			for (uint16_t i = *symn; i < *symn + (sh[shi].size / sh[shi].entsize); i++) {
				sym[i].name += *strn;
				sym[i].value += *bn;
			}
			nymn += sh[shi].size / sh[shi].entsize;
		}
		else if (sh[shi].type == 3 && shi != shndx) { //strtab
			memcpy(str + *strn, data + sh[shi].offset, sh[shi].size);
			ntrn += sh[shi].size;
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
	*strn = ntrn;
	*symn = nymn;
}

int8_t zn_rlct_32(uint8_t* bits, uint32_t bn, int8_t* str, uint32_t strn, elf_st32_t* sym, uint16_t symn, elf_r32_t* rel, uint16_t reln) {
	for (uint16_t symi = 0; symi < symn; symi++) {
		if (sym[symi].shndx == 0) { //undefined symbol
			for (uint16_t symj = 0; symj < symn; symj++) {
				if (sym[symj].shndx && (!strcmp(str + sym[symi].name, str + sym[symj].name))) {
					for (uint8_t reli = 0; reli < reln; reli++) {
						if (rel[reli].info >> 8 == symi) {
							rel[reli].info &= 255 + (symj << 8);
						}
					}
				}
				else if (symj + 1 == symn) {
					printf("error: undefined symbol %s\n", str + sym[symi].name);
					return -1;
				}
			}
		}
	}
	for (uint8_t i = 0; i < reln; i++) {
		avr_rel(bits + rel[i].offset, rel[i].offset, sym[rel[i].info >> 8].value, rel[i].info & 255);
	}
	return 0;
}

int8_t main(int32_t argc, int8_t** argv) {
	if (argc < 3) {
		return -1;
	}
	
	uint8_t bits[65535];
	uint32_t bn = 0;
	int8_t str[65535];
	uint32_t strn = 0;
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
				zn_prse_32(bits, &bn, str, &strn, sym, &symn, rel, &reln, data, sh, eh->shnum, eh->shstrndx);
			}
		}
		else if (data[4] == 2) {
			elf_e64_t* eh = data + 16;
			elf_p64_t* ph = data + eh->phoff;
			elf_sh64_t* sh = data + eh->shoff;
		}
		
		free(data);
	}
	
	uint8_t e = zn_rlct_32(bits, bn, str, strn, sym, symn, rel, reln);
	
	if (e) {
		return -1;
	}
	
	for (uint16_t i = 0; i < symn; i++) {
		printf("symbol: %s\noffset: %i\ninfo: %i\n", str + sym[i].name, sym[i].value, sym[i].info);
	}
	printf("\n");
	for (uint16_t i = 0; i < reln; i++) {
		printf("rel: %s\noffset: %i\ntype: %i\n", str + sym[rel[i].info >> 8].name, rel[i].offset, rel[i].info & 255);
	}
	
	elf_e32_t eh;
	eh.type = 2;
	eh.machine = 83;
	eh.version = 1;
	eh.entry = 0;
	eh.phoff = 52;
	eh.shoff = 0;
	eh.flags = 2;
	eh.ehsize = 52;
	eh.phentsize = 32;
	eh.phnum = 1;
	eh.shentsize = 40;
	eh.shnum = 0;
	eh.shstrndx = 0;
	
	elf_p32_t ph;
	ph.type = 1;
	ph.offset = 84;
	ph.vaddr = 0;
	ph.paddr = 0;
	ph.filesz = bn;
	ph.memsz = bn;
	ph.flags = 7;
	ph.align = 0;
	
	FILE* f = fopen(argv[argc - 1], "w");
	
	uint8_t* elf = elf_write_32(&eh, &ph, 0, bits, bn);
	uint64_t esz = 84 + bn;
	
	for (uint64_t i = 0; i < esz; i++) {
		fprintf(f, "%c", elf[i]);
	}
	
	fclose(f);
	free(elf);
	return 0;
}
