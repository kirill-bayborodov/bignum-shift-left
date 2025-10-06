# Makefile for bignum-shift-left product
# Makefile для библиотеки bignum_shift_left

LIB_NAME     := bignum_shift_left
VER          := 1.0.0

# --- Compiler and Flags ---
CC           := gcc
AS           := yasm
ASFLAGS      := -f elf64 -g dwarf2
# -Iinclude: наш собственный include
# -Ilibs/common/include: include из сабмодуля
CFLAGS       := -Wall -Wextra -O2 -std=c11 -I. -fno-inline -fno-omit-frame-pointer -march=native -g
LD_FLAGS     := -no-pie
THREAD_FLAGS := -pthread

# --- Directories ---
SRC_DIR      := src
OBJ_DIR    := build
BIN_DIR      := bin
TEST_DIR     := tests
BENCH_DIR    := benchmarks
DOC_DIR      := doc
INC_DIR      := include
LIBS_DIR     := libs
BIGNUM_DIR   := $(LIBS_DIR)/common/include
REP_DIR      := $(BENCH_DIR)/reports

# --- Files ---
# Собираем ассемблерную версию
ASM_SRC      := $(SRC_DIR)/$(LIB_NAME).asm
HEAD_NAME    := $(INC_DIR)/$(LIB_NAME).h
OBJ          := $(OBJ_DIR)/$(LIB_NAME).o
TEST_SRC     := $(wildcard $(TEST_DIR)/*.c)
TEST_BINS    := $(patsubst $(TEST_DIR)/%.c, $(BIN_DIR)/%, $(TEST_SRC))
BENCH_SRC    := $(wildcard $(BENCH_DIR)/*.c)
BENCH_BINS   := $(patsubst $(BENCH_DIR)/%.c, $(BIN_DIR)/%, $(BENCH_SRC))

# Параметры сборки для микробенчмарка
PERF_CFLAGS        := -g -O2 -fno-inline -fno-optimize-sibling-calls -fno-omit-frame-pointer -march=native -fsanitize=address
PERF_UTL           := /usr/local/bin/perf
PERF_SYMBOL_FILTER := '$(LIB_NAME)\.(bit_shift_loop|normalize_loop|set_new_len|epilogue)'
PERF_BIN           := bench_$(LIB_NAME)
PERF_SRC           := $(BENCH_DIR)/$(PERF_BIN).c
PERF_REPORT_NAME   := report_$(PERF_BIN)
PERF_REPORT        := $(REP_DIR)/$(PERF_REPORT_NAME)
TXT_REPORT         := $(DOC_DIR)/$(PERF_REPORT_NAME)
NP                 :=  $(shell nproc | awk '{print $1}')


.PHONY: all build test benchmark clean help

# Цель по умолчанию: собрать библиотеку и тесты
all: build

# --- Main Targets ---

# Цель для сборки нашего продукта - объектного файла
build: $(OBJ) 

# Цель для запуска всех тестов
test: $(TEST_BINS)
	@echo "Running tests..."
	@for test in $(TEST_BINS); do \
		./$$test; \
	done

# Цель для запуска всех бенчмарков
benchmark: $(BENCH_BINS) | $(REP_DIR)
	@echo "Running benchmarks..."
	sudo sysctl -w kernel.perf_event_max_sample_rate=10000
	taskset 0x1 \
	$(PERF_UTL) record -F 1000 -e cycles,cache-misses,branch-misses -g --call-graph fp -o $(PERF_REPORT) -- $(BIN_DIR)/$(PERF_BIN)
	#@for bench in $(BENCH_SRC); do \
	#	@echo $bench; \
	#done
	taskset --cpu-list 1-$(NP) \
	$(PERF_UTL)  record  -F 1000 \
	-e cycles,cache-misses,branch-misses \
	-g  \
	-o $(PERF_REPORT)_mt -- $(BIN_DIR)/$(PERF_BIN)_mt
	@echo "Make reports..."
	$(PERF_UTL) report -i $(PERF_REPORT) --stdio --percent-limit 1.0 --sort comm,dso,symbol \
	--dsos $(PERF_BIN) --symbol-filter=$(PERF_SYMBOL_FILTER) > $(TXT_REPORT).txt
	$(PERF_UTL) report -i $(PERF_REPORT)_mt --stdio --percent-limit 1.0 --sort comm,dso,symbol \
	--dsos $(PERF_BIN)_mt  --symbol-filter=$(PERF_SYMBOL_FILTER) > $(TXT_REPORT)_mt.txt


# --- Compilation Rules ---

# Правило для сборки объектного файла из .asm
$(OBJ): $(ASM_SRC) $(HEAD_NAME) 
	mkdir -p $(OBJ_DIR)
	$(AS) $(ASFLAGS) -o $@ $<

# Правило для сборки исполняемых файлов тестов
$(BIN_DIR)/%: $(TEST_DIR)/%.c $(OBJ) | $(BIN_DIR)
	$(CC) -I $(INC_DIR) -I $(BIGNUM_DIR) $(CFLAGS) $< $(OBJ) -o $@ $(THREAD_FLAGS) $(LD_FLAGS)  -pedantic -lm

# Правило для сборки исполняемых файлов бенчмарков
$(BIN_DIR)/%: $(BENCH_DIR)/%.c $(OBJ) | $(BIN_DIR)
	$(CC) -I $(INC_DIR) -I $(BIGNUM_DIR) $(PERF_CFLAGS) $< $(OBJ) -o $@ $(THREAD_FLAGS) $(LD_FLAGS)  -pedantic -lm

# --- Utility Targets ---

# Создание нужных директорий
$(BIN_DIR) $(REP_DIR):
	mkdir -p $@

clean:
	@echo "Cleaning up..."
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(REP_DIR) $(TXT_REPORT).txt $(TXT_REPORT)_mt.txt

help:
	@echo "Available targets:"
	@echo "  all/build  - Build the bignum_shift_left.o object file."
	@echo "  test       - Build and run all correctness tests."
	@echo "  benchmark  - Build and run all performance benchmarks."
	@echo "  clean      - Remove all generated files."