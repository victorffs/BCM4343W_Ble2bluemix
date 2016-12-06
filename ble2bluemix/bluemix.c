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
#include "platform_common.h"
#include "commands.h"

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

static zos_result_t mqtt_connection_event_cb( mqtt_event_info_t* event );
static void mqtt_app_connect  ( void* arg );
static void mqtt_app_subscribe( void* arg );
static void mqtt_app_publish  ( void* arg );

/******************************************************
 *               Variable Definitions
 ******************************************************/
static mqtt_connection_t* mqtt_connection = NULL;
static char pub_topic[MAX_TOPIC_STRING_SIZE];
static char sub_topic[MAX_TOPIC_STRING_SIZE];
static char host_url[MAX_HOST_URL_STRING_SIZE];
static char mqtt_message[MAX_MESSAGE_STRING_SIZE];
static char client[MAX_CLIENT_STRING_SIZE];
static char username[MAX_USER_STRING_SIZE];
static mqtt_callback_t callback = mqtt_connection_event_cb;

/******************************************************
 *               Function Definitions
 ******************************************************/

void bluemix_init( void )
{
    ble2bluemix_settings_t* settings;

    /* Memory allocated for MQTT object*/
    zn_malloc((uint8_t**)&mqtt_connection, sizeof(mqtt_connection_t));
    if ( mqtt_connection == NULL )
    {
        ZOS_LOG("Failed to allocate MQTT object...\n");
        return;
    }

    /* Initialize MQTT library */
    if ( mqtt_init( mqtt_connection ) != ZOS_SUCCESS )
    {
        ZOS_LOG("Error initializing");
        zn_free(mqtt_connection);
        mqtt_connection = NULL;
        return;
    }

    ZOS_NVM_GET_REF( settings );

    /* MQTT settings */
    sprintf(pub_topic, MQTT_PUB_TOPIC);
    sprintf(sub_topic, MQTT_SUB_TOPIC);
    sprintf(username, MQTT_USERNAME);
    sprintf(host_url , MQTT_HOST_URL, settings->org_id);
    sprintf(client, MQTT_CLIENT_ID, settings->org_id, settings->device_type, settings->device_id);

    /* Connect to IBM Bluemix broker */
    mqtt_app_connect( NULL );
}

void bluemix_deinit( void )
{
    /* Free MQTT object and deinit library */
    mqtt_deinit( mqtt_connection );
    zn_free(mqtt_connection);
    mqtt_connection = NULL;
}

/*************************************************************************************************/
/*
 * Connect to broker (sent connect frame)
 */
void mqtt_app_connect( void *arg )
{
    ble2bluemix_settings_t* settings;
    mqtt_pkt_connect_t conninfo;
    zos_result_t ret = ZOS_SUCCESS;

    ZOS_LOG("Connecting to '%s:%u'", host_url, MQTT_PORT);
    ret = mqtt_open( mqtt_connection, host_url, MQTT_PORT, ZOS_WLAN, callback, MQTT_SECURITY );
    if ( ret != ZOS_SUCCESS )
    {
        ZOS_LOG("Error opening connection (keys and certificates are set properly?)");
        return;
    }

    /* Now, after socket is connected we can send the CONNECT frame safely */
    memset( &conninfo, 0, sizeof( conninfo ) );

    ZOS_NVM_GET_REF( settings );

    conninfo.mqtt_version = MQTT_PROTOCOL_VER4;
    conninfo.clean_session = 1;
    conninfo.client_id = (uint8_t*)client;
    conninfo.keep_alive = MQTT_KEEPALIVE;
    conninfo.password = strlen((char*)settings->device_token) > 0 ? settings->device_token : NULL;
    conninfo.username = (uint8_t*)username;
    ret = mqtt_connect( mqtt_connection, &conninfo );
    if ( ret != ZOS_SUCCESS )
    {
        ZOS_LOG("Error connecting");
    }
}


/*************************************************************************************************/
/*
 * Subscribe to topic
 */
void mqtt_app_subscribe( void *arg )
{
    mqtt_msgid_t pktid;
    ZOS_LOG("Subscribing to topic '%s'", sub_topic);
    pktid = mqtt_subscribe( mqtt_connection, (uint8_t*)sub_topic, MQTT_QOS );
    if ( pktid == 0 )
    {
        ZOS_LOG("Error subscribing...");
    }
}


/*************************************************************************************************/
/*
 * Publish (send) message to topic
 */
void mqtt_app_publish( void *arg )
{
    mqtt_msgid_t pktid;
//    ZOS_LOG("Publishing to topic: '%s', message: '%s'", pub_topic, mqtt_message);
    pktid = mqtt_publish( mqtt_connection, (uint8_t*)pub_topic, (uint8_t*)mqtt_message, (uint32_t)strlen(mqtt_message), MQTT_QOS );

    if ( pktid == 0 )
    {
//        ZOS_LOG("Error publishing...");
    }
}

/******************************************************
 *               Static Function Definitions
 ******************************************************/
/*
 * Call back function to handle connection events.
 */
static zos_result_t mqtt_connection_event_cb( mqtt_event_info_t *event )
{
    ble2bluemix_settings_t* settings;
    ZOS_NVM_GET_REF( settings );

    switch ( event->type )
    {
        case MQTT_EVENT_TYPE_CONNECTED:
            ZOS_LOG("CONNECTED" );
            mqtt_app_subscribe( NULL );
            break;
        case MQTT_EVENT_TYPE_DISCONNECTED:
            ZOS_LOG("DISCONNECTED" );
            mqtt_app_connect( NULL );
            break;
        case MQTT_EVENT_TYPE_PUBLISHED:
//            ZOS_LOG("PUBLISHED" );
            break;
        case MQTT_EVENT_TYPE_SUBCRIBED:
            ZOS_LOG("TOPIC SUBSCRIBED" );
            ZOS_LOG("Go to your Node-RED application to control your device:");
            ZOS_LOG("Node-RED application: <app_name>.mybluemix.net/red/\r\n", settings->org_id);

            break;
        case MQTT_EVENT_TYPE_UNSUBSCRIBED:
            ZOS_LOG("TOPIC UNSUBSCRIBED" );
            break;
        case MQTT_EVENT_TYPE_PUBLISH_MSG_RECEIVED:
        {
            mqtt_topic_msg_t msg = event->data.pub_recvd;
            ZOS_LOG("MESSAGE RECEIVED");

            ZOS_LOG("----------------------------");
            ZOS_LOG("Topic   : %.*s", msg.topic_len, msg.topic );
            ZOS_LOG("Message : %.*s", msg.data_len, msg.data);
            ZOS_LOG("----------------------------");
        }
            break;
        default:
            break;
    }
    return ZOS_SUCCESS;
}

void bluemix_publish_bool_value( const char* message, zos_bool_t value )
{
    memset( mqtt_message, 0, sizeof( mqtt_message ) );
    sprintf((char*)mqtt_message, message, ( value == ZOS_TRUE ) ? "true" : "false" );
    mqtt_app_publish(NULL);
}

void bluemix_publish_fpi_value( const char* message, const fpi_word_t* value )
{
    char string_buffer[16] = { 0 };

    memset( mqtt_message, 0, sizeof( mqtt_message ) );
    fpi_to_str(string_buffer, (fpi_word_t*)value );
    sprintf((char*)mqtt_message, message, string_buffer );
    mqtt_app_publish(NULL);
}
