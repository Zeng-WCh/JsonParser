CC := gcc

CC_FLAGS := -g -O0 -Wall -Werror -march=native -fsanitize=address -fno-omit-frame-pointer

# Note: by default, we do not enable address sanitizer
MEMORY_FLAGS := -fsanitize=address -fno-omit-frame-pointer -lasan -static-libasan

LIBINCLUDE := -Iinclude/

LD_FLAGS := -fPIC -shared

DRIVERSRC := $(wildcard *.c)
DRIVEROBJ := $(patsubst %.c, %.o, $(DRIVERSRC))
DRIVER := driver

FRAMESRC := $(wildcard lib/*.c)
FRAMEOBJ := $(patsubst %.c, %.o, $(FRAMESRC))
FRAMELIB := libcjson.so

.PHONE: all clean

all: $(DRIVER)

$(DRIVER): $(FRAMELIB) $(DRIVEROBJ)
	$(CC) $(CC_FLAGS) $(LIBINCLUDE) $(DRIVERINCLUDE) -o $@ $^ -L. -lcjson

$(DRIVEROBJ): %.o:%.c
	$(CC) $(CC_FLAGS) $(LIBINCLUDE) $(DRIVERINCLUDE) -c $< -o $@

$(FRAMELIB): $(FRAMEOBJ)
	$(CC) $(CC_FLAGS) $(LIBINCLUDE) $(LD_FLAGS) -o $@ $^

$(FRAMEOBJ): %.o:%.c
	$(CC) $(CC_FLAGS) $(LIBINCLUDE) $(LD_FLAGS) -c $< -o $@

clean:
	rm -rf $(DRIVER)
	rm -rf $(DRIVEROBJ)
	rm -rf $(FRAMELIB)
	rm -rf $(FRAMEOBJ)