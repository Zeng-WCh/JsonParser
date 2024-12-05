CXX := g++
MODE ?= Release

ifeq ($(MODE), Debug)
	CXX_FLAGS := -g -O0 -Wall -Werror -fsanitize=address -fno-omit-frame-pointer -lasan
else ifeq ($(MODE), Release)
	CXX_FLAGS := -O3 -Wall -Werror -DNDEBUG
else
	CXX_FLAGS := -O2 -Wall -Werror
endif

CROSS_COMPILE ?=

CXX := $(CROSS_COMPILE)$(CXX)

ifeq ($(CROSS_COMPILE),)
	CXX_FLAGS += -march=native
endif

# Note: by default, we do not enable address sanitizer
MEMORY_FLAGS := -fsanitize=address -fno-omit-frame-pointer -lasan -static-libasan

LIBINCLUDE := -Iinclude/
SHARED := -fPIC -shared
LD_FLAGS :=

DRIVERSRC := $(wildcard *.cpp)
DRIVEROBJ := $(patsubst %.cpp, %.o, $(DRIVERSRC))
DRIVER := driver

JSONSRC := $(wildcard lib/*.cpp)
JSONOBJ := $(patsubst %.cpp, %.o, $(JSONSRC))
JSONLIB := libjson.so

.PHONE: all lib clean rebuild

all: $(DRIVER)

lib: $(JSONLIB)

$(DRIVER): $(DRIVEROBJ) $(JSONLIB) 
	$(CXX) $(CXX_FLAGS) $(LIBINCLUDE) $(DRIVERINCLUDE) -o $@ $< $(JSONLIB)

$(DRIVEROBJ): %.o:%.cpp
	$(CXX) $(CXX_FLAGS) $(SHARED) $(LIBINCLUDE) $(DRIVERINCLUDE) -c $< -o $@

$(JSONLIB): $(JSONOBJ)
	$(CXX) $(CXX_FLAGS) $(SHARED) $(LIBINCLUDE) $(LD_FLAGS) -o $@ $^

$(JSONOBJ): %.o:%.cpp
	$(CXX) $(CXX_FLAGS) $(SHARED) $(LIBINCLUDE) -c $< -o $@

clean:
	rm -rf $(DRIVER)
	rm -rf $(DRIVEROBJ)
	rm -rf $(JSONLIB)
	rm -rf $(JSONOBJ)

rebuild: clean all
