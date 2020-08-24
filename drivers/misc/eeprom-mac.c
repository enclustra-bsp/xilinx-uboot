#include <common.h>
#include <linux/compat.h>
#include <dm.h>
#include <i2c.h>
#include <enclustra/eeprom-mac.h>
#include <atsha204a-i2c.h>

static int i2c_get_device(uint chip_addr, int alen, struct udevice **devp)
{
	struct dm_i2c_chip *chip;
	int ret;

	ret = i2c_get_chip_for_busnum(CONFIG_SYS_I2C_EEPROM_BUS, chip_addr,
				      alen, devp);
	if (ret)
		return ret;

	chip = dev_get_parent_platdata(*devp);
	if (chip->offset_len != alen) {
		printf("I2C chip %x: requested alen %d does not match chip "
		       "offset_len %d\n", chip_addr, alen, chip->offset_len);
		return -EADDRNOTAVAIL;
	}

	return 0;
}

static struct udevice *get_atsha204a_dev(void)
{
	static struct udevice *dev;

	if (uclass_get_device_by_name(UCLASS_MISC, "atsha204a@64", &dev)) {
		debug("Cannot find ATSHA204A on I2C bus!\n");
		dev = NULL;
	}

	return dev;
}

int ds28_get_mac(u8 *buffer)
{
	int ret;
	struct udevice *dev;

	ret = i2c_get_device(DS28_I2C_ADDR, 1, &dev);
	if (ret != 0)
		return ret;

	return dm_i2c_read(dev,
		   0x10,
		   buffer,
		   6);
}
EXPORT_SYMBOL_GPL(ds28_get_mac);

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
