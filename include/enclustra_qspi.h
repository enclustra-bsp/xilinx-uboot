struct flash_partition_map {
    char *chip_name;
    int flash_size;
    int bootimage_size;
    int bootscr_size;
    int devicetree_size;
    int kernel_size;
    int rootfs_size;
};

#define FLASH_MAP(cn, sz, bis, bss, dts, ks, rs) \
{               \
    .chip_name = cn, \
    .flash_size = sz,   \
    .bootimage_size = bis, \
    .bootscr_size = bss,    \
    .devicetree_size = dts,  \
    .kernel_size = ks,  \
    .rootfs_size = rs,  \
}


/* Last array item should contain default offset map in case of missing chip name */
#ifdef CONFIG_ARCH_ZYNQMP
static struct flash_partition_map flash_maps[] = {
    FLASH_MAP("zu15eg", 64, 0x01f00000, 0x00080000, 0x00080000, 0x00e00000, 0),
    FLASH_MAP("zu9eg", 64, 0x01d00000, 0x00080000, 0x00080000, 0x00e00000, 0),
    FLASH_MAP("zu7ev", 64, 0x01500000, 0x00080000, 0x00080000, 0x00e00000, 0),
    FLASH_MAP("zu6eg", 64, 0x01d00000, 0x00080000, 0x00080000, 0x00e00000, 0),
    FLASH_MAP("zu5ev", 64, 0x00900000, 0x00080000, 0x00080000, 0x00e00000, 0),
    FLASH_MAP("zu4cg", 64, 0x00900000, 0x00080000, 0x00080000, 0x00e00000, 0),
    FLASH_MAP("zu4ev", 64, 0x00900000, 0x00080000, 0x00080000, 0x00e00000, 0),
    FLASH_MAP("zu3eg", 64, 0x00900000, 0x00080000, 0x00080000, 0x00e00000, 0),
    FLASH_MAP("zu2eg", 64, 0x00900000, 0x00080000, 0x00080000, 0x00e00000, 0),
    FLASH_MAP("zu2cg", 64, 0x00900000, 0x00080000, 0x00080000, 0x00e00000, 0),
    FLASH_MAP("default", 0, 0x01b00000, 0x00080000, 0x00080000, 0x00e00000, 0),
};
#else
static struct flash_partition_map flash_maps[] = {
    FLASH_MAP("7z010", 64, 0x00600000, 0x00080000, 0x00080000, 0x00500000, 0),
    FLASH_MAP("7z015", 64, 0x00600000, 0x00080000, 0x00080000, 0x00500000, 0),
    FLASH_MAP("7z020", 64, 0x00600000, 0x00080000, 0x00080000, 0x00500000, 0),
    FLASH_MAP("7z030", 64, 0x00700000, 0x00080000, 0x00080000, 0x00500000, 0),
    FLASH_MAP("7z035", 64, 0x00E00000, 0x00080000, 0x00080000, 0x00500000, 0),
    FLASH_MAP("7z045", 64, 0x00E00000, 0x00080000, 0x00080000, 0x00500000, 0),
    FLASH_MAP("default", 0, 0x00E00000, 0x00080000, 0x00080000, 0x00500000, 0),
};
#endif

static inline int env_set_hex_if_empty(const char *varname, ulong value)
{
    if (env_get(varname) == NULL)
        env_set_hex(varname, value);

    return 0;
}

// return best match for chip name and flash size
struct flash_partition_map *match_flash_entry(char *cn, int sz)
{
    int i;
    struct flash_partition_map *fm = NULL;
    for(i=0; i<ARRAY_SIZE(flash_maps); i++){
        if (!strcmp(cn, flash_maps[i].chip_name) && (sz >= flash_maps[i].flash_size)){
            if (!fm || (flash_maps[i].flash_size > fm->flash_size))
                fm = &flash_maps[i];
        }
    }
    /* use default map if match not found */
    if(!fm){
        fm = &flash_maps[ARRAY_SIZE(flash_maps)-1];
    }
    return fm;
}

static inline int calculate_rootfs_size(struct flash_partition_map* fm, int fsz)
{
    int space_left;

    space_left = fsz*1024*1024 - (fm->bootimage_size + fm->bootscr_size +
                                 fm->devicetree_size + fm->kernel_size +
                                 QSPI_BOOTARGS_SIZE);

    if(space_left < 0)
        space_left = 0;

    return space_left;
}

static inline int setup_qspi_args(int flash_sz, char *chip_name)
{
    struct flash_partition_map *fm;
    int boot_off, env_off, kern_off, dtb_off, bscr_off, rfs_off;
    int flash_sz_in_bytes = flash_sz * 1024 * 1024;

    if(flash_sz != 64){
	    printf("Warning: partition layout for flash memmories with capacity different than 64MB is not supported!\n");
	    return 1;
    }

    fm = match_flash_entry(chip_name, flash_sz);
    if(fm){
       boot_off = QSPI_BOOT_OFFSET;
       /* env is at the end of the memory */
       env_off = flash_sz_in_bytes - QSPI_BOOTARGS_SIZE;
       kern_off = boot_off + fm->bootimage_size;
       dtb_off = kern_off + fm->kernel_size;
       bscr_off = dtb_off + fm->devicetree_size;
       rfs_off = bscr_off + fm->bootscr_size;
       if(fm->rootfs_size == 0)
            fm->rootfs_size = calculate_rootfs_size(fm, flash_sz);
       env_set_hex_if_empty("ramdisk_size", fm->rootfs_size);
       env_set_hex_if_empty("jffs2_size", fm->rootfs_size);
       env_set_hex_if_empty("ubifs_size", fm->rootfs_size);
       env_set_hex_if_empty("kernel_size", fm->kernel_size);
       env_set_hex_if_empty("devicetree_size", fm->devicetree_size);
       env_set_hex_if_empty("bootscript_size", fm->bootscr_size);
       env_set_hex_if_empty("bootimage_size", fm->bootimage_size);
       env_set_hex_if_empty("fullboot_size", flash_sz*1024*1024);
       env_set_hex_if_empty("qspi_env_size", QSPI_BOOTARGS_SIZE);
       env_set_hex_if_empty("qspi_bootimage_offset", boot_off);
       env_set_hex_if_empty("qspi_kernel_offset", kern_off);
       env_set_hex_if_empty("qspi_ramdisk_offset", rfs_off);
       env_set_hex_if_empty("qspi_rootfs_offset", rfs_off);
       env_set_hex_if_empty("qspi_devicetree_offset", dtb_off);
       env_set_hex_if_empty("qspi_bootscript_offset", bscr_off);
       env_set_hex_if_empty("qspi_env_offset", env_off);
    }

    return 0;
}
