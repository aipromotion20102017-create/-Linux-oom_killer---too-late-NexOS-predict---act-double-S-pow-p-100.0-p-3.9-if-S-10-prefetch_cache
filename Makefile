.PHONY: build run bench clean

CC := gcc
CFLAGS := -Wall -Wextra -O3 -std=c11 -pthread -D_GNU_SOURCE
LDFLAGS := -lm

SRC_DIR := src
BENCH_DIR := bench
BUILD_DIR := build

SOURCES := $(SRC_DIR)/mathelisophe.c $(SRC_DIR)/main.c
OBJECTS := $(BUILD_DIR)/mathelisophe.o $(BUILD_DIR)/main.o
TARGET := $(BUILD_DIR)/nexos

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

build: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) $(LDFLAGS) -o $(TARGET)
	@echo "✓ Build complete: $(TARGET)"

run: build
	./$(TARGET)

bench: build
	@echo "Running benchmarks..."
	./$(TARGET) --bench 2>&1 | tee $(BENCH_DIR)/results.txt
	@echo "✓ Results saved to $(BENCH_DIR)/results.txt"

clean:
	rm -rf $(BUILD_DIR)
	@echo "✓ Clean complete"

all: clean build
