/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2016.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */
#include "zos.h"
#include "commands.h"
#include "bluemix.h"

/*************************************************
 * Default Settings
 *
 * These settings are stored in the 'Read-Only Memory'
 * This memory does NOT get loaded into RAM and resides
 * on the serial flash.
 *
 * To retrieve these settings, use the zn_load_ro_memory() API.
 */
RO_MEM ble2bluemix_settings_t default_settings =
{
        .magic          = SETTINGS_MAGIC_NUMBER,
        .org_id         = "",
        .device_token   = "",
        .device_id      = "zentri_#",
        .device_type    = "ZentriDevice",
        .sensor_bdaddr.octet = { 0 },
};

/*************************************************************************************************
 * Getters List
 *************************************************************************************************/
ZOS_GETTERS_START(mqtt)
    ZOS_ADD_GETTER("bluemix.org_id",        mqtt_org_id),
    ZOS_ADD_GETTER("bluemix.device_token",  mqtt_device_token),
    ZOS_ADD_GETTER("bluemix.device_id",     mqtt_device_id),
    ZOS_ADD_GETTER("bluemix.device_type",   mqtt_device_type),
ZOS_GETTERS_END

ZOS_GETTERS_START(ble2cloud)
    ZOS_ADD_GETTER("ble2cloud.sensor_bdaddr", ble2cloud_sensor_bdaddr),
ZOS_GETTERS_END

/*************************************************************************************************
 * Setters List
 *************************************************************************************************/
ZOS_SETTERS_START(mqtt)
    ZOS_ADD_SETTER("bluemix.org_id",        mqtt_org_id),
    ZOS_ADD_SETTER("bluemix.device_token",  mqtt_device_token),
    ZOS_ADD_SETTER("bluemix.device_id",     mqtt_device_id),
    ZOS_ADD_SETTER("bluemix.device_type",   mqtt_device_type),
ZOS_SETTERS_END

ZOS_SETTERS_START(ble2cloud)
    ZOS_ADD_SETTER("ble2cloud.sensor_bdaddr", ble2cloud_sensor_bdaddr),
ZOS_SETTERS_END

/*************************************************************************************************
 * Commands List
 *************************************************************************************************/
ZOS_COMMANDS_START(mqtt)
ZOS_COMMANDS_END

ZOS_COMMAND_LISTS(mqtt);

ZOS_COMMANDS_START(ble2cloud)
ZOS_COMMANDS_END

ZOS_COMMAND_LISTS(ble2cloud);

/*************************************************************************************************/
void commands_init(void)
{
    ble2bluemix_settings_t *settings;

    ZOS_CMD_REGISTER_COMMANDS(mqtt);

    ZOS_NVM_GET_REF(settings);

    // check that app_settings_t hasn't overflowed the NVM
    // if the build fails here then app_settings_t is too large to fit into the NVM
    // you need to make this struct smaller
    BUILD_CHECK_NVM_SIZE(ble2bluemix_settings_t);

    // if the nvm settings haven't been initialized, do it now
    if(settings->magic != SETTINGS_MAGIC_NUMBER)
    {
        if(zn_load_ro_memory(settings, sizeof(ble2bluemix_settings_t), &default_settings, 0) != ZOS_SUCCESS)
        {
            ZOS_LOG("Failed to loaded default settings");
        }
        else
        {
            // this is done once at first init, then we depend on setter to convert
            convert_device_id((char*)settings->device_id);
        }
    }

    ZOS_CMD_REGISTER_COMMANDS(ble2cloud);
}

/*************************************************************************************************/
void commands_deinit(void)
{
    ZOS_CMD_UNREGISTER_COMMANDS(mqtt);
    ZOS_CMD_UNREGISTER_COMMANDS(ble2cloud);
}

/*************************************************************************************************/
static void convert_device_id(char *device)
{
    char *name_end = &device[strlen(device)-1];

    if(*name_end == '#')
    {
        char mac_str[32] = {0};

        zn_network_get_mac(mac_str);
        mac_str[14] = mac_str[13];
        mac_str[13] = mac_str[12];
        strcpy(name_end, &mac_str[13]);
    }
}

/*************************************************************************************************
 * Getters
 *************************************************************************************************/
ZOS_DEFINE_GETTER(mqtt_org_id)
{
    ble2bluemix_settings_t *settings;
    ZOS_NVM_GET_REF(settings);
    zn_cmd_format_response(CMD_SUCCESS, "%s", settings->org_id);
    return CMD_SUCCESS;
}

/*************************************************************************************************/
ZOS_DEFINE_GETTER(mqtt_device_token)
{
    ble2bluemix_settings_t *settings;
    ZOS_NVM_GET_REF(settings);
    zn_cmd_format_response(CMD_SUCCESS, "%s", settings->device_token);
    return CMD_SUCCESS;
}

/*************************************************************************************************/
ZOS_DEFINE_GETTER(mqtt_device_id)
{
    ble2bluemix_settings_t *settings;
    ZOS_NVM_GET_REF(settings);
    zn_cmd_format_response(CMD_SUCCESS, "%s", settings->device_id);
    return CMD_SUCCESS;
}

/*************************************************************************************************/
ZOS_DEFINE_GETTER(mqtt_device_type)
{
    ble2bluemix_settings_t *settings;
    ZOS_NVM_GET_REF(settings);
    zn_cmd_format_response(CMD_SUCCESS, "%s", settings->device_type);
    return CMD_SUCCESS;
}

/*************************************************************************************************/
ZOS_DEFINE_GETTER(ble2cloud_sensor_bdaddr)
{
    ble2bluemix_settings_t *settings;

    ZOS_NVM_GET_REF(settings);

    zn_cmd_format_response( CMD_SUCCESS, "%02X:%02X:%02X:%02X:%02X:%02X",
                            settings->sensor_bdaddr.octet[5],
                            settings->sensor_bdaddr.octet[4],
                            settings->sensor_bdaddr.octet[3],
                            settings->sensor_bdaddr.octet[2],
                            settings->sensor_bdaddr.octet[1],
                            settings->sensor_bdaddr.octet[0] );

    return CMD_SUCCESS;
}

/*************************************************************************************************
 * Setters
 *************************************************************************************************/
ZOS_DEFINE_SETTER(mqtt_org_id)
{
    ble2bluemix_settings_t *settings;

    if(strlen(argv[1]) > sizeof(settings->org_id))
    {
        ZOS_LOG("Failed (maximum length is %u)", sizeof(settings->org_id));
        return CMD_BAD_ARGS;
    }
    else
    {
        ZOS_NVM_GET_REF(settings);
        strcpy((char*)settings->org_id, argv[1]);
        return CMD_SET_OK;
    }
}

/*************************************************************************************************/
ZOS_DEFINE_SETTER(mqtt_device_token)
{
    ble2bluemix_settings_t *settings;

    if(strlen(argv[1]) > sizeof(settings->device_token))
    {
        ZOS_LOG("Failed (maximum length is %u)", sizeof(settings->device_token));
        return CMD_BAD_ARGS;
    }
    else
    {
        ZOS_NVM_GET_REF(settings);
        strcpy((char*)settings->device_token, argv[1]);
        return CMD_SET_OK;
    }
}

/*************************************************************************************************/
ZOS_DEFINE_SETTER(mqtt_device_id)
{
    ble2bluemix_settings_t *settings;

    if(strlen(argv[1]) > (sizeof(settings->device_id) - 4))
    {
        ZOS_LOG("Failed (maximum length is %u)", sizeof(settings->device_id));
        return CMD_BAD_ARGS;
    }
    else
    {
        ZOS_NVM_GET_REF(settings);
        strcpy((char*)settings->device_id, argv[1]);
        convert_device_id((char*)settings->device_id);
        return CMD_SET_OK;
    }
}

/*************************************************************************************************/
ZOS_DEFINE_SETTER(mqtt_device_type)
{
    ble2bluemix_settings_t *settings;

    if(strlen(argv[1]) > sizeof(settings->device_type))
    {
        ZOS_LOG("Failed (maximum length is %u)", sizeof(settings->device_type));
        return CMD_BAD_ARGS;
    }
    else
    {
        ZOS_NVM_GET_REF(settings);
        strcpy((char*)settings->device_type, argv[1]);
        return CMD_SET_OK;
    }
}

/*************************************************************************************************/
ZOS_DEFINE_SETTER(ble2cloud_sensor_bdaddr)
{
    ble2bluemix_settings_t *settings;
    zos_bt_address_t bdaddr;

    UNUSED_PARAMETER(argc);

    if ( str_to_mac( argv[1], (zos_mac_t*)&bdaddr ) != ZOS_SUCCESS )
    {
        return CMD_BAD_ARGS;
    }

    ZOS_NVM_GET_REF(settings);

    /* swap orders */
    settings->sensor_bdaddr.octet[5] = bdaddr.octet[0];
    settings->sensor_bdaddr.octet[4] = bdaddr.octet[1];
    settings->sensor_bdaddr.octet[3] = bdaddr.octet[2];
    settings->sensor_bdaddr.octet[2] = bdaddr.octet[3];
    settings->sensor_bdaddr.octet[1] = bdaddr.octet[4];
    settings->sensor_bdaddr.octet[0] = bdaddr.octet[5];
    return CMD_SET_OK;
}
