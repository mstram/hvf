/*
 * (C) Copyright 2007-2011  Josef 'Jeff' Sipek <jeffpc@josefsipek.net>
 *
 * This file is released under the GPLv2.  See the COPYING file for more
 * details.
 */

#include <directory.h>
#include <sched.h>
#include <dat.h>
#include <vcpu.h>

/*
 * FIXME:
 * - issue any pending interruptions
 */
void run_guest(struct virt_sys *sys)
{
	struct psw *psw = &sys->task->cpu->sie_cb.gpsw;
	u64 save_gpr[16];

	if (psw->w) {
		if (!psw->io && !psw->ex && !psw->m) {
			con_printf(sys->con, "ENTERED CP DUE TO DISABLED WAIT\n");
			sys->task->cpu->state = GUEST_STOPPED;
			return;
		}

		/* wait state */
		schedule();
		return;
	}

	/*
	 * FIXME: need to ->icptcode = 0;
	 */

	/*
	 * FIXME: load FPRs & FPCR
	 */

	/*
	 * IMPORTANT: We MUST keep a valid stack address in R15. This way,
	 * if SIE gets interrupted via an interrupt in the host, the
	 * scheduler can still get to the struct task pointer on the stack
	 */
	asm volatile(
		/* save current regs */
		"	stmg	0,15,%0\n"
		/* load the address of the guest state save area */
		"	lr	14,%1\n"
		/* load the address of the reg save area */
		"	la	15,%0\n"
		/* load guest's R0-R13 */
		"	lmg	0,13,%2(14)\n"
		/* SIE */
		"	sie	%3(14)\n"
		/* save guest's R0-R13 */
		"	stmg	0,13,%2(14)\n"
		/* restore all regs */
		"	lmg	0,15,0(15)\n"

	: /* output */
	  "+m" (save_gpr)
	: /* input */
	  "a" (sys->task->cpu),
	  "J" (offsetof(struct virt_cpu, regs.gpr)),
	  "J" (offsetof(struct virt_cpu, sie_cb))
	: /* clobbered */
	  "memory"
	);

	/*
	 * FIXME: store FPRs & FPCR
	 */

	handle_interception(sys);
}
