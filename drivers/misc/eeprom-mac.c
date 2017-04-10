#include <common.h>
#include <i2c.h>
#include <enclustra/eeprom-mac.h>

int ds28_get_mac(u8 i2c_address, u8* buffer) {

	return i2c_read(i2c_address,
		   0x10,
		   1,
		   buffer,
		   6);
}
EXPORT_SYMBOL_GPL(ds28_get_mac);

u16 atsha204_crc16(const u8 *buf, const u8 len)
{
		u8 i;
		u16 crc16 = 0;

		for (i = 0; i < len; i++) {
				u8 shift;

				for (shift = 0x01; shift > 0x00; shift <<= 1) {
						u8 data_bit = (buf[i] & shift) ? 1 : 0;
						u8 crc_bit = crc16 >> 15;

						crc16 <<= 1;

						if ((data_bit ^ crc_bit) != 0)
								crc16 ^= 0x8005;
				}
		}

		return crc16;
}

int atsha204_send_read_cmd(u8 i2c_address, u8 zone, u16 address, u8 read_block) {

	int ret;
	u16 crc;
	struct atsha204_read_command packet;

	packet.count = sizeof(struct atsha204_read_command);
	packet.opcode = ATSHA204_READ_CMD;
	packet.param1 = zone;
	if(read_block) packet.param1 |= ATSHA204_READ32_BYTES_FLAG;
	packet.param2 = address;

	crc = atsha204_crc16((u8*)(&packet), sizeof(struct atsha204_read_command) - 2);
	packet.checksum = crc;

	ret = i2c_write(i2c_address,
		  ATSHA204_COMMAND,
		  1,
		  (u8*)&packet,
		  sizeof(struct atsha204_read_command));
	if(ret) {
		printf("Writing failed \n");
		return ret;
	}
	/* reading may take up to 4 ms */
	udelay(4000);

	return 0;
}

int atsha204_wakeup(u8 i2c_address) {

	u8 wake_cmd = 0x0;

	return i2c_write(i2c_address,
		  0,
		  0,
		  &wake_cmd,
		  1);

}
EXPORT_SYMBOL_GPL(atsha204_wakeup);

int atsha204_read_data(u8 i2c_address, u8* buffer, u8 len) {

	int ret = 0;
	u8 first_word[4];
	u8 msg_len;
	u8 i;

	if (len < 4) return -ENOMEM;
	/* read the first 4 bytes from the device*/
	ret = i2c_read(i2c_address,
			0,
			0,
			first_word,
			4);

	if(ret) return ret;

	/* the first transferred byte is total length of the msg */
	msg_len = first_word[0];

	for(i = 0; i < 3; i++) buffer[i] = first_word[i+1];

	msg_len -= 4;

	if(!msg_len) {
		buffer[3] = 0xff;
		return 4;
	}

	if( (len-3) < msg_len ) return -ENOMEM;

	/* receive the rest of the data */

	ret = i2c_read(i2c_address,
			0,
			0,
			(u8*)(buffer + 3),
			msg_len);
	if(ret) return ret;
	return msg_len + 3;
}

int atsha204_read_otp_register(u8 i2c_address, u8 reg, u8* buffer) {

	u8 data[8];
	u8 i;
	int ret;

	ret = atsha204_wakeup(i2c_address);
	if(ret) return ret;

	ret = atsha204_send_read_cmd(i2c_address, ATSHA204_OTP_ZONE, reg, 0);
	if(ret) return ret;

	/* Attempt to read the register */

	ret = atsha204_read_data(i2c_address, data, 8);
	if(ret < 0) return ret;

	for(i = 0; i < 4; i++) buffer[i] = data[i];

	return 0;
}

int atsha204_get_mac(u8 i2c_address, u8* buffer) {

	int ret;
	u8 data[4];
	u8 i;

	ret = atsha204_read_otp_register(i2c_address, 4, data);
	if(ret) return ret;
	else for(i = 0; i < 4; i++) buffer[i] = data[i];

	ret = atsha204_read_otp_register(i2c_address, 5, data);
	if(ret) return ret;
	else {
		buffer[4] = data[0];
		buffer[5] = data[1];
	}
	return 0;
}
EXPORT_SYMBOL_GPL(atsha204_get_mac);
