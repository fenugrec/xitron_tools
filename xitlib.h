/* collection of funcs for working with Xitron 2000 EEPROM dump
 * (c) fenugrec 2022
 * GPLv3
 */

#pragma once

#include <stdio.h>	//just for FILE
#include <stdint.h>
#include "stypes.h"

/********** eeprom contents *****/
struct memstep {
	u8 ms_data[0x0c];
	u8 ms_cks[2];
} __attribute__((packed));

#define MEMSTEPS 10

struct eep_cal {
	u8 equations[0x24];
	u8 eq_cks[2];

	u8 tempdata[0x0e];
	u8 tempdata_cks[2];

	u8 extcal[0xa8];
	u8 extcal_cks[2];

	u8 unk_e0[0x02];	//[0] : gpib address, [1] baudrate (enum)
	u8 unk_e0_cks[2];

	u8 defaultdata[0x0c];
	u8 defaultdata_cks[2];

	u8 password[0x0c];
	u8 password_cks[2];

	struct memstep memsteps[MEMSTEPS];

	u8 defaults[4];
	u8 defaults_cks[2];
} __attribute__((packed));


/********** useful funcs ********/

/** parse & analyze ROM */
void parse_eepdump(FILE *i_file);

/** get file length but restore position */
uint32_t flen(FILE *hf);

#define EEPSIZE 512		//maximum (bytes), e.g. Partially-filled 93C66
