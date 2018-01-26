/*
  Easy IoT (Ethernet) to Scalenics

  This sketch connects to a IoT cloud "Scalenics" (https://api.scalenics.io/console/)
  using an EnOcean Shield (TCM410J) by SiMICS and Arduino Ethernet Shield 2.

  The circuit:
  *Input PIN
    RX:EnOcean (TCM410J)
    ICSP 1pin:MISO
  *Output PIN
    ICSP 3pin:SCK
    ICSP 4pin:MOSI
    D10:SS(W5500)
    D4 :SS(SD card)

  Created 1 May 2016
  by LoonaiFactory

  https://github.com/simics-co/EnOcean
*/

#include <SPI.h>
#include <Ethernet2.h>
#include <PubSubClient.h>
#include "ESP3Parser.h"
#include "EnOceanProfile.h"

/* Device ID */
#define SEND_DEVICE_SW1 0x00200000  // Rocker Switch (EEP F6-02-04)
#define SEND_DEVICE_CN1 0x04000000  // Contact Sensor (EEP D5-00-01)
#define SEND_DEVICE_TM1 0x04000001  // Temperature Sensor (EEP A5-02-30)
#define SEND_DEVICE_TM2 0x04000002  // Temperature Sensor (EEP A5-02-05)
#define SEND_DEVICE_OC1 0x04000003  // Occupancy Sensor (EEP A5-07-01)
#define SEND_DEVICE_CO1 0x04000004  // CO2 Sensor (EEP A5-09-04)

#define BUFF_SIZE 15
typedef struct {
  uint32_t ID;
  uint32_t data;
  uint8_t  rssi;
} StoreDataType;
StoreDataType storeDataSet[BUFF_SIZE];

uint8_t bfWritePoint;
uint8_t bfReadPoint;
bool isDataAvailable;

// Connection info data lengths
#define IP_ADDR_LEN     4   // Length of IP address in bytes

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0x90, 0xA2, 0xDA, 0x10, 0xXX, 0xXX };

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 0, 1);

// Scalenics
#define SC_USER "YOUR_SCALENICS_ACCOUNT"
#define DEVICE_TOKEN "YOUR_DEVICE_TOKEN_HERE"
#define CLIENT_ID "enocean"
#define MQTT_SERVER "api.scalenics.io"

EthernetClient client;

// MQTT client
PubSubClient mqttClient(MQTT_SERVER, 1883, NULL, client);

static void connect(void);
static void storeData(uint8_t rorg, uint32_t ID, uint32_t data, uint8_t rssi);
static void getStoreData(void);

ESP3Parser parser(storeData);
EnOceanProfile profile;


void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(57600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  bfWritePoint = 0;
  bfReadPoint = 0;
  isDataAvailable = false;

  connect();
  parser.initialization();
}

#define TIME_DELAY 50  /* msec */
uint32_t getID, getData;
uint8_t getRssi;
String topic;
String postData;
char mqtt_topic[64];
char mqtt_payload[32];

char deviceID[10];
char state[10];
char hmd[10];
char temp[10];
char rssi[5];

void loop()
{
  delay(TIME_DELAY);
  
  if(isDataAvailable == true) {
    noInterrupts();	/* Disable interrupt */
    getStoreData();
    interrupts();	/* Enable interrupt */
    
    topic = DEVICE_TOKEN;
    topic += "/";
    sprintf(deviceID, "%08lX", getID);
    topic += deviceID;
    topic.toCharArray(mqtt_topic, topic.length() + 1);
    
    postData = "";
    postData += "v=";

    switch(getID) {
      case SEND_DEVICE_SW1:
        sprintf(state, "%d", profile.getSwitchStatus(EEP_F6_02_04, getData));
        postData += state;
        postData.toCharArray(mqtt_payload, postData.length() + 1);
        break;
      case SEND_DEVICE_CN1:
        sprintf(state, "%d", profile.getContact(EEP_D5_00_01, getData));
        postData += state;
        postData.toCharArray(mqtt_payload, postData.length() + 1);
        break;
      case SEND_DEVICE_TM1:
        dtostrf((double)profile.getTemperature(EEP_A5_02_30, getData), 4, 1, temp);
        postData += temp;
        postData.toCharArray(mqtt_payload, postData.length() + 1);
        break;
      case SEND_DEVICE_TM2:
        dtostrf((double)profile.getTemperature(EEP_A5_02_05, getData), 4, 1, temp);
        postData += temp;
        postData.toCharArray(mqtt_payload, postData.length() + 1);
        break; 
      case SEND_DEVICE_OC1:
        sprintf(state, "%d", profile.getPIRStatus(EEP_A5_07_01, getData));
        postData += state;
        postData.toCharArray(mqtt_payload, postData.length() + 1);
        break;
      case SEND_DEVICE_CO1:
        sprintf(state, "%d", profile.getCO2(EEP_A5_09_04, getData));
        postData += state;
        postData += "&v2=";
        dtostrf((double)profile.getTemperature(EEP_A5_09_04, getData), 4, 1, temp);
        postData += temp;
        postData += "&v3=";
        dtostrf((double)profile.getHumidity(EEP_A5_09_04, getData), 4, 1, hmd);
        postData += hmd;
        postData.toCharArray(mqtt_payload, postData.length() + 1);
        break;
      default:
        Serial.print(F("getID = "));
        Serial.println(getID, HEX);
        break;
    }
    
    Serial.print(F("  mqtt_topic : "));
    Serial.println(mqtt_topic);
    Serial.print(F("  mqtt_payload : "));
    Serial.println(mqtt_payload);
    
    if (mqttClient.connect(deviceID, SC_USER, DEVICE_TOKEN)) {
      Serial.println(F("  Connection to MQTT server succeeded"));
      mqttClient.publish(mqtt_topic, mqtt_payload);
    } else {
      int state = mqttClient.state();
      Serial.print(F("  Connection to MQTT server failed: "));
      Serial.println(state);
    }
  }
}

static void connect(void)
{
  int i;
  
  Serial.println();
  
  // give the ethernet module time to boot up:
  delay(2000);
  
  // start the Ethernet connection:
  Serial.println("Connecting...");
  
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip);
  }
  
  Serial.print(F("IP Address: "));
  for (i = 0; i < IP_ADDR_LEN; i++) {
    Serial.print(Ethernet.localIP()[i], DEC);
    if ( i < IP_ADDR_LEN - 1 ) {
      Serial.print(F("."));
    }
  }
  Serial.println();
}

static void storeData(uint8_t rorg, uint32_t ID, uint32_t data, uint8_t rssi)
{
  switch(ID) {
    case SEND_DEVICE_SW1:
    case SEND_DEVICE_CN1:
    case SEND_DEVICE_TM1:
    case SEND_DEVICE_TM2:
    case SEND_DEVICE_OC1:
    case SEND_DEVICE_CO1:

      if(((rorg == RORG_1BS) && !(data & EEP_1BS_LRN_BIT))
        || ((rorg == RORG_4BS) && !(data & EEP_4BS_LRN_BIT))) {
        break;
      }

      storeDataSet[bfWritePoint].ID = ID;
      storeDataSet[bfWritePoint].data = data;
      storeDataSet[bfWritePoint].rssi = rssi;
      bfWritePoint = ((++bfWritePoint) % BUFF_SIZE);

      if(bfWritePoint == bfReadPoint) {  /* Buffer overflow */
        Serial.print(F(" Buffer Overflow! : wp = rp = "));
        Serial.println(bfWritePoint, DEC);

        if(bfWritePoint % BUFF_SIZE) {
          bfWritePoint--;
        } else {
          bfWritePoint = BUFF_SIZE - 1;
        }
      }
      isDataAvailable = true;
      break;

    default:
      break;
  }
}

static void getStoreData(void)
{
  getID = storeDataSet[bfReadPoint].ID;
  getData = storeDataSet[bfReadPoint].data;
  getRssi = storeDataSet[bfReadPoint].rssi;
  bfReadPoint = ((++bfReadPoint) % BUFF_SIZE);

  if(bfWritePoint == bfReadPoint) {
    isDataAvailable = false;
  }
}
