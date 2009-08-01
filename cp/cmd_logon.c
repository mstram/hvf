/*
 *!!! LOGON
 *!p >>--LOGON--userid-------------------------------------------------------------><
 *!! AUTH G
 *!! PURPOSE
 *! Log on to a virtual machine.
 */
static int cmd_logon(struct virt_sys *data, char *cmd, int len)
{
	struct console *con = (struct console*) data;
	struct user *u;

	u = find_user_by_id(cmd);
	if (IS_ERR(u)) {
		con_printf(con, "INVALID USERID\n");
		return 0;
	}

	spawn_user_cp(con, u);

	return 0;
}

static int cmd_logon_fail(struct virt_sys *sys, char *cmd, int len)
{
	con_printf(sys->con, "ALREADY LOGGED ON\n");
	return 0;
}
