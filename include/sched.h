#ifndef __SCHED_H
#define __SCHED_H

#include <list.h>

#define SCHED_CANSLEEP		1	/* safe to sleep */

#define TASK_RUNNING		0
#define TASK_SLEEPING		1

#define STACK_FRAME_SIZE	160

struct psw {
	u8 _zero0:1,
	   r:1,			/* PER Mask (R)			*/
	   _zero1:3,
	   t:1,			/* DAT Mode (T)			*/
	   io:1,		/* I/O Mask (IO)		*/
	   ex:1;		/* External Mask (EX)		*/

	u8 key:4,		/* Key				*/
	   _zero2:1,
	   m:1,			/* Machine-Check Mask (M)	*/
	   w:1,			/* Wait State (W)		*/
	   p:1;			/* Problem State (P)		*/
		
	u8 as:2,		/* Address-Space Control (AS)	*/
	   cc:2,		/* Condition Code (CC)		*/
	   prog_mask:4;		/* Program Mask			*/

	u8 _zero3:7,	
	   ea:1;		/* Extended Addressing (EA)	*/

	u32 ba:1,		/* Basic Addressing (BA)	*/
	    _zero4:31;

	u64 ptr;
};

struct regs {
	struct psw psw;
	u64 gpr[16];
	u64 cr[16];
	u32 ar[16];
	/* FIXME: fpr[16] */
};

/*
 * This structure describes a running process.
 */
struct task {
	struct regs regs;		/* saved registers */

	struct list_head run_queue;	/* runnable list */
	struct list_head proc_list;	/* processes list */

	int state;			/* state */
};

extern void init_sched();		/* initialize the scheduler */
extern int create_task(int (*f)());	/* create a new task */

#endif
