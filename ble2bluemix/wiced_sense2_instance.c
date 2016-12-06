/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2016.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */
#include "zos.h"
#include "ble_sensor.h"
#include "wiced_sense2.h"
#include "bluemix.h"
#include "wiced_sense2_instance.h"

static zos_bool_t is_ready = ZOS_FALSE;

extern void start_ble_scan( void );
static void ready_callback( ble_sensor_t* sensor );
static void disconnect_callback( ble_sensor_t* sensor );
static void characteristic_data_callback( ble_sensor_characteristic_t* characteristic, ble_sensor_characteristic_data_t* data );
static void characteristic_write_callback( ble_sensor_characteristic_t* characteristic, zos_gatt_status_t status );
static ble_sensor_notification_result_t notification_handler( struct ble_sensor_characteristic* characteristic, uint16_t length, const uint8_t* data );
static void descriptor_data_callback( ble_sensor_descriptor_t* descriptor, ble_sensor_descriptor_data_t* data );
static void descriptor_write_callback( ble_sensor_descriptor_t* descriptor, zos_gatt_status_t status );

/* WICED Sense Service
 * |- Data Characteristic
 *    |- CCC Descriptor (for enabling notification)
 */
BLE_SENSOR_DESCRIPTOR( wiced_sense_ccc, ZOS_BT_UUID_DESCRIPTOR_CCC, descriptor_write_callback, descriptor_data_callback );

BLE_SENSOR_CHARACTERISTIC_START( wiced_sense_data, WICED_SENSE_UUID_CHARACTERISTIC_DATA, characteristic_write_callback, characteristic_data_callback, notification_handler )
    BLE_SENSOR_ADD_DESCRIPTOR( wiced_sense_ccc )
BLE_SENSOR_CHARACTERISTIC_END;

BLE_SENSOR_SERVICE_START( wiced_sense, WICED_SENSE_UUID_SERVICE )
    BLE_SENSOR_ADD_CHARACTERISTIC( wiced_sense_data )
BLE_SENSOR_SERVICE_END;

/* WICED Sense BLE Sensor Definition */
BLE_SENSOR_START( wiced_sense2, ready_callback, disconnect_callback )
    BLE_SENSOR_ADD_SERVICE( wiced_sense )
BLE_SENSOR_END;


static void ready_callback( ble_sensor_t* sensor )
{
    /* Enable data notification */
    ble_sensor_enable_notification( BLE_SENSOR_GET( wiced_sense2 ), BLE_SENSOR_GET_DESCRIPTOR( wiced_sense_ccc ) );
    is_ready = ZOS_TRUE;
    ZOS_LOG( "%s ready and streaming data to Bluemix", WICED_SENSE2_NAME );
}

static void disconnect_callback( ble_sensor_t* sensor )
{
    ZOS_LOG( "%s disconnected", WICED_SENSE2_NAME );
    is_ready = ZOS_FALSE;
    start_ble_scan();
}

static void characteristic_data_callback( ble_sensor_characteristic_t* characteristic, ble_sensor_characteristic_data_t* data )
{
    if ( is_ready == ZOS_FALSE )
    {
        return;
    }

    if ( characteristic == BLE_SENSOR_GET_CHARACTERISTIC( wiced_sense_data ) )
    {
        wiced_sense2_data_t formatted_data;

        /* Publish data to IBM Bluemix */
        wiced_sense2_convert_data( (const wiced_sense2_raw_data_t*)data->value, &formatted_data );
        if ( formatted_data.type == WICED_SENSE2_DATA_1 )
        {
            bluemix_publish_fpi_value( ACCELERATION_X,   (const fpi_word_t*)&formatted_data.data.data1.x_acceleration_g );
            bluemix_publish_fpi_value( ACCELERATION_Y,   (const fpi_word_t*)&formatted_data.data.data1.y_acceleration_g );
            bluemix_publish_fpi_value( ACCELERATION_Z,   (const fpi_word_t*)&formatted_data.data.data1.z_acceleration_g );
        }
        else if ( formatted_data.type == WICED_SENSE2_DATA_2 )
        {
            bluemix_publish_fpi_value( TEMPERATURE, (const fpi_word_t*)&formatted_data.data.data2.temperature_celcius );
            bluemix_publish_fpi_value( HUMIDITY,    (const fpi_word_t*)&formatted_data.data.data2.humidity_percent );
            bluemix_publish_fpi_value( PRESSURE,    (const fpi_word_t*)&formatted_data.data.data2.pressure_hpa );
        }
    }
}

static void characteristic_write_callback( ble_sensor_characteristic_t* characteristic, zos_gatt_status_t status )
{
}

static ble_sensor_notification_result_t notification_handler( struct ble_sensor_characteristic* characteristic, uint16_t length, const uint8_t* data )
{
    if ( is_ready == ZOS_FALSE )
    {
        return BLE_SENSOR_DISCARD_NOTIFICATION;
    }
    else
    {
        /* Defer notification processing to characteristic_receive_callback() */
        return BLE_SENSOR_PROCESS_NOTIFICATION;
    }
}

static void descriptor_data_callback( ble_sensor_descriptor_t* descriptor, ble_sensor_descriptor_data_t* data )
{
}

static void descriptor_write_callback( ble_sensor_descriptor_t* descriptor, zos_gatt_status_t status )
{
}
