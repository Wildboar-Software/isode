#!/bin/sh
M=/bin/make
if [ -f /usr/bin/make ]; then
    M=/usr/bin/make
fi

exec $M TOPDIR= -f config/CONFIG.make -f Makefile ${1+"$@"}
