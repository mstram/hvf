#include <sched.h>
#include <cp.h>

static const intercept_handler_t instruction_funcs[256] = {
	[0xb2] = handle_instruction_priv,	/* assorted priv. insts */
};

int handle_instruction(struct user *user)
{
	intercept_handler_t h;
	int err = -EINVAL;

	con_printf(user->con, "INTRCPT: INST (%04x %08x)\n",
		   current->guest->sie_cb.ipa,
		   current->guest->sie_cb.ipb);

	h = instruction_funcs[current->guest->sie_cb.ipa >> 8];
	if (h)
		err = h(user);

	return err;
}
