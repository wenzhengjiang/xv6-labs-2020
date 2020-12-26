// Buffer cache.
//
// The buffer cache is a linked list of buf structures holding
// cached copies of disk block contents.  Caching disk blocks
// in memory reduces the number of disk reads and also provides
// a synchronization point for disk blocks used by multiple processes.
//
// Interface:
// * To get a buffer for a particular disk block, call bread.
// * After changing buffer data, call bwrite to write it to disk.
// * When done with the buffer, call brelse.
// * Do not use the buffer after calling brelse.
// * Only one process at a time can use a buffer,
//     so do not keep them longer than necessary.


#include "types.h"
#include "param.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "riscv.h"
#include "defs.h"
#include "fs.h"
#include "buf.h"

#define NBUCKET 13

struct {
  struct spinlock lock;
  struct buf buf[NBUF];
} bcache;

struct spinlock bucket_lock[NBUCKET];
struct buf *buckets[NBUCKET][NBUF];

void
binit(void)
{
  struct buf *b;

  initlock(&bcache.lock, "bcache");

  for (int i = 0; i < NBUCKET; i++) {
    initlock(&bucket_lock[i], "bcache");
  }
  // Create linked list of buffers
  int id = 0;
  for(b = bcache.buf; b < bcache.buf+NBUF; b++){
    initsleeplock(&b->lock, "buffer");
    b->id = id;
    buckets[b->blockno%NBUCKET][id] = b;
    id++;
  }
}

// Look through buffer cache for block on device dev.
// If not found, allocate a buffer.
// In either case, return locked buffer.
static struct buf*
bget(uint dev, uint blockno)
{
  int bucketno = blockno % NBUCKET;
  // Is the block already cached?
  acquire(&bucket_lock[bucketno]);
  for (int i = 0; i < NBUF; i++) {
    struct buf *b = buckets[bucketno][i];
    if (!b) continue;
    if(b->dev == dev && b->blockno == blockno){
      b->refcnt++;
      release(&bucket_lock[bucketno]);
      acquiresleep(&b->lock);
      return b;
    }
  }
  // Release the current bucket lock and re-acqure it after acquiring bcache.lock to avoid deadlock.
  release(&bucket_lock[bucketno]);

  // Not cached.
  // Create a critical zone to avoid deadlock - in this zone, we can freely acquire multiple bucket locks without worrying about causing deadlocks.
  acquire(&bcache.lock);
  acquire(&bucket_lock[bucketno]);
  // In case the buf was created by another process.
  for (int i = 0; i < NBUF; i++) {
    struct buf *b = buckets[bucketno][i];
    if (!b) continue;
    if(b->dev == dev && b->blockno == blockno){
      b->refcnt++;
      release(&bucket_lock[bucketno]);
      acquiresleep(&b->lock);
      return b;
    }
  }

  // Find least recently used (LRU) unused buffer.
  struct buf *lru = 0;
  while (lru == 0) {
    for(struct buf *b = bcache.buf; b < bcache.buf+NBUF; b++){
      if (b->refcnt == 0 && (lru == 0 || b->last_time_used < lru->last_time_used)) {
        lru = b;
      }
    }
    if(lru) {
      int lru_bucketno = lru->blockno % NBUCKET;
      if (lru_bucketno != bucketno) {
        acquire(&bucket_lock[lru_bucketno]);
      }
      // In case the buf was referred by another process. In that case, we need to find an LRU unused buffer again.
      if (lru->refcnt != 0) {
        lru = 0;
        if (lru_bucketno != bucketno) {
          release(&bucket_lock[lru_bucketno]);
        }
        continue;
      }
      buckets[lru_bucketno][lru->id] = 0;
      if (lru_bucketno != bucketno) {
        release(&bucket_lock[lru->blockno%NBUCKET]);
      }
      lru->dev = dev;
      lru->blockno = blockno;
      lru->valid = 0;
      lru->refcnt = 1;
      buckets[bucketno][lru->id] = lru;
      release(&bucket_lock[bucketno]);
      release(&bcache.lock);
      acquiresleep(&lru->lock);
      return lru;
    }
  }
  panic("bget: no buffers");
}

// Return a locked buf with the contents of the indicated block.
struct buf*
bread(uint dev, uint blockno)
{
  struct buf *b;

  b = bget(dev, blockno);
  if(!b->valid) {
    virtio_disk_rw(b, 0);
    b->valid = 1;
  }
  return b;
}

// Write b's contents to disk.  Must be locked.
void
bwrite(struct buf *b)
{
if(!holdingsleep(&b->lock))
    panic("bwrite");
  virtio_disk_rw(b, 1);
}

// Release a locked buffer.
void
brelse(struct buf *b)
{
  if(!holdingsleep(&b->lock))
    panic("brelse");

  releasesleep(&b->lock);

  acquire(&bucket_lock[b->blockno%NBUCKET]);
  b->last_time_used = ticks;
  b->refcnt--;
  release(&bucket_lock[b->blockno%NBUCKET]);
}

void
bpin(struct buf *b) {
  acquire(&bucket_lock[b->blockno%NBUCKET]);
  b->refcnt++;
  release(&bucket_lock[b->blockno%NBUCKET]);
}

void
bunpin(struct buf *b) {
  acquire(&bucket_lock[b->blockno%NBUCKET]);
  b->refcnt--;
  release(&bucket_lock[b->blockno%NBUCKET]);
}


