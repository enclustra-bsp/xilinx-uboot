#include <common.h>
#include <zynqpl.h>
#include <asm/arch/hardware.h>
#include <asm/arch/sys_proto.h>
#include <asm/io.h>

#define SDIO1MUX 0x280

int last_stage_init(void)
{
    /* Set SDIO1 pinctrl */
    zynq_slcr_unlock();
    writel(SDIO1MUX,  &slcr_base->mio_pin[10]); /* Pin 0, SDIO1 */
    writel(SDIO1MUX,  &slcr_base->mio_pin[11]); /* Pin 1, SDIO1 */
    writel(SDIO1MUX,  &slcr_base->mio_pin[12]); /* Pin 2, SDIO1 */
    writel(SDIO1MUX,  &slcr_base->mio_pin[13]); /* Pin 3, SDIO1 */
    writel(SDIO1MUX,  &slcr_base->mio_pin[14]); /* Pin 4, SDIO1 */
    writel(SDIO1MUX,  &slcr_base->mio_pin[15]); /* Pin 5, SDIO1 */
    zynq_slcr_lock();

    return 0;
}
