CC := gcc
CC_FLAGS := -O3 -Wall -Werror -march=native

# Note: by default, we do not enable address sanitizer
MEMORY_FLAGS := -fsanitize=address -fno-omit-frame-pointer -lasan -static-libasan

LIBINCLUDE := -Iinclude/

LD_FLAGS := -fPIC -shared

DRIVERSRC := $(wildcard *.c)
DRIVEROBJ := $(patsubst %.c, %.o, $(DRIVERSRC))
DRIVER := driver

JSONSRC := $(wildcard lib/*.c)
JSONOBJ := $(patsubst %.c, %.o, $(JSONSRC))
JSONLIB := libcjson.so

.PHONE: all lib clean

all: $(DRIVER)

lib: $(JSONLIB)

$(DRIVER): $(DRIVEROBJ) $(JSONLIB) 
	$(CC) $(CC_FLAGS) $(LIBINCLUDE) $(DRIVERINCLUDE) -o $@ $< -L. -lcjson

$(DRIVEROBJ): %.o:%.c
	$(CC) $(CC_FLAGS) $(LIBINCLUDE) $(DRIVERINCLUDE) -c $< -o $@

$(JSONLIB): $(JSONOBJ)
	$(CC) $(CC_FLAGS) $(LIBINCLUDE) $(LD_FLAGS) -o $@ $^

$(JSONOBJ): %.o:%.c
	$(CC) $(CC_FLAGS) $(LIBINCLUDE) $(LD_FLAGS) -c $< -o $@

clean:
	rm -rf $(DRIVER)
	rm -rf $(DRIVEROBJ)
	rm -rf $(JSONLIB)
	rm -rf $(JSONOBJ)