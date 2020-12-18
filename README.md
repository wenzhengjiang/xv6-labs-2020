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

Notes:
* MMU gets the physical addr of the page table and uses it to translate virtual address to physical address.
* The page table in xv6 is a tree structure with page-sized nodes. 
* Each non-leaf node contains 512 PTEs. Each PTE contains the physical addr of another PTE or leaf node and permission.
* Each leaf node is a physical memory page.
* Kernel page table is directly mapped to physical memory, so we won't have this "how to get physical addr of page table before setting up MMU" problem.
* I did not pass the "execout" test for the kernel page table user mapping :(

### [Traps](https://github.com/wenzhengjiang/xv6-labs-2020/tree/traps)
Implemented backtrace by walking up the stack, and periodic callback system calls.

This lab requires a good understanding about what's happending during the trap. I was surprised how easy to implement callback is the os level with a bit of hacking in trap functions.

Note:
* In xv6, each process has exactly one kernel stack page and user stack page, which store all the function calls states in kernel space and user space respectively.
* All necessary trap handling data is stored in the trapframe. which includes the registers and a few kernel states.