/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2016.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */
#include "zos.h"
#include "bluemix.h"
#include "ble_sensor.h"
#include "cc2650_sensortag.h"
#include "cc2650_sensortag_instance.h"

static zos_bool_t is_ready = ZOS_FALSE;

extern void start_ble_scan( void );
static void characteristic_receive_callback( ble_sensor_characteristic_t* characteristic, ble_sensor_characteristic_data_t* data );
static void characteristic_write_callback( ble_sensor_characteristic_t* characteristic, zos_gatt_status_t status );
static ble_sensor_notification_result_t notification_handler( struct ble_sensor_characteristic* characteristic, uint16_t length, const uint8_t* data );
static void descriptor_data_callback( ble_sensor_descriptor_t* descriptor, ble_sensor_descriptor_data_t* data );
static void descriptor_write_callback( ble_sensor_descriptor_t* descriptor, zos_gatt_status_t status );
static void ready_callback( ble_sensor_t* sensor );
static void disconnect_callback( ble_sensor_t* sensor );

/* Temperature Sensor Service
 * |- Data Characteristic
 * |  |- CCC Descriptor (for enabling notification)
 * |- Configuration Characteristic (enable = 0x01, disable = 0x00)
 */
BLE_SENSOR_DESCRIPTOR( temp_sensor_ccc, ZOS_BT_UUID_DESCRIPTOR_CCC, descriptor_write_callback, descriptor_data_callback );

BLE_SENSOR_CHARACTERISTIC( temp_sensor_config, CC2650_UUID_CHARACTERISTIC_TEMP_SENSOR_CONFIG, characteristic_write_callback, characteristic_receive_callback, notification_handler );

BLE_SENSOR_CHARACTERISTIC_START( temp_sensor_data, CC2650_UUID_CHARACTERISTIC_TEMP_SENSOR_DATA, characteristic_write_callback, characteristic_receive_callback, notification_handler )
    BLE_SENSOR_ADD_DESCRIPTOR( temp_sensor_ccc )
BLE_SENSOR_CHARACTERISTIC_END;

BLE_SENSOR_SERVICE_START( temp_sensor, CC2650_UUID_SERVICE_TEMP_SENSOR )
    BLE_SENSOR_ADD_CHARACTERISTIC( temp_sensor_data )
    BLE_SENSOR_ADD_CHARACTERISTIC( temp_sensor_config )
BLE_SENSOR_SERVICE_END;

/* Motion Sensor Service
 * |- Data characteristic
 * |  |- CCC Descriptor (for enabling notification)
 * |- Configuration Characteristic
 */
BLE_SENSOR_DESCRIPTOR( motion_sensor_ccc, ZOS_BT_UUID_DESCRIPTOR_CCC, descriptor_write_callback, descriptor_data_callback );

BLE_SENSOR_CHARACTERISTIC( motion_sensor_config, CC2650_UUID_CHARACTERISTIC_MOTION_SENSOR_CONFIG, characteristic_write_callback, characteristic_receive_callback, notification_handler );

BLE_SENSOR_CHARACTERISTIC_START( motion_sensor_data, CC2650_UUID_CHARACTERISTIC_MOTION_SENSOR_DATA, characteristic_write_callback, characteristic_receive_callback, notification_handler )
    BLE_SENSOR_ADD_DESCRIPTOR( motion_sensor_ccc )
BLE_SENSOR_CHARACTERISTIC_END;

BLE_SENSOR_SERVICE_START( motion_sensor, CC2650_UUID_SERVICE_MOTION_SENSOR )
    BLE_SENSOR_ADD_CHARACTERISTIC( motion_sensor_data )
    BLE_SENSOR_ADD_CHARACTERISTIC( motion_sensor_config )
BLE_SENSOR_SERVICE_END;

/* Humidity Sensor Service
 * |- Data Characteristic
 * |  |- CCC Descriptor (for enabling notification)
 * |- Configuration Characteristic
 */
BLE_SENSOR_DESCRIPTOR( humidity_sensor_ccc, ZOS_BT_UUID_DESCRIPTOR_CCC, descriptor_write_callback, descriptor_data_callback );

BLE_SENSOR_CHARACTERISTIC( humidity_sensor_config, CC2650_UUID_CHARACTERISTIC_HUMIDITY_SENSOR_CONFIG, characteristic_write_callback, characteristic_receive_callback, notification_handler );

BLE_SENSOR_CHARACTERISTIC_START( humidity_sensor_data, CC2650_UUID_CHARACTERISTIC_HUMIDITY_SENSOR_DATA , characteristic_write_callback, characteristic_receive_callback, notification_handler )
    BLE_SENSOR_ADD_DESCRIPTOR( humidity_sensor_ccc )
BLE_SENSOR_CHARACTERISTIC_END;

BLE_SENSOR_SERVICE_START( humidity_sensor, CC2650_UUID_SERVICE_HUMIDITY_SENSOR )
    BLE_SENSOR_ADD_CHARACTERISTIC( humidity_sensor_data )
    BLE_SENSOR_ADD_CHARACTERISTIC( humidity_sensor_config )
BLE_SENSOR_SERVICE_END;

/* Optical Sensor Service
 * |- Data Characteristic
 * |  |- CCC Descriptor (for enabling notification)
 * |- Configuration Characteristic
 */
BLE_SENSOR_DESCRIPTOR( optical_sensor_ccc, ZOS_BT_UUID_DESCRIPTOR_CCC, descriptor_write_callback, descriptor_data_callback );

BLE_SENSOR_CHARACTERISTIC( optical_sensor_config, CC2650_UUID_CHARACTERISTIC_OPTICAL_SENSOR_CONFIG, characteristic_write_callback, characteristic_receive_callback, notification_handler );

BLE_SENSOR_CHARACTERISTIC_START( optical_sensor_data, CC2650_UUID_CHARACTERISTIC_OPTICAL_SENSOR_DATA, characteristic_write_callback, characteristic_receive_callback, notification_handler )
    BLE_SENSOR_ADD_DESCRIPTOR( optical_sensor_ccc )
BLE_SENSOR_CHARACTERISTIC_END;

BLE_SENSOR_SERVICE_START( optical_sensor, CC2650_UUID_SERVICE_OPTICAL_SENSOR )
    BLE_SENSOR_ADD_CHARACTERISTIC( optical_sensor_data )
    BLE_SENSOR_ADD_CHARACTERISTIC( optical_sensor_config )
BLE_SENSOR_SERVICE_END;

/* Simple Keys (Buttons) Service
 * |- Data Characteristic
 *    |- CCC Descriptor (for enabling notification)
 */
BLE_SENSOR_DESCRIPTOR( simple_keys_ccc, ZOS_BT_UUID_DESCRIPTOR_CCC, descriptor_write_callback, descriptor_data_callback );

BLE_SENSOR_CHARACTERISTIC_START( simple_keys_data, CC2650_UUID_CHARACTERISTIC_SIMPLE_KEYS_DATA, characteristic_write_callback, characteristic_receive_callback, notification_handler )
    BLE_SENSOR_ADD_DESCRIPTOR( simple_keys_ccc )
BLE_SENSOR_CHARACTERISTIC_END;

BLE_SENSOR_SERVICE_START( simple_keys, CC2650_UUID_SERVICE_SIMPLE_KEYS )
    BLE_SENSOR_ADD_CHARACTERISTIC( simple_keys_data )
BLE_SENSOR_SERVICE_END;

/* IO Service
 * |- Data Characteristic
 * |- Configuration Characteristic
 */
BLE_SENSOR_CHARACTERISTIC( io_data, CC2650_UUID_CHARACTERISTIC_IO_DATA, characteristic_write_callback, characteristic_receive_callback, notification_handler );
BLE_SENSOR_CHARACTERISTIC( io_config, CC2650_UUID_CHARACTERISTIC_IO_CONFIG, characteristic_write_callback, characteristic_receive_callback, notification_handler );

BLE_SENSOR_SERVICE_START( io, CC2650_UUID_SERVICE_IO )
    BLE_SENSOR_ADD_CHARACTERISTIC( io_data )
    BLE_SENSOR_ADD_CHARACTERISTIC( io_config )
BLE_SENSOR_SERVICE_END;

/* CC2650 BLE Sensor Definition */
BLE_SENSOR_START( cc2650, ready_callback, disconnect_callback )
    BLE_SENSOR_ADD_SERVICE( temp_sensor )
    BLE_SENSOR_ADD_SERVICE( motion_sensor )
    BLE_SENSOR_ADD_SERVICE( humidity_sensor )
    BLE_SENSOR_ADD_SERVICE( optical_sensor )
    BLE_SENSOR_ADD_SERVICE( simple_keys )
    BLE_SENSOR_ADD_SERVICE( io )
BLE_SENSOR_END;

static void characteristic_receive_callback( ble_sensor_characteristic_t* characteristic, ble_sensor_characteristic_data_t* data )
{
    ZOS_LOG("Characteristic[%d,%d,%d,%d]", data->group_handle.start, data->group_handle.end, data->value_handle, data->length );
}

static void characteristic_write_callback( ble_sensor_characteristic_t* characteristic, zos_gatt_status_t status )
{
    ZOS_LOG("char write[%d,%d]", characteristic->data->value_handle, status );
}

static ble_sensor_notification_result_t notification_handler( struct ble_sensor_characteristic* characteristic, uint16_t length, const uint8_t* data )
{
    ZOS_LOG("Notification[%d,%d]", characteristic->data->value_handle, length );
    return BLE_SENSOR_PROCESS_NOTIFICATION;
}

static void descriptor_data_callback( ble_sensor_descriptor_t* descriptor, ble_sensor_descriptor_data_t* data )
{
    ZOS_LOG("desc[%d,%d]", data->handle, data->length );
}

static void descriptor_write_callback( ble_sensor_descriptor_t* descriptor, zos_gatt_status_t status )
{
    ZOS_LOG("desc write[%d,%d]", descriptor->data->handle, status );
}

static void ready_callback( ble_sensor_t* sensor )
{
    ZOS_LOG( "CC2650 Ready" );

    ble_sensor_enable_notification( BLE_SENSOR_GET( cc2650 ), BLE_SENSOR_GET_DESCRIPTOR( simple_keys_ccc ) );
}

static void disconnect_callback( ble_sensor_t* sensor )
{
    ZOS_LOG( "CC2650 disconnected" );
    cc2650_start_scan();
}
