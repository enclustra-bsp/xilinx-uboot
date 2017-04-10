#define ATSHA204_COMMAND    0x03
#define ATSHA204_READ_CMD   0x02

#define ATSHA204_CONFIG_ZONE    0x00
#define ATSHA204_OTP_ZONE   0x01
#define ATSHA204_DATA_ZONE  0x02

#define ATSHA204_READ32_BYTES_FLAG  (1<<7)

struct eeprom_mem {
    u8 i2c_addr;
    int (*mac_reader)(u8 i2c_address, u8* buffer);
    int (*wakeup)(u8 i2c_address);
};

struct __attribute__((packed, aligned(1))) atsha204_read_command {
    u8 count;
    u8 opcode;
    u8 param1;
    u16 param2;
    u16 checksum;
};

int ds28_get_mac(u8 i2c_address, u8* buffer);

int atsha204_wakeup(u8 i2c_address);
int atsha204_get_mac(u8 i2c_address, u8* buffer);
