# drbdstat 1.7
# Simple program to continuously display DRBD status with adjustable update speed
# (c)2012 Miles Lott <mlott@gie.com> for Gulf Interstate Engineering
#
# Compile with: gcc -o drbdstat drbdstat.c
#
# This code is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This code is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# $Id: Makefile,v 1.5 2016/02/25 19:51:03 gulfconfig Exp $

PROGRAM=drbdstat
CC=gcc
RM=/bin/rm -f
MV=/bin/mv
STRIP=/usr/bin/strip

PREFIX=/usr
SBINDIR=$(PREFIX)/sbin


strip: $(PROGRAM)
	$(STRIP) $(PROGRAM)

$(PROGRAM):
	$(CC) -Wall -o $(PROGRAM) $(PROGRAM).c

install: strip
	$(MV) $(PROGRAM) $(SBINDIR)

uninstall:
	$(RM) $(SBINDIR)/$(PROGRAM)

clean:
	$(RM) $(PROGRAM)

