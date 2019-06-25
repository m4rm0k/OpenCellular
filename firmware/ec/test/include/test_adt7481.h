/**
 * Copyright (c) 2017-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */
#ifndef _TEST_ADT7481_H
#define _TEST_ADT7481_H

#include "fake/fake_GPIO.h"
#include "fake/fake_I2C.h"
#include "fake/fake_ThreadedISR.h"
#include "inc/devices/adt7481.h"
#include <stdbool.h>
#include <string.h>
#include "unity.h"

#define ADT7481_CONFIG1_SET_VALUE 0x59
#define ADT7481_CONFIG1_VALUE 0x58
#define ADT7481_COVERSION_RATE_CHANNEL_SEL 0x61
#define ADT7481_DEFAULT_VAL 0xFF
#define ADT7481_INVALID_DEV_ID 0x80
#define ADT7481_INVALID_MANF_ID 0x40
#define ADT7481_LOCAL_TEMP_R_VAL 0x63
#define ADT7481_REMOTE_1_TEMP_HIGH_BYTE_R_VAL 0x54
#define ADT7481_REMOTE_1_TEMP_LOW_BYTE_R_VAL 0x65
#define ADT7481_REMOTE_2_TEMP_HIGH_BYTE_R_VAL 0x73
#define ADT7481_REMOTE_2_TEMP_LOW_BYTE_R_VAL 0x67
#define ADT7481_SET_COVERSION_RATE_CHANNEL_SEL 0x62
#define ADT7481_SET_LOCAL_TEMP_LIMIT1 -63
#define ADT7481_SET_LOCAL_TEMP_LIMIT2 -62
#define ADT7481_SET_LOCAL_TEMP_LIMIT3 -61
#define ADT7481_SET_LOCAL_TEMP_LIMIT4 -60
#define ADT7481_STATUS_1_R_VAL 0x63
#define ADT7481_STATUS_2_R_VAL 0x64
#define ADT7481_TEMP_LIMIT1 0x41
#define ADT7481_TEMP_LIMIT2 0x42
#define ADT7481_TEMP_LIMIT3 0x43
#define ADT7481_TEMP_LIMIT4 0x44
#define ADT7481_REMOTE_TEMP_LIMIT 0x62
#define ADT7481_REMOTE_1_TEMP_HIGH_LIMIT_LOW_BYTE_R_VAl 0x1A
#define ADT7481_REMOTE_1_TEMP_HIGH_LIMIT_R_VAL 0x50
#define ADT7481_REMOTE_1_THER_LIMIT_R_VAL 0x85
#define ADT7481_REMOTE_1_TEMP_LOW_LIMIT_LOW_BYTE_R_VAL 0x67
#define ADT7481_REMOTE_1_TEMP_LOW_LIMIT_R_VAL 0x89
#define ADT7481_REMOTE_1_TEMP_OFFSET_HIGH_BYTE_R 0x70
#define ADT7481_REMOTE_1_TEMP_OFFSET_LOW_BYTE_R 0x67
#define ADT7481_REMOTE_2_TEMP_LIMIT 0x90
#define CONF_TEMP_ADT7481_INVALID_PARAM 4
#define POST_DATA_NULL 0x00
#define REG_U8_TO_TEMP(y) (y - 64)
#define REG_U16_TO_TEMP(y) (y - 64)
#define TEMP_TO_REG_U8(x) (x + 64)
#define TEMP_TO_REG_U16(x) ((x + 64) << 8)

typedef enum ADT7481Regs {
    ADT7481_REG_LOCAL_TEMP_R = 0x00,
    ADT7481_REG_REMOTE_1_TEMP_HIGH_BYTE_R,
    ADT7481_REG_STATUS_1_R,
    ADT7481_REG_CONFIGURATION_1_R,
    ADT7481_REG_COVERSION_RATE_CHANNEL_SEL_R,
    ADT7481_REG_LOCAL_TEMP_HIGH_LIMIT_R,
    ADT7481_REG_LOCAL_TEMP_LOW_LIMIT_R,
    ADT7481_REG_REMOTE_1_TEMP_HIGH_LIMIT_R,
    ADT7481_REG_REMOTE_1_TEMP_LOW_LIMIT_R,
    ADT7481_REG_CONFIGURATION_1_W,
    ADT7481_REG_COVERSION_RATE_CHANNEL_SEL_W,
    ADT7481_REG_LOCAL_TEMP_HIGH_LIMIT_W,
    ADT7481_REG_LOCAL_TEMP_LOW_LIMIT_W,
    ADT7481_REG_REMOTE_1_TEMP_HIGH_LIMIT_W,
    ADT7481_REG_REMOTE_1_TEMP_LOW_LIMIT_W,
    ADT7481_REG_ONE_SHOT_W,
    ADT7481_REG_REMOTE_1_TEMP_LOW_BYTE_R,
    ADT7481_REG_REMOTE_1_TEMP_OFFSET_HIGH_BYTE_R,
    ADT7481_REG_REMOTE_1_TEMP_OFFSET_LOW_BYTE_R,
    ADT7481_REG_REMOTE_1_TEMP_HIGH_LIMIT_LOW_BYTE_R,
    ADT7481_REG_REMOTE_1_TEMP_LOW_LIMIT_LOW_BYTE_R,
    ADT7481_REG_REMOTE_1_THERM_LIMIT_R = 0x19,
    ADT7481_REG_LOCAL_THERM_LIMIT_R = 0x20,
    ADT7481_REG_THERM_HYSTERESIS_R,
    ADT7481_REG_CONSECUTIVE_ALERT_R,
    ADT7481_REG_STATUS_2_R,
    ADT7481_REG_CONFIGURATION_2_R,
    ADT7481_REG_REMOTE_2_TEMP_HIGH_BYTE_R = 0x30,
    ADT7481_REG_REMOTE_2_TEMP_HIGH_LIMIT_R,
    ADT7481_REG_REMOTE_2_TEMP_LOW_LIMIT_R,
    ADT7481_REG_REMOTE_2_TEMP_LOW_BYTE_R,
    ADT7481_REG_REMOTE_2_TEMP_OFFSET_HIGH_BYTE_R,
    ADT7481_REG_REMOTE_2_TEMP_OFFSET_LOW_BYTE_R,
    ADT7481_REG_REMOTE_2_TEMP_HIGH_LIMIT_LOW_BYTE_R,
    ADT7481_REG_REMOTE_2_TEMP_LOW_LIMIT_LOW_BYTE_R,
    ADT7481_REG_REMOTE_2_THERM_LIMIT_R = 0x39,
    ADT7481_REG_DEVICE_ID_R = 0x3D,
    ADT7481_REG_MAN_ID_R,
    ADT7481_REG_END = 0xFF,
} ADT7481Regs;

#endif