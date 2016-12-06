/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */
#pragma once
#include "zos.h"
#include "ble_sensor.h"

#define WICED_SENSE2_NAME "WICED Sense2 Kit"

/* WICEDSense2 BLE sensor object */
BLE_SENSOR_DECLARE( wiced_sense2 );

/* WICED sense attributes */
BLE_SENSOR_DECLARE_SERVICE( wiced_sense );
BLE_SENSOR_DECLARE_CHARACTERISTIC( wiced_sense_data );
BLE_SENSOR_DECLARE_DESCRIPTOR( wiced_sense_ccc );
