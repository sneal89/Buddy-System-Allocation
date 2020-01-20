#include "BuddyAllocator.h"
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <cmath>

using namespace std;

BuddyAllocator::BuddyAllocator(int _basic_block_size, int _total_memory_length) {
	basic_block_size = _basic_block_size;
	total_memory_size = _total_memory_length; 
	start = new char[total_memory_size];
	//get how large FreeList should be
	int index = findPower(_total_memory_length, _basic_block_size);
	FreeList.resize(index + 1);
	//initialize all freelist with null overwrite the last 
	for(int i = 0;  i < index; i++) 
	{
		FreeList[i].head =  NULL;
	}
	//initialize head of last index as non-null
	BlockHeader* init = (BlockHeader*)start;
	init->block_size = _total_memory_length;
	init->next = NULL;
	FreeList[index].insert(init);
}

BuddyAllocator::~BuddyAllocator() {
	//free(memory) total memory length
	//free(freeList)
	delete start;
	int index = findPower(total_memory_size, basic_block_size);
	for(int i = 0;  i < index; i++) 
	{
		FreeList[i].head =  NULL;
	}
}

BlockHeader * BuddyAllocator::getbuddy(BlockHeader * addr)
{
	//finds the address of the buddy block by adding the size to the start index
	BlockHeader* addy = (BlockHeader*) ((((char*)addr - start) ^ (addr->block_size)) + start);
	return addy;
}

bool BuddyAllocator::arebuddies(BlockHeader * block1, BlockHeader * block2)
{
	//check if the block addresses make them buddies
	if (getbuddy(block1) == block2)
	{
		return true;
	}
	return false;
}

int BuddyAllocator::findPower(double inUse, double _basic_block_size)
{
	//cout << (int)log2(ceil((float)inUse / (float)_basic_block_size)); 
	//cout << temp << endl;
	float temp = ceil(inUse / _basic_block_size);
	//cout << temp << endl;
	temp = ceil(log2(temp));
	//cout << temp << endl;
	//cout << (int)temp << endl;
	return (int)temp; 
	//log2 returns a floating point must cast int
}

BlockHeader * BuddyAllocator::merge(BlockHeader * block1, BlockHeader * block2)
{
	//given two addresses compare them and for the smaller address ... .. ..
	
	BlockHeader* closerB;
	BlockHeader* farB;
	if(((char*)block1 - start) < ((char*)block2 - start))
	{
		closerB = block1; 
		farB = block2;
	}
	else
	{
		closerB = block2;
		farB = block1;
	}
	closerB->block_size = closerB->block_size * 2;
	closerB->next = farB->next;
	farB->next = nullptr;
	return closerB;
}

BlockHeader * BuddyAllocator::split(BlockHeader * block)
{
	//BlockHeader* budBlock = ((char*)block + ((block->block_size) / 2));
	BlockHeader* budBlock = block;
	budBlock = (BlockHeader*)((char*)block + (block->block_size / 2));
	budBlock->block_size = block->block_size / 2;
	block->block_size = block->block_size / 2;
	budBlock->next = nullptr;
	int i = findPower(budBlock->block_size , basic_block_size);
	return budBlock;
}

char* BuddyAllocator::alloc(int _length) {
	/* This preliminary implementation simply hands the call over the
	the C standard library!
	Of course this needs to be replaced by your implementation.
	*/
	int inUse = _length + sizeof(BlockHeader); //check if this is correct sizeof(thing) could cause seg fault :3
	//cout << basic_block_size << endl;
	//cout << inUse << endl;
	//exits program if not enough memory is allocated
	if(inUse > total_memory_size)
	{
		cout << "Please allocate more memory or choose lower inputs for ackerrman." << endl;
		exit(0);
	}
	int i = findPower(inUse, basic_block_size);
	BlockHeader* tempB;
	BlockHeader* tempBB;
	// no need to split
	if (FreeList[i].head != NULL) //segfault //
	{
		BlockHeader* b = FreeList[i].head;
		//cout << "FREE LIST HEAD " << b << endl;
		//cout << "Block size" << b->block_size << endl;
		//cout << "This is the first remove in alloc : " << FreeList[i].head << endl;
		FreeList[i].remove(FreeList[i].head);
		//cout << "List after non null remove for: " << b->block_size << endl;
		//printlist();
		return (char*)(b + 1);
	}
	else
	{
		int j = i;
		while (FreeList[j].head == NULL && j < FreeList.size()) //it should not go past the end freelist.size() make sure it doesnt go past it in the while loop 
		{
			j++;
		}
		if (j == i)
		{
			return nullptr;
		}
		
		while (j > i) //break up bigger block into 2 smaller
		{
			tempB = FreeList[j].head; //maybe j
			//cout << FreeList[j].head << endl;
			//cout << "List after while loop remove" << endl;
			//cout << "tempB : " << tempB << endl;
			//cout << "j before second : " << j << endl;
			//cout << "second remove in alloc : " << FreeList[j].head << endl;
			FreeList[j].remove(FreeList[j].head);
			tempBB = split(tempB); //issue
			FreeList[j - 1].insert(tempB);
			FreeList[j - 1].insert(tempBB);
			//printlist();
			j--;
		}
		//cout << "this is the final remove : " << tempB << endl;
		FreeList[i].remove(tempB); //remove block to be allocated
		//cout << "List after removing before return for: " << tempB->block_size << endl;
		//printlist();
		return (char*)(tempB + 1); //possibly return b more declaration
	}
	//return new char[_length];
	return nullptr;
}

int BuddyAllocator::free(char* _a) {
	/* Same here! */
	//address should be between start and end of big chunk of mem
	
	bool first = true;
	BlockHeader* b = (BlockHeader*)(_a - sizeof(BlockHeader));
	b->isFree = true;
	BlockHeader* m;
	int i = findPower(b->block_size, basic_block_size);
	BlockHeader * bBud = getbuddy(b);
	// buddy isnt available to merge
	if(bBud->isFree == false || FreeList[i].head == NULL)
	{
		b->isFree = true;
		FreeList[i].insert(b);
		return 0;
	}
	else
	{
		//FreeList[i].remove(bBud);
		//m = merge(b,bBud);

		//go through the list, merge what you can
		while(i < FreeList.size())
		{
			if(first) //special case for first
			{
				first = false;
				FreeList[i].remove(bBud);
				m = merge(b, bBud);
				i++;
			}
			b = m;
			bBud = getbuddy(b);
			if(bBud ->isFree != true || FreeList[i].head == NULL) //no more to free up, break out of loop
			{

				break;
			}
			FreeList[i].remove(bBud); 
			m = merge(b, bBud);
			i++;
			
			
		}	
		//finish merging, insert last largest block into freelist
		FreeList[i].insert(m);
	}
}

void BuddyAllocator::printlist() {
	cout << "Printing the Freelist in the format \"[index] (block size) : # of blocks\"" << endl;
	for (int i = 0; i<FreeList.size(); i++) {
		cout << "[" << i << "] (" << ((1 << i) * basic_block_size) << ") : ";  // block size at index should always be 2^i * bbs
		int count = 0;
		BlockHeader* b = FreeList[i].head;
		// go through the list from head to tail and count
		while (b) {
			count++;
			// block size at index should always be 2^i * bbs
			// checking to make sure that the block is not out of place
			if (b->block_size != (1 << i) * basic_block_size) {
				cerr << "ERROR:: Block is in a wrong list" << endl;
				exit(-1);
			}
			b = b->next;
		}
		cout << count << endl;
	}
}