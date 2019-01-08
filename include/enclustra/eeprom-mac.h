#define DS28_I2C_ADDR 0x5C

struct eeprom_mem {
    int (*mac_reader)(u8 *buffer);
};

int ds28_get_mac(u8 *buffer);
int atsha204_get_mac(u8 *buffer);
