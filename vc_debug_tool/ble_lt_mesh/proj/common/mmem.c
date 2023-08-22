/********************************************************************************************************
 * @file	mmem.c
 *
 * @brief	for TLSR chips
 *
 * @author	telink
 * @date	Sep. 30, 2010
 *
 * @par     Copyright (c) 2010, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *
 *******************************************************************************************************/
/*
 * This file is part of the Contiki operating system.
 */

/**
 * \file
 *         Implementation of the managed memory allocator
 */


#include "mmem.h"
#include "list.h"

#define		MMEM_SIZE	2048

LIST(mmemlist);
unsigned int avail_memory;
static char memory[MMEM_SIZE];

/*---------------------------------------------------------------------------*/
/**
 * \brief      Allocate a managed memory block
 * \param m    A pointer to a struct mmem.
 * \param size The size of the requested memory block
 * \return     Non-zero if the memory could be allocated, zero if memory
 *             was not available.
 * \author     Adam Dunkels
 *
 *             This function allocates a chunk of managed memory. The
 *             memory allocated with this function must be deallocated
 *             using the mmem_free() function.
 *
 *             \note This function does NOT return a pointer to the
 *             allocated memory, but a pointer to a structure that
 *             contains information about the managed memory. The
 *             macro MMEM_PTR() is used to get a pointer to the
 *             allocated memory.
 *
 */
int
mmem_alloc(struct mmem *m, unsigned int size)
{
  /* Check if we have enough memory left for this allocation. */
  if(avail_memory < size) {
    return 0;
  }

  /* We had enough memory so we add this memory block to the end of
     the list of allocated memory blocks. */
  list_add(mmemlist, m);

  /* Set up the pointer so that it points to the first available byte
     in the memory block. */
  m->ptr = &memory[MMEM_SIZE - avail_memory];

  /* Remember the size of this memory block. */
  m->size = size;

  /* Decrease the amount of available memory. */
  avail_memory -= size;

  /* Return non-zero to indicate that we were able to allocate
     memory. */
  return 1;
}
/*---------------------------------------------------------------------------*/
/**
 * \brief      Deallocate a managed memory block
 * \param m    A pointer to the managed memory block
 * \author     Adam Dunkels
 *
 *             This function deallocates a managed memory block that
 *             previously has been allocated with mmem_alloc().
 *
 */
void *  memmove(void * dest, const void * src, unsigned int n);
void
mmem_free(struct mmem *m)
{
  struct mmem *n;

  if(m->next != 0) {
    /* Compact the memory after the allocation that is to be removed
       by moving it downwards. */
    memmove(m->ptr, m->next->ptr,
	    &memory[MMEM_SIZE - avail_memory] - (char *)m->next->ptr);

    /* Update all the memory pointers that points to memory that is
       after the allocation that is to be removed. */
    for(n = m->next; n != 0; n = n->next) {
      n->ptr = (void *)((char *)n->ptr - m->size);
    }
  }

  avail_memory += m->size;

  /* Remove the memory block from the list. */
  list_remove(mmemlist, m);
}
/*---------------------------------------------------------------------------*/
/**
 * \brief      Initialize the managed memory module
 * \author     Adam Dunkels
 *
 *             This function initializes the managed memory module and
 *             should be called before any other function from the
 *             module.
 *
 */
void
mmem_init(void)
{
  list_init(mmemlist);
  avail_memory = MMEM_SIZE;
}
/*---------------------------------------------------------------------------*/

/** @} */
