/* collection of funcs for working with Xitron 2000 EEPROM dumps
 * (c) fenugrec 2018
 * GPLv3
 */

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stypes.h"
#include "xitlib.h"


// validate offsets within eeprom structs
static_assert(sizeof(struct memstep) == 0x0e, "bad struct size");
static_assert(offsetof(struct eep_cal, tempdata) == 0x26, "bad struct padding");
static_assert(offsetof(struct eep_cal, extcal) == 0x36, "bad struct padding");
static_assert(offsetof(struct eep_cal, unk_e0) == 0xe0, "bad struct padding");
static_assert(offsetof(struct eep_cal, defaultdata) == 0xe4, "bad struct padding");
static_assert(offsetof(struct eep_cal, password) == 0xf2, "bad struct padding");
static_assert(offsetof(struct eep_cal, memsteps) == 0x100, "bad struct padding");
static_assert(sizeof(struct eep_cal) == 0x192, "bad struct size");


static uint16_t reconst_16(const uint8_t *buf) {
	return buf[0] << 8 | buf[1];
}

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
	for (; len > 0; len -= 1) {
		cks += *buf;
		buf += 1;
	}
	return cks;
}


/** ret 1 if good checksum
 * @param len of data excluding cks
 * @param buf should have sizeof(data)+2
 */
static bool check_block(const u8 *buf, unsigned len) {
	u16 cks = cks_1(buf, len);
	u16 want = reconst_16(&buf[len]);

	if (cks == want) {
		return 1;
	}
	printf("cks: want %X, got %X\n", (unsigned) want, (unsigned) cks);
	return 0;
}

#define CHECK_ONE_BLOCK(eepcal, member) \
	check_block((u8 *)(eepcal)+offsetof(struct eep_cal, member), sizeof(((struct eep_cal *)0)->member))

#define QUOTE(seq) ""#seq""

#define CHECK_BLOCK_P(eepcal, member) do {\
		if (!CHECK_ONE_BLOCK(eepcal, member)) { \
			printf("bad cks in block \"" QUOTE(member) "\".\n"); \
		} \
	} while (0)

void parse_eepdump(FILE *i_file) {
	u32 file_len;

	assert(i_file);

	rewind(i_file);
	file_len = flen(i_file);
	if ((file_len >= EEPSIZE) ||
			(file_len < sizeof(struct eep_cal))){
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

	struct eep_cal eepcal;
	memcpy(&eepcal, src, sizeof(struct eep_cal));

	CHECK_BLOCK_P(&eepcal, equations);
	CHECK_BLOCK_P(&eepcal, tempdata);
	CHECK_BLOCK_P(&eepcal, extcal);
	CHECK_BLOCK_P(&eepcal, unk_e0);
	CHECK_BLOCK_P(&eepcal, defaultdata);
	CHECK_BLOCK_P(&eepcal, password);	// no custom password results in bad cks

	// pain because of array-of-structs
	unsigned ms_idx;
	for (ms_idx = 0; ms_idx < MEMSTEPS; ms_idx++) {
		if (!check_block(eepcal.memsteps[ms_idx].ms_data, sizeof(((struct memstep *)0)->ms_data))) {
			printf("(not fatal) bad cks in memstep %u\n", ms_idx);
		}
	}
	CHECK_BLOCK_P(&eepcal, defaults);

	free(src);
	return;
}
