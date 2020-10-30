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
#include <asm/arch/ps7_init_gpl.h>

#define NANDMUX         0x00000010
#define NANDMUX_PULLUP  0x00001010
#define QSPIMUX         0x00000002
#define QSPIMUX_PULLUP  0x00001002
#define TRISTATE_PULLUP 0x00001001
#define GPIO_PULLUP     0x00001000
#define IO_TYPE_MASK    0x00000E00

#if defined(ENCLUSTRA_MARS_ZX) || defined(ENCLUSTRA_MERCURY_ZX)

extern void zynq_slcr_unlock(void);
extern void zynq_slcr_lock(void);
/**
 * Set pin muxing for NAND access
 */
static void set_mio_mux_nand( void ){

	zynq_slcr_unlock();

	/* Define MuxIO for NAND */
	/* Caution: overwrite some QSPI muxing !!! */
	writel(NANDMUX         | (readl(&slcr_base->mio_pin[0])  & IO_TYPE_MASK), &slcr_base->mio_pin[0]);  /* Pin 0, NAND Flash Chip Select */
	writel(TRISTATE_PULLUP | (readl(&slcr_base->mio_pin[1])  & IO_TYPE_MASK), &slcr_base->mio_pin[1]);  /* Pin 1, not NAND */
	writel(NANDMUX         | (readl(&slcr_base->mio_pin[2])  & IO_TYPE_MASK), &slcr_base->mio_pin[2]);  /* Pin 2, NAND Flash ALEn */
	writel(NANDMUX         | (readl(&slcr_base->mio_pin[3])  & IO_TYPE_MASK), &slcr_base->mio_pin[3]);  /* Pin 3, NAND WE_B */
	writel(NANDMUX         | (readl(&slcr_base->mio_pin[4])  & IO_TYPE_MASK), &slcr_base->mio_pin[4]);  /* Pin 4, NAND Flash IO Bit 2 */
	writel(NANDMUX         | (readl(&slcr_base->mio_pin[5])  & IO_TYPE_MASK), &slcr_base->mio_pin[5]);  /* Pin 5, NAND Flash IO Bit 0 */
	writel(NANDMUX         | (readl(&slcr_base->mio_pin[6])  & IO_TYPE_MASK), &slcr_base->mio_pin[6]);  /* Pin 6, NAND Flash IO Bit 1 */
	writel(NANDMUX         | (readl(&slcr_base->mio_pin[7])  & IO_TYPE_MASK), &slcr_base->mio_pin[7]);  /* Pin 7, NAND Flash CLE_B */
	writel(NANDMUX         | (readl(&slcr_base->mio_pin[8])  & IO_TYPE_MASK), &slcr_base->mio_pin[8]);  /* Pin 8, NAND Flash RD_B */
	writel(NANDMUX_PULLUP  | (readl(&slcr_base->mio_pin[9])  & IO_TYPE_MASK), &slcr_base->mio_pin[9]);  /* Pin 9, NAND Flash IO Bit 4 */
	writel(NANDMUX_PULLUP  | (readl(&slcr_base->mio_pin[10]) & IO_TYPE_MASK), &slcr_base->mio_pin[10]); /* Pin 10, NAND Flash IO Bit 5 */
	writel(NANDMUX_PULLUP  | (readl(&slcr_base->mio_pin[11]) & IO_TYPE_MASK), &slcr_base->mio_pin[11]); /* Pin 11, NAND Flash IO Bit 6 */
	writel(NANDMUX_PULLUP  | (readl(&slcr_base->mio_pin[12]) & IO_TYPE_MASK), &slcr_base->mio_pin[12]); /* Pin 12, NAND Flash IO Bit 7 */
	writel(NANDMUX_PULLUP  | (readl(&slcr_base->mio_pin[13]) & IO_TYPE_MASK), &slcr_base->mio_pin[13]); /* Pin 13, NAND Flash IO Bit 3 */
	writel(NANDMUX         | (readl(&slcr_base->mio_pin[14]) & IO_TYPE_MASK), &slcr_base->mio_pin[14]); /* Pin 14, NAND Flash Busy */

	zynq_slcr_lock();
}

/**
 * Set the pin muxing for QSPI NOR access
 */
static void set_mio_mux_qspi( void ){

	zynq_slcr_unlock();

	/* Define MuxIO for QSPI */
	/* Caution: overwrite some NAND muxing !!! */
	writel(GPIO_PULLUP    | (readl(&slcr_base->mio_pin[0])  & IO_TYPE_MASK), &slcr_base->mio_pin[0]);  /* Pin 0, Level 3 Mux */
	writel(QSPIMUX_PULLUP | (readl(&slcr_base->mio_pin[1])  & IO_TYPE_MASK), &slcr_base->mio_pin[1]);  /* Pin 1, Quad SPI 0 Chip Select */
	writel(QSPIMUX        | (readl(&slcr_base->mio_pin[2])  & IO_TYPE_MASK), &slcr_base->mio_pin[2]);  /* Pin 2, Quad SPI 0 IO Bit 0 */
	writel(QSPIMUX        | (readl(&slcr_base->mio_pin[3])  & IO_TYPE_MASK), &slcr_base->mio_pin[3]);  /* Pin 3, Quad SPI 0 IO Bit 1 */
	writel(QSPIMUX        | (readl(&slcr_base->mio_pin[4])  & IO_TYPE_MASK), &slcr_base->mio_pin[4]);  /* Pin 4, Quad SPI 0 IO Bit 2 */
	writel(QSPIMUX        | (readl(&slcr_base->mio_pin[5])  & IO_TYPE_MASK), &slcr_base->mio_pin[5]);  /* Pin 5, Quad SPI 0 IO Bit 3 */
	writel(QSPIMUX        | (readl(&slcr_base->mio_pin[6])  & IO_TYPE_MASK), &slcr_base->mio_pin[6]);  /* Pin 6, Quad SPI 0 Clock */
	writel(QSPIMUX        | (readl(&slcr_base->mio_pin[7])  & IO_TYPE_MASK), &slcr_base->mio_pin[7]);  /* Pin 7, Reserved*/
	writel(QSPIMUX        | (readl(&slcr_base->mio_pin[8])  & IO_TYPE_MASK), &slcr_base->mio_pin[8]);  /* Pin 8, Quad SPI Feedback Clock */
	writel(GPIO_PULLUP    | (readl(&slcr_base->mio_pin[9])  & IO_TYPE_MASK), &slcr_base->mio_pin[9]);  /* Pin 9, Level mux -> disable */
	writel(GPIO_PULLUP    | (readl(&slcr_base->mio_pin[10]) & IO_TYPE_MASK), &slcr_base->mio_pin[10]); /* Pin 10, Level mux -> disable */
	writel(GPIO_PULLUP    | (readl(&slcr_base->mio_pin[11]) & IO_TYPE_MASK), &slcr_base->mio_pin[11]); /* Pin 11, Level mux -> disable */
	writel(GPIO_PULLUP    | (readl(&slcr_base->mio_pin[12]) & IO_TYPE_MASK), &slcr_base->mio_pin[12]); /* Pin 12, Level mux -> disable */
	writel(GPIO_PULLUP    | (readl(&slcr_base->mio_pin[13]) & IO_TYPE_MASK), &slcr_base->mio_pin[13]); /* Pin 13, Level mux -> disable */
	writel(GPIO_PULLUP    | (readl(&slcr_base->mio_pin[14]) & IO_TYPE_MASK), &slcr_base->mio_pin[14]); /* Pin 14, Level mux -> disable */

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
#endif