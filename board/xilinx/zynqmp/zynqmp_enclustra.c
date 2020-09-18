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


#if defined(CONFIG_FPGA) && defined(CONFIG_FPGA_ZYNQMPPL) && \
    !defined(CONFIG_SPL_BUILD)
static xilinx_desc zynqmppl = XILINX_ZYNQMP_DESC;

static const struct {
	u32 id;
	u32 ver;
	char *name;
	bool evexists;
} zynqmp_devices[] = {
	{
		.id = 0x10,
		.name = "3eg",
	},
	{
		.id = 0x10,
		.ver = 0x2c,
		.name = "3cg",
	},
	{
		.id = 0x11,
		.name = "2eg",
	},
	{
		.id = 0x11,
		.ver = 0x2c,
		.name = "2cg",
	},
	{
		.id = 0x20,
		.name = "5ev",
		.evexists = 1,
	},
	{
		.id = 0x20,
		.ver = 0x100,
		.name = "5eg",
		.evexists = 1,
	},
	{
		.id = 0x20,
		.ver = 0x12c,
		.name = "5cg",
		.evexists = 1,
	},
	{
		.id = 0x21,
		.name = "4ev",
		.evexists = 1,
	},
	{
		.id = 0x21,
		.ver = 0x100,
		.name = "4eg",
		.evexists = 1,
	},
	{
		.id = 0x21,
		.ver = 0x12c,
		.name = "4cg",
		.evexists = 1,
	},
	{
		.id = 0x30,
		.name = "7ev",
		.evexists = 1,
	},
	{
		.id = 0x30,
		.ver = 0x100,
		.name = "7eg",
		.evexists = 1,
	},
	{
		.id = 0x30,
		.ver = 0x12c,
		.name = "7cg",
		.evexists = 1,
	},
	{
		.id = 0x38,
		.name = "9eg",
	},
	{
		.id = 0x38,
		.ver = 0x2c,
		.name = "9cg",
	},
	{
		.id = 0x39,
		.name = "6eg",
	},
	{
		.id = 0x39,
		.ver = 0x2c,
		.name = "6cg",
	},
	{
		.id = 0x40,
		.name = "11eg",
	},
	{ /* For testing purpose only */
		.id = 0x50,
		.ver = 0x2c,
		.name = "15cg",
	},
	{
		.id = 0x50,
		.name = "15eg",
	},
	{
		.id = 0x58,
		.name = "19eg",
	},
	{
		.id = 0x59,
		.name = "17eg",
	},
	{
		.id = 0x61,
		.name = "21dr",
	},
	{
		.id = 0x63,
		.name = "23dr",
	},
	{
		.id = 0x65,
		.name = "25dr",
	},
	{
		.id = 0x64,
		.name = "27dr",
	},
	{
		.id = 0x60,
		.name = "28dr",
	},
	{
		.id = 0x62,
		.name = "29dr",
	},
	{
		.id = 0x66,
		.name = "39dr",
	},
	{
		.id = 0x7b,
		.name = "48dr",
	},
	{
		.id = 0x7e,
		.name = "49dr",
	},
};
#endif

extern int chip_id(unsigned char id);

#define ZYNQMP_VERSION_SIZE		9
#define ZYNQMP_PL_STATUS_BIT		9
#define ZYNQMP_IPDIS_VCU_BIT		8
#define ZYNQMP_PL_STATUS_MASK		BIT(ZYNQMP_PL_STATUS_BIT)
#define ZYNQMP_CSU_VERSION_MASK		~(ZYNQMP_PL_STATUS_MASK)
#define ZYNQMP_CSU_VCUDIS_VER_MASK	ZYNQMP_CSU_VERSION_MASK & \
					~BIT(ZYNQMP_IPDIS_VCU_BIT)
#define MAX_VARIANTS_EV			3

#if defined(CONFIG_FPGA) && defined(CONFIG_FPGA_ZYNQMPPL) && \
	!defined(CONFIG_SPL_BUILD)
static char *zynqmp_get_silicon_idcode_name(void)
{
	u32 i, id, ver, j;
	char *buf;
	static char name[ZYNQMP_VERSION_SIZE];

	id = chip_id(IDCODE);
	ver = chip_id(IDCODE2);

	for (i = 0; i < ARRAY_SIZE(zynqmp_devices); i++) {
		if (zynqmp_devices[i].id == id) {
			if (zynqmp_devices[i].evexists &&
			    !(ver & ZYNQMP_PL_STATUS_MASK))
				break;
			if (zynqmp_devices[i].ver == (ver &
			    ZYNQMP_CSU_VERSION_MASK))
				break;
		}
	}

	if (i >= ARRAY_SIZE(zynqmp_devices))
		return "unknown";

	strncat(name, "zu", 2);
	if (!zynqmp_devices[i].evexists ||
	    (ver & ZYNQMP_PL_STATUS_MASK)) {
		strncat(name, zynqmp_devices[i].name,
			ZYNQMP_VERSION_SIZE - 3);
		return name;
	}

	/*
	 * Here we are means, PL not powered up and ev variant
	 * exists. So, we need to ignore VCU disable bit(8) in
	 * version and findout if its CG or EG/EV variant.
	 */
	for (j = 0; j < MAX_VARIANTS_EV; j++, i++) {
		if ((zynqmp_devices[i].ver & ~BIT(ZYNQMP_IPDIS_VCU_BIT)) ==
		    (ver & ZYNQMP_CSU_VCUDIS_VER_MASK)) {
			strncat(name, zynqmp_devices[i].name,
				ZYNQMP_VERSION_SIZE - 3);
			break;
		}
	}

	if (j >= MAX_VARIANTS_EV)
		return "unknown";

	if (strstr(name, "eg") || strstr(name, "ev")) {
		buf = strstr(name, "e");
		*buf = '\0';
	}

	return name;
}
#endif

int enclustra_board(void)
{
#if defined(CONFIG_FPGA) && defined(CONFIG_FPGA_ZYNQMPPL) && \
    !defined(CONFIG_SPL_BUILD) || (defined(CONFIG_SPL_FPGA_SUPPORT) && \
    defined(CONFIG_SPL_BUILD))
	if (current_el() != 3) {
		zynqmppl.name = zynqmp_get_silicon_idcode_name();
	}

	/* Probe the QSPI flash */
#if defined(CONFIG_ENCLUSTRA_QSPI_FLASHMAP) && \
	!defined(CONFIG_SPL_BUILD)
	struct spi_flash *env_flash;
	u32 flash_size;

	env_flash = spi_flash_probe(0, 0, 1000000,
					SPI_RX_QUAD | SPI_TX_QUAD | SPI_MODE_2);
	if (env_flash) {
		flash_size = env_flash->size / 1024 / 1024;
		setup_qspi_args(flash_size, zynqmppl.name);
	}
#endif
#endif
	return 0;
}
