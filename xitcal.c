/* Util for working with Vitrek / Xitron 2000 calibrator EEPROM dumps
*
*
*/

#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "stypes.h"

#include "xitlib.h"

#include <getopt.h>

#define static_assert(x,y) _Static_assert(x, y)

// validate offsets within struct eep_cal
static_assert(offsetof(struct eep_cal, tempdata) == 0x26, "bad struct padding");
static_assert(offsetof(struct eep_cal, extcal) == 0x36, "bad struct padding");
static_assert(offsetof(struct eep_cal, unk_e0) == 0xe0, "bad struct padding");
static_assert(offsetof(struct eep_cal, defaultdata) == 0xe4, "bad struct padding");
static_assert(offsetof(struct eep_cal, password) == 0xf2, "bad struct padding");
static_assert(offsetof(struct eep_cal, memsteps) == 0x100, "bad struct padding");
static_assert(sizeof(struct eep_cal) == 0x18e, "bad struct size");




/********* main stuff (options etc) */



static struct option long_options[] = {
//	{ "debug", no_argument, 0, 'd' },
	{ "help", no_argument, 0, 'h' },
	{ NULL, 0, 0, 0 }
};

static void usage(void)
{
	fprintf(stderr, "usage:\n"
		"--file    \t-f <filename>\tbinary ROM dump\n"
		"");
}


int main(int argc, char * argv[]) {
	char c;
	int optidx;
	FILE *file = NULL;

	printf(	"**** %s\n"
		"**** (c) 2022 fenugrec\n", argv[0]);

	while((c = getopt_long(argc, argv, "f:h",
			       long_options, &optidx)) != -1) {
		switch(c) {
		case 'h':
			usage();
			return 0;
		case 'f':
			if (file) {
				fprintf(stderr, "-f given twice");
				goto bad_exit;
			}
			file = fopen(optarg, "rb");
			if (!file) {
				fprintf(stderr, "fopen() failed: %s\n", strerror(errno));
				goto bad_exit;
			}
			break;
		default:
			usage();
			goto bad_exit;
		}
	}
	if (!file) {
		printf("some missing args.\n");
		usage();
		goto bad_exit;
	}

	if (optind <= 1) {
		usage();
		goto bad_exit;
	}

	parse_eepdump(file);
	fclose(file);
	return 0;

bad_exit:
	if (file) {
		fclose(file);
	}
	return 1;
}
