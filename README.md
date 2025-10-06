# bignum-shift-left

**Version: 1.0.0**

`bignum-shift-left` is a high-performance, standalone module for performing a logical left shift on an arbitrary-precision integer (`bignum_t`). This implementation is written in x86-64 assembly for maximum efficiency.

This module is part of the `bignum-lib` ecosystem and depends on `bignum-common` for core data structures.

## Features

*   Optimized logical left shift for `bignum_t` structures.
*   Written in Yasm x86-64 assembly (System V ABI).
*   Provides a comprehensive test suite and performance benchmarks.

## Prerequisites

To build and test this project, you will need:
*   `make`
*   `gcc` (C compiler)
*   `yasm` (Assembler)

## How to Build and Use

This project produces an object file (`bignum_shift_left.o`) which you can link with your own application.

**1. Clone the repository with submodules:**
```bash
git clone --recurse-submodules https://github.com/kirill-bayborodov/bignum-shift-left.git
cd bignum-shift-left
```

**2. Build the object file:**
```bash
make build
```
The output will be located at `build/bignum_shift_left.o`.

**3. Link with your application:**
When compiling your project, include the object file and specify the include paths for the headers.
```bash
gcc your_app.c build/bignum_shift_left.o -I./include -I./libs/common/include -o your_app
```

## How to Test

The project includes correctness tests and performance benchmarks.

**1. Run correctness tests:**
This will build and run all tests located in the `tests/` directory.
```bash
make test
```

**2. Run performance benchmarks:**
This will build and run all benchmarks from the `benchmarks/` directory.
```bash
make benchmark
```
The output will be located at `doc/*.txt`.

## Clean Up

To remove all generated files (object files, executables, reports ):
```bash
make clean
```
