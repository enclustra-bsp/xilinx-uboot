/*
 * (C) Copyright 2013 Xilinx, Inc.
 * (C) Copyright 2015 Antmicro Ltd
 *
 * Configuration for Enclustra Cosmos Board
 *
 * See zynq-common.h for Zynq common configs
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_ZYNQ_COSMOS_H
#define __CONFIG_ZYNQ_COSMOS_H

#define CONFIG_MARS_ZX
#define CONFIG_CPU_FREQ_HZ		666666666
#define CONFIG_ZYNQ_GEM_PHY_ADDR0	0
#define CONFIG_ZYNQ_USB
#define CONFIG_ZYNQ_I2C0

/* Select Micrel PHY */
#define CONFIG_PHY_MICREL

#include <configs/zynq-common.h>
#include <configs/enclustra_zx_common.h>

/* Unselect Marvell PHY (selected by zynq-common) */
#ifdef CONFIG_PHY_MARVELL
#undef CONFIG_PHY_MARVELL
#endif

#endif /* __CONFIG_ZYNQ_COSMOS_H */
