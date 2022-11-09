CC = gcc
BASICFLAGS = -std=gnu11 -Wall -Wextra -Wpedantic
OPTFLAGS = -g
CFLAGS = $(BASICFLAGS) $(OPTFLAGS) $(EXFLAGS)

TGTLIST = xitcal

all: $(TGTLIST)


xitcal:	xitcal.c xitlib.c

clean:
	rm -f *.o
	rm -f $(TGTLIST)
