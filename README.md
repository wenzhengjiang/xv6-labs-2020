xv6 is a re-implementation of Dennis Ritchie's and Ken Thompson's Unix
Version 6 (v6).  xv6 loosely follows the structure and style of v6,
but is implemented for a modern RISC-V multiprocessor using ANSI C.

This repo implements an **enhanced** version of xv6 by following the labs of [6.S081](https://pdos.csail.mit.edu/6.828/2020/).

## Building and Running xv6

You will need a RISC-V "newlib" tool chain from
https://github.com/riscv/riscv-gnu-toolchain, and qemu compiled for
riscv64-softmmu. Once they are installed, and in your shell
search path, you can run "make qemu".

## Lab Notes

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

My commit history describes my strategy for tackling this lab. Each commit represents a baby-step for implementing the solution.

```
Wed Dec 23 12:19:56 cow: Modified fork so parent and child share the same physical pages and got store page fault as expected
Wed Dec 23 12:21:35 cow: Modified traps so the store page fault on copy-on-write page get handled, and met the problem that freeproc freed pages what are still used by parent process
Wed Dec 23 12:52:29 cow: Added reference count so we don't delete page that is sill being used, got OOM because COW does not free page
Wed Dec 23 12:53:23 cow: free page in OOM and passed three test, but failed at file test
Wed Dec 23 13:18:53 cow: file test failed because all processes shared the same copy-on-write page; Fixed it by properly handling COW in copyout; Passed cowtest but usertests failed with panic walk.
Wed Dec 23 13:24:46 cow: Fixed panic walk by checking input address in copytout
Wed Dec 23 14:00:53 cow: got a zero pointer bug in memmove, and found and fixed the bug with gdb
```

### [Thread](https://github.com/wenzhengjiang/xv6-labs-2020/tree/thread)
Implemented a simple user-level threading library and some threading programs.

This is a fairly straightforward lab, yet I was still amazed how simple threading can be implemented (by restoring and recovering a few registers).

### [Lock](https://github.com/wenzhengjiang/xv6-labs-2020/tree/lock)
Improved parallelism of memory allocator and block cache by reducing lock contention.

This is quite fun lab. The way to reduce lock contention is usually using more fine-grained locks, but that ofen introduces deadlock issues, which is the main challenge for this lab.

Note:
* In the per-cpu memory allocator implementation, it's possible to at most acquire a freelist lock at one time if every time only one page is stolen by a cpu.
* In the block cache implementation, it is unavoidable to acquire multiple locks when moving a buffer from one bucket to another bucket, and it has to be in a critical section. I managed to minimize the critical section to only cover the buffer evication part, so there wasn't many lock conflits caused in the tests.

### [File system](https://github.com/wenzhengjiang/xv6-labs-2020/tree/fs)
Improved the filesystem to support large files and symbolic links.

This labs is mainly used to enhance your understanding of inode and pathname implementation of xv6 filesystem.

Note:
* in-memory inodes is managed by a reference count mechanism. For example, `open` a file will increase the reference count for the underlying inode.

### [Mmap](https://github.com/wenzhengjiang/xv6-labs-2020/tree/mmap)
Implemented mmap and munmap system calls.

This labs is not difficult to complete as long as you carefully take  small steps.

Note:
* in-memory inodes is managed by a reference count mechanism. For example, `open` a file will increase the reference count for the underlying inode.