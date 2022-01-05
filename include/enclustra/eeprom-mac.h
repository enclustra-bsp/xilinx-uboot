/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * (C) Copyright 2021 Enclustra GmbH
 * <info@enclustra.com>
 */

struct eeprom_mem {
    int (*mac_reader)(u8 *buffer);
};

int atsha204_get_mac(u8 *buffer);
