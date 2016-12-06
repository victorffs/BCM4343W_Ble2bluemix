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
#include "bluemix.h"
#include "wiced_sense2.h"
#include "wiced_sense2_instance.h"

#define RESTART_SCAN_PERIOD_MS  ( 10000 )

static zos_result_t find_device_name        ( const zos_ble_scan_result_t* result );
static void         scan_callback           ( const zos_ble_scan_result_t* result );
static void         scan_to_connect_callback( const zos_ble_scan_result_t* result );
static void         restart_scan_callback   ( void* arg );

static uint32_t scan_result_count = 0;

zos_result_t start_ble_scan( void )
{
    ble2bluemix_settings_t* settings;
    zos_ble_scan_result_handler_t callback;

    ZOS_NVM_GET_REF(settings);

    /* Check if BD_ADDR is set. Inform user with list of device address to set */
    if ( memcmp( &settings->sensor_bdaddr, ZOS_ZERO_BT_ADDRESS, sizeof( zos_bt_address_t)) == 0 )
    {
        callback = scan_callback;
        scan_result_count = 0;
        zn_event_register_timed( restart_scan_callback, NULL, RESTART_SCAN_PERIOD_MS, 0 );
        ZOS_LOG( "Sensor BD_ADDR is not set. Run 'set ble2cloud.sensor_bdaddr <BD_ADDR>'" );
    }
    else
    {
        callback = scan_to_connect_callback;
    }

    ZOS_LOG( "Scanning for %s", WICED_SENSE2_NAME );
    if ( zn_ble_start_scan( ZOS_BLE_SCAN_DUPLICATE_FILTER_ENABLED, callback ) != ZOS_SUCCESS )
    {
        ZOS_LOG( "Failed to start BLE scan" );
        return ZOS_ERROR;
    }

    return ZOS_SUCCESS;
}

static zos_result_t find_device_name( const zos_ble_scan_result_t* result )
{
    int8_t  length   = result->adv_data_length;
    uint8_t* current = (uint8_t*)result->adv_data;

    while ( length > 0 )
    {
        zos_ble_adv_data_t* data = (zos_ble_adv_data_t*)current;
        if ( ( data->type == 0x09 ) && ( memcmp( data->data, WICED_SENSE2_NAME, data->length - 1 ) == 0 ) )
        {
            return ZOS_SUCCESS;
        }
        current += (data->length + 1);
        length  -= (data->length + 1);
    }

    return ZOS_NOT_FOUND;
}

static void restart_scan_callback( void* arg )
{
    zn_ble_stop_scan( );
    ZOS_LOG( "Scan complete\r\n" );
    start_ble_scan( );
}

static void scan_callback( const zos_ble_scan_result_t* result )
{
    if ( find_device_name( result ) != ZOS_SUCCESS )
    {
        return;
    }
    if ( scan_result_count == 0 )
    {
        ZOS_LOG( " #  BD_ADDR" );
    }

    ZOS_LOG( "%2u  %02X:%02X:%02X:%02X:%02X:%02X", ++scan_result_count, result->address.address.octet[5], result->address.address.octet[4], result->address.address.octet[3], result->address.address.octet[2], result->address.address.octet[1], result->address.address.octet[0] );
}

static void scan_to_connect_callback( const zos_ble_scan_result_t* result )
{
    zos_result_t return_value;

    ble2bluemix_settings_t* settings;

    ZOS_NVM_GET_REF(settings);

    if ( find_device_name( result ) != ZOS_SUCCESS )
    {
        return;
    }
    if ( memcmp( &result->address.address, &settings->sensor_bdaddr, sizeof(zos_bt_address_t) ) != 0 )
    {
        return;
    }

    ZOS_LOG( "%s (%02X:%02X:%02X:%02X:%02X:%02X) is found. Connecting to sensor", WICED_SENSE2_NAME, result->address.address.octet[5], result->address.address.octet[4], result->address.address.octet[3], result->address.address.octet[2], result->address.address.octet[1], result->address.address.octet[0] );

    zn_ble_stop_scan( );

    return_value = ble_sensor_connect( BLE_SENSOR_GET( wiced_sense2 ), (const zos_ble_address_t*)&result->address );
    if ( return_value == ZOS_SUCCESS || return_value == ZOS_PENDING )
    {
    }
    else
    {
        ZOS_LOG( "Create connection failed" );
        start_ble_scan();
    }
}
