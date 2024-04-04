UNAME := $(shell uname)
CFLAGS += -I../sml/include/ -g -std=c99 -Wall -Wextra -pedantic
OBJS = CrcTest.o
LIBSML = ../sml/lib/libsml.a
INSTALL = /usr/bin/install
prefix = /usr/local
exec_prefix = ${prefix}
bindir = ${exec_prefix}/bin
libdir = ${exec_prefix}/lib
includedir = ${prefix}/include

LIBS = -lm
ifneq ($(UNAME), Darwin)
LIBS += -luuid
endif

all: CrcTest

CrcTest: $(OBJS) $(LIBSML)
	$(CC) $(OBJS) $(LIBSML) $(LIBS) -o CrcTest

%.o: %.c %.cpp
	$(CC) $(CFLAGS) -c $^ -o $@

.PHONY: clean install uninstall
clean:
	@rm -f *.o
	@rm -f CrcTest


