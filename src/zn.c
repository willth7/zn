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
	
	*bn += binnum;
	*symn += symnum;
	*reln += relnum;
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
		
	}
	else if (!strcmp(argv[argc - 1] + strlen(argv[argc - 1]) - 3, ".zn")) {
		
	}
	else {
		printf("error: invalid output format\n");
		return -1;
	}
	
	uint8_t* bin = malloc(1000);
	uint64_t bn = 0;
	zn_sym_t* sym = malloc(1000);
	uint64_t symn = 0;
	zn_sym_t* rel = malloc(1000);
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
	}
	
	free(bin);
	free(sym);
	free(rel);
	return 0;
}
