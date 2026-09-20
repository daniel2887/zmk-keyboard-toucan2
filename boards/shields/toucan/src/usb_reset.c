#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/drivers/uart/cdc_acm.h>
#include <zephyr/usb/usb_device.h>
#include <dt-bindings/zmk/reset.h>

#if defined(CONFIG_CDC_ACM_DTE_RATE_CALLBACK_SUPPORT) && DT_HAS_COMPAT_STATUS_OKAY(zephyr_cdc_acm_uart)

static struct k_work_delayable reset_work;

static void reset_work_handler(struct k_work *work) {
    ARG_UNUSED(work);
    usb_disable();
    k_msleep(50);
    sys_reboot(RST_UF2);
}

static void cdc_acm_rate_cb(const struct device *dev, uint32_t rate) {
    ARG_UNUSED(dev);
    if (rate == 1200) {
        k_work_schedule(&reset_work, K_MSEC(50));
    }
}

static int toucan_usb_reset_init(void) {
    const struct device *dev = DEVICE_DT_GET_ONE(zephyr_cdc_acm_uart);
    if (!device_is_ready(dev)) {
        return -ENODEV;
    }
    k_work_init_delayable(&reset_work, reset_work_handler);
    return cdc_acm_dte_rate_callback_set(dev, cdc_acm_rate_cb);
}

SYS_INIT(toucan_usb_reset_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
#endif
