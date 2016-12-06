/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2015.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */
/** @file
 *
 * BLE-to-Bluemix Example Application
 *
 * The application connects to the Bluemix broker:
 * <org_id>.messaging.internetofthings.ibmcloud.com
 * *
 * If the Device ID ends with #, the last 4 digits of the WLAN MAC address are
 * appended to the device ID (example: zentri_# will be converted zentri_abcd)
 *
 * This ZAP is controlled via Node-RED application at:
 * http://<app_name>.mybluemix.net/red/
 *
 */

#include "bluemix.h"
#include "zos.h"
#include "platform_common.h"
#include "ble_sensor.h"
#include "commands.h"

extern void start_ble_scan( void );

void zn_app_init( void )
{
    zos_result_t result;
    ble2bluemix_settings_t* settings;
    zos_bool_t org_id_valid       = ZOS_FALSE;
    zos_bool_t device_token_valid = ZOS_FALSE;
    zos_bool_t device_type_valid  = ZOS_FALSE;
    zos_bool_t device_id_valid    = ZOS_FALSE;

    ZOS_LOG("BLE-to-Bluemix Demo starting ...");

    commands_init( );

    /* Bring up the network interface */
    if (!zn_network_is_up(ZOS_WLAN))
    {
        ZOS_LOG("Network is down - Restarting Network...\r\n");
        if(ZOS_FAILED(result,zn_network_restart(ZOS_WLAN)))
        {
            ZOS_LOG("Failed to restart network: %d\r\n\r\n", result);
            ZOS_LOG("----------------------------------------------------------------");
            ZOS_LOG("This app expects valid network credentials have been configured.");
            ZOS_LOG("Please run the following commands show below and then reboot ...");
            ZOS_LOG("                                                                ");
            ZOS_LOG("> set wlan.ssid <SSID>                                          ");
            ZOS_LOG("> set wlan.passkey <passkey>                                    ");
            ZOS_LOG("----------------------------------------------------------------");
            ZOS_LOG("\r\n                                                            ");
            return;
        }
    }

    ZOS_NVM_GET_REF( settings );

    org_id_valid       = ( strlen( (char*) settings->org_id ) != 0 ) ? ZOS_TRUE : ZOS_FALSE;
    device_token_valid = ( strlen( (char*) settings->device_token ) != 0 ) ? ZOS_TRUE : ZOS_FALSE;
    device_id_valid    = ( strlen( (char*) settings->device_id ) != 0 ) ? ZOS_TRUE : ZOS_FALSE;
    device_type_valid  = ( strlen( (char*) settings->device_type ) != 0 ) ? ZOS_TRUE : ZOS_FALSE;

    if ( org_id_valid == ZOS_FALSE || device_id_valid == ZOS_FALSE || device_token_valid == ZOS_FALSE || device_type_valid == ZOS_FALSE )
    {
        ZOS_LOG("\r\n                                                       ");
        ZOS_LOG("-----------------------------------------------------------");
        ZOS_LOG("This app expects valid bluemix variables.                  ");
        ZOS_LOG("Please run the commands shown below and then reboot ...    ");
        ZOS_LOG("                                                           ");
        if ( org_id_valid == ZOS_FALSE )
        {
           ZOS_LOG("> set bluemix.org_id <org_id>");
        }
        if( device_token_valid == ZOS_FALSE )
        {
           ZOS_LOG("> set bluemix.device_token <device_token>");
        }
        if(device_id_valid == ZOS_FALSE )
        {
           ZOS_LOG("> set bluemix.device_id <device_id>");
        }
        if( device_type_valid == ZOS_FALSE )
        {
           ZOS_LOG("> set bluemix.device_type <device_type>");
        }
        ZOS_LOG("-----------------------------------------------------------");
        ZOS_LOG("\r\n                                                       ");
        return;
    }

    bluemix_init( );

    ZOS_LOG("\r\nIBM Bluemix Demo Application Started");
    ZOS_LOG("  - Organization ID : %s",     settings->org_id);
    ZOS_LOG("  - Device Type     : %s",     settings->device_type);
    ZOS_LOG("  - Device ID       : %s",     settings->device_id);
    ZOS_LOG("  - Device Token    : %s",     settings->device_token);

    ZOS_LOG("Bluemix Dashboard: https://%s.internetofthings.ibmcloud.com/dashboard/\r\n", settings->org_id);

    result = zn_bt_init( );
    if ( result != ZOS_SUCCESS )
    {
        ZOS_LOG( "Failed to init Bluetooth" );
        return;
    }
    ZOS_LOG( "Bluetooth initialized" );

    start_ble_scan( );
}

void zn_app_deinit(void)
{
    bluemix_deinit( );
}

zos_bool_t zn_app_idle( void )
{
    return ZOS_TRUE;
}
