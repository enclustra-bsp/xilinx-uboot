/*
 * (C) Copyright 2013 - 2015 Xilinx, Inc.
 *
 * Xilinx Zynq SD Host Controller Interface
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <dm.h>
#include <fdtdec.h>
#include <libfdt.h>
#include <malloc.h>
#include <sdhci.h>

#ifndef CONFIG_ZYNQ_SDHCI_MIN_FREQ
# define CONFIG_ZYNQ_SDHCI_MIN_FREQ	0
#endif

DECLARE_GLOBAL_DATA_PTR;

struct arasan_sdhci_priv {
	struct sdhci_host *host;
	u8 is_emmc;
};

static int arasan_sdhci_probe(struct udevice *dev)
{
	struct mmc_uclass_priv *upriv = dev_get_uclass_priv(dev);
	struct arasan_sdhci_priv *priv = dev_get_priv(dev);
	struct sdhci_host *host;

	host = priv->host;

	host->quirks = SDHCI_QUIRK_WAIT_SEND_CMD |
		       SDHCI_QUIRK_BROKEN_R1B;

#ifdef CONFIG_ZYNQ_HISPD_BROKEN
	host->quirks |= SDHCI_QUIRK_NO_HISPD_BIT;
#endif

	if (priv->is_emmc)
		host->quirks |= SDHCI_QUIRK_EMMC_INIT;

	host->version = sdhci_readw(host, SDHCI_HOST_VERSION);

	add_sdhci(host, CONFIG_ZYNQ_SDHCI_MAX_FREQ,
		  CONFIG_ZYNQ_SDHCI_MIN_FREQ);

	upriv->mmc = host->mmc;
	host->mmc->dev = dev;

	return 0;
}

static int arasan_sdhci_ofdata_to_platdata(struct udevice *dev)
{
	struct arasan_sdhci_priv *priv = dev_get_priv(dev);

	priv->host = calloc(1, sizeof(struct sdhci_host));
	if (priv->host == NULL)
		return -1;

	priv->host->name = dev->name;
	priv->host->ioaddr = (void *)dev_get_addr(dev);

	if (fdt_get_property(gd->fdt_blob, dev->of_offset, "is_emmc", NULL))
		priv->is_emmc = 1;
	else
		priv->is_emmc = 0;

	return 0;
}

static const struct udevice_id arasan_sdhci_ids[] = {
	{ .compatible = "arasan,sdhci-8.9a" },
	{ }
};

U_BOOT_DRIVER(arasan_sdhci_drv) = {
	.name		= "arasan_sdhci",
	.id		= UCLASS_MMC,
	.of_match	= arasan_sdhci_ids,
	.ofdata_to_platdata = arasan_sdhci_ofdata_to_platdata,
	.probe		= arasan_sdhci_probe,
	.priv_auto_alloc_size = sizeof(struct sdhci_host),
};
