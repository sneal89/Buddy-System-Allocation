#include "Ackerman.h"
#include "BuddyAllocator.h"
#include <stdio.h>
#include <unistd.h> 
#include <stdlib.h>
#include <cmath>

void easytest(BuddyAllocator* ba){
  // be creative here
  // know what to expect after every allocation/deallocation cycle

  // here are a few examples
  ba->printlist();
  // allocating a byte
  char * mem = ba->alloc (1);
  //char * mem2 = ba -> alloc(1);
  //char * mem3 = ba -> alloc(1);
  //char * mem4 = ba->alloc(1);
  // now print again, how should the list look now
  //ba->printlist ();
  //ba->free(mem3);

  ba->printlist ();
  //ba->free(mem2);
  //ba->printlist ();
  ba->free (mem); // give back the memory you just allocated
  ba->printlist ();
  //ba->free(mem4);
  //ba->printlist(); // shouldn't the list now look like as in the beginning

}

int main(int argc, char ** argv) {

  int basic_block_size = 128, memory_length = 128 * 1024 * 1024;
  //basic_block_size = 1; memory_length = 0;

  int c;
  while ((c = getopt (argc, argv, "b:s:")) != -1)
  {
    //cout << optarg << endl;
    //cout << c << endl;
    switch (c)
      {
      case 'b':
        basic_block_size = atoi(optarg);
        break;
      case 's':
        memory_length = atoi(optarg);
        break;
      case '?':
        if (optopt == 'c')
          fprintf (stderr, "Option -%c requires an argument.\n", optopt);
        else if (isprint (optopt))
          fprintf (stderr, "Unknown option `-%c'.\n", optopt);
        else
          fprintf (stderr,
                   "Unknown option character `\\x%x'.\n",
                   optopt);
        return 1;
      default:
        abort ();
      }
  }
  //cout << basic_block_size << endl;
  //cout << memory_length << endl;
  //check if memory is neg
  if(basic_block_size < 0 || memory_length < 0)
  {
    cout << "Cannot run: Block size and total memory must both be positive." << endl;
    exit(0);
  }  
  //check if bbs > total allocated
  if(basic_block_size > memory_length)
  {
    cout << "Cannot run: Block size must be larger than total memory." << endl;
    exit(0);
  }
  //check if bbs and total are powers of 2
  if(ceil(log2(basic_block_size)) != floor(log2(basic_block_size)) || ceil(log2(memory_length)) != floor(log2(memory_length)))
  {
    cout << "Cannot run: Block size and total memory size must be powers of two." << endl;
    exit(0);
  }

  // create memory manager
  BuddyAllocator * allocator = new BuddyAllocator(basic_block_size, memory_length);

  // the following won't print anything until you start using FreeList and replace the "new" with your own implementation
  easytest (allocator);

  
  // stress-test the memory manager, do this only after you are done with small test cases
  Ackerman* am = new Ackerman ();
  am->test(allocator); // this is the full-fledged test. 
  
  // destroy memory manager
  delete allocator;
}
//is buddy checks if buddy is free, and if it is, merge



//getopt goes in here
//taken as string then convert to num for both option
//mark them as optional or manditory
//mark both as optional
//call in while loop and that takes care of it till its NULL
//even if it doesnt find any values it should still work
//check if its actually a number 
