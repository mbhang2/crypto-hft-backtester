# Variables
CC = g++
CFLAGS = --coverage -ftest-coverage -fprofile-arcs -fprofile-dir=build/coverage -Iinclude
LDFLAGS = --coverage
APP = crypto_back_testing
APP_TESTER = unit_tests

APP_SOURCES = $(wildcard src/*.cpp)
APP_TESTS = $(wildcard tests/unit_tests/*.cpp)

GTEST_DIR = googletest/googletest
GTEST_REPO = https://github.com/google/googletest.git
GTEST_TAG = v1.14.0
GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
                $(GTEST_DIR)/include/gtest/internal/*.h
GTEST_SRCS_ = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)

# Flags passed to the preprocessor.
CPPFLAGS += -I$(GTEST_DIR)/include
# Flags passed to the C++ compiler.
CXXFLAGS += -std=c++20 -g -Wall -Wextra -pthread -Wno-deprecated

ARCH := $(shell uname -m)
ifeq ($(ARCH),x86_64)
    CMAKE_ARCH = -DCMAKE_OSX_ARCHITECTURES=x86_64
else ifeq ($(ARCH),arm64)
    CMAKE_ARCH = -DCMAKE_OSX_ARCHITECTURES=arm64
else
    CMAKE_ARCH = ""
endif

# Targets
all: $(GTEST_DIR) $(APP) $(APP_TESTER)

$(APP): $(APP_SOURCES:%.cpp=build/%.o)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

build/%.o: %.cpp
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<

$(GTEST_DIR):
	git clone --branch $(GTEST_TAG) $(GTEST_REPO) $@
	cd $@ && mkdir -p build && cd build && cmake $(CMAKE_ARCH) .. && make

gtest-all.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c $(GTEST_DIR)/src/gtest-all.cc

gtest_main.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c $(GTEST_DIR)/src/gtest_main.cc

gtest.a : gtest-all.o
	$(AR) $(ARFLAGS) $@ $^

gtest_main.a : gtest-all.o gtest_main.o
	$(AR) $(ARFLAGS) $@ $^

# Builds test executable.
$(APP_TESTER): $(APP_SOURCES) $(APP_TESTS) $(GTEST_SRCS_)
	mkdir -p build/tests/unit_tests/
	$(CC) $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) -Itests/unit_tests -c $(APP_TESTS) -o $(APP_TESTS:%.cpp=build/%.o)

#$(APP_TESTS): build/%.o: %.cpp gtest_main.a
#	mkdir -p $(dir $@)
#	$(CC) $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<
build/tests/unit_tests/%.o: tests/unit_tests/%.cpp gtest_main.a
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<

# Clean
.PHONY: clean
clean:
	rm -rf build