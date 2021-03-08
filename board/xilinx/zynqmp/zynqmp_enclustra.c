// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2020 Enclustra GmbH
 */

#include <common.h>
#include <zynqmppl.h>
#include <spi.h>
#include <spi_flash.h>
#include <enclustra_qspi.h>
#include <asm/arch/sys_proto.h>

int enclustra_board(void)
{
#if defined(CONFIG_FPGA) && defined(CONFIG_FPGA_ZYNQMPPL) && \
    !defined(CONFIG_SPL_BUILD) || (defined(CONFIG_SPL_FPGA_SUPPORT) && \
    defined(CONFIG_SPL_BUILD))

	/* Probe the QSPI flash */
#if defined(CONFIG_ENCLUSTRA_QSPI_FLASHMAP) && \
	!defined(CONFIG_SPL_BUILD)
	int count = fpga_count();

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
	
	if (!ret && (count > 0)) {
		u32 flash_size = flash->size / 1024 / 1024;
		const fpga_desc * const desc = fpga_get_desc(0);
		const xilinx_desc * const xil_desc = desc->devdesc;
		setup_qspi_args(flash_size, xil_desc->name);
	}
#endif
#endif
	return 0;
}
