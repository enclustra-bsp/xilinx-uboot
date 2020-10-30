// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2020 Enclustra GmbH
 */
 
#include <common.h>
#include <command.h>
#include <i2c.h>
#include <zynqpl.h>
#include <asm/arch/hardware.h>
#include <asm/arch/sys_proto.h>
#include <asm/io.h>

#ifndef CONFIG_SPL_BUILD

#define MGT_DEVICES_NO 8
#define MGT_OUTPUT_NO  5

#define MGT_I2C_MUX_ADDR 0x74

#define MGT_IP1 (1 << 0)
#define MGT_IP2 (1 << 1)
#define MGT_IP3 (1 << 2)
#define MGT_IP4 (1 << 3)
#define MGT_IP5 (1 << 4)
#define MGT_IP6 (1 << 5)
#define MGT_IP7 (1 << 6)
#define MGT_IP8 (1 << 7)

#define MGT_CROSS_DISABLED 0
#define MGT_CROSS_ENABLED  1

#define MGT_SYS_CTRL    0x01
#define MGT_CROSS5_CTRL 0x07
#define MGT_SW_CTRL     0x08

#define MGT_PASS  0
#define MGT_CROSS 1

#define MGT_SWITCH_EN (1 << 7)

static const struct {
	uint8_t i2c_mux_port;
	uint8_t i2c_addr;
} mgt_devices[MGT_DEVICES_NO] = {
	{
		.i2c_mux_port = 0,
		.i2c_addr = 0x30,
	},
	{
		.i2c_mux_port = 0,
		.i2c_addr = 0x32,
	},
	{
		.i2c_mux_port = 0,
		.i2c_addr = 0x34,
	},
	{
		.i2c_mux_port = 0,
		.i2c_addr = 0x36,
	},
	{
		.i2c_mux_port = 1,
		.i2c_addr = 0x30,
	},
	{
		.i2c_mux_port = 1,
		.i2c_addr = 0x32,
	},
	{
		.i2c_mux_port = 1,
		.i2c_addr = 0x34,
	},
	{
		.i2c_mux_port = 1,
		.i2c_addr = 0x36,
	},
};

static const struct {
	uint8_t inputs;
	uint8_t cross;
} mgt_outputs[MGT_OUTPUT_NO] = {
	{
		.inputs = MGT_IP1 | MGT_IP2 | MGT_IP3,
		.cross  = MGT_CROSS_DISABLED,
	},
	{
		.inputs = MGT_IP1 | MGT_IP2 | MGT_IP3,
		.cross  = MGT_CROSS_DISABLED,
	},
	{
		.inputs = MGT_IP4 | MGT_IP5 | MGT_IP6,
		.cross  = MGT_CROSS_DISABLED,
	},
	{
		.inputs = MGT_IP4 | MGT_IP5 | MGT_IP6,
		.cross  = MGT_CROSS_DISABLED,
	},
	{
		.inputs = MGT_IP7 | MGT_IP8,
		.cross  = MGT_CROSS_ENABLED,
	},
};

static int mgt_set_routing(uint8_t mgt_dev, uint8_t mgt_input,
		uint8_t mgt_output, uint8_t cross)
{
	uint8_t i2c_buf;
	struct udevice *mux, *dev;

	if (mgt_dev > ARRAY_SIZE(mgt_devices)) {
		printf("Error: Wrong MGT device\n");
		return CMD_RET_FAILURE;
	}

	if (mgt_output > ARRAY_SIZE(mgt_outputs)) {
		printf("Error: Wrong MGT output\n");
		return CMD_RET_FAILURE;
	}

	if (!((1 << (mgt_input - 1)) & mgt_outputs[mgt_output - 1].inputs)) {
		printf("Error: Wrong MGT input\n");
		return CMD_RET_FAILURE;
	}

	if (cross && mgt_outputs[mgt_output - 1].cross == MGT_CROSS_DISABLED) {
		printf("Error: This output does not support cross connection\n");
		return CMD_RET_FAILURE;
	}

	if (i2c_get_chip_for_busnum(0, MGT_I2C_MUX_ADDR, 0, &mux)) {
		printf("Cannot find MUX on the I2C bus\n");
		return CMD_RET_FAILURE;
	}

	if (i2c_get_chip_for_busnum(0, mgt_devices[mgt_dev].i2c_addr, 1,
				    &dev)) {
		printf("Cannot find MGT device on the I2C bus\n");
		return CMD_RET_FAILURE;
	}

	/* Configure I2C MUX to appropriate port */
	i2c_buf = 0x08 | mgt_devices[mgt_dev].i2c_mux_port;
	dm_i2c_write(mux, 0, &i2c_buf, 1);

	/* Write 1 to SWITCH_EN bit to ensure device is active */
	i2c_buf = MGT_SWITCH_EN;
	dm_i2c_write(dev, MGT_SYS_CTRL, &i2c_buf, 1);

	/* Configure MGT mux port */
	i2c_buf = (1 << (mgt_input - 1));
	dm_i2c_write(dev, mgt_output + 1, &i2c_buf, 1);

	/* Configure cross connection in case of output 5 */
	if (mgt_outputs[mgt_output - 1].cross == MGT_CROSS_ENABLED) {
		if (cross) {
			i2c_buf = (1 << MGT_CROSS);
			dm_i2c_write(dev, MGT_CROSS5_CTRL, &i2c_buf, 1);
		} else {
			i2c_buf = (1 << MGT_PASS);
			dm_i2c_write(dev, MGT_CROSS5_CTRL, &i2c_buf, 1);
		}
	}

	/* Enable configuration registers in SW_CTRL register */
	i2c_buf = (1 << (mgt_output - 1));

	if (mgt_outputs[mgt_output - 1].cross == MGT_CROSS_ENABLED)
		i2c_buf |= (1 << 5);

	if (dm_i2c_write(dev, MGT_SW_CTRL, &i2c_buf, 1)) {
		printf("Error: I2C write operation failed\n");
		return CMD_RET_FAILURE;
	}

	return CMD_RET_SUCCESS;
}

static int mgt_set_routing_cmd(cmd_tbl_t *cmdtp, int flag,
		int argc, char * const argv[])
{
	int mgt_dev = 0, mgt_input = 0, mgt_output = 0, cross = 0;
	char *endp;

	if (argc != 4 && argc != 5)
		return CMD_RET_USAGE;

	mgt_dev = simple_strtoul(argv[1], &endp, 10);
	if (endp == argv[1])
		return CMD_RET_USAGE;

	mgt_input = simple_strtoul(argv[2], &endp, 10);
	if (endp == argv[2])
		return CMD_RET_USAGE;

	mgt_output = simple_strtoul(argv[3], &endp, 10);
	if (endp == argv[3])
		return CMD_RET_USAGE;

	if (argc == 5) {
		if (!strcmp(argv[4], "-x"))
			cross = 1;
		else
			return CMD_RET_USAGE;
	}

	return mgt_set_routing((uint8_t)mgt_dev, (uint8_t)mgt_input,
			(uint8_t)mgt_output, (uint8_t)cross);
}

U_BOOT_CMD(mgt_set_routing, 5, 0, mgt_set_routing_cmd,
	   "Set signal routing in CBTL08GP053 MGT multiplexer",
	   "mgt_set_routing device input output [-x] - Configure device number $dev\n"
	   "                                   to connect $input to $output. If -x\n"
	   "                                   is specified, set the connection to\n"
	   "                                   crossed mode (only output no 5 has\n"
	   "                                   support for this functionality)");

#endif

