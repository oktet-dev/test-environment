# SPDX-License-Identifier: Apache-2.0
# Copyright (C) 2006-2022 OKTET Labs Ltd. All rights reserved.
ifndef KPATH
KPATH = /usr/src/linux
endif

obj-m += mangler.o

all: kernel userland

kernel:
	make -C $(KPATH) M=$(PWD) modules

userland: mangle_setup

mangle_setup.c: if_mangle.h

clean:
	make -C $(KPATH) M=$(PWD) clean

