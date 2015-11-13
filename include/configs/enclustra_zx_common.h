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
#define CONFIG_CMD_ELF
/*#define CONFIG_CMD_UBI
 * #define CONFIG_CMD_UBIFS
 * #define CONFIG_LZO*/

/* QSPI Flash Memory Map */
#define QSPI_BOOT_OFFSET           0x00000000 // Storage for Bootimage
#define QSPI_BOOTARGS_OFFSET       0x00700000 // Storage for Uboot Environment
#define QSPI_BOOTARGS_SIZE         0x00080000 // size 512kB

/* 64M QSPI Flash */
#define QSPI_64M_BOOT_SIZE         0x00700000 // size 7MB
#define QSPI_64M_LINUX_OFFSET      0x00780000 // Storage for Linux Kernel
/* env goes here, size 512kB */
#define QSPI_64M_LINUX_SIZE        0x00500000 // size 5MB
#define QSPI_64M_DTB_OFFSET        0x00C80000 // Storage for Linux Devicetree
#define QSPI_64M_DTB_SIZE          0x00080000 // size 512kB
#define QSPI_64M_BOOTSCRIPT_OFFSET 0x00D00000 // Storage for Uboot boot script
#define QSPI_64M_BOOTSCRIPT_SIZE   0x00080000 // size 256kB
#define QSPI_64M_ROOTFS_OFFSET     0x00D80000 // Storage for Linux Root FS
#define QSPI_64M_ROOTFS_SIZE       0x03280000 // size 50.5MB
#define QSPI_64M_FULLBOOT_SIZE     0x04000000 // whole 64MB

/* 16M QSPI Flash */
#define QSPI_16M_BOOT_SIZE         0x00600000 // size 6MB
#define QSPI_16M_DTB_OFFSET        0x00600000 // Storage for Linux Devicetree
#define QSPI_16M_DTB_SIZE          0x00080000 // size 512kB
#define QSPI_16M_BOOTSCRIPT_OFFSET 0x00680000 // Storage for Uboot boot script
#define QSPI_16M_BOOTSCRIPT_SIZE   0x00080000 // size 512kB
/* env goes here, size 512kB */
#define QSPI_16M_LINUX_OFFSET      0x00780000 // Storage for Linux Kernel
#define QSPI_16M_LINUX_SIZE        0x00500000 // size 5MB
#define QSPI_16M_ROOTFS_OFFSET     0x00C80000 // Storage for Linux Root FS
#define QSPI_16M_ROOTFS_SIZE       0x00380000 // size 3.5MB
#define QSPI_16M_FULLBOOT_SIZE     0x01000000 // whole 16MB

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
