/*
 * Copyright (c) 2025
 * Regis Rousseau
 * Univ Lyon, INSA Lyon, Inria, CITI, EA3720
 * SPDX-License-Identifier: Apache-2.0
 */

//  ========== includes ====================================================================
#include "app_lorawan.h"
#include "app_flash.h"

//  ========== globals =====================================================================
static const struct gpio_dt_spec led_tx = GPIO_DT_SPEC_GET(LED_TX, gpios);
static const struct gpio_dt_spec led_rx = GPIO_DT_SPEC_GET(LED_RX, gpios);

// downlink callback
static void dl_callback(uint8_t port, bool data_pending, int16_t rssi, int8_t snr, uint8_t len, const uint8_t *data)
{
	printk("downlink data received: ");
    for(int8_t i=0; i < len; i++) {
		printk("%02X ", data[i]);
	}
    printk("\n");
	printk("port: %d, pending: %d, RSSI: %ddB, SNR: %dBm\n", port, data_pending, rssi, snr);
}

// ADR change callback
static void lorwan_datarate_changed(enum lorawan_datarate dr)
{
	uint8_t unused, max_size;

	lorawan_get_payload_sizes(&unused, &max_size);
	printk("new datarate: DR_%d, max payload: %d\n", dr, max_size);
}

//  ========== app_loarwan_init ============================================================
int8_t app_lorawan_init(const struct device *dev)
{
	struct lorawan_join_config join_cfg;
	static struct nvs_fs fs;
	uint16_t dev_nonce = 0u;

    int8_t ret = 0;
	int8_t itr = 1;
	ssize_t err = 0;
	uint8_t dev_eui[] 	= LORAWAN_DEV_EUI;
	uint8_t join_eui[]	= LORAWAN_JOIN_EUI;
	uint8_t app_key[]	= LORAWAN_APP_KEY;

	// initialize non-volatile storage (NVS) and read the current dev_nonce value
	app_flash_init(&fs);
	app_flash_init_param(&fs, NVS_DEVNONCE_ID, &dev_nonce);

	printk("starting lorawan node initialization\n");

    // retrieve the LoRa SX1276 device
	dev = DEVICE_DT_GET(DT_ALIAS(lora0));
	if (!device_is_ready(dev)) {
		printk("%s: lorawan device not ready\n", dev->name);
		return 0;
	}

	printk("starting lorawan stack\n");

    // set the region (Europe)
	ret = lorawan_set_region(LORAWAN_REGION_EU868);
	if (ret < 0) {
		printk("failed to start LoRaWAN stack. error: %d\n", ret);
		return 0;
	}

	// indicate device activity by toggling the transmission LED
	gpio_pin_set_dt(&led_tx, 1);

	// start the LoRaWAN stack
	ret = lorawan_start();
	if (ret < 0) {
		printk("failed to start lorawan stack. error: %d\n", ret);
		return 0;
	} else {
			// allow some time for the stack to stabilize
			k_sleep(K_MSEC(500));
	}

	// enable Adaptive Data Rate (ADR) to optimize communication settings
    lorawan_enable_adr(true);

    // register downlink and data rate change callbacks for receiving messages and updates
	struct lorawan_downlink_cb downlink_cb = {
		.port = LW_RECV_PORT_ANY,
		.cb = dl_callback
	};
	lorawan_register_downlink_callback(&downlink_cb);
	lorawan_register_dr_changed_callback(lorwan_datarate_changed);  

	// configuration of lorawan network using OTAA
    join_cfg.mode = LORAWAN_ACT_OTAA;
	join_cfg.dev_eui = dev_eui;
	join_cfg.otaa.join_eui = join_eui;
	join_cfg.otaa.app_key = app_key;
	join_cfg.otaa.nwk_key = app_key;
	join_cfg.otaa.dev_nonce = dev_nonce;

	// attempt to join the LoRaWAN network using OTAA
	do {
		printk("attempting to join LoRaWAN network using OTAA. Dev nonce: %d, attempt: %d\n", join_cfg.otaa.dev_nonce, itr++);

		// indicate receiving activity by toggling the reception LED
		gpio_pin_set_dt(&led_rx, 1);

		ret = lorawan_join(&join_cfg);
		if (ret < 0) {
			if (ret == -ETIMEDOUT) {
				printk("join request timed out. Retrying...\n");
			} else {
				printk("failed to join network. Error: %d\n", ret);
			}
		} else {
			printk("successfully joined LoRaWAN network using OTAA.\n");
		}

		// increment and save the device nonce in NVS for the next attempt
		dev_nonce++;
		join_cfg.otaa.dev_nonce = dev_nonce;

		// save value away in Non-Volatile Storage.
		err = nvs_write(&fs, NVS_DEVNONCE_ID, &dev_nonce, sizeof(dev_nonce));
		if (err < 0) {
			printk("NVS: failed to write dev_nonce (id %d). error: %d\n", NVS_DEVNONCE_ID, err);
		}

		// if the join attempt failed, wait before retrying
		if (ret < 0) {
			// if failed, wait before re-trying.
			k_sleep(K_MSEC(10000));
		}
	} while (ret != 0 && itr < MAX_JOIN_ATTEMPTS);

	// turn off LEDs to indicate the end of the process
	gpio_pin_set_dt(&led_tx, 0);
	gpio_pin_set_dt(&led_rx, 0);
	
    return 0;
}

  
    