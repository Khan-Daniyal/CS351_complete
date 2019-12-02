/*
 *  * mm.c - The fastest, least memory-efficient malloc package.
 *   */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#include "mm.h"
#include "memlib.h"
/****DEFINING MACROS************/
/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)

#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))
#define Blk_Hdr_Size (ALIGN(sizeof(blockHdr)))
#define Blk_Fdr_Size (ALIGN(sizeof(blockFdr)))
#define Free_Blk_Size (ALIGN(sizeof(blockHdr)+sizeof(blockFdr)))
#define Min_Size 32 // For split purpose

/****DEFINING FUNCTIONS*********/
void print_heap();
void *find_fit (size_t size);
void split();
void coalesce();
/*******DEFINING STRUCT********/
typedef struct footer blockFdr;
struct footer{
  size_t size;
 };

typedef struct header blockHdr;
struct header{
  size_t size;
  blockHdr *next;
  blockHdr *prior;
};


/*
 *  * mm_init - initialize the malloc package.
 *   */
int mm_init(void)
{
  /*initiate header*/
  blockHdr *p = mem_sbrk((Free_Blk_Size));
  p->size = Free_Blk_Size;
  p->next = p;
  p->prior = p;
  /*initiate footer*/
  blockFdr *fp = (blockFdr*)( (char*)p + Blk_Hdr_Size);
  fp->size = Free_Blk_Size;
  return 0;
}

void print_heap(){
  blockHdr *bp = mem_heap_lo();
  while (bp < (blockHdr*)mem_heap_hi()){
    printf("%s block at %p, size %d\n",
	   (bp->size&1)? "allocated":"free",
	    bp,
	   (int) (bp->size & ~1));
    bp = (blockHdr *) ((char *) bp + (bp->size & ~1));
  }
}
/*
 *  * mm_malloc - Allocate a block by incrementing the brk pointer.
 *   *     Always allocate a block whose size is a multiple of the alignment.
 *    */
void *mm_malloc(size_t size)
{
  if (size>100 && size<128){
    size =128;
  }
  if (size>400 && size <512){
    size = 512;
  }
  int newsize = ALIGN(size + Free_Blk_Size);
  /* blockHdr *bp =NULL; */
  if (size <=0)  return NULL;
  /*else if (newsize==4095) newsize=8190; */
  blockHdr *bp = find_fit(newsize);
  blockFdr *fp = NULL;
  if (bp == NULL  ) {
    /*Did not find the right size*/
    bp = mem_sbrk(newsize); // low level malloc

    if ( (long)bp == -1 ){
      return NULL;
    } /* error exception */
    else {
      /*new block allocated */
      bp-> size = newsize|1;
      fp = (blockFdr*) ((char*)bp + newsize - Blk_Fdr_Size);
      fp -> size = newsize |1;
    }
  }
  else {
    if ( bp->size > 2*newsize ){
      split (bp,fp,newsize); /* hardcode */
    }

    else{
      fp = (blockFdr*) ((char*)bp + bp->size - Blk_Fdr_Size);
      fp -> size |=1;
      /*find fit found a size block;*/
      bp-> size |=1;
      /* Get rid of current free_block */
      bp->prior -> next = bp->next;
      bp->next ->prior = bp->prior;
    }
  }

  return (void * ) ((char *) bp + Blk_Hdr_Size) ;

}

/* newsize is required_payload + sizeof(headrer) + sizeof(footer) */
void split(blockHdr* bp, blockFdr* fp,  size_t newsize){

  size_t freespace =  bp->size - newsize;
  blockHdr* freehead = NULL;
  blockFdr* freefoot = NULL;

  freehead = (blockHdr*) ((char*)bp + newsize);
  freehead->size= freespace & ~1;
  freefoot = (blockFdr*) ((char*)freehead + freespace - Blk_Fdr_Size);
  freefoot->size = freespace & ~1;

  fp =(blockFdr*) ((char*)freehead - Blk_Fdr_Size);
  fp->size = newsize|1;
  bp->size = newsize|1;
  bp->next->prior = bp->prior;
  bp->prior->next = bp->next;

  blockHdr* p = NULL ;
  p = mem_heap_lo();
  freehead->next = p-> next;
  freehead -> prior = p;
  p->next = freehead;
  freehead->next->prior = freehead;

}

void *find_fit (size_t size)
{

  blockHdr *p;
  for (p = ((blockHdr *)mem_heap_lo())-> next;
       p != mem_heap_lo() && p->size < size;
       p = p->next);
  if(p != mem_heap_lo())
    {
      return p;
    }
  else{
    return NULL;
  }
}

/*
 *  * mm_free - Freeing a block does nothing.
 *   */
void mm_free(void *ptr)
{
  blockHdr *bp = ptr - Blk_Hdr_Size;
  blockHdr *head = mem_heap_lo();
  bp->size &= ~1;
  bp->next = head->next;
  bp->prior = head;
  head->next = bp;
  bp ->next->prior = bp;
  /* coalesce()+ append back to the list  ; */
  blockFdr *fp = (blockFdr*) ((char*)bp +bp->size - Blk_Fdr_Size);
  fp->size &= ~1;
  coalesce(bp);
}

/*
 *  *coalesce
 *   *
 *    */
void coalesce (blockHdr* bp) {
  /*Previous*/
  blockFdr *pr_fp = (blockFdr*)((char*) bp- Blk_Fdr_Size);
  int  pr_al;
  /*next*/
  blockHdr *nx_bp = (blockHdr*)((char*) bp+ bp->size);
  int ne_al ;

  if((char*)nx_bp+Free_Blk_Size < (char*) mem_heap_hi()){
    ne_al=nx_bp->size & 1;
  }
  else{
    ne_al = 1;
  }
  if((char*)pr_fp > (char*) mem_heap_lo()+Free_Blk_Size){
    pr_al = pr_fp->size & 1;
  }
  else {
    pr_al = 1;
  }
  if (pr_al && ne_al )
    {
      return;
    }
  else if (ne_al && !pr_al){
    ((blockHdr*)((char*)bp - pr_fp->size))->size += bp->size;
    ((blockHdr*)((char*)bp + bp->size - Blk_Fdr_Size))->size = ((blockHdr*)((char*)bp - pr_fp->size))->size;
    bp->next->prior = bp->prior;
    bp->prior->next = bp->next;
    }

  else if (pr_al && !ne_al) {
    bp->size += nx_bp->size;
    ((blockFdr *)((char *)nx_bp + nx_bp->size - Blk_Fdr_Size))->size = bp->size;
    nx_bp->next->prior = nx_bp->prior;
    nx_bp->prior->next = nx_bp->next;
    }

  else if (!pr_al && !ne_al){
    ((blockHdr *)((char*)bp - pr_fp->size))->size += (bp->size + nx_bp->size);
    ((blockHdr*)((char*)bp + bp->size - Blk_Fdr_Size))->size = ((blockHdr *)((char*)bp - pr_fp->size))->size;
    ((blockFdr*)((char*)nx_bp+nx_bp->size - Blk_Fdr_Size))->size = ((blockHdr*)((char *)bp-pr_fp->size))->size;
    bp->next->prior = bp->prior;
    bp->prior -> next = bp->next;
    nx_bp->next->prior = nx_bp->prior;
    nx_bp->prior->next = nx_bp->next;
    }
  return;
}

/*
 *  * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 *   */
void *mm_realloc(void *ptr, size_t size)
{
  blockHdr *bp = ptr - Blk_Hdr_Size;
  void *newptr = NULL;
  size_t _size = ALIGN(size + Free_Blk_Size);
  size_t oldsize = bp->size;
  if(_size ==0){
    mm_free(ptr);
    return 0;
  }
  if (oldsize > _size){
    return ptr;
  }

  if (ptr == NULL) {
    return mm_malloc(_size);
  }

  newptr = mm_malloc(_size);
  if (!newptr){
    return 0;
  }
  int copySize = bp->size-Blk_Hdr_Size;
  if(size < copySize) copySize  = _size;
  memcpy(newptr, ptr, copySize);
  mm_free(ptr);
  return newptr;
}
