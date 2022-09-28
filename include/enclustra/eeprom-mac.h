/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * (C) Copyright 2021 Enclustra GmbH
 * <info@enclustra.com>
 */
#define DS28_I2C_ADDR 0x5C
#define DS28_SYS_I2C_EEPROM_BUS 0

struct eeprom_mem {
    int (*mac_reader)(u8 *buffer);
};

int ds28_get_mac(u8 *buffer);
int atsha204_get_mac(u8 *buffer);
