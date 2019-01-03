/*
 * (C) Copyright 2012 Michal Simek <monstr@monstr.eu>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <fdtdec.h>
#include <fpga.h>
#include <mmc.h>
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
#include <enclustra_qspi.h>
#include <enclustra/eeprom-mac.h>
#include <asm/arch/ps7_init_gpl.h>

DECLARE_GLOBAL_DATA_PTR;

#if (defined(CONFIG_FPGA) && !defined(CONFIG_SPL_BUILD)) || \
    (defined(CONFIG_SPL_FPGA_SUPPORT) && defined(CONFIG_SPL_BUILD))
static xilinx_desc fpga;

/* It can be done differently */
static xilinx_desc fpga007s = XILINX_XC7Z007S_DESC(0x7);
static xilinx_desc fpga010 = XILINX_XC7Z010_DESC(0x10);
static xilinx_desc fpga012s = XILINX_XC7Z012S_DESC(0x12);
static xilinx_desc fpga014s = XILINX_XC7Z014S_DESC(0x14);
static xilinx_desc fpga015 = XILINX_XC7Z015_DESC(0x15);
static xilinx_desc fpga020 = XILINX_XC7Z020_DESC(0x20);
static xilinx_desc fpga030 = XILINX_XC7Z030_DESC(0x30);
static xilinx_desc fpga035 = XILINX_XC7Z035_DESC(0x35);
static xilinx_desc fpga045 = XILINX_XC7Z045_DESC(0x45);
static xilinx_desc fpga100 = XILINX_XC7Z100_DESC(0x100);
#endif

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
		.id = XILINX_ZYNQ_7007S,
		.name = "7z007s",
	},
	{
		.id = XILINX_ZYNQ_7010,
		.name = "7z010",
	},
	{
		.id = XILINX_ZYNQ_7012S,
		.name = "7z012s",
	},
	{
		.id = XILINX_ZYNQ_7014S,
		.name = "7z014s",
	},
	{
		.id = XILINX_ZYNQ_7015,
		.name = "7z015",
	},
	{
		.id = XILINX_ZYNQ_7020,
		.name = "7z020",
	},
	{
		.id = XILINX_ZYNQ_7030,
		.name = "7z030",
	},
	{
		.id = XILINX_ZYNQ_7035,
		.name = "7z035",
	},
	{
		.id = XILINX_ZYNQ_7045,
		.name = "7z045",
	},
	{
		.id = XILINX_ZYNQ_7100,
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
	case XILINX_ZYNQ_7007S:
		fpga = fpga007s;
		break;
	case XILINX_ZYNQ_7010:
		fpga = fpga010;
		break;
	case XILINX_ZYNQ_7012S:
		fpga = fpga012s;
		break;
	case XILINX_ZYNQ_7014S:
		fpga = fpga014s;
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

static struct eeprom_mem eeproms[] = {
	{ .i2c_addr = 0x64,
	  .mac_reader = atsha204_get_mac,
	  .wakeup = atsha204_wakeup,
	},
	{ .i2c_addr = 0x5C,
	  .mac_reader = ds28_get_mac,
	  .wakeup = NULL,}
};

int board_late_init(void)
{
	switch ((zynq_slcr_get_boot_mode()) & ZYNQ_BM_MASK) {
	case ZYNQ_BM_QSPI:
		env_set("modeboot", "qspiboot");
		break;
	case ZYNQ_BM_NAND:
		env_set("modeboot", "nandboot");
		break;
	case ZYNQ_BM_NOR:
		env_set("modeboot", "norboot");
		break;
	case ZYNQ_BM_SD:
		env_set("modeboot", "sdboot");
		break;
	case ZYNQ_BM_JTAG:
		env_set("modeboot", "jtagboot");
		break;
	default:
		env_set("modeboot", "");
		break;
	}

#if defined(ENCLUSTRA_MARS_ZX) || defined(ENCLUSTRA_MARS_ZX2) || defined(ENCLUSTRA_MERCURY_ZX)
	int i, ret;
	u8 hwaddr[6] = {0, 0, 0, 0, 0, 0};
	u32 hwaddr_h;
	char hwaddr_str[18];
	bool hwaddr_set;

	hwaddr_set = false;

	if (env_get("ethaddr") == NULL) {
		/* Init i2c */
		i2c_init(0, 0);
		i2c_set_bus_num(0);

		for (i = 0; i < ARRAY_SIZE(eeproms); i++) {

			if(eeproms[i].wakeup)
				eeproms[i].wakeup(eeproms[i].i2c_addr);

			/* Probe the chip */
			ret = i2c_probe(eeproms[i].i2c_addr);
			if (ret != 0) continue;

			if(eeproms[i].mac_reader(eeproms[i].i2c_addr, hwaddr))
				continue;

			/* Workaround for incorrect MAC address caused by
			 * flashing to EEPROM adresses like 20:B0:F0:XX:XX:XX
			 * instead of 20:B0:F7:XX:XX:XX */
			hwaddr[2] = (hwaddr[2] == 0xF0) ? 0xF7 : hwaddr[2];

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
			env_set("ethaddr", hwaddr_str);
			hwaddr_set = true;
			break;
		}
		if(!hwaddr_set)
			env_set("ethaddr", ENCLUSTRA_ETHADDR_DEFAULT);
	}

#if defined(CONFIG_ZYNQ_QSPI)
#define xstr(s) str(s)
#define str(s) #s
	struct spi_flash *env_flash;
	uint32_t flash_size;

	/* Probe the QSPI flash */
	env_flash = spi_flash_probe(0, 0, 1000000, SPI_MODE_3);

	if (env_flash) {
		/* Calculate the size in megabytes */
		flash_size = env_flash->size / 1024 / 1024;
		if(!fpga.name)
			setup_qspi_args(flash_size, zx_get_idcode_name());
		else
			setup_qspi_args(flash_size, fpga.name);
	}
#endif
#endif

	return 0;
}

#ifdef CONFIG_DISPLAY_BOARDINFO
int checkboard(void)
{
	u32 version = zynq_get_silicon_version();

	version <<= 1;
	if (version > (PCW_SILICON_VERSION_3 << 1))
		version += 1;

	puts("Board: Xilinx Zynq\n");
	printf("Silicon: v%d.%d\n", version >> 1, version & 1);

	return 0;
}
#endif

int zynq_board_read_rom_ethaddr(unsigned char *ethaddr)
{
#if defined(CONFIG_ZYNQ_GEM_EEPROM_ADDR) && \
    defined(CONFIG_ZYNQ_GEM_I2C_MAC_OFFSET)
	if (eeprom_read(CONFIG_ZYNQ_GEM_EEPROM_ADDR,
			CONFIG_ZYNQ_GEM_I2C_MAC_OFFSET,
			ethaddr, 6))
		printf("I2C EEPROM MAC address read failed\n");
#endif

	return 0;
}

/* Setup clock skews for ethernet PHY on Enclustra ZX boards */
#if defined(ENCLUSTRA_MARS_ZX) || defined(ENCLUSTRA_MARS_ZX2) || defined(ENCLUSTRA_MERCURY_ZX)
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

#if !defined(CONFIG_SYS_SDRAM_BASE) && !defined(CONFIG_SYS_SDRAM_SIZE)
int dram_init_banksize(void)
{
	return fdtdec_setup_memory_banksize();
}

int dram_init(void)
{
	if (fdtdec_setup_memory_size() != 0)
		return -EINVAL;

	zynq_ddrc_init();

	return 0;
}
#else
int dram_init(void)
{
	gd->ram_size = CONFIG_SYS_SDRAM_SIZE;

	zynq_ddrc_init();

	return 0;
}
#endif
