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

	struct spi_flash *env_flash = spi_flash_probe(CONFIG_SF_DEFAULT_BUS,
						      CONFIG_SF_DEFAULT_CS,
						      CONFIG_SF_DEFAULT_SPEED,
						      CONFIG_SF_DEFAULT_MODE);
	if (env_flash && (count > 0)) {
		u32 flash_size = env_flash->size / 1024 / 1024;
		const fpga_desc * const desc = fpga_get_desc(0);
		const xilinx_desc * const xil_desc = desc->devdesc;
		setup_qspi_args(flash_size, xil_desc->name);
	}
#endif
#endif
	return 0;
}
