/* collection of funcs for working with Tektronix TDS7xx ROMs (5xx/6xx probably also)
 * (c) fenugrec 2018
 * GPLv3
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stypes.h"
#include "xitlib.h"


// hax, get file length but restore position
u32 flen(FILE *hf) {
	long siz;
	long orig;

	if (!hf) return 0;
	orig = ftell(hf);
	if (orig < 0) return 0;

	if (fseek(hf, 0, SEEK_END)) return 0;

	siz = ftell(hf);
	if (siz < 0) siz=0;
	if ((long long) siz == (long long) UINT32_MAX) siz = 0;

	if (fseek(hf, orig, SEEK_SET)) return 0;
	return (u32) siz;
}

static u16 cks_1(const u8 *buf, unsigned len) {
	u16 cks = 0xA5;
	for (; len > 0; len -= 2) {
		cks += *buf;
		buf += 1;
	}
	return cks;
}


void parse_eepdump(FILE *i_file) {
	u32 file_len;

	rewind(i_file);
	file_len = flen(i_file);
	if (file_len >= EEPSIZE) {
		printf("wrong dump size (%lu bytes)\n", (unsigned long) file_len);
		return;
	}

	u8 *src = malloc(file_len);
	if (!src) {
		printf("malloc choke\n");
		return;
	}

	/* load whole dump */
	if (fread(src,1,file_len,i_file) != file_len) {
		printf("trouble reading\n");
		free(src);
		return;
	}

	free(src);
	return;
}
