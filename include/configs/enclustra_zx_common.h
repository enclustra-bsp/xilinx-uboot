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
#endif

#ifdef CONFIG_ENV_SIZE
#undef CONFIG_ENV_SIZE
#define CONFIG_ENV_SIZE QSPI_BOOTARGS_SIZE
#endif

#ifdef CONFIG_ENV_OFFSET
#undef CONFIG_ENV_OFFSET
#endif
#define CONFIG_ENV_OFFSET QSPI_BOOTARGS_OFFSET

#define STRINGIFY_PRE(x)   #x
#define STRINGIFY(x)   STRINGIFY_PRE(x)
#ifdef CONFIG_EXTRA_ENV_SETTINGS
#undef CONFIG_EXTRA_ENV_SETTINGS
#endif
#define CONFIG_EXTRA_ENV_SETTINGS               \
    "loadaddr=0x200000\0"                       \
    "ethaddr=00:0a:35:00:01:22\0"               \
    "ipaddr=192.168.1.113\0"                    \
    "serverip=192.168.1.103\0"                  \
    "netmask=255.255.255.0\0"                   \
                                                \
    "kernel_image=uImage\0"                     \
    "ramdisk_image=uramdisk\0"                  \
    "devicetree_image=devicetree.dtb\0"         \
    "bootscript_image=uboot.scr\0"              \
    "bootimage_image=boot.bin\0"		\
    "jffs2_image=rootfs.jffs2\0"                \
                                                \
    "kernel_loadaddr=0x6000000\0"               \
    "devicetree_loadaddr=0x6600000\0"           \
    "ramdisk_loadaddr=0x2000000\0"              \
    "jffs2_loadaddr=0x2000000\0"                \
    "bootscript_loadaddr=0x1000000\0"           \
    "bootimage_loadaddr=0x4000000\0"		\
                                                \
    "ramdisk_size="    STRINGIFY(QSPI_RESCUEROOTFS_SIZE) "\0"   \
    "jffs2_size="      STRINGIFY(QSPI_RESCUEROOTFS_SIZE) "\0"   \
    "kernel_size="     STRINGIFY(QSPI_RESCUELINUX_SIZE)  "\0"   \
    "devicetree_size=" STRINGIFY(QSPI_RESCUEDTB_SIZE)    "\0"   \
    "bootscript_size=" STRINGIFY(QSPI_BOOTSCRIPT_SIZE)   "\0"   \
    "bootimage_size="  STRINGIFY(QSPI_BOOT_SIZE)         "\0"   \
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
        "source ${bootscript_loadaddr} \0"

#endif
