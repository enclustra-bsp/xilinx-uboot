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


#if defined(ENCLUSTRA_MARS_ZX) || defined(ENCLUSTRA_MERCURY_ZX)
extern void zynq_slcr_unlock(void);
extern void zynq_slcr_lock(void);
/**
 * Set pin muxing for NAND access
 */
static void set_mio_mux_nand( void ){
#define NANDMUX0 0x0000001610
#define NANDMUX  0x0000000610

	zynq_slcr_unlock();

		/* Define MuxIO for NAND */
		/* Caution: overwrite some QSPI muxing !!! */
		writel(NANDMUX,  &slcr_base->mio_pin[0]);       /* Pin 0, NAND Flash Chip Select */
		writel(0x1601,   &slcr_base->mio_pin[1]);       /* Pin 1, not NAND */
		writel(NANDMUX,  &slcr_base->mio_pin[2]);       /* Pin 2, NAND Flash ALEn */
		writel(NANDMUX,  &slcr_base->mio_pin[3]);       /* Pin 3, NAND WE_B */
		writel(NANDMUX,  &slcr_base->mio_pin[4]);       /* Pin 4, NAND Flash IO Bit 2 */
		writel(NANDMUX,  &slcr_base->mio_pin[5]);       /* Pin 5, NAND Flash IO Bit 0 */
		writel(NANDMUX,  &slcr_base->mio_pin[6]);       /* Pin 6, NAND Flash IO Bit 1 */
		writel(NANDMUX,  &slcr_base->mio_pin[7]);       /* Pin 7, NAND Flash CLE_B */
		writel(NANDMUX,  &slcr_base->mio_pin[8]);       /* Pin 8, NAND Flash RD_B */
		writel(NANDMUX0, &slcr_base->mio_pin[9]);       /* Pin 9, NAND Flash IO Bit 4 */
		writel(NANDMUX0, &slcr_base->mio_pin[10]);      /* Pin 10, NAND Flash IO Bit 5 */
		writel(NANDMUX0, &slcr_base->mio_pin[11]);      /* Pin 11, NAND Flash IO Bit 6 */
		writel(NANDMUX0, &slcr_base->mio_pin[12]);      /* Pin 12, NAND Flash IO Bit 7 */
		writel(NANDMUX0, &slcr_base->mio_pin[13]);      /* Pin 13, NAND Flash IO Bit 3 */
		writel(NANDMUX,  &slcr_base->mio_pin[14]);      /* Pin 14, NAND Flash Busy */

	zynq_slcr_lock();
}

/**
 * Set the pin muxing for QSPI NOR access
 */
static void set_mio_mux_qspi( void ){
#define QSPIMUX 0x0000000602

	zynq_slcr_unlock();

		/* Define MuxIO for QSPI */
		/* Caution: overwrite some NAND muxing !!! */
		writel(0x00001600, &slcr_base->mio_pin[0]);             /* Pin 0, Level 3 Mux */
		writel(0x00001602, &slcr_base->mio_pin[1]);             /* Pin 1, Quad SPI 0 Chip Select */
		writel(QSPIMUX,    &slcr_base->mio_pin[2]);             /* Pin 2, Quad SPI 0 IO Bit 0 */
		writel(QSPIMUX,    &slcr_base->mio_pin[3]);             /* Pin 3, Quad SPI 0 IO Bit 1 */
		writel(QSPIMUX,    &slcr_base->mio_pin[4]);             /* Pin 4, Quad SPI 0 IO Bit 2 */
		writel(QSPIMUX,    &slcr_base->mio_pin[5]);             /* Pin 5, Quad SPI 0 IO Bit 3 */
		writel(QSPIMUX,    &slcr_base->mio_pin[6]);             /* Pin 6, Quad SPI 0 Clock */
		writel(QSPIMUX,    &slcr_base->mio_pin[7]);             /* Pin 7, Reserved*/
		writel(QSPIMUX,    &slcr_base->mio_pin[8]);             /* Pin 8, Quad SPI Feedback Clock */
		writel(0x00001600, &slcr_base->mio_pin[9]);             /* Pin 9, Level mux -> disable */
		writel(0x00001600, &slcr_base->mio_pin[10]);    /* Pin 10, Level mux -> disable */
		writel(0x00001600, &slcr_base->mio_pin[11]);    /* Pin 11, Level mux -> disable */
		writel(0x00001600, &slcr_base->mio_pin[12]);    /* Pin 12, Level mux -> disable */
		writel(0x00001600, &slcr_base->mio_pin[13]);    /* Pin 13, Level mux -> disable */
		writel(0x00001600, &slcr_base->mio_pin[14]);    /* Pin 14, Level mux -> disable */

	zynq_slcr_lock();
}

static int zx_current_storage = ZX_NONE;

void zx_set_storage (int store) {
		if (store == zx_current_storage)
				return;

		switch (store)
		{
				case ZX_NAND:
						set_mio_mux_nand ();
						zx_current_storage = ZX_NAND;
						break;
				case ZX_QSPI:
						set_mio_mux_qspi();
						zx_current_storage = ZX_QSPI;
						break;
				default:
						zx_current_storage = ZX_NONE;
						break;
		}
}

int zx_set_storage_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	if(argc != 2)
		return CMD_RET_USAGE;
	if(!strcmp(argv[1], "NAND"))
		zx_set_storage(ZX_NAND);
	else if (!strcmp(argv[1], "QSPI"))
		zx_set_storage(ZX_QSPI);
	else return CMD_RET_USAGE;

	return CMD_RET_SUCCESS;
}

#elif defined(ENCLUSTRA_MARS_ZX2)
/* Mars ZX2 do not have NAND flash, so there is no point to change pinmuxes.
 * Just return SUCCESS
 */

int zx_set_storage_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	return CMD_RET_SUCCESS;
}
#endif

#if defined(ENCLUSTRA_MARS_ZX) || defined(ENCLUSTRA_MARS_ZX2) || defined(ENCLUSTRA_MERCURY_ZX)
U_BOOT_CMD(zx_set_storage, 2, 0, zx_set_storage_cmd,
	"Set non volatile memory access",
	"<NAND|QSPI> - Set access for the selected memory device");

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
#if defined(ENCLUSTRA_MARS_ZX) || defined(ENCLUSTRA_MARS_ZX2) || defined(ENCLUSTRA_MERCURY_ZX)
#if defined(CONFIG_ZYNQ_QSPI)
#define xstr(s) str(s)
#define str(s) #s
	struct spi_flash *env_flash;
	uint32_t flash_size;

	/* Probe the QSPI flash */
	env_flash = spi_flash_probe(0, 0, 1000000,
					SPI_RX_QUAD | SPI_TX_QUAD | SPI_MODE_3);

	if (env_flash) {
		/* Calculate the size in megabytes */
		flash_size = env_flash->size / 1024 / 1024;
		setup_qspi_args(flash_size, zx_get_idcode_name());
	}
#endif
#endif
	return 0;
}

