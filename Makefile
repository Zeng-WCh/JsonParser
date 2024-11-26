CC := gcc
CC_FLAGS := -O3 -Wall -Werror

CROSS_COMPILE ?=

CC := $(CROSS_COMPILE)$(CC)

ifeq ($(CROSS_COMPILE),)
	CC_FLAGS += -march=native
endif

# Note: by default, we do not enable address sanitizer
MEMORY_FLAGS := -fsanitize=address -fno-omit-frame-pointer -lasan -static-libasan

LIBINCLUDE := -Iinclude/
SHARED := -fPIC -shared
LD_FLAGS :=

DRIVERSRC := $(wildcard *.c)
DRIVEROBJ := $(patsubst %.c, %.o, $(DRIVERSRC))
DRIVER := driver

JSONSRC := $(wildcard lib/*.c)
JSONOBJ := $(patsubst %.c, %.o, $(JSONSRC))
JSONLIB := libjson.so

.PHONE: all lib clean

all: $(DRIVER)

lib: $(JSONLIB)

$(DRIVER): $(DRIVEROBJ) $(JSONLIB) 
	$(CC) $(CC_FLAGS) $(LIBINCLUDE) $(DRIVERINCLUDE) -o $@ $< $(JSONLIB)

$(DRIVEROBJ): %.o:%.c
	$(CC) $(CC_FLAGS) $(SHARED) $(LIBINCLUDE) $(DRIVERINCLUDE) -c $< -o $@

$(JSONLIB): $(JSONOBJ)
	$(CC) $(CC_FLAGS) $(SHARED) $(LIBINCLUDE) $(LD_FLAGS) -o $@ $^

$(JSONOBJ): %.o:%.c
	$(CC) $(CC_FLAGS) $(SHARED) $(LIBINCLUDE) -c $< -o $@

clean:
	rm -rf $(DRIVER)
	rm -rf $(DRIVEROBJ)
	rm -rf $(JSONLIB)
	rm -rf $(JSONOBJ)
