/*
 * (C) Copyright 2013 Xilinx, Inc.
 * (C) Copyright 2015 Antmicro Ltd
 *
 * Configuration for Enclustra Mars ZX3 SOM
 * See zynq-common.h for Zynq common configs
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef __CONFIG_ZYNQ_MARS_ZX3_H
#define __CONFIG_ZYNQ_MARS_ZX3_H

#define CONFIG_CPU_FREQ_HZ		666666666

#define CONFIG_SYS_SDRAM_SIZE		(512 * 1024 * 1024)

#define CONFIG_ZYNQ_SERIAL_UART1
#define CONFIG_ZYNQ_GEM0
#define CONFIG_ZYNQ_GEM_PHY_ADDR0	0

#define CONFIG_SYS_NO_FLASH

#define CONFIG_ZYNQ_USB
#define CONFIG_ZYNQ_SDHCI0
#define CONFIG_ZYNQ_QSPI

#define CONFIG_ZYNQ_BOOT_FREEBSD

#include <configs/zynq-common.h>

/*#define MTDIDS_DEFAULT          "nand0=nand"
#define MTDPARTS_DEFAULT        "mtdparts=" \
                                "nand:5m(nand-linux)," \
                                "1m(nand-device-tree)," \
                                "-(nand-rootfs)"

#define MTDIDS_DEFAULT          "nand0=nand"
#define MTDPARTS_DEFAULT        "mtdparts=" \
                                "nand:5m(nand-linux)," \
                                "1m(nand-device-tree)," \
                                "-(nand-rootfs)"
#define CONFIG_CMD_MTDPARTS
#define CONFIG_MTD_PARTITIONS
#define CONFIG_RBTREE
#define CONFIG_CMD_UBI*/

/* QSPI Flash Memory Map */
#define QSPI_BOOT_OFFSET          0x00000000   // Storage for Bootimage (FSBL, FPGA Bitstream, UBoot)
#define QSPI_BOOT_SIZE            0x00600000   // size 6MB
#define QSPI_RESCUELINUX_OFFSET   0x00600000   // Storage for Linux Kernel
#define QSPI_RESCUELINUX_SIZE     0x00300000   // size 3MB
#define QSPI_RESCUEDTB_OFFSET     0x00900000   // Storage for Linux Devicetree
#define QSPI_RESCUEDTB_SIZE       0x00080000   // size 512kB
#define QSPI_BOOTARGS_OFFSET      0x00980000   // Storage for Uboot Environment
#define QSPI_BOOTARGS_SIZE        0x00080000   // size 512kB
#define QSPI_RESCUEROOTFS_OFFSET  0x00A00000   // Storage for Linux Root FS
#define QSPI_RESCUEROOTFS_SIZE    0x00600000   // size 6MB

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
    "serverip=192.168.1.62\0"                   \
    "netmask=255.255.255.0\0"                   \
                                                \
    "kernel_image=uImage\0"                     \
    "ramdisk_image=uramdisk.image.gz\0"         \
    "devicetree_image=devicetree.dtb\0"         \
                                                \
    "ramdisk_size="    STRINGIFY(QSPI_RESCUEROOTFS_SIZE) "\0"   \
    "kernel_size="     STRINGIFY(QSPI_RESCUELINUX_SIZE)  "\0"   \
    "devicetree_size=" STRINGIFY(QSPI_RESCUEDTB_SIZE)    "\0"   \
                                                \
    "nand_kernel_size=0x500000\0"               \
    "nand_devicetree_size=0x10000\0"            \
                                                \
    "fdt_high=0x20000000\0"                     \
    "initrd_high=0x20000000\0"                  \
   /* "mtdids=" MTDIDS_DEFAULT "\0" */   \
  /*  "mtdparts=" MTDPARTS_DEFAULT "\0" */\
    "nandargs=setenv bootargs console=ttyPS0,115200 " \
        "root=ubi0:ubi-rootfs rw " \
        "rootfstype=ubifs " \
        "ubi.mtd=nand-rootfs " \
        "ip=:::::eth0:off " \
        "$othbootargs " \
        "earlyprintk\0" \
    "ramargs=setenv bootargs console=ttyPS0,115200 " \
        "root=/dev/ram rw " \
        "ip=:::::eth0:off " \
        "$othbootargs " \
        "earlyprintk\0" \
    "prodboot=echo Booting on NAND...;" \
        "nand device 0;" \
        "ubi part nand-linux;" \
        "ubi read 0x3000000 kernel ${nand_kernel_size};" \
        "ubi part nand-device-tree;" \
        "ubi read 0x2A00000 dtb ${nand_devicetree_size};" \
        "run nandargs;" \
        "bootm 0x3000000 - 0x2A00000\0" \
    "rescboot=echo Booting on QSPI Flash...;" \
        "sf probe;" \
        "sf read 0x3000000 " STRINGIFY(QSPI_RESCUELINUX_OFFSET) " ${kernel_size};" \
        "sf read 0x2A00000 " STRINGIFY(QSPI_RESCUEDTB_OFFSET) " ${devicetree_size};" \
        "sf read 0x2000000 " STRINGIFY(QSPI_RESCUEROOTFS_OFFSET) " ${ramdisk_size};" \
    "nand device 0;" \
        "run ramargs;" \
        "bootm 0x3000000 0x2000000 0x2A00000;\0" \
    "jtagboot=echo Booting on TFTP...;" \
        "tftp 0x3000000 ${kernel_image};" \
        "tftp 0x2A00000 ${devicetree_image};" \
        "tftp 0x2000000 ${ramdisk_image};" \
        "run ramargs;" \
        "bootm 0x3000000 0x2000000 0x2A00000\0" \
    "modeboot=prodboot\0"


#endif /* __CONFIG_ZYNQ_MARS_ZX3_H */
