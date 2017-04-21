/*
 * Configuration for Enclustra XU* SOMs
 *
 * (C) Copyright 2015 Xilinx, Inc.
 * (C) Copyright 2016 Antmicro Ltd
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_ENCLUSTRA_XU_H
#define __CONFIG_ECNLUSTRA_XU_H

#define CONFIG_ZYNQ_SDHCI1
#define CONFIG_ZYNQ_I2C0
#define CONFIG_ZYNQ_I2C1
#define CONFIG_SYS_I2C_ZYNQ

#define CONFIG_SATA_CEVA

#define CONFIG_ZYNQMP_XHCI_LIST {ZYNQMP_USB0_XHCI_BASEADDR}

#define CONFIG_SYS_I2C_EEPROM_ADDR_LEN	1
#define CONFIG_CMD_EEPROM

#include <configs/xilinx_zynqmp.h>
#include <configs/enclustra_zx_common.h>

#ifdef CONFIG_SCSI
#undef CONFIG_SCSI
#endif

#ifdef CONFIG_ENV_IS_NOWHERE
#undef CONFIG_ENV_IS_NOWHERE
#endif

#define CONFIG_ENV_SECT_SIZE CONFIG_ENV_SIZE
#define CONFIG_ENV_IS_IN_SPI_FLASH

#endif /* __CONFIG_ENCLUSTRA_XU_H */
