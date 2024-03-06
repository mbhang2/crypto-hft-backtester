# Variables
CC = g++
CFLAGS = -std=c++20 --coverage -ftest-coverage -fprofile-arcs -fprofile-dir=build/coverage
LDFLAGS = --coverage
APP = crypto_back_testing
APP_TESTER = crypto_tests
APP_SOURCES = src/BackTesting.cpp src/main.cpp
APP_TESTS = tests/CryptoTest.cpp
GTEST_DIR = googletest
GTEST_REPO = https://github.com/google/googletest.git
GTEST_TAG = v1.14.0
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

$(APP): $(APP_SOURCES)
	rm -rf build
	mkdir build -p
	$(CC) $(CFLAGS) -o build/$@ $^ $(LDFLAGS)

$(GTEST_DIR):
	git clone --branch $(GTEST_TAG) $(GTEST_REPO) $@
	cd $@ && mkdir build && cd build && cmake $(CMAKE_ARCH) .. && make

$(APP_TESTER): $(APP_SOURCES) $(APP_TESTS)
	$(CC) $(CFLAGS) -o build/$@ $^ -I$(GTEST_DIR)/googletest/include -L$(GTEST_DIR)/build/lib -lgtest -lgtest_main -pthread $(LDFLAGS)
	mkdir -p ./build/coverage
	mv *.gcno build/coverage


.PHONY: clean
clean:
	rm -f build/$(APP) build/$(APP_TESTER)