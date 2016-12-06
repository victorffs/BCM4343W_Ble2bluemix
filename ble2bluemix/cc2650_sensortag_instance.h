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

#define CC2650_SENSORTAG_NAME "CC2650 SensorTag"

/* CC2650 BLE sensor object */
BLE_SENSOR_DECLARE( cc2650 );

/* Temperature sensor attributes */
BLE_SENSOR_DECLARE_SERVICE( temp_sensor );
BLE_SENSOR_DECLARE_CHARACTERISTIC( temp_sensor_config );
BLE_SENSOR_DECLARE_CHARACTERISTIC( temp_sensor_data );
BLE_SENSOR_DECLARE_DESCRIPTOR( temp_sensor_ccc );

/* Motion sensor attributes */
BLE_SENSOR_DECLARE_SERVICE( motion_sensor );
BLE_SENSOR_DECLARE_CHARACTERISTIC( motion_sensor_config );
BLE_SENSOR_DECLARE_CHARACTERISTIC( motion_sensor_data );
BLE_SENSOR_DECLARE_DESCRIPTOR( motion_sensor_ccc );

/* Humidity sensor attributes */
BLE_SENSOR_DECLARE_SERVICE( humidity_sensor );
BLE_SENSOR_DECLARE_CHARACTERISTIC( humidity_sensor_config );
BLE_SENSOR_DECLARE_CHARACTERISTIC( humidity_sensor_data );
BLE_SENSOR_DECLARE_DESCRIPTOR( humidity_sensor_ccc );

/* Optical sensor attributes */
BLE_SENSOR_DECLARE_SERVICE( optical_sensor );
BLE_SENSOR_DECLARE_CHARACTERISTIC( optical_sensor_config );
BLE_SENSOR_DECLARE_CHARACTERISTIC( optical_sensor_data );
BLE_SENSOR_DECLARE_DESCRIPTOR( optical_sensor_ccc );

/* Simple keys attributes */
BLE_SENSOR_DECLARE_SERVICE( simple_keys );
BLE_SENSOR_DECLARE_CHARACTERISTIC( simple_keys_data );
BLE_SENSOR_DECLARE_DESCRIPTOR( simple_keys_ccc );

/* IO line attributes */
BLE_SENSOR_DECLARE_SERVICE( io );
BLE_SENSOR_DECLARE_CHARACTERISTIC( io_data );
BLE_SENSOR_DECLARE_CHARACTERISTIC( io_config );
