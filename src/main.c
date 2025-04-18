/*
 * Copyright (c) 2025
 * Regis Rousseau
 * Univ Lyon, INSA Lyon, Inria, CITI, EA3720
 * SPDX-License-Identifier: Apache-2.0
 */

//  ========== includes ====================================================================
#include "app_lorawan.h"

//  ========== globals =====================================================================
// define GPIO specifications for the LEDs used to indicate transmission (TX) and reception (RX)
static const struct gpio_dt_spec led_tx = GPIO_DT_SPEC_GET(LED_TX, gpios);
static const struct gpio_dt_spec led_rx = GPIO_DT_SPEC_GET(LED_RX, gpios);

//  ========== main ========================================================================
int8_t main(void)
{
	const struct device *dev;
	uint8_t payload[PAYLOAD_SIZE] = {0};
    int8_t ret = 0;

	// structure to hold simulated sensor data
	struct data_t {
		int16_t vbat;  // battery voltage in millivolts
		int16_t temp;  // temperature in tenths of a degree Celsius
		int16_t hum;   // humidity as a percentage
		int16_t vadc;  // ADC voltage in millivolts
	} data;

	// configure LEDs for TX and RX indication
	gpio_pin_configure_dt(&led_tx, GPIO_OUTPUT_ACTIVE);
	gpio_pin_configure_dt(&led_rx, GPIO_OUTPUT_ACTIVE);
	gpio_pin_set_dt(&led_tx, 0);		// turn off TX LED
	gpio_pin_set_dt(&led_rx, 0);		// turn off TX LED

	// initialize LoRaWAN protocol and register the device
	app_lorawan_init(dev);

	printk("Geophone Measurement Simulation and Process Information\nBoard: %s\n", CONFIG_BOARD);
	
	// start the main loop for data simulation and transmission
	while (1) {

		// generate random simulated sensor data
		data.vbat = sys_rand16_get() % 101;            // battery voltage: 0-100
		data.temp = sys_rand16_get() % 201 - 100;      // temperature: -100 to 100
		data.hum = sys_rand16_get() % 101;             // humidity: 0-100
		data.vadc = sys_rand16_get() % 3301;           // ADC voltage: 0-3300 mV
		
		// encode sensor data into the payload
        payload[0] = (data.vbat >> 8) & 0xFF;
        payload[1] = data.vbat & 0xFF;

        payload[2] = (data.temp >> 8) & 0xFF;
        payload[3] = data.temp & 0xFF;

        payload[4] = (data.hum >> 8) & 0xFF;
        payload[5] = data.hum & 0xFF;

        payload[6] = (data.vadc >> 8) & 0xFF;
        payload[7] = data.vadc & 0xFF;

		// indicate data transmission with the TX LED
		printk("sending random data...\n");
		gpio_pin_set_dt(&led_tx, 1);

		// send the payload over LoRaWAN (unconfirmed message)
		ret = lorawan_send(LORAWAN_PORT, payload, PAYLOAD_SIZE, LORAWAN_MSG_UNCONFIRMED);
		
		// handle transmission errors
		if (ret == -EAGAIN) {
			printk("LoRaWAN send failed (retry): %d. continuing...\n", ret);
			k_sleep(K_SECONDS(10));
			continue;
		}
		
		if (ret < 0) {
			printk("LoRaWAN send failed: %d\n", ret);
			return(0);
		}

		printk("data sent successfully!\n");
		gpio_pin_set_dt(&led_tx, 0);

		// wait before the next iteration
		k_sleep(DELAY);
	}
	return 0;
}