// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2020 Enclustra GmbH
 */

#include <common.h>
#include <command.h>
#include <env.h>
#include <fdtdec.h>
#include <zynqpl.h>
#include <asm/arch/hardware.h>
#include <asm/arch/sys_proto.h>
#include <asm/io.h>
#include <spi.h>
#include <spi_flash.h>
#include <enclustra_qspi.h>
#include <enclustra/eeprom-mac.h>
#include <asm/arch/ps7_init_gpl.h>

#include "../common/board.h"

#if defined(CONFIG_ZYNQ_QSPI)

static const struct {
	uint32_t id;
	char *name;
} zynq_devices[] = {
	{
		.id = XILINX_ZYNQ_XC7Z007S,
		.name = "7z007s",
	},
	{
		.id = XILINX_ZYNQ_XC7Z010,
		.name = "7z010",
	},
	{
		.id = XILINX_ZYNQ_XC7Z012S,
		.name = "7z012s",
	},
	{
		.id = XILINX_ZYNQ_XC7Z014S,
		.name = "7z014s",
	},
	{
		.id = XILINX_ZYNQ_XC7Z015,
		.name = "7z015",
	},
	{
		.id = XILINX_ZYNQ_XC7Z020,
		.name = "7z020",
	},
	{
		.id = XILINX_ZYNQ_XC7Z030,
		.name = "7z030",
	},
	{
		.id = XILINX_ZYNQ_XC7Z035,
		.name = "7z035",
	},
	{
		.id = XILINX_ZYNQ_XC7Z045,
		.name = "7z045",
	},
	{
		.id = XILINX_ZYNQ_XC7Z100,
		.name = "7z100",
	},
};

static char *zx_get_idcode_name(void)
{
	int id, i;
	id = zynq_slcr_get_idcode();
	for(i = 0; i < ARRAY_SIZE(zynq_devices); i++){
		if (zynq_devices[i].id == id)
			return zynq_devices[i].name;
	}
	return "unknown";
}
#endif

int enclustra_board(void)
{
#if defined(CONFIG_ZYNQ_QSPI)
#define xstr(s) str(s)
#define str(s) #s
	struct spi_flash *env_flash;
	uint32_t flash_size;

	/* probe the QSPI flash like "sf probe" and get the flash size */

	struct udevice *new, *bus_dev;
	struct spi_flash *flash = NULL;

	int ret = spi_find_bus_and_cs(CONFIG_SF_DEFAULT_BUS,
				      CONFIG_SF_DEFAULT_CS,
				      &bus_dev, &new);
	if (!ret) {
		device_remove(new, DM_REMOVE_NORMAL);
	}
	ret = spi_flash_probe_bus_cs(CONFIG_SF_DEFAULT_BUS,
				     CONFIG_SF_DEFAULT_CS,
				     CONFIG_SF_DEFAULT_SPEED,
				     CONFIG_SF_DEFAULT_MODE,
				     &new);
	flash = dev_get_uclass_priv(new);

	if (flash) {
		/* Calculate the size in megabytes */
		flash_size = flash->size / 1024 / 1024;
		setup_qspi_args(flash_size, zx_get_idcode_name());
	}
#endif
	return 0;
}

