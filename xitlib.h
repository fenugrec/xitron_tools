/* collection of funcs for working with HP 8643A ROMs
 * (c) fenugrec 2022
 * GPLv3
 */

#pragma once

#include <stdio.h>	//just for FILE
#include <stdint.h>
#include "stypes.h"

/********** useful funcs ********/

/** parse & analyze ROM */
void parse_eepdump(FILE *i_file);

/** get file length but restore position */
uint32_t flen(FILE *hf);

#define EEPSIZE 512		//maximum (bytes), e.g. Partially-filled 93C66
