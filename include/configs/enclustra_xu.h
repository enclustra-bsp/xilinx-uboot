/*
 * Configuration for Enclustra XU* SOMs
 *
 * (C) Copyright 2015 Xilinx, Inc.
 * (C) Copyright 2016 Antmicro Ltd
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_ENCLUSTRA_XU_H
#define __CONFIG_ENCLUSTRA_XU_H

#define CONFIG_SATA_CEVA

#define CONFIG_SYS_I2C_EEPROM_ADDR_LEN	1
#define CONFIG_CMD_EEPROM

#include <configs/xilinx_zynqmp.h>
#include <configs/enclustra_zx_common.h>

#ifdef CONFIG_SCSI
#undef CONFIG_SCSI
#endif

#endif /* __CONFIG_ENCLUSTRA_XU_H */
