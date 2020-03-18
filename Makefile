CC=gcc
CFLAGS=-Wall -I src/

H_FILES=$(shell find src/ -type f -name "*.h")
C_FILES=$(patsubst src/%.h,src/%.c,$(H_FILES))
O_FILES=$(patsubst src/%.c,obj/%.o,$(C_FILES))
T_FILES=$(shell find src/ -type f -name "*_test.c")
TESTS=$(patsubst src/%.c,build/%,$(T_FILES))
TARGET=build/main

all: $(TARGET)

$(TARGET): $(O_FILES) $(patsubst build/%,obj/%.o,$(TARGET))
	$(CC) $(CFLAGS) -o $@ $^

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -o $@ -c $<

build/%_test: src/%_test.c $(O_FILES)
	$(CC) $(CFLAGS) -o $@ $^

test: $(TESTS)

.PHONY: clean

clean:
	rm -f obj/*.o
	rm -f build/*
	rm -f test
