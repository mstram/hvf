/*
 * (C) Copyright 2007-2011  Josef 'Jeff' Sipek <jeffpc@josefsipek.net>
 *
 * This file is released under the GPLv2.  See the COPYING file for more
 * details.
 */

#include <binfmt_elf.h>
#include <symtab.h>
#include <mm.h>
#include <cpu.h>
#include <dat.h>
#include <slab.h>
#include <page.h>
#include <buddy.h>
#include <io.h>
#include <sched.h>
#include <device.h>
#include <console.h>
#include <interrupt.h>
#include <magic.h>
#include <guest.h>
#include <sclp.h>

static struct psw new_io_psw = {
	.ea	= 1,
	.ba	= 1,

	.ptr  = (u64) &IO_INT,
};

static struct psw new_ext_psw = {
	.ea	= 1,
	.ba	= 1,

	.ptr	= (u64) &EXT_INT,
};

static struct psw new_svc_psw = {
	.ea	= 1,
	.ba	= 1,

	.ptr	= (u64) &SVC_INT,
};

static struct psw new_pgm_psw = {
	.ea	= 1,
	.ba	= 1,

	.ptr	= (u64) &PGM_INT,
};

static struct psw new_mch_psw = {
	.ea	= 1,
	.ba	= 1,

	.ptr	= (u64) &MCH_INT,
};

u8 *int_stack_ptr;

struct fs *sysfs;

const u8 zeropage[PAGE_SIZE];

/* the time HVF got IPLd */
struct datetime ipltime;

static void init_int_stack(void)
{
	struct page *page;

	page = alloc_pages(0, ZONE_NORMAL);
	assert(page);

	int_stack_ptr = PAGE_SIZE + (u8*)page_to_addr(page);
}

static void idle_task_body(void)
{
	/*
	 * Warning: hack alert! The following overrides what __init_task
	 * set, this allows us to skip the usual start_task wrapper.
	 */
	current->regs.psw.w   = 1;
	current->regs.psw.ptr = MAGIC_PSW_IDLE_CODE;

	/*
	 * Load the new PSW that'll wait with special magic code set
	 */
	lpswe(&current->regs.psw);

	BUG();
}

static void __add_internal_users()
{
	struct list_head vdevs;
	struct directory_prop prop = {
		.got_storage = 1,
		.storage = 1024 * 1024, /* 1MB */
	};

	INIT_LIST_HEAD(&vdevs);

	directory_alloc_user("*LOGIN", AUTH_G, &prop, &vdevs);
}

static int __finish_loading(void *data)
{
	struct virt_sys *login;
	u32 iplsch;
	char *err;
	int ret;

	iplsch = (u32) (u64) data;

	/*
	 * Load the config file
	 */
	sysfs = load_config(iplsch);
	if (IS_ERR(sysfs))
		BUG();

	/* add internal users */
	__add_internal_users();

	/*
	 * Alright, we have the config, we now need to set up the *LOGIN
	 * guest and attach the operator console rdev to it
	 */
	login = guest_create("*LOGIN", NULL);
	if (!login) {
		err = "failed to create guest";
		goto die;
	}

	ret = guest_ipl_nss(login, "login");
	if (ret) {
		err = "failed to IPL NSS";
		goto die;
	}

	/* start *LOGIN */
	ret = guest_begin(login);
	if (ret) {
		err = "failed to begin guest";
		goto die;
	}

	/*
	 * attach the operator rdev to *LOGIN, any available vdev
	 *
	 * Since we already IPL'd & began the guest, this will generate a
	 * Channel Report Word and a Machine Check Interrupt in the guest.
	 */
	ret = guest_attach(login, sysconf.oper_con, -1);
	if (ret) {
		err = "failed to attach operator console";
		goto die;
	}

	/*
	 * At this point we know that *LOGIN is running.  From this point
	 * on, we can handle Machine Check Interrupts (of the
	 * Channel-Report-Pending subclass) safely and sanely.  Let's enable
	 * the the subclass.
	 */
	set_cr(14, get_cr(14) | BIT64(35));

	/*
	 * IPL is more or less done
	 */
	get_parsed_tod(&ipltime);

	sclp_msg("IPL AT %02d:%02d:%02d UTC %04d-%02d-%02d\n\n",
		   ipltime.th, ipltime.tm, ipltime.ts, ipltime.dy,
		   ipltime.dm, ipltime.dd);

	return 0;

die:
	sclp_msg("Failed to initialize '*LOGIN' guest: %s\n", err);
	BUG();
	return 0;
}

/*
 * This is where everything starts
 */
void start(u64 __memsize, u32 __iplsch, Elf64_Ehdr *__elfhdr)
{
	u64 first_free_page;
	u64 struct_page_bytes;
	struct psw psw;

	/*
	 * ticks starts at 0
	 */
	ticks = 0;

	/*
	 * save total system memory size & the symbol table pointer
	 */
	memsize = __memsize;
	symtab = __elfhdr;

	/*
	 * Initialize struct page entries
	 */
	init_pages();

	/*
	 * Calculate address of the first free page (we may have to round
	 * up)
	 */
	struct_page_bytes = (memsize >> PAGE_SHIFT) * sizeof(struct page);

	first_free_page = (u64) PAGE_INFO_BASE + struct_page_bytes;
	if (struct_page_bytes & (PAGE_SIZE-1))
		first_free_page += PAGE_SIZE - (struct_page_bytes & (PAGE_SIZE-1));

	/*
	 * Initialize the buddy allocator
	 */
	init_buddy_alloc(first_free_page);

	/*
	 * Initialize slab allocator default caches
	 */
	init_slab();

	/*
	 * Set up interrupt PSWs
	 */
	memcpy(IO_INT_NEW_PSW, &new_io_psw, sizeof(struct psw));
	memcpy(EXT_INT_NEW_PSW, &new_ext_psw, sizeof(struct psw));
	memcpy(SVC_INT_NEW_PSW, &new_svc_psw, sizeof(struct psw));
	memcpy(PGM_INT_NEW_PSW, &new_pgm_psw, sizeof(struct psw));
	memcpy(MCH_INT_NEW_PSW, &new_mch_psw, sizeof(struct psw));

	/* Turn on Low-address Protection */
	lap_on();

	/*
	 * Set up page table entries for the nucleus
	 */
	setup_dat();

	/*
	 * Allocate & initialize the interrupt stack
	 */
	init_int_stack();

	/*
	 * Initialize the io subsystem
	 */
	init_io();

	/*
	 * Register all the device drivers
	 */
	register_drivers();

	/*
	 * Time to enable interrupts => load new psw
	 */
	memset(&psw, 0, sizeof(struct psw));
	psw.io	= 1;
	psw.ea	= 1;
	psw.ba	= 1;

	asm volatile(
		"	larl	%%r1,0f\n"
		"	stg	%%r1,%0\n"
		"	lpswe	%1\n"
		"0:\n"
	: /* output */
	  "=m" (psw.ptr)
	: /* input */
	  "m" (psw)
	: /* clobbered */
	  "r1"
	);

	/*
	 * Let's discover all the devices attached
	 */
	scan_devices();

	/*
	 * Initialize the process scheduler
	 */
	init_sched();

	/*
	 * Start tracking locking dependencies
	 */
	ldep_on();

	/*
	 * Create a thread that'll finish setting everything up for us
	 */
	create_task("*finish-loading", __finish_loading, (void*) (u64) __iplsch);

	/*
	 * THIS IS WHERE THE IDLE TASK BEGINS
	 */

	idle_task_body();
}

