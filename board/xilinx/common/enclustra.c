// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2020 Enclustra GmbH
 */

#include <common.h>
#include <env.h>
#include <enclustra/eeprom-mac.h>

#define ENCLUSTRA_MAC               0xF7B020

/* Default MAC address */
#define ENCLUSTRA_ETHADDR_DEFAULT "00:0A:35:01:02:03"
#define ENCLUSTRA_ETH1ADDR_DEFAULT "00:0A:35:01:02:04"

static struct eeprom_mem eeproms[] = {
	{ .mac_reader = atsha204_get_mac },
	{ .mac_reader = ds28_get_mac }
};

int enclustra_common(void)
{
	int i;
	u8 hwaddr[6] = {0, 0, 0, 0, 0, 0};
	u32 hwaddr_h;
	char hwaddr_str[18];
	bool hwaddr_set = false;
	
#if defined(CONFIG_ENCLUSTRA_EEPROM_MAC)

	if (!env_get("ethaddr")) {
		for (i = 0; i < ARRAY_SIZE(eeproms); i++) {
			if (eeproms[i].mac_reader(hwaddr))
				continue;

			/* Workaround for incorrect MAC address caused by
			 * flashing to EEPROM addresses like 20:B0:F0:XX:XX:XX
			 * instead of 20:B0:F7:XX:XX:XX
			 */
			hwaddr[2] = (hwaddr[2] == 0xF0) ? 0xF7 : hwaddr[2];

			/* Check if the value is a valid mac registered for
			 * Enclustra  GmbH
			 */
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

			/* increment MAC addr */
			hwaddr_h = (hwaddr[3] << 16) | (hwaddr[4] << 8) | hwaddr[5];
			hwaddr_h = (hwaddr_h + 1) & 0xFFFFFF;
			hwaddr[3] = (hwaddr_h >> 16) & 0xFF;
			hwaddr[4] = (hwaddr_h >> 8) & 0xFF;
			hwaddr[5] = hwaddr_h & 0xFF;

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
			env_set("eth1addr", hwaddr_str);

			hwaddr_set = true;
			break;
		}

		if (!hwaddr_set) {
			env_set("ethaddr", ENCLUSTRA_ETHADDR_DEFAULT);
			env_set("eth1addr", ENCLUSTRA_ETH1ADDR_DEFAULT);
		}
	}
	return 0;
#endif
}

