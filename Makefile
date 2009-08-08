#
# HVF: Hobbyist Virtualization Facility
#

#
# This is the top-level makefile not much happens here
#

all:
	make -C sys
	make -C nss/8ball
	make -C doc/manual

clean:
	make -C sys clean
	make -C nss/8ball clean
	make -C doc/manual clean
