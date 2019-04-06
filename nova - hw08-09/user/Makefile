ALL = hello

CC = gcc
CFLAGS = -m32 -g -nostdlib -fomit-frame-pointer -O2 -Wall -Ilib
CXXFLAGS = $(CFLAGS) -fno-rtti -fno-exceptions
ASFLAGS = -m32 -g
LDFLAGS = -Wl,--build-id=none -Wl,--gc-sections -Wl,-Map=$@.map

all: $(ALL)

clean:
	rm -f $(ALL) $(wildcard *.o)

# Userspace

LIB_OBJ = console.o console_kernel.o stdio.o start.o init.o
libc.a: $(LIB_OBJ:%=lib/%)
	ar rcs $@ $^

%: %.o libc.a
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -Wl,-Tlinker.ld $^ -o $@

# Disable builtin rules

%: %.c
%: %.cc
%: %.o
