/* collection of funcs for working with HP 8643A ROMs
 * (c) fenugrec 2022
 * GPLv3
 */

#pragma once

#include <stdio.h>	//just for FILE
#include <stdint.h>
#include "stypes.h"

/********** eeprom contents *****/

struct eep_cal {
	u8 equations[0x24];
	u8 eq_cks[2];

	u8 tempdata[0x0e];
	u8 tempdata_cks[2];

	u8 extcal[0xa8];
	u8 extcal_cks[2];

	u8 unk_e0[0x02];	//possibly baud rate
	u8 unk_e0_cks[2];

	u8 defaultdata[0x0c];
	u8 defaultdata_cks[2];

	u8 password[0x0c];
	u8 password_cks[2];

	u8 memsteps[0x8c];
	u8 memsteps_cks[2];
} __attribute__((packed));


/********** useful funcs ********/

/** parse & analyze ROM */
void parse_eepdump(FILE *i_file);

/** get file length but restore position */
uint32_t flen(FILE *hf);

#define EEPSIZE 512		//maximum (bytes), e.g. Partially-filled 93C66
