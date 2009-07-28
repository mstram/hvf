#include <device.h>
#include <console.h>
#include <list.h>

static int d3390_snprintf(struct device *dev, char* buf, int len)
{
	int cyls = 0;

	switch(dev->model) {
		case 0x0c: cyls =  3339; break; /* 3390-3 */
		case 0x0a: cyls = 10017; break; /* 3390-9 */
	}

	return snprintf(buf, len, "%10d CYL ", cyls);
}

static int d3390_reg(struct device *dev)
{
	switch(dev->model) {
		case 0x0c: return 0; /* 3390-3 */
		case 0x0a: return 0; /* 3390-9 */
	}

	return -ENOENT;
}

static struct device_type d3390 = {
	.types		= LIST_HEAD_INIT(d3390.types),
	.reg		= d3390_reg,
	.interrupt	= NULL,
	.snprintf	= d3390_snprintf,
	.type		= 0x3390,
	.all_models	= 1,
};

int register_driver_dasd(void)
{
	return register_device_type(&d3390);
}

