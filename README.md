xv6 is a re-implementation of Dennis Ritchie's and Ken Thompson's Unix
Version 6 (v6).  xv6 loosely follows the structure and style of v6,
but is implemented for a modern RISC-V multiprocessor using ANSI C.

This repo implements an **enhanced** version of xv6 by following the labs of [6.S081](https://pdos.csail.mit.edu/6.828/2020/).

## Building and Running xv6

You will need a RISC-V "newlib" tool chain from
https://github.com/riscv/riscv-gnu-toolchain, and qemu compiled for
riscv64-softmmu. Once they are installed, and in your shell
search path, you can run "make qemu".

## Labs

### [Unix utilities](https://github.com/wenzhengjiang/xv6-labs-2020/tree/pgtbl)
Did some warmup exercises for getting used to C and unix syscalls. 

Generally easy to complete. But pipes were quite interesting to play with :)

### [System calls](https://github.com/wenzhengjiang/xv6-labs-2020/tree/syscall)
Implemented two simple syscalls.

Still fairly easy to complete.

### [Page tables](https://github.com/wenzhengjiang/xv6-labs-2020/tree/pgtbl)
Implemented a per-process kernel page table which also includes user memory mapping.

This lab was a lot of harder than the previous two. Page table looked easy, yet, it's powerful and not straightforward to use. I was glad I learned a lot more about it after this lab.

NOTE: I did not pass the "execout" test for the kernel page table user mapping.