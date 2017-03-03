struct flash_partition_map {
    int flash_size;
    int bootimage_size;
    int bootscr_size;
    int devicetree_size;
    int kernel_size;
    int rootfs_size;
};

#define FLASH_MAP(sz, bis, bss, dts, ks, rs) \
{               \
    .flash_size = sz,   \
    .bootimage_size = bis, \
    .bootscr_size = bss,    \
    .devicetree_size = dts,  \
    .kernel_size = ks,  \
    .rootfs_size = rs,  \
}

#ifdef CONFIG_ARCH_ZYNQMP
static struct flash_partition_map flash_maps[] = {
    FLASH_MAP(64, 0x01b00000, 0x00080000, 0x00080000, 0x00e00000, 0),
};
#else
static struct flash_partition_map flash_maps[] = {
    FLASH_MAP(16, 0x00600000, 0x00080000, 0x00080000, 0x00500000, 0x00380000),
    FLASH_MAP(64, 0x00700000, 0x00080000, 0x00080000, 0x00500000, 0x03280000),
};
#endif

static inline int setup_qspi_args(int flash_sz) {
    int i;
    struct flash_partition_map fm;
    int boot_off, env_off, kern_off, dtb_off, bscr_off, rfs_off;
    for(i=0; i<ARRAY_SIZE(flash_maps); i++){
        if (flash_sz == flash_maps[i].flash_size){
            fm = flash_maps[i];
            boot_off = QSPI_BOOT_OFFSET;
            env_off = boot_off + fm.bootimage_size;
            kern_off = env_off + QSPI_BOOTARGS_SIZE;
            dtb_off = kern_off + fm.kernel_size;
            bscr_off = dtb_off + fm.devicetree_size;
            rfs_off = bscr_off + fm.bootscr_size;
            setenv_hex("ramdisk_size", fm.rootfs_size);
            setenv_hex("jffs2_size", fm.rootfs_size);
            setenv_hex("ubifs_size", fm.rootfs_size);
            setenv_hex("kernel_size", fm.kernel_size);
            setenv_hex("devicetree_size", fm.devicetree_size);
            setenv_hex("bootscript_size", fm.bootscr_size);
            setenv_hex("bootimage_size", fm.bootimage_size);
            setenv_hex("fullboot_size", flash_sz*1024*1024);
            setenv_hex("qspi_bootimage_offset", boot_off);
            setenv_hex("qspi_kernel_offset", kern_off);
            setenv_hex("qspi_ramdisk_offset", rfs_off);
            setenv_hex("qspi_devicetree_offset", dtb_off);
            setenv_hex("qspi_bootscript_offset", bscr_off);
            break;
        }
    }
}
