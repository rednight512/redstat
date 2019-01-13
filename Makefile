###############################################################################
# Makefile for redstat
#
# Copyright (c) 2019, Michael Jensen All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
#   * Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#
#   * Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer
#     in the documentation and/or other materials provided with the
#     distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
##############################################################################

CLFAGS=
LIBS=-lX11 -lossaudio -lprop
LIBPATH=-L/usr/X11R7/lib -R/usr/X11R7/lib
INCLUDEPATH=-I/usr/X11R7/include

OBJS=main.o batt.o date_time.o vol.o

all: redstat

redstat: ${OBJS} 
	$(CC) $(CFLAGS) -o redstat ${OBJS} ${LIBS} ${INCLUDEPATH} ${LIBPATH}

main.o:
	${CC} -c main.c ${INCLUDEPATH}

batt.o:
	${CC} -c batt.c

date_time.o:
	${CC} -c date_time.c

vol.o:
	${CC} -c vol.c

clean:
	rm -f ${OBJS}

binclean:
	rm -f redstat
