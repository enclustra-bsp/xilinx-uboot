#ifndef __ENCLUSTRA_ZX_COMMON
#define __ENCLUSTRA_ZX_COMMON

/* Select which flash type currently uses Pins */
#define ZX_NONE    (0)
#define ZX_NAND    (1)
#define ZX_QSPI    (2)

#define PHY_ANEG_TIMEOUT	8000	/* PHY needs a longer aneg time */

#define MTDIDS_DEFAULT          "nand0=nand"
#define MTDPARTS_DEFAULT        "mtdparts=" \
	                                "nand:5M(nand-linux),"  \
                                "1M(nand-device-tree)," \
                                "1M(nand-bootscript),"  \
                                "-(nand-rootfs)"
#define CONFIG_MTD_DEVICE
#define CONFIG_CMD_MTDPARTS
#define CONFIG_MTD_PARTITIONS
#define CONFIG_RBTREE
/*#define CONFIG_CMD_UBI
 * #define CONFIG_CMD_UBIFS
 * #define CONFIG_LZO*/

/* QSPI Flash Memory Map */
/* 64MB of QSPI Flash on Mercury */
#ifdef CONFIG_ENCLUSTRA_QSPI_64M
#define QSPI_BOOT_OFFSET          0x00000000   // Storage for Bootimage (FSBL, FPGA Bitstream, UBoot)
#define QSPI_BOOT_SIZE            0x00700000   // size 7MB
#define QSPI_RESCUELINUX_OFFSET   0x00700000   // Storage for Linux Kernel
#define QSPI_RESCUELINUX_SIZE     0x00500000   // size 5MB
#define QSPI_RESCUEDTB_OFFSET     0x00C00000   // Storage for Linux Devicetree
#define QSPI_RESCUEDTB_SIZE       0x00080000   // size 512kB
#define QSPI_BOOTARGS_OFFSET      0x00C80000   // Storage for Uboot Environment
#define QSPI_BOOTARGS_SIZE        0x00080000   // size 512kB
#define QSPI_BOOTSCRIPT_OFFSET    0x00D00000   // Storage for Uboot boot script
#define QSPI_BOOTSCRIPT_SIZE      0x00040000   // size 256kB
#define QSPI_RESCUEROOTFS_OFFSET  0x00D40000   // Storage for Linux Root FS
#define QSPI_RESCUEROOTFS_SIZE    0x032C0000   // size 51.9MB
#define QSPI_FULLBOOT_SIZE        0x04000000
#else
/* 16MB of QSPI Flash on Mars */
#define QSPI_BOOT_OFFSET          0x00000000   // Storage for Bootimage (FSBL, FPGA Bitstream, UBoot)
#define QSPI_BOOT_SIZE            0x00600000   // size 6MB
#define QSPI_RESCUELINUX_OFFSET   0x00600000   // Storage for Linux Kernel
#define QSPI_RESCUELINUX_SIZE     0x00500000   // size 5MB
#define QSPI_RESCUEDTB_OFFSET     0x00B00000   // Storage for Linux Devicetree
#define QSPI_RESCUEDTB_SIZE       0x00080000   // size 512kB
#define QSPI_BOOTARGS_OFFSET      0x00B80000   // Storage for Uboot Environment
#define QSPI_BOOTARGS_SIZE        0x00080000   // size 512kB
#define QSPI_BOOTSCRIPT_OFFSET    0x00C00000   // Storage for Uboot boot script
#define QSPI_BOOTSCRIPT_SIZE      0x00040000   // size 256kB
#define QSPI_RESCUEROOTFS_OFFSET  0x00C40000   // Storage for Linux Root FS
#define QSPI_RESCUEROOTFS_SIZE    0x003C0000   // size 3.84MB
#define QSPI_FULLBOOT_SIZE        0x01000000
#endif

#ifdef CONFIG_ENV_SIZE
#undef CONFIG_ENV_SIZE
#endif
#define CONFIG_ENV_SIZE QSPI_BOOTARGS_SIZE

#ifdef CONFIG_ENV_OFFSET
#undef CONFIG_ENV_OFFSET
#endif
#define CONFIG_ENV_OFFSET QSPI_BOOTARGS_OFFSET

#define ENCLUSTRA_EEPROM_ADDR_TAB   {0x5C,0x64,0x0B,0x0F}
#define ENCLUSTRA_EEPROM_HWMAC_REG  0x10
#define ENCLUSTRA_MAC               0xF7B020

#define STRINGIFY_PRE(x)   #x
#define STRINGIFY(x)   STRINGIFY_PRE(x)
#ifdef CONFIG_EXTRA_ENV_SETTINGS
#undef CONFIG_EXTRA_ENV_SETTINGS
#endif
#define CONFIG_EXTRA_ENV_SETTINGS               \
    "loadaddr=0x200000\0"                       \
    "ipaddr=192.168.1.113\0"                    \
    "serverip=192.168.1.103\0"                  \
    "serverpath=/srv/nfs/rootfs\0"              \
    "netmask=255.255.255.0\0"                   \
                                                \
    "kernel_image=uImage\0"                     \
    "ramdisk_image=uramdisk\0"                  \
    "devicetree_image=devicetree.dtb\0"         \
    "bootscript_image=uboot.scr\0"              \
    "bootimage_image=boot.bin\0"		\
    "bootimage_image_full=boot_full.bin\0"      \
    "jffs2_image=rootfs.jffs2\0"                \
    "ubifs_image=rootfs.ubi\0"                  \
                                                \
    "kernel_loadaddr=0x6000000\0"               \
    "devicetree_loadaddr=0x6600000\0"           \
    "ramdisk_loadaddr=0x2000000\0"              \
    "jffs2_loadaddr=0x2000000\0"                \
    "ubifs_loadaddr=0x2000000\0"                \
    "bootscript_loadaddr=0x1000000\0"           \
    "bootimage_loadaddr=0x4000000\0"		\
                                                \
    "ramdisk_size="    STRINGIFY(QSPI_RESCUEROOTFS_SIZE) "\0"   \
    "jffs2_size="      STRINGIFY(QSPI_RESCUEROOTFS_SIZE) "\0"   \
    "ubifs_size="      STRINGIFY(QSPI_RESCUEROOTFS_SIZE) "\0"   \
    "kernel_size="     STRINGIFY(QSPI_RESCUELINUX_SIZE)  "\0"   \
    "devicetree_size=" STRINGIFY(QSPI_RESCUEDTB_SIZE)    "\0"   \
    "bootscript_size=" STRINGIFY(QSPI_BOOTSCRIPT_SIZE)   "\0"   \
    "bootimage_size="  STRINGIFY(QSPI_BOOT_SIZE)         "\0"   \
    "fullboot_size="   STRINGIFY(QSPI_FULLBOOT_SIZE)     "\0"   \
								\
    "qspi_kernel_offset="     STRINGIFY(QSPI_RESCUELINUX_OFFSET) "\0"\
    "qspi_ramdisk_offset="    STRINGIFY(QSPI_RESCUEROOTFS_OFFSET)"\0"\
    "qspi_devicetree_offset=" STRINGIFY(QSPI_RESCUEDTB_OFFSET)   "\0"\
    "qspi_bootscript_offset=" STRINGIFY(QSPI_BOOTSCRIPT_OFFSET)  "\0"\
    "qspi_bootimage_offset="  STRINGIFY(QSPI_BOOT_OFFSET)        "\0"\
                                                \
    "nand_kernel_size=0x500000\0"               \
    "nand_devicetree_size=0x10000\0"            \
                                                \
    "def_args=console=ttyPS0,115200 rw earlyprintk\0"\
    "ramdisk_args=setenv bootargs ${def_args} root=/dev/ram\0"\
    "mmc_args=setenv bootargs ${def_args} rootwait root=/dev/mmcblk0p2\0"\
    "nand_args=setenv bootargs ${def_args} rootwait=1 ubi.mtd=3 rootfstype=ubifs root=ubi0:rootfs\0"\
    "qspi_args=setenv bootargs ${def_args} root=/dev/mtdblock5 rootfstype=jffs2 rootwait\0"\
    "net_args=setenv bootargs ${def_args} rootwait root=/dev/nfs nfsroot=${serverip}:${serverpath},v3 ip=dhcp\0"\
                                                \
    "fdt_high=0x20000000\0"                     \
    "initrd_high=0x20000000\0"                  \
    "mtdids=" MTDIDS_DEFAULT "\0"               \
    "mtdparts=" MTDPARTS_DEFAULT "\0"           \
                                                \
    "qspiboot=echo Bootinq on QSPI Flash ...; " \
        "zx_set_storage QSPI && "               \
        "sf probe && "                          \
        "sf read ${bootscript_loadaddr} ${qspi_bootscript_offset} ${bootscript_size} && "\
        "source ${bootscript_loadaddr}\0"       \
                                                \
    "sdboot=echo Booting on SD Card ...; "      \
        "mmc rescan && "                        \
        "load mmc 0 ${bootscript_loadaddr} ${bootscript_image} && "\
        "source ${bootscript_loadaddr}\0"       \
                                                \
    "jtagboot=echo Booting on TFTP ...; "       \
        "tftpboot ${bootscript_loadaddr} ${bootscript_image} && "\
        "source ${bootscript_loadaddr}\0"       \
                                                \
    "nandboot=echo Booting on NAND ...; "       \
        "zx_set_storage NAND && "               \
        "nand read ${bootscript_loadaddr} nand-bootscript ${bootscript_size} && " \
        "source ${bootscript_loadaddr} \0"	\
						\
    "usbboot=echo Booting on USB ...; "       	\
	"usb start && "				\
        "load usb 0 ${bootscript_loadaddr} ${bootscript_image} && " \
        "source ${bootscript_loadaddr} \0"      \
                                                \
    "netboot=echo Booting from TFTP/NFS ...; "  \
        "tftpboot ${bootscript_loadaddr} ${bootscript_image} && "\
        "source ${bootscript_loadaddr}\0"

#endif
