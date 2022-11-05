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
#include <unistd.h>
#include <stdio.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "arm/32.h"
#include "arm/64.h"
#include "x86/x86.h"

typedef struct zn_sym_s {
	int64_t strl;
	int64_t strh;
	uint64_t addr;
	uint8_t typ;
} zn_sym_t;

void (*zn_rel) (uint8_t*, uint64_t,  uint64_t,uint8_t);

void (*zn_writ) (uint8_t*, uint64_t, zn_sym_t*, uint64_t, zn_sym_t*, uint64_t, int8_t*);

void zn_rlct(uint8_t* bin, zn_sym_t* sym, uint64_t symn, zn_sym_t* rel, uint64_t reln) {
	for (uint64_t i = 0; i < reln; i++) {
		for (uint8_t j = 0; j < symn; j++) {
			if (rel[i].strl == sym[j].strl && rel[i].strh == sym[j].strh) {
				zn_rel(bin, rel[i].addr, sym[j].addr, rel[i].typ);
			}
		}
	}
}

void zn_read_zn(uint8_t* bin, uint64_t* bn, zn_sym_t* sym, uint64_t* symn, zn_sym_t* rel, uint64_t* reln, int8_t* path, int8_t* e) {
	int32_t fd = open(path, O_RDONLY);
    if (fd == -1) {
        printf("error: failed to open file '%s'\n", path);
		*e = -1;
        return;
    }
	
    struct stat fs;
    fstat(fd, &fs);
	
    uint8_t* fx = mmap(0, fs.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);
	
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
	
	memcpy(bin + *bn, fx + binoff, binnum);
	
	for (uint64_t i = 0; i < symnum; i++) {
		sym[i + *symn].strl = *((uint64_t*) (fx + symoff + (25 * i)));
		sym[i + *symn].strh = *((uint64_t*) (fx + symoff + (25 * i) + 8));
		sym[i + *symn].addr = *((uint64_t*) (fx + symoff + (25 * i) + 16)) + *bn;
		sym[i + *symn].typ = *(fx + symoff + (25 * i) + 24);
		for (uint64_t j = 0; j < *symn; j++) {
			if (sym[i + *symn].strl == sym[j].strl && sym[i + *symn].strh == sym[j].strh) {
				printf("[%s] error: symbol '%s%x' already defined\n", path, (int8_t*) &(sym[i + *symn].strl), (int8_t*) &(sym[i + *symn].strh));
				*e = -1;
			}
		}
	}
	
	for (uint64_t i = 0; i < relnum; i++) {
		rel[i + *reln].strl = *((uint64_t*) (fx + reloff + (25 * i)));
		rel[i + *reln].strh = *((uint64_t*) (fx + reloff + (25 * i) + 8));
		rel[i + *reln].addr = *((uint64_t*) (fx + reloff + (25 * i) + 16)) + *bn;
		rel[i + *reln].typ = *(fx + reloff + (25 * i) + 24);
	}
	
	*bn += binnum;
	*symn += symnum;
	*reln += relnum;
	
	munmap(fx, fs.st_size);
}

void zn_writ_bin(uint8_t* bin, uint64_t bn, zn_sym_t* sym, uint64_t symn, zn_sym_t* rel, uint64_t reln, int8_t* path) {
	int32_t fd = open(path, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
    if (fd == -1) {
        printf("error: failed to create file '%s'\n", path);
        return;
    }
    ftruncate(fd, bn);
    uint8_t* mem = mmap(0, bn, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	memcpy(mem, bin, bn);
	munmap(mem, bn);
	close(fd);
}

void zn_writ_zn(uint8_t* bin, uint64_t bn, zn_sym_t* sym, uint64_t symn, zn_sym_t* rel, uint64_t reln, int8_t* path) {
	uint64_t memsz = 52 + bn + (symn * 25) + (reln * 25);
	
	int32_t fd = open(path, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU);
    if (fd == -1) {
        printf("error: failed to create file '%s'\n", path);
        return;
    }
    ftruncate(fd, memsz);
    uint8_t* mem = mmap(0, memsz, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	
	uint64_t binoff = 52;
	uint64_t symoff = 52 + bn;
	uint64_t reloff = 52 + bn + (symn * 25);
	
	memcpy(mem, "zinc", 4);
	memcpy(mem + 4, &binoff, 8);
	memcpy(mem + 12, &bn, 8);
	memcpy(mem + 20, &symoff, 8);
	memcpy(mem + 28, &symn, 8);
	memcpy(mem + 36, &reloff, 8);
	memcpy(mem + 44, &reln, 8);
	
	memcpy(mem + binoff, bin, bn);
	for (uint64_t i = 0; i < symn; i++) {
		memcpy(mem + symoff + (25 * i), &(sym[i].strl), 8);
		memcpy(mem + symoff + (25 * i) + 8, &(sym[i].strh), 8);
		memcpy(mem + symoff + (25 * i) + 16, &(sym[i].addr), 8);
		memcpy(mem + symoff + (25 * i) + 24, &(sym[i].typ), 1);
	}
	
	for (uint64_t i = 0; i < reln; i++) {
		memcpy(mem + reloff + (25 * i), &(rel[i].strl), 8);
		memcpy(mem + reloff + (25 * i) + 8, &(rel[i].strh), 8);
		memcpy(mem + reloff + (25 * i) + 16, &(rel[i].addr), 8);
		memcpy(mem + reloff + (25 * i) + 24, &(rel[i].typ), 1);
	}
	
	munmap(mem, memsz);
	close(fd);
}

int8_t main(uint32_t argc, int8_t** argv) {
	if (argc < 4) {
		printf("usage: au [architecture] [link.zn] [link.zn] ... [link.zn] [binary.bin or link.zn]\n");
		return -1;
	}
	
	if (!strcmp(argv[1], "arm32")) {
		zn_rel = arm_32_rel;
	}
	else if (!strcmp(argv[1], "arm64")) {
		zn_rel = arm_64_rel;
	}
	else if (!strcmp(argv[1], "x86")) {
		zn_rel = x86_rel;
	}
	else {
		printf("error: unsupported architecture\n");
	}
	
	for (uint32_t i = 2; i < argc - 1; i++) {
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
	
	for (uint32_t i = 2; i < argc - 1; i++) {
		zn_read_zn(bin, &bn, sym, &symn, rel, &reln, argv[i], &e);
	}
	
	zn_rlct(bin, sym, symn, rel, reln);
	
	if (!e) {
		zn_writ(bin, bn, sym, symn, rel, reln, argv[argc - 1]);
	}
	
	free(bin);
	free(sym);
	free(rel);
	return 0;
}
