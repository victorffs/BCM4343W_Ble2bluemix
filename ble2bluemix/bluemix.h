/*
 * ZentriOS SDK LICENSE AGREEMENT | Zentri.com, 2016.
 *
 * Use of source code and/or libraries contained in the ZentriOS SDK is
 * subject to the Zentri Operating System SDK license agreement and
 * applicable open source license agreements.
 *
 */
#pragma once


#include "mqtt_api.h"
#include "zos.h"

#define TEMPERATURE                   "{\"d\":{\"Temperature\": %s}}"
#define HUMIDITY                      "{\"d\":{\"Humidity\": %s}}"
#define PRESSURE                      "{\"d\":{\"Pressure\": %s}}"
#define ACCELERATION_X                "{\"d\":{\"Acceleration X\": %s}}"
#define ACCELERATION_Y                "{\"d\":{\"Acceleration Y\": %s}}"
#define ACCELERATION_Z                "{\"d\":{\"Acceleration Z\": %s}}"
#define ROTATION_X                    "{\"d\":{\"Rotation X\": %s}}"
#define ROTATION_Y                    "{\"d\":{\"Rotation Y\": %s}}"
#define ROTATION_Z                    "{\"d\":{\"Rotation Z\": %s}}"
#define MAGNETIC_FIELD_X              "{\"d\":{\"Magnetic Field X\": %s}}"
#define MAGNETIC_FIELD_Y              "{\"d\":{\"Magnetic Field Y\": %s}}"
#define MAGNETIC_FIELD_Z              "{\"d\":{\"Magnetic Field Z\": %s}}"

#define SETTINGS_MAGIC_NUMBER         0xB121A12AUL

#define MQTT_USERNAME                 "use-token-auth"
#define MQTT_MESSAGE                  "{\"d\":{\"signal strength\": %s, \"temperature\": %s}}"
#define MQTT_PUB_TOPIC                "iot-2/evt/update/fmt/json"
#define MQTT_SUB_TOPIC                "iot-2/cmd/update/fmt/json"
#define MQTT_HOST_URL                 "%s.messaging.internetofthings.ibmcloud.com"
#define MQTT_CLIENT_ID                "d:%s:%s:%s"
#define MQTT_PORT                     1883
#define MQTT_KEEPALIVE                60
#define MQTT_QOS                      MQTT_QOS_DELIVER_AT_MOST_ONCE
#define MQTT_SECURITY                 ZOS_FALSE

#define MAX_TOPIC_STRING_SIZE         100
#define MAX_MESSAGE_STRING_SIZE       100
#define MAX_HOST_URL_STRING_SIZE      100
#define MAX_CLIENT_STRING_SIZE        100
#define MAX_USER_STRING_SIZE          100

#define MAX_ORG_STRING_SIZE           64
#define MAX_DEVICE_ID_STRING_SIZE     64
#define MAX_DEVICE_TYPE_STRING_SIZE   64
#define MAX_DEVICE_TOKEN_STRING_SIZE  32

typedef struct
{
    uint32_t magic;
    uint8_t org_id      [MAX_ORG_STRING_SIZE];
    // if device_id ends with #, the last 4 digits
    // of the WLAN MAC address are appended to it
    uint8_t device_id   [MAX_DEVICE_ID_STRING_SIZE];
    uint8_t device_type [MAX_DEVICE_TYPE_STRING_SIZE];
    uint8_t device_token[MAX_DEVICE_TOKEN_STRING_SIZE];
    zos_bt_address_t sensor_bdaddr;
} ble2bluemix_settings_t;

void bluemix_init  ( void );
void bluemix_deinit( void );
void bluemix_publish_bool_value( const char* message, zos_bool_t value );
void bluemix_publish_fpi_value ( const char* message, const fpi_word_t* value );
