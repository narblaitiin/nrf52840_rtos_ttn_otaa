/*
 * Copyright (c) 2025
 * Regis Rousseau
 * Univ Lyon, INSA Lyon, Inria, CITI, EA3720
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_RTC_H
#define APP_RTC_H

//  ========== includes ====================================================================
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/rtc.h>
#include <zephyr/drivers/rtc/maxim_ds3231.h>

//  ========== prototypes ==================================================================
int8_t app_rtc_init(const struct device *dev);
int32_t app_rtc_get_time(const struct device *dev);

#endif /* APP_RTC_H */