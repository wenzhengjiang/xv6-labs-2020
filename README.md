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

### [Lazy](https://github.com/wenzhengjiang/xv6-labs-2020/tree/lazy)
Implemented lazy allocation for sbrk.

No new idea in this lab, but lazy allocation is a good demonstration of how flexible the os can be by combining page table and trap(page fault).

### [COW](https://github.com/wenzhengjiang/xv6-labs-2020/tree/cow2)
Implemented copy-on-write fork.

This is another great lab (besides of the page table one). Specifically I learned a lot from [the instructer's solution](https://www.youtube.com/watch?v=S8ZTJKzhQao&feature=youtu.be) which was a good desmonstration on taking baby-steps and verify each step.

My commit history for this lab is like below, with each commit represents a baby-step.

```
Wed Dec 23 12:19:56 cow: Modified fork so parent and child share the same physical pages and got store page fault as expected
Wed Dec 23 12:21:35 cow: Modified traps so the store page fault on copy-on-write page get handled, and met the problem that freeproc freed pages what are still used by parent process
Wed Dec 23 12:52:29 cow: Added reference count so we don't delete page that is sill being used, got OOM because COW does not free page
Wed Dec 23 12:53:23 cow: free page in OOM and passed three test, but failed at file test
Wed Dec 23 13:18:53 cow: file test failed because all processes shared the same copy-on-write page; Fixed it by properly handling COW in copyout; Passed cowtest but usertests failed with panic walk.
Wed Dec 23 13:24:46 cow: Fixed panic walk by checking input address in copytout
Wed Dec 23 14:00:53 cow: got a zero pointer bug in memmove, and found and fixed the bug with gdb
```

### [COW](https://github.com/wenzhengjiang/xv6-labs-2020/tree/thread)
Implemented a simple user-level threading library and some threading programs.

This is a fairly straightforward lab, yet I was still amazed how simple threading can be implemented (by restoring and recovering a few registers).