// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmems[NCPU];

void
kinit()
{
  for (int i = 0; i < NCPU; i++) {
    initlock(&kmems[i].lock, "kmem");
  }
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;
  push_off();
  int cid = cpuid();
  acquire(&kmems[cid].lock);
  r->next = kmems[cid].freelist;
  kmems[cid].freelist = r;
  release(&kmems[cid].lock);
  pop_off();
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  push_off();
  int cid = cpuid();

  acquire(&kmems[cid].lock);
  r = kmems[cid].freelist;
  if(r) {
    kmems[cid].freelist = r->next;
  }
  release(&kmems[cid].lock);

  // If the current CPU does not have free memory, steal one from another CPU. Note that:
  //
  //    1. Make sure to acquire locks of other CPUs based on a specific order to avoid deadlock.
  //    2. once a page is stolen by a cpu, it will belong to that cpu after that page is freed.
  //       In that way, the free memory that originally belongs to the freerange cpu will be gradually
  //       moved to other cpus. 
  if (!r) {
    for(int i = 0; i < NCPU && !r; i++) {
      acquire(&kmems[i].lock);
      r = kmems[i].freelist;
      if (r) {
        kmems[i].freelist = r->next;
      }
      release(&kmems[i].lock);
    }
  }

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk

  pop_off();
  return (void*)r;
}
