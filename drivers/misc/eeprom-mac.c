#include <common.h>
#include <linux/compat.h>
#include <dm.h>
#include <i2c.h>
#include <enclustra/eeprom-mac.h>
#include <atsha204a-i2c.h>

static struct udevice *get_atsha204a_dev(void)
{
	static struct udevice *dev;

	if (uclass_get_device_by_name(UCLASS_MISC, "atsha204a@64", &dev)) {
		debug("Cannot find ATSHA204A on I2C bus!\n");
		dev = NULL;
	}

	return dev;
}

int atsha204_get_mac(u8 *buffer)
{
	int ret;
	u8 data[4];
	u8 i;
	struct udevice *dev = get_atsha204a_dev();

	if (!dev)
		return -ENODEV;

#ifdef CONFIG_ATSHA204A
	ret = atsha204a_wakeup(dev);

	if (ret)
		return ret;

	ret = atsha204a_read(dev, ATSHA204A_ZONE_OTP, false,
			     4, data);

	if (ret) {
		return ret;
	} else {
		for (i = 0; i < 4; i++)
			buffer[i] = data[i];
	}

	ret = atsha204a_read(dev, ATSHA204A_ZONE_OTP, false,
			     5, data);
	if (ret) {
		return ret;
	} else {
		buffer[4] = data[0];
		buffer[5] = data[1];
	}

	atsha204a_sleep(dev);
#endif

	return 0;
}
EXPORT_SYMBOL_GPL(atsha204_get_mac);
