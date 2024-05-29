/*
* Tencent is pleased to support the open source community by making Libco available.

* Copyright (C) 2014 THL A29 Limited, a Tencent company. All rights reserved.
*
* Licensed under the Apache License, Version 2.0 (the "License"); 
* you may not use this file except in compliance with the License. 
* You may obtain a copy of the License at
*
*	http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, 
* software distributed under the License is distributed on an "AS IS" BASIS, 
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
* See the License for the specific language governing permissions and 
* limitations under the License.
*/


#ifndef __CO_ROUTINE_INNER_H__

#include "co_routine.h"
#include "coctx.h"
struct stCoRoutineEnv_t;
struct stCoSpec_t
{
	void *value;
};

struct stStackMem_t
{
	stCoRoutine_t* occupy_co;  // 占用该栈内存的协程指针。这个字段指向当前正在使用这块栈内存的协程。如果该栈内存未被任何协程占用，则该指针可能为空。
	int stack_size;
	char* stack_bp; //stack_buffer + stack_size  // 栈基指针（Base Pointer），指向栈内存的基址（stack base）。具体来说，它指向 stack_buffer + stack_size 位置，即栈的顶部位置。注意，栈通常是向低地址方向增长的，因此栈基指针指向栈的最高地址。
	char* stack_buffer;  

};

struct stShareStack_t
{
	unsigned int alloc_idx;
	int stack_size;
	int count;
	stStackMem_t** stack_array;
};



struct stCoRoutine_t
{
	stCoRoutineEnv_t *env;  // 协程环境指针，指向该协程所属的环境。这个环境通常包含协程调度器和其他协程相关的上下文信息。
	pfn_co_routine_t pfn;  // 协程的入口函数指针。当协程被调度执行时，会调用这个函数。
	void *arg;  // 传递给协程入口函数的参数。通过这个指针，协程可以访问调用者传递的数据。
	coctx_t ctx;  // 协程的上下文，包括寄存器状态和栈指针。ctx 保存了协程的 CPU 寄存器和栈信息，以便协程可以在暂停后恢复执行。

	char cStart;  // 标志协程是否已经启动。0 表示未启动，1 表示已启动。
	char cEnd;  // 标志协程是否已结束。0 表示未结束，1 表示已结束。
	char cIsMain;  // 标志是否是主协程。1 表示是主协程，0 表示不是
	char cEnableSysHook;  // 标志是否启用了系统钩子。系统钩子可以让协程在某些系统调用（如网络 I/O）上挂起
	char cIsShareStack;  // 标志协程是否使用共享栈。1 表示使用共享栈，0 表示使用独立栈

	void *pvEnv;  // 通用环境变量指针，可以用于存储与协程相关的其他环境信息

	//char sRunStack[ 1024 * 128 ];
	stStackMem_t* stack_mem;  // 指向协程栈内存的指针。当协程使用独立栈时，该指针指向栈的内存结构体


	//save satck buffer while confilct on same stack_buffer;
	char* stack_sp;   // 协程栈指针，指向协程当前的栈顶位置。在协程切换时保存和恢复
	unsigned int save_size;  // 保存的栈数据大小。当协程切换时，用于保存协程栈的大小
	char* save_buffer;  // 保存栈数据的缓冲区。当协程切换时，用于临时存储协程的栈数据

	stCoSpec_t aSpec[1024];  // 协程特定的数据存储，用于存储每个协程的私有数据。可以通过特定索引访问这些数据

};



//1.env
void 				co_init_curr_thread_env();
stCoRoutineEnv_t *	co_get_curr_thread_env();

//2.coroutine
void    co_free( stCoRoutine_t * co );
void    co_yield_env(  stCoRoutineEnv_t *env );

//3.func



//-----------------------------------------------------------------------------------------------

struct stTimeout_t;
struct stTimeoutItem_t ;

stTimeout_t *AllocTimeout( int iSize );
void 	FreeTimeout( stTimeout_t *apTimeout );
int  	AddTimeout( stTimeout_t *apTimeout,stTimeoutItem_t *apItem ,uint64_t allNow );

struct stCoEpoll_t;
stCoEpoll_t * AllocEpoll();
void 		FreeEpoll( stCoEpoll_t *ctx );

stCoRoutine_t *		GetCurrThreadCo();
void 				SetEpoll( stCoRoutineEnv_t *env,stCoEpoll_t *ev );

typedef void (*pfnCoRoutineFunc_t)();

#endif

#define __CO_ROUTINE_INNER_H__
