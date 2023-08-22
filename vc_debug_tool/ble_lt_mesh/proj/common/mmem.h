/********************************************************************************************************
 * @file	mmem.h
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
 * \defgroup mmem Managed memory allocator
 *
 * The managed memory allocator is a fragmentation-free memory
 * manager. It keeps the allocated memory free from fragmentation by
 * compacting the memory when blocks are freed. A program that uses
 * the managed memory module cannot be sure that allocated memory
 * stays in place. Therefore, a level of indirection is used: access
 * to allocated memory must always be done using a special macro.
 *
 * \note This module has not been heavily tested.
 * @{
 */

/**
 * \file
 *         Header file for the managed memory allocator
 *
 */

#pragma once

/*---------------------------------------------------------------------------*/
/**
 * \brief      Get a pointer to the managed memory
 * \param m    A pointer to the struct mmem
 * \return     A pointer to the memory block, or NULL if memory could
 *             not be allcated.
 * \author     Adam Dunkels
 *
 *             This macro is used to get a pointer to a memory block
 *             allocated with mmem_alloc().
 *
 * \hideinitializer
 */
#define MMEM_PTR(m) (struct mmem *)(m)->ptr
struct mmem {
  struct mmem *next;
  unsigned int size;
  void *ptr;
};

/* XXX: tagga minne med "interrupt usage", vilke gr att man r
   speciellt varsam under free(). */

int  mmem_alloc(struct mmem *m, unsigned int size);
void mmem_free(struct mmem *);
void mmem_init(void);


/** @} */
/** @} */
