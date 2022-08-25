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

//   gloria in excelsis deo

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct zn_sym_s {
	int64_t str;
	uint64_t addr;
	uint8_t typ;
} zn_sym_t;

void (*zn_rel) (uint8_t*, zn_sym_t*, uint64_t, zn_sym_t*, uint64_t, int8_t*);

void (*zn_writ) (uint8_t*, uint64_t, zn_sym_t*, uint64_t, zn_sym_t*, uint64_t, int8_t*);

void zn_read_zn(uint8_t* bin, uint64_t* bn, zn_sym_t* sym, uint64_t* symn, zn_sym_t* rel, uint64_t* reln, int8_t* path, int8_t* e) {
	FILE* f = fopen(path, "r");
	if (!f) {
		printf("[%s] error: file doesn't exist\n", path);
		*e = -1;
		return;
	}
	fseek(f, 0, SEEK_END);
	uint64_t fn = ftell(f);
	uint8_t* fx = malloc(fn);
	fseek(f, 0, SEEK_SET);
	fread(fx, fn, 1, f);
	fclose(f);
	
	uint32_t mag = *((uint32_t*) fx);
	uint64_t binoff = *((uint64_t*) (fx + 4));
	uint64_t binnum = *((uint64_t*) (fx + 12));
	uint64_t symoff = *((uint64_t*) (fx + 20));
	uint64_t symnum = *((uint64_t*) (fx + 28));
	uint64_t reloff = *((uint64_t*) (fx + 36));
	uint64_t relnum = *((uint64_t*) (fx + 44));
	
	if (mag != 1668180346) {
		printf("[%s] error: corrupted file\n", path);
		*e = -1;
		return;
	}
	
	memcpy(bin, fx + binoff, binnum);
	memcpy(sym, fx + symoff, symnum * 17);
	memcpy(rel, fx + reloff, relnum * 17);
	
	for (uint64_t i = 0; i < symnum; i++) {
		sym[i + *symn].addr += *bn;
		for (uint64_t j = 0; j < *symn; j++) {
			if (sym[i + *symn].str == sym[j].str) {
				printf("[%s] error: symbol '%s' already defined\n", path, (int8_t*) &(sym[i + *symn].str));
				*e = -1;
			}
		}
	}
	
	for (uint64_t i = 0; i < relnum; i++) {
		rel[i + *reln].addr += *bn;
	}
	
	*bn += binnum;
	*symn += symnum;
	*reln += relnum;
}

void zn_writ_bin(uint8_t* bin, uint64_t bn, zn_sym_t* sym, uint64_t symn, zn_sym_t* rel, uint64_t reln, int8_t* path) {
	FILE* f = fopen(path, "w");
	fwrite(bin, bn, 1, f);
	fclose(f);
}

void zn_writ_zn(uint8_t* bin, uint64_t bn, zn_sym_t* sym, uint64_t symn, zn_sym_t* rel, uint64_t reln, int8_t* path) {
	uint8_t* buf = malloc(52 + bn + (symn * 17) + (reln * 17));
	
	uint64_t binoff = 52;
	uint64_t symoff = 52 + bn;
	uint64_t reloff = 52 + bn + (symn * 17);
	
	memcpy(buf, "zinc", 4);
	memcpy(buf + 4, &binoff, 8);
	memcpy(buf + 12, &bn, 8);
	memcpy(buf + 20, &symoff, 8);
	memcpy(buf + 28, &symn, 8);
	memcpy(buf + 36, &reloff, 8);
	memcpy(buf + 44, &reln, 8);
	
	memcpy(buf + binoff, bin, bn);
	memcpy(buf + symoff, sym, symn * 17);
	memcpy(buf + reloff, rel, reln * 17);
	
	FILE* f = fopen(path, "w");
	fwrite(buf, 52 + bn + (symn * 17) + (reln * 17), 1, f);
	fclose(f);
	
	free(buf);
}

int8_t main(uint32_t argc, int8_t** argv) {
	if (argc < 3) {
		printf("usage: au [link.zn] [link.zn] ... [link.zn] [binary.bin or link.zn]\n");
		return -1;
	}
	
	for (uint32_t i = 1; i < argc - 1; i++) {
		if (strcmp(argv[i] + strlen(argv[i]) - 3, ".zn")) {
			printf("error: expected .zn file\n");
			return -1;
		}
	}
	
	if (!strcmp(argv[argc - 1] + strlen(argv[argc - 1]) - 4, ".bin")) {
		zn_writ = zn_writ_bin;
	}
	else if (!strcmp(argv[argc - 1] + strlen(argv[argc - 1]) - 3, ".zn")) {
		zn_writ = zn_writ_zn;
	}
	else {
		printf("error: invalid output format\n");
		return -1;
	}
	
	uint8_t* bin = malloc(1000000);
	uint64_t bn = 0;
	zn_sym_t* sym = malloc(1000000);
	uint64_t symn = 0;
	zn_sym_t* rel = malloc(1000000);
	uint64_t reln = 0;
	int8_t e = 0;
	
	for (uint32_t i = 1; i < argc - 1; i++) {
		zn_read_zn(bin, &bn, sym, &symn, rel, &reln, argv[i], &e);
	}
	
	if (!e) {
		for (uint16_t i = 0; i < symn; i++) {
			printf("[sym]\tname: %s\n\taddr: %lu\n\ttyp: %hhu\n", (int8_t*) &(sym[i].str), sym[i].addr, sym[i].typ);
		}
		
		for (uint16_t i = 0; i < reln; i++) {
			printf("[rel]\tname: %s\n\taddr: %lu\n\ttyp: %hhu\n", (int8_t*) &(rel[i].str), rel[i].addr, rel[i].typ);
		}
		zn_writ(bin, bn, sym, symn, rel, reln, argv[argc - 1]);
	}
	
	free(bin);
	free(sym);
	free(rel);
	return 0;
}
