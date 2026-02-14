#!/bin/bash

# Build all benchmarks
gcc -o l_syscall_bench l_syscall_bench.c -lrt
gcc -o l_ethernet_bench l_ethernet_bench.c -lrt
nasm sys_test.asm -o sys_test.app
