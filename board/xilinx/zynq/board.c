/*
 * (C) Copyright 2012 Michal Simek <monstr@monstr.eu>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <fdtdec.h>
#include <fpga.h>
#include <mmc.h>
#include <netdev.h>
#include <zynqpl.h>
#include <asm/arch/hardware.h>
#include <asm/arch/sys_proto.h>
#include <micrel.h>
#include <miiphy.h>
#include <asm/io.h>
#include <nand.h>
#include <i2c.h>
#include <spi.h>
#include <spi_flash.h>

DECLARE_GLOBAL_DATA_PTR;

#if (defined(CONFIG_FPGA) && !defined(CONFIG_SPL_BUILD)) || \
    (defined(CONFIG_SPL_FPGA_SUPPORT) && defined(CONFIG_SPL_BUILD))
static xilinx_desc fpga;

/* It can be done differently */
static xilinx_desc fpga010 = XILINX_XC7Z010_DESC(0x10);
static xilinx_desc fpga015 = XILINX_XC7Z015_DESC(0x15);
static xilinx_desc fpga020 = XILINX_XC7Z020_DESC(0x20);
static xilinx_desc fpga030 = XILINX_XC7Z030_DESC(0x30);
static xilinx_desc fpga035 = XILINX_XC7Z035_DESC(0x35);
static xilinx_desc fpga045 = XILINX_XC7Z045_DESC(0x45);
static xilinx_desc fpga100 = XILINX_XC7Z100_DESC(0x100);
#endif

#if defined(CONFIG_MARS_ZX) || defined(CONFIG_MERCURY_ZX)

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
        writel(NANDMUX0,  &slcr_base->mio_pin[9]);       /* Pin 9, NAND Flash IO Bit 4 */
        writel(NANDMUX0,  &slcr_base->mio_pin[10]);      /* Pin 10, NAND Flash IO Bit 5 */
        writel(NANDMUX0,  &slcr_base->mio_pin[11]);      /* Pin 11, NAND Flash IO Bit 6 */
        writel(NANDMUX0,  &slcr_base->mio_pin[12]);      /* Pin 12, NAND Flash IO Bit 7 */
        writel(NANDMUX0,  &slcr_base->mio_pin[13]);      /* Pin 13, NAND Flash IO Bit 3 */
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

#elif defined(CONFIG_MARS_ZX2)
/* Mars ZX2 do not have NAND flash, so there is no point to change pinmuxes.
 * Just return SUCCESS
 */

int zx_set_storage_cmd(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	return CMD_RET_SUCCESS;
}
#endif

#if defined(CONFIG_MARS_ZX) || defined(CONFIG_MARS_ZX2) || defined(CONFIG_MERCURY_ZX)
U_BOOT_CMD(zx_set_storage, 2, 0, zx_set_storage_cmd,
	"Set non volatile memory access",
	"<NAND|QSPI> - Set access for the selected memory device");
#endif

int board_init(void)
{
#if defined(CONFIG_ENV_IS_IN_EEPROM) && !defined(CONFIG_SPL_BUILD)
	unsigned char eepromsel = CONFIG_SYS_I2C_MUX_EEPROM_SEL;
#endif
#if (defined(CONFIG_FPGA) && !defined(CONFIG_SPL_BUILD)) || \
    (defined(CONFIG_SPL_FPGA_SUPPORT) && defined(CONFIG_SPL_BUILD))
	u32 idcode;

	idcode = zynq_slcr_get_idcode();

	switch (idcode) {
	case XILINX_ZYNQ_7010:
		fpga = fpga010;
		break;
	case XILINX_ZYNQ_7015:
		fpga = fpga015;
		break;
	case XILINX_ZYNQ_7020:
		fpga = fpga020;
		break;
	case XILINX_ZYNQ_7030:
		fpga = fpga030;
		break;
	case XILINX_ZYNQ_7035:
		fpga = fpga035;
		break;
	case XILINX_ZYNQ_7045:
		fpga = fpga045;
		break;
	case XILINX_ZYNQ_7100:
		fpga = fpga100;
		break;
	}
#endif

#if (defined(CONFIG_FPGA) && !defined(CONFIG_SPL_BUILD)) || \
    (defined(CONFIG_SPL_FPGA_SUPPORT) && defined(CONFIG_SPL_BUILD))
	fpga_init();
	fpga_add(fpga_xilinx, &fpga);
#endif
#if defined(CONFIG_ENV_IS_IN_EEPROM) && !defined(CONFIG_SPL_BUILD)
	if (eeprom_write(CONFIG_SYS_I2C_MUX_ADDR, 0, &eepromsel, 1))
		puts("I2C:EEPROM selection failed\n");
#endif
	return 0;
}

int board_late_init(void)
{
	switch ((zynq_slcr_get_boot_mode()) & ZYNQ_BM_MASK) {
	case ZYNQ_BM_QSPI:
		setenv("modeboot", "qspiboot");
		break;
	case ZYNQ_BM_NAND:
		setenv("modeboot", "nandboot");
		break;
	case ZYNQ_BM_NOR:
		setenv("modeboot", "norboot");
		break;
	case ZYNQ_BM_SD:
		setenv("modeboot", "sdboot");
		break;
	case ZYNQ_BM_JTAG:
		setenv("modeboot", "jtagboot");
		break;
	default:
		setenv("modeboot", "");
		break;
	}

#if defined(ENCLUSTRA_EEPROM_ADDR_TAB) && defined(ENCLUSTRA_EEPROM_HWMAC_REG)
	u8 chip_addr_tab[] = ENCLUSTRA_EEPROM_ADDR_TAB;
	int i, ret;
	u8 hwaddr[6];
	u32 hwaddr_h;
	char hwaddr_str[16];

	if (getenv("ethaddr") == NULL) {
		/* Init i2c */
		i2c_init(0, 0);
		i2c_set_bus_num(0);

		for (i = 0; i < ARRAY_SIZE(chip_addr_tab); i++) {
			/* Probe the chip */
			if (i2c_probe(chip_addr_tab[i]) != 0)
				continue;

			/* Attempt to read the mac address */
			ret = i2c_read(chip_addr_tab[i],
				       ENCLUSTRA_EEPROM_HWMAC_REG,
				       1,
				       hwaddr,
				       6);

			/* Do not continue if read failed */
			if (ret)
				continue;

			/* Check if the value is a valid mac registered for
			 * Enclustra  GmbH */
			hwaddr_h = hwaddr[0] | hwaddr[1] << 8 | hwaddr[2] << 16;
			if ((hwaddr_h & 0xFFFFFF) != ENCLUSTRA_MAC)
				continue;

			/* Format the address using a string */
			sprintf(hwaddr_str,
				"%02X:%02X:%02X:%02X:%02X:%02X",
				hwaddr[0],
				hwaddr[1],
				hwaddr[2],
				hwaddr[3],
				hwaddr[4],
				hwaddr[5]);

			/* Set the actual env variable */
			setenv("ethaddr", hwaddr_str);
			break;
		}
	}
#endif

#if defined(CONFIG_ZYNQ_QSPI)
#if defined(CONFIG_MARS_ZX) || defined(CONFIG_MERCURY_ZX)
#define xstr(s) str(s)
#define str(s) #s
	struct spi_flash *env_flash;
	uint32_t flash_size;

	/* Probe the QSPI flash */
	env_flash = spi_flash_probe(0, 0, 1000000, SPI_MODE_3);

	if (env_flash) {
		/* Calculate the size in megabytes */
		flash_size = env_flash->size / 1024.0 / 1024.0;

		if (flash_size >= 64) {
			setenv("ramdisk_size",
			       xstr(QSPI_64M_ROOTFS_SIZE));
			setenv("jffs2_size",
			       xstr(QSPI_64M_ROOTFS_SIZE));
			setenv("ubifs_size",
			       xstr(QSPI_64M_ROOTFS_SIZE));
			setenv("kernel_size",
			       xstr(QSPI_64M_LINUX_SIZE));
			setenv("devicetree_size",
			       xstr(QSPI_64M_DTB_SIZE));
			setenv("bootscript_size",
			       xstr(QSPI_64M_BOOTSCRIPT_SIZE));
			setenv("bootimage_size",
			       xstr(QSPI_64M_BOOT_SIZE));
			setenv("fullboot_size",
			       xstr(QSPI_64M_FULLBOOT_SIZE));
			setenv("qspi_bootimage_offset",
			       xstr(QSPI_BOOT_OFFSET));
			setenv("qspi_kernel_offset",
			       xstr(QSPI_64M_LINUX_OFFSET));
			setenv("qspi_ramdisk_offset",
			       xstr(QSPI_64M_ROOTFS_OFFSET));
			setenv("qspi_devicetree_offset",
			       xstr(QSPI_64M_DTB_OFFSET));
			setenv("qspi_bootscript_offset",
			       xstr(QSPI_64M_BOOTSCRIPT_OFFSET));
		} else if (flash_size >= 16) {
			setenv("ramdisk_size",
			       xstr(QSPI_16M_ROOTFS_SIZE));
			setenv("jffs2_size",
			       xstr(QSPI_16M_ROOTFS_SIZE));
			setenv("ubifs_size",
			       xstr(QSPI_16M_ROOTFS_SIZE));
			setenv("kernel_size",
			       xstr(QSPI_16M_LINUX_SIZE));
			setenv("devicetree_size",
			       xstr(QSPI_16M_DTB_SIZE));
			setenv("bootscript_size",
			       xstr(QSPI_16M_BOOTSCRIPT_SIZE));
			setenv("bootimage_size",
			       xstr(QSPI_16M_BOOT_SIZE));
			setenv("fullboot_size",
			       xstr(QSPI_16M_FULLBOOT_SIZE));
			setenv("qspi_bootimage_offset",
			       xstr(QSPI_BOOT_OFFSET));
			setenv("qspi_kernel_offset",
			       xstr(QSPI_16M_LINUX_OFFSET));
			setenv("qspi_ramdisk_offset",
			       xstr(QSPI_16M_ROOTFS_OFFSET));
			setenv("qspi_devicetree_offset",
			       xstr(QSPI_16M_DTB_OFFSET));
			setenv("qspi_bootscript_offset",
			       xstr(QSPI_16M_BOOTSCRIPT_OFFSET));
		}
	}
#endif
#endif

	return 0;
}

#ifdef CONFIG_DISPLAY_BOARDINFO
int checkboard(void)
{
	puts("Board:\tXilinx Zynq\n");
	return 0;
}
#endif

int board_eth_init(bd_t *bis)
{
	u32 ret = 0;

#ifdef CONFIG_XILINX_AXIEMAC
	ret |= xilinx_axiemac_initialize(bis, XILINX_AXIEMAC_BASEADDR,
						XILINX_AXIDMA_BASEADDR);
#endif
#ifdef CONFIG_XILINX_EMACLITE
	u32 txpp = 0;
	u32 rxpp = 0;
# ifdef CONFIG_XILINX_EMACLITE_TX_PING_PONG
	txpp = 1;
# endif
# ifdef CONFIG_XILINX_EMACLITE_RX_PING_PONG
	rxpp = 1;
# endif
	ret |= xilinx_emaclite_initialize(bis, XILINX_EMACLITE_BASEADDR,
			txpp, rxpp);
#endif

#if defined(CONFIG_ZYNQ_GEM)
# if defined(CONFIG_ZYNQ_GEM0)
	ret |= zynq_gem_initialize(bis, ZYNQ_GEM_BASEADDR0,
				   CONFIG_ZYNQ_GEM_PHY_ADDR0,
				   CONFIG_ZYNQ_GEM_EMIO0);
# endif
# if defined(CONFIG_ZYNQ_GEM1)
	ret |= zynq_gem_initialize(bis, ZYNQ_GEM_BASEADDR1,
				   CONFIG_ZYNQ_GEM_PHY_ADDR1,
				   CONFIG_ZYNQ_GEM_EMIO1);
# endif
#endif
	return ret;
}

/* Setup clock skews for ethernet PHY on Enclustra ZX boards */
#if defined(CONFIG_MARS_ZX) || defined(CONFIG_MARS_ZX2) || defined(CONFIG_MERCURY_ZX)
#define ENCLUSTRA_ZX_CLK_SKEW_VAL	0x3FF
#define ENCLUSTRA_ZX_DATA_SKEW_VAL	0x00

int board_phy_config(struct phy_device *phydev)
{
	/* min rx/tx ctrl delay */
	ksz9031_phy_extended_write(phydev, 2,
				   MII_KSZ9031_EXT_RGMII_CTRL_SIG_SKEW,
				   MII_KSZ9031_MOD_DATA_NO_POST_INC,
				   ENCLUSTRA_ZX_DATA_SKEW_VAL);
	/* min rx delay */
	ksz9031_phy_extended_write(phydev, 2,
				   MII_KSZ9031_EXT_RGMII_RX_DATA_SKEW,
				   MII_KSZ9031_MOD_DATA_NO_POST_INC,
				   ENCLUSTRA_ZX_DATA_SKEW_VAL);
	/* max tx delay */
	ksz9031_phy_extended_write(phydev, 2,
				   MII_KSZ9031_EXT_RGMII_TX_DATA_SKEW,
				   MII_KSZ9031_MOD_DATA_NO_POST_INC,
				   ENCLUSTRA_ZX_DATA_SKEW_VAL);
	/* rx/tx clk skew */
	ksz9031_phy_extended_write(phydev, 2,
				   MII_KSZ9031_EXT_RGMII_CLOCK_SKEW,
				   MII_KSZ9031_MOD_DATA_NO_POST_INC,
				   ENCLUSTRA_ZX_CLK_SKEW_VAL);

	phydev->drv->config(phydev);

	return 0;
}
#endif

#ifdef CONFIG_CMD_MMC
int board_mmc_init(bd_t *bd)
{
	int ret = 0;

#if defined(CONFIG_ZYNQ_SDHCI)
# if defined(CONFIG_ZYNQ_SDHCI0)
	ret = zynq_sdhci_init(ZYNQ_SDHCI_BASEADDR0);
# endif
# if defined(CONFIG_ZYNQ_SDHCI1)
	ret |= zynq_sdhci_init(ZYNQ_SDHCI_BASEADDR1);
# endif
#endif
	return ret;
}
#endif

int dram_init(void)
{
#ifdef CONFIG_OF_CONTROL
	int node;
	fdt_addr_t addr;
	fdt_size_t size;
	const void *blob = gd->fdt_blob;

	node = fdt_node_offset_by_prop_value(blob, -1, "device_type",
					     "memory", 7);
	if (node == -FDT_ERR_NOTFOUND) {
		debug("ZYNQ DRAM: Can't get memory node\n");
		return -1;
	}
	addr = fdtdec_get_addr_size(blob, node, "reg", &size);
	if (addr == FDT_ADDR_T_NONE || size == 0) {
		debug("ZYNQ DRAM: Can't get base address or size\n");
		return -1;
	}
	gd->ram_size = size;
#else
	gd->ram_size = CONFIG_SYS_SDRAM_SIZE;
#endif
	zynq_ddrc_init();

	return 0;
}
