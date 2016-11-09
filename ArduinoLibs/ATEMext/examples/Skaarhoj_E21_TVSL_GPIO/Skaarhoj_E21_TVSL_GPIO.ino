/*****************
   Basis control for the SKAARHOJ E21TVS series devices
   This example is programmed for ATEM TVS versions
   The button rows are assumed to be configured as 1-2-3-4-5-6 (PGM) / 1-2-3-4-5-6 (PRV) / CUT, AUTO / KEY, MPL1

   This example also uses several custom libraries which you must install first.
   Search for "#include" in this file to find the libraries. Then download the libraries from http://skaarhoj.com/wiki/index.php/Libraries_for_Arduino

   Works with ethernet-enabled arduino devices (Arduino Ethernet or a model with Ethernet shield)
   Make sure to configure IP and addresses first using the sketch "ConfigEthernetAddresses"

   - kasper
*/



// Including libraries:
#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include "WebServer.h"  // For web interface
#include <EEPROM.h>      // For storing IP numbers
#include <SkaarhojPgmspace.h>
#include <Streaming.h>
#include <SkaarhojTools.h>
SkaarhojTools sTools(1);    // 0=No runtime serial logging, 1=Moderate runtime serial logging, 2=more verbose... etc.

// Include ATEM library and make an instance:
#include <ATEMbase.h>
#include <ATEMext.h>
ATEMext AtemSwitcher;

#include <MemoryFree.h>

static uint8_t default_ip[] = {     // IP for Configuration Mode (192.168.10.99)
  192, 168, 10, 99
};
// Configure the IP addresses and MAC address with the sketch "ConfigEthernetAddresses":
uint8_t ip[4];        // Will hold the Arduino IP address
uint8_t atem_ip[4];  // Will hold the ATEM IP address
uint8_t mac[6];    // Will hold the Arduino Ethernet shield/board MAC address (loaded from EEPROM memory, set with ConfigEthernetAddresses example sketch)



// All related to library "SkaarhojBI8":
#include "Wire.h"
#include "MCP23017.h"
#include "PCA9685.h"
#include "SkaarhojBI16.h"
#include "SkaarhojUtils.h"
#include "SkaarhojGPIO2x8.h"

SkaarhojBI16 buttons;
SkaarhojUtils utils;
SkaarhojGPIO2x8 GPIOboard;

int greenled = 2;
int redled = 3;



/*************************************************************


                       Webserver


 **********************************************************/


#define PREFIX ""
WebServer webserver(PREFIX, 80);

void logoCmd(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  /* this data was taken from a PNG file that was converted to a C data structure
     by running it through the directfb-csource application.
     (Alternatively by PHPSH script "HeaderGraphicsWebInterfaceInUnitsPNG8.phpsh")
  */
  P(logoData) = {
    0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00,
    0x00, 0x0d, 0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x02, 0xa2,
    0x00, 0x00, 0x00, 0x2b, 0x08, 0x03, 0x00, 0x00, 0x00, 0x94,
    0xff, 0x1a, 0xf8, 0x00, 0x00, 0x00, 0x19, 0x74, 0x45, 0x58,
    0x74, 0x53, 0x6f, 0x66, 0x74, 0x77, 0x61, 0x72, 0x65, 0x00,
    0x41, 0x64, 0x6f, 0x62, 0x65, 0x20, 0x49, 0x6d, 0x61, 0x67,
    0x65, 0x52, 0x65, 0x61, 0x64, 0x79, 0x71, 0xc9, 0x65, 0x3c,
    0x00, 0x00, 0x00, 0x30, 0x50, 0x4c, 0x54, 0x45, 0xe6, 0xe6,
    0xe6, 0x0d, 0x5c, 0xaf, 0x0c, 0x61, 0xbd, 0x57, 0x89, 0xcc,
    0xd7, 0xe2, 0xf2, 0xfa, 0xfa, 0xfa, 0x85, 0xa8, 0xd9, 0xab,
    0xc2, 0xe3, 0xc7, 0xd6, 0xec, 0x0b, 0x51, 0x9d, 0x36, 0x76,
    0xc4, 0x23, 0x68, 0xb9, 0x0a, 0x65, 0xc5, 0x0a, 0x65, 0xc6,
    0x0d, 0x5e, 0xb5, 0xff, 0xff, 0xff, 0xe6, 0xb3, 0x8a, 0x8c,
    0x00, 0x00, 0x07, 0x1e, 0x49, 0x44, 0x41, 0x54, 0x78, 0xda,
    0xec, 0x5d, 0x0d, 0x7b, 0xa3, 0x20, 0x0c, 0x56, 0xa8, 0xc0,
    0xd4, 0xe2, 0xff, 0xff, 0xb7, 0x67, 0x05, 0x42, 0x12, 0x82,
    0xda, 0xae, 0xf7, 0xcc, 0x6e, 0xc4, 0x75, 0x53, 0xf2, 0x8d,
    0xaf, 0xaf, 0xca, 0xed, 0xee, 0xba, 0x25, 0xca, 0xe0, 0xc6,
    0xf9, 0xde, 0xa4, 0xc9, 0x45, 0x44, 0x8d, 0x6e, 0x08, 0xc8,
    0xec, 0x00, 0xa1, 0xaa, 0x4d, 0x4b, 0x93, 0x2b, 0xc9, 0x1c,
    0x31, 0x1a, 0x21, 0x3a, 0x0c, 0x63, 0x9b, 0x93, 0x26, 0x17,
    0xe3, 0x51, 0x06, 0xd1, 0x46, 0xa2, 0x4d, 0xae, 0x86, 0xd1,
    0x61, 0xc0, 0x37, 0xfa, 0xae, 0xcd, 0x48, 0x93, 0xab, 0x49,
    0x87, 0x21, 0x3a, 0x34, 0x88, 0x36, 0xb9, 0x9c, 0x10, 0x16,
    0x1d, 0x86, 0x36, 0x21, 0x4d, 0x2e, 0xce, 0xa2, 0xf3, 0xbc,
    0xbe, 0x43, 0xa5, 0x77, 0xa9, 0xf8, 0xb9, 0xa3, 0x65, 0xa8,
    0xa0, 0x9f, 0x83, 0x6a, 0xfd, 0x31, 0x0b, 0x23, 0xd9, 0x75,
    0x4e, 0xdb, 0x9c, 0x62, 0x04, 0xbb, 0xcd, 0x67, 0xfb, 0x1e,
    0x13, 0x91, 0x48, 0x31, 0x54, 0xb0, 0x03, 0x1b, 0xe4, 0x99,
    0x7d, 0xe6, 0x6c, 0x1f, 0x7d, 0x90, 0xd1, 0x7c, 0xa7, 0xda,
    0x58, 0xfd, 0x7c, 0xc7, 0x3d, 0xcd, 0x2c, 0x4b, 0x2a, 0x1c,
    0x77, 0x9e, 0x5b, 0x40, 0xe3, 0x78, 0x52, 0x50, 0x40, 0xc8,
    0x87, 0xcd, 0x52, 0xc5, 0x77, 0x16, 0x36, 0x55, 0x19, 0x1d,
    0x67, 0xe8, 0xfb, 0x8e, 0x6b, 0x7c, 0x29, 0xda, 0xcb, 0x15,
    0x3c, 0xe9, 0xfa, 0xe6, 0x66, 0xd1, 0x11, 0xc0, 0xa3, 0x23,
    0x8b, 0x4e, 0xdd, 0xfc, 0xa7, 0xe4, 0x7e, 0x46, 0x73, 0xff,
    0xd9, 0x4a, 0x7e, 0x7e, 0x2e, 0x7e, 0x3a, 0x15, 0x7f, 0x16,
    0x9d, 0x9b, 0x34, 0xb9, 0x96, 0x9c, 0x64, 0x51, 0x63, 0x6e,
    0xa5, 0x98, 0xf5, 0x82, 0xd0, 0xdb, 0x9e, 0x4e, 0x76, 0x3a,
    0xab, 0xc2, 0x05, 0x33, 0xd2, 0xe3, 0x2c, 0xa5, 0x42, 0xdf,
    0xa4, 0x1c, 0x63, 0x56, 0x59, 0xe4, 0xee, 0x68, 0xde, 0xd5,
    0x24, 0x97, 0x68, 0x8c, 0x11, 0x2f, 0x5e, 0x53, 0x44, 0x69,
    0xf2, 0x01, 0xb7, 0xba, 0x53, 0x2c, 0x2a, 0xe1, 0xf3, 0x01,
    0x90, 0x7b, 0x02, 0x4f, 0x80, 0x8a, 0x02, 0x94, 0xe9, 0x3b,
    0x77, 0x1d, 0xe7, 0x68, 0x52, 0x51, 0xd4, 0x20, 0x1a, 0x40,
    0x7c, 0x04, 0xd1, 0x89, 0x55, 0xe8, 0x6e, 0x46, 0xe3, 0x06,
    0x14, 0x4a, 0xf9, 0x46, 0x88, 0xaa, 0xff, 0x7e, 0x7e, 0xd4,
    0x5f, 0x02, 0xa3, 0x3a, 0xcb, 0xa2, 0x8a, 0x6d, 0xb3, 0xb2,
    0xb7, 0x0a, 0x78, 0x14, 0x86, 0x28, 0x1c, 0xdc, 0x9c, 0x86,
    0x30, 0x23, 0x02, 0x9a, 0x0a, 0x35, 0x94, 0x8a, 0x38, 0xa4,
    0xeb, 0x69, 0x12, 0x44, 0x73, 0x55, 0x09, 0xa2, 0xdb, 0x91,
    0xa9, 0xb8, 0xa9, 0x14, 0x3a, 0xfc, 0xb4, 0x45, 0x94, 0xdc,
    0x63, 0x65, 0xdb, 0xd3, 0xed, 0xaa, 0x9e, 0x51, 0x9e, 0xb1,
    0x7b, 0x25, 0xd5, 0x37, 0x2a, 0x78, 0x36, 0xdd, 0x7b, 0x9b,
    0x25, 0x87, 0xfc, 0x59, 0x34, 0xe9, 0xb1, 0x69, 0x82, 0x8e,
    0x1d, 0x85, 0x18, 0x09, 0xa2, 0x0a, 0xf6, 0x6f, 0x6e, 0xcc,
    0xc9, 0x32, 0x78, 0x98, 0xb3, 0xa0, 0xe0, 0x38, 0x34, 0x16,
    0x2c, 0xf6, 0x21, 0x6a, 0xc2, 0x81, 0x33, 0x5b, 0x28, 0x70,
    0x33, 0x38, 0x5f, 0x2e, 0xc6, 0xd2, 0xee, 0x4e, 0x6c, 0x6f,
    0x52, 0x3d, 0x99, 0xf5, 0xfb, 0xd1, 0x5e, 0xaf, 0xa0, 0x3c,
    0xf8, 0x6f, 0xa9, 0x8e, 0x37, 0xce, 0xa2, 0x21, 0x3e, 0xc9,
    0xe4, 0x6e, 0xe9, 0x06, 0x29, 0x14, 0xa2, 0x33, 0x54, 0x34,
    0x45, 0x32, 0xe1, 0xca, 0x88, 0x2f, 0xa8, 0x9d, 0x2b, 0x72,
    0x28, 0x8b, 0x2e, 0x01, 0x9b, 0x2c, 0x34, 0xb5, 0x5c, 0x37,
    0x04, 0x51, 0x93, 0x58, 0x13, 0xae, 0xa9, 0x58, 0xb1, 0x56,
    0xa4, 0x97, 0x04, 0x51, 0x85, 0x67, 0x90, 0xce, 0x66, 0x7d,
    0x9e, 0x77, 0xed, 0xe6, 0xea, 0x29, 0x55, 0x95, 0x93, 0xac,
    0x76, 0xd0, 0x50, 0x1f, 0xdf, 0x8f, 0x26, 0x17, 0x7f, 0x5c,
    0x41, 0xe9, 0x52, 0xda, 0xef, 0x17, 0xff, 0xce, 0x66, 0x8b,
    0xf9, 0xbe, 0x0b, 0x2c, 0xaa, 0x48, 0x5c, 0x80, 0xa8, 0x51,
    0x4a, 0x38, 0x07, 0x3a, 0x81, 0x01, 0x10, 0x3a, 0xdd, 0x55,
    0x4c, 0xa0, 0x22, 0x7a, 0x0c, 0x05, 0x8c, 0x02, 0x58, 0x61,
    0x05, 0x62, 0x51, 0x45, 0x79, 0xf5, 0xe6, 0x54, 0x46, 0x6f,
    0xca, 0x1d, 0x21, 0xba, 0xee, 0x4d, 0x0e, 0x41, 0x3b, 0x7c,
    0x8d, 0x37, 0x14, 0x6a, 0xcb, 0x58, 0xb0, 0x28, 0x9a, 0x09,
    0x05, 0x7e, 0xe4, 0x4b, 0x65, 0x0d, 0x0a, 0x2d, 0x1c, 0xce,
    0x6c, 0xbe, 0x58, 0xcc, 0x72, 0xe2, 0x71, 0xdc, 0xe2, 0x33,
    0x2b, 0xa9, 0x92, 0x33, 0xd1, 0xd2, 0x2e, 0xde, 0x29, 0xf0,
    0x26, 0x55, 0xc0, 0xf7, 0x59, 0x6f, 0x47, 0xc5, 0x3f, 0x93,
    0x6a, 0xbf, 0xd9, 0xb9, 0xec, 0xb4, 0x64, 0x51, 0x55, 0x4a,
    0xe6, 0x32, 0x41, 0x99, 0x21, 0x9a, 0x10, 0xaa, 0x90, 0xd9,
    0x18, 0x41, 0xeb, 0x22, 0x7b, 0xed, 0x2b, 0x12, 0x0e, 0xb9,
    0xd9, 0x7a, 0xa7, 0x2f, 0x55, 0x0e, 0xd0, 0x1d, 0x80, 0xe7,
    0xa6, 0x0c, 0x2a, 0x78, 0xbe, 0x98, 0x68, 0xad, 0xb6, 0x88,
    0xd2, 0xe4, 0xfa, 0x52, 0x3e, 0x8b, 0x96, 0x62, 0xd0, 0xb3,
    0x68, 0x15, 0xa2, 0x3a, 0x53, 0x2d, 0xf7, 0xd4, 0x33, 0xb0,
    0xe5, 0xae, 0xa2, 0x0e, 0xd1, 0x69, 0x07, 0xa2, 0x11, 0x78,
    0x86, 0xd4, 0x16, 0x1d, 0xf5, 0x2c, 0x94, 0xd3, 0x20, 0xfa,
    0x61, 0x72, 0xcc, 0xa2, 0x4a, 0x7c, 0xa3, 0xb7, 0x66, 0xc4,
    0x10, 0x15, 0x11, 0x3a, 0x26, 0x2e, 0x1b, 0xe3, 0xfd, 0x7a,
    0x5f, 0x51, 0xe2, 0xd0, 0xf2, 0x1b, 0xbd, 0x04, 0x51, 0x27,
    0x64, 0x16, 0x07, 0xe7, 0x06, 0xd1, 0xdf, 0xca, 0xa2, 0x4a,
    0x5e, 0xd4, 0x59, 0xef, 0xd3, 0x80, 0x2b, 0x73, 0xe3, 0x3c,
    0x99, 0xdd, 0x0c, 0x60, 0x0d, 0x48, 0x4d, 0x56, 0x70, 0x1c,
    0xc6, 0x17, 0xf5, 0x87, 0xba, 0xb6, 0x1e, 0x95, 0x21, 0xca,
    0x08, 0x33, 0x42, 0xd4, 0xd3, 0x7a, 0xba, 0x06, 0xd1, 0x5f,
    0xca, 0xa2, 0x4a, 0x4d, 0x46, 0x64, 0xd2, 0xcc, 0xa2, 0xae,
    0x4e, 0xa2, 0x2b, 0xdb, 0x7a, 0x4d, 0x68, 0xb4, 0xa2, 0xa8,
    0xaf, 0x8b, 0xaa, 0x63, 0x88, 0x4a, 0x2c, 0xea, 0x0c, 0x6b,
    0xa2, 0xb1, 0xe8, 0x47, 0x42, 0x94, 0xb2, 0x68, 0xcd, 0xcc,
    0x63, 0x3e, 0xd2, 0xb0, 0xf2, 0xe8, 0x13, 0x44, 0x47, 0x01,
    0x28, 0x18, 0x11, 0x0e, 0xb3, 0x6c, 0xa9, 0x58, 0x03, 0xf9,
    0x0a, 0x0e, 0xdd, 0x83, 0x0a, 0xab, 0x10, 0xf5, 0x40, 0xc4,
    0xd2, 0x8d, 0x9e, 0xb3, 0x7a, 0x63, 0xd1, 0x5f, 0xc0, 0xa2,
    0xeb, 0x29, 0xf7, 0xb0, 0xa5, 0x9d, 0x00, 0x4f, 0xaf, 0x60,
    0xc8, 0xa7, 0x97, 0xfc, 0x44, 0x83, 0xda, 0x8f, 0x69, 0xe9,
    0xc7, 0xab, 0x68, 0x34, 0x02, 0x8a, 0x32, 0xc6, 0xa6, 0x87,
    0x6e, 0x04, 0x06, 0x04, 0xc5, 0xe3, 0x7d, 0xdc, 0x4b, 0x38,
    0x34, 0xe6, 0x91, 0xdd, 0xc3, 0x33, 0x00, 0x14, 0x01, 0x18,
    0x8c, 0xb5, 0x18, 0x52, 0xf4, 0x94, 0x52, 0xa4, 0x6a, 0xb6,
    0x26, 0xe2, 0xa5, 0x81, 0x1b, 0x83, 0x7d, 0xd6, 0x72, 0x3e,
    0x0c, 0x3f, 0x49, 0xf7, 0x58, 0x81, 0xac, 0xb1, 0x0b, 0x8e,
    0x21, 0xc4, 0x22, 0xf1, 0x7d, 0x51, 0x42, 0x9c, 0x74, 0xea,
    0x57, 0x89, 0xe6, 0x85, 0x13, 0xf6, 0x8a, 0x4f, 0x32, 0x78,
    0xb2, 0xf8, 0x77, 0x36, 0xeb, 0xc5, 0xea, 0x4a, 0x16, 0x65,
    0x73, 0x5e, 0x04, 0xc5, 0x88, 0xbb, 0xc1, 0x8d, 0x1e, 0x80,
    0xb7, 0x62, 0xd4, 0x13, 0x18, 0x53, 0xc4, 0x3d, 0x74, 0xb2,
    0x22, 0x3f, 0x8b, 0x6e, 0x19, 0x52, 0xb8, 0x7e, 0xab, 0x00,
    0xab, 0xc2, 0x16, 0x21, 0xba, 0xea, 0xe2, 0xa2, 0x13, 0x29,
    0x3c, 0x8e, 0x85, 0x03, 0x68, 0xde, 0x42, 0x85, 0xb9, 0x2b,
    0x61, 0x47, 0xda, 0x3c, 0x36, 0x42, 0xc7, 0x82, 0xe5, 0xe9,
    0x61, 0x79, 0x68, 0xb7, 0x8c, 0xe7, 0xa2, 0x7d, 0xc7, 0xe7,
    0x5b, 0xe9, 0xde, 0xd4, 0x6c, 0xfc, 0xc1, 0x9f, 0x45, 0xb7,
    0xe9, 0xf7, 0xf9, 0xcc, 0x86, 0x4f, 0x1c, 0x4d, 0x43, 0x89,
    0xf0, 0x9c, 0xef, 0xd3, 0xa2, 0x93, 0x4f, 0xac, 0xb9, 0xbe,
    0x44, 0x6d, 0x36, 0xe2, 0xbd, 0xd9, 0x4d, 0x87, 0x0a, 0x1b,
    0x61, 0x90, 0xd6, 0x59, 0x7d, 0x3e, 0xb0, 0x3e, 0x57, 0xe7,
    0x20, 0x6f, 0x58, 0x5c, 0xed, 0x34, 0x14, 0xab, 0x7c, 0x9f,
    0x1e, 0x1f, 0x08, 0xa4, 0x26, 0x3c, 0x8a, 0x9a, 0x42, 0x3b,
    0xb9, 0x59, 0x55, 0x34, 0x5e, 0xda, 0xe0, 0x19, 0x52, 0x85,
    0x92, 0x84, 0x57, 0x2c, 0xa2, 0xaa, 0x79, 0xa8, 0x32, 0xdc,
    0xd9, 0x68, 0xd8, 0xee, 0xa4, 0x8f, 0x92, 0x2b, 0xc0, 0xf5,
    0x1d, 0x16, 0xff, 0x6a, 0xb3, 0xd2, 0xd4, 0xd1, 0x70, 0x71,
    0x97, 0xb3, 0x28, 0x3b, 0x13, 0x54, 0x60, 0xc8, 0x00, 0x7e,
    0x22, 0x44, 0x7b, 0x84, 0xdb, 0x69, 0x33, 0x91, 0x7f, 0xf7,
    0xc4, 0xec, 0x28, 0x00, 0x87, 0x41, 0x60, 0x1d, 0xcb, 0x73,
    0xd5, 0x63, 0xc4, 0xe5, 0xbc, 0x69, 0x39, 0x21, 0x6b, 0x6d,
    0xfc, 0x5d, 0x01, 0x5a, 0xbd, 0x05, 0x3b, 0xd2, 0xa5, 0x92,
    0xda, 0x15, 0x31, 0xcc, 0xa7, 0xba, 0x98, 0x1d, 0x55, 0xce,
    0x94, 0x92, 0x6c, 0x4b, 0x00, 0x4a, 0x57, 0x81, 0x90, 0x6d,
    0x37, 0x9a, 0x70, 0xb1, 0x1d, 0xfb, 0x88, 0xa5, 0x3c, 0x59,
    0xfc, 0x6b, 0xcd, 0x2a, 0x5f, 0xe3, 0x09, 0x36, 0x05, 0xec,
    0x6f, 0x80, 0xfa, 0x33, 0x32, 0x1a, 0x78, 0x5b, 0x49, 0x2c,
    0x8a, 0x41, 0x65, 0xc7, 0xbc, 0x9f, 0x51, 0x32, 0x92, 0x3f,
    0x28, 0x12, 0x14, 0x3d, 0x83, 0xe8, 0xa4, 0x01, 0xbc, 0x5c,
    0x95, 0x21, 0x8a, 0xb0, 0xd7, 0x59, 0x33, 0x86, 0xea, 0x1c,
    0xc5, 0x6c, 0xbc, 0xaa, 0xec, 0xad, 0x08, 0xd2, 0xe4, 0x13,
    0xa4, 0xce, 0xa2, 0x59, 0xaa, 0xab, 0x41, 0x9e, 0xb0, 0x28,
    0xf0, 0x99, 0x1b, 0x7b, 0x78, 0x89, 0x41, 0x18, 0x09, 0x23,
    0x55, 0x85, 0x2f, 0x70, 0x98, 0x7e, 0x3f, 0xa4, 0xf7, 0xfb,
    0x10, 0xf5, 0xd2, 0xba, 0xad, 0xd3, 0x22, 0x95, 0xdb, 0xa9,
    0x9d, 0xf3, 0x4f, 0x83, 0xe8, 0x72, 0xcc, 0xa2, 0xba, 0xb6,
    0xe2, 0x93, 0x21, 0xba, 0xd9, 0xf5, 0x3d, 0x60, 0x54, 0xe3,
    0x7b, 0x7e, 0x64, 0x45, 0x47, 0x1f, 0x3d, 0xb9, 0x62, 0x2a,
    0x71, 0x98, 0x30, 0x7a, 0x04, 0x51, 0xaf, 0x0b, 0x2c, 0x9a,
    0x49, 0x7a, 0xda, 0x70, 0xa6, 0x9d, 0xf1, 0x3f, 0xc3, 0xa2,
    0x36, 0x60, 0x80, 0xb2, 0xa8, 0x07, 0x8c, 0x8a, 0xb7, 0x5a,
    0x86, 0x6e, 0xa6, 0xb0, 0x25, 0x0e, 0x01, 0xa3, 0x63, 0x0d,
    0xa2, 0x29, 0xef, 0x7a, 0x75, 0xe0, 0xbf, 0x18, 0xa2, 0xd7,
    0x81, 0x02, 0xa2, 0xeb, 0xb0, 0xef, 0xdb, 0x19, 0xff, 0x74,
    0x16, 0x95, 0x4e, 0x61, 0xef, 0xe5, 0xd1, 0x88, 0x8d, 0xed,
    0x3b, 0x19, 0xed, 0x45, 0x97, 0x68, 0x24, 0xa8, 0x82, 0xa6,
    0xef, 0x45, 0xfc, 0xf4, 0x0f, 0x45, 0xe9, 0xd4, 0x7b, 0x92,
    0xd7, 0x07, 0x9b, 0x14, 0x48, 0x46, 0x62, 0xc3, 0xe7, 0x27,
    0x0a, 0x7f, 0xa3, 0x8f, 0x10, 0xea, 0x85, 0x8d, 0xed, 0x14,
    0x56, 0xdc, 0xb3, 0x74, 0x49, 0x9e, 0xa5, 0x42, 0x74, 0xad,
    0x55, 0x71, 0x30, 0x5a, 0xef, 0x60, 0x57, 0xc1, 0x5b, 0x94,
    0xac, 0x4b, 0xed, 0x5e, 0xd3, 0x72, 0x80, 0x9a, 0x07, 0x9f,
    0xa5, 0xe3, 0xea, 0x05, 0x8f, 0xe3, 0x86, 0x4f, 0x55, 0xf0,
    0x62, 0xba, 0x37, 0x35, 0xcb, 0x5d, 0xf8, 0xb3, 0x68, 0xa0,
    0xac, 0x00, 0xa4, 0xf8, 0x05, 0xc0, 0xea, 0x81, 0xd1, 0x7a,
    0x34, 0xee, 0xc1, 0x10, 0x9b, 0x64, 0x53, 0x50, 0x79, 0x7a,
    0x48, 0xdd, 0x7a, 0xec, 0xe9, 0x79, 0x78, 0x48, 0xeb, 0x69,
    0x9c, 0x22, 0x37, 0xfb, 0x06, 0xd5, 0xa2, 0xa2, 0x41, 0x23,
    0x54, 0x81, 0x83, 0x7a, 0xa2, 0xf5, 0x3d, 0x6d, 0xcd, 0xe7,
    0xc0, 0x24, 0x7b, 0x4e, 0x83, 0x2d, 0x89, 0x0f, 0x9f, 0x87,
    0x5c, 0x10, 0xcb, 0xeb, 0x9f, 0x8d, 0x26, 0xd4, 0xf2, 0x5a,
    0x05, 0xcf, 0xa4, 0x7b, 0x77, 0xb3, 0x6c, 0xaa, 0x8b, 0x67,
    0x51, 0x00, 0x44, 0x93, 0x26, 0x17, 0x11, 0xb6, 0x2e, 0xda,
    0x26, 0xa4, 0xc9, 0xd5, 0xa4, 0x60, 0xd1, 0x26, 0x4d, 0x2e,
    0x06, 0x51, 0xfa, 0x4f, 0xe0, 0x7e, 0xb5, 0x19, 0x69, 0x72,
    0x6d, 0x16, 0x9d, 0xda, 0x8c, 0x34, 0xb9, 0x96, 0x4c, 0x8c,
    0x45, 0xed, 0x57, 0xe3, 0xd1, 0x26, 0x17, 0x92, 0xaf, 0x2f,
    0x3b, 0xd0, 0xff, 0x31, 0xa4, 0xb3, 0xd3, 0x57, 0x93, 0x26,
    0x97, 0x91, 0xc9, 0x76, 0xf4, 0x8d, 0x7e, 0x18, 0xba, 0x26,
    0x4d, 0x2e, 0x25, 0x03, 0x65, 0xd1, 0x15, 0xa3, 0x0d, 0xa4,
    0x4d, 0x2e, 0x05, 0x50, 0xf6, 0x5f, 0x83, 0x3d, 0x40, 0xba,
    0xe1, 0xf4, 0xa1, 0xc9, 0x9f, 0x25, 0x1c, 0x2f, 0x69, 0x7c,
    0x49, 0x7a, 0xf4, 0x73, 0xc1, 0x7b, 0x5c, 0x03, 0x9f, 0x34,
    0xc2, 0x83, 0x0f, 0x52, 0xf0, 0xec, 0x20, 0x8e, 0x0c, 0xb2,
    0x0d, 0x3e, 0x58, 0xb0, 0xeb, 0x52, 0xf5, 0x5c, 0x68, 0x61,
    0xa1, 0x8a, 0x9d, 0x6e, 0x68, 0xc1, 0x03, 0xe9, 0x45, 0x98,
    0x9e, 0x45, 0xc8, 0xb9, 0x94, 0xbd, 0x2c, 0xbc, 0xf6, 0x85,
    0x54, 0xf4, 0x4c, 0xb4, 0xd7, 0x2a, 0x58, 0x4e, 0xba, 0x2e,
    0xc5, 0xdc, 0xbd, 0xad, 0xd9, 0x81, 0xfa, 0x2c, 0x09, 0xa0,
    0xcb, 0xf2, 0x4f, 0x80, 0x01, 0x00, 0x3d, 0x86, 0x58, 0xcf,
    0x39, 0xc2, 0xf5, 0xc6, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45,
    0x4e, 0x44, 0xae, 0x42, 0x60, 0x82
  };

  if (type == WebServer::POST)
  {
    // ignore POST data
    server.httpFail();
    return;
  }

  /* for a GET or HEAD, send the standard "it's all OK headers" but identify our data as a PNG file */
  server.httpSuccess("image/png");

  /* we don't output the body for a HEAD request */
  if (type == WebServer::GET)
  {
    server.writeP(logoData, sizeof(logoData));
  }
}

// commands are functions that get called by the webserver framework
// they can read any posted data from client, and they output to server
void webDefaultView(WebServer &server, WebServer::ConnectionType type)
{
  P(htmlHead) =
    "<html>"
    "<head>"
    "<title>SKAARHOJ Device Configuration</title>"
    "<style type=\"text/css\">"
    "BODY { font-family: sans-serif }"
    "H1 { font-size: 14pt; }"
    "P  { font-size: 10pt; }"
    "</style>"
    "</head>"
    "<body>"
    "<img src='logo.png'><br/>";

  int i;
  server.httpSuccess();
  server.printP(htmlHead);

  server << F("<div style='width:660px; margin-left:10px;'><form action='") << PREFIX << F("/form' method='post'>");

  // Panel IP:
  server << F("<h1>SKAARHOJ Device IP Address:</h1><p>");
  for (i = 0; i <= 3; ++i)
  {
    server << F("<input type='text' name='IP") << i << F("' value='") << EEPROM.read(i + 2) << F("' id='IP") << i << F("' size='4'>");
    if (i < 3)  server << F(".");
  }
  server << F("<hr/>");

  // ATEM Switcher Panel IP:
  server << F("<h1>ATEM Switcher IP Address:</h1><p>");
  for (i = 0; i <= 3; ++i)
  {
    server << F("<input type='text' name='ATEM_IP") << i << F("' value='") << EEPROM.read(i + 2 + 4) << F("' id='ATEM_IP") << i << F("' size='4'>");
    if (i < 3)  server << F(".");
  }
  server << F("<hr/>");

  ////////////////////////////////////////////////////////

  // Set first routing for Top buttons
  server << F("<h1>Set functions for 2 top buttons:</h1><table border=0 cellspacing=3><tr>");
  server << F("<td><p>Left Button") << F( ":</p>");
  server << F("<td><p>Right Button") << F(":</p>");
  server << F("</tr></n>");
  for (i = 1; i <= 2; ++i)
  {
    server << F("<td></p>");
    server << F("<select name='Buttons1A1") << i << F(":'div style='width:78px;>");
    server << F("<option value='0'></option>");

    server << F("<option value='") << 0 << F("'") << (EEPROM.read(614 + i) == 0 ? F(" selected='selected'") : F("")) << F(">ATEM1ME") << F("</option>");
    server << F("<option value='") << 1 << F("'") << (EEPROM.read(614 + i) == 1 ? F(" selected='selected'") : F("")) << F(">ATEM2ME") << F("</option>");
    server << F("</select></td>");
  }
  server << F("</tr></n>");
  for (i = 1; i <= 2; ++i)
  {
    server << F("<td></p>");
    server << F("<select name='Buttons1F1") << i << F(":'div style='width:78px;>");
    server << F("<option value='0'></option>");

    server << F("<option value='") << 0 << F("'") << (EEPROM.read(414 + i) == 0 ? F(" selected='selected'") : F("")) << F("> \t") << F("</option>");

    // Preview bus inputs 1-20
    for (uint8_t j = 1; j <= 20; j++)  {
      server << F("<option value='") << j << F("'") << (EEPROM.read(414 + i) == j ? F(" selected='selected'") : F("")) << F(">PVW ") << j << F("</option>");
    }
    // Program bus inputs 1-20
    for (uint8_t j = 1; j <= 20; j++)  {
      server << F("<option value='") << j + 20 << F("'") << (EEPROM.read(414 + i) == j + 20 ? F(" selected='selected'") : F("")) << F(">PGM ") << j << F("</option>");
    }

    // Other input sources
    server << F("<option value='") << 41 << F("'") << (EEPROM.read(414 + i) == 41 ? F(" selected='selected'") : F("")) << F(">PvwBlack") << F("</option>");
    server << F("<option value='") << 42 << F("'") << (EEPROM.read(414 + i) == 42 ? F(" selected='selected'") : F("")) << F(">PgmBlack") << F("</option>");
    server << F("<option value='") << 43 << F("'") << (EEPROM.read(414 + i) == 43 ? F(" selected='selected'") : F("")) << F(">PvwBars") << F("</option>");
    server << F("<option value='") << 44 << F("'") << (EEPROM.read(414 + i) == 44 ? F(" selected='selected'") : F("")) << F(">PgmBars") << F("</option>");
    server << F("<option value='") << 45 << F("'") << (EEPROM.read(414 + i) == 45 ? F(" selected='selected'") : F("")) << F(">PvwColor1") << F("</option>");
    server << F("<option value='") << 46 << F("'") << (EEPROM.read(414 + i) == 46 ? F(" selected='selected'") : F("")) << F(">PgmColor1") << F("</option>");
    server << F("<option value='") << 47 << F("'") << (EEPROM.read(414 + i) == 47 ? F(" selected='selected'") : F("")) << F(">PvwColor2") << F("</option>");
    server << F("<option value='") << 48 << F("'") << (EEPROM.read(414 + i) == 48 ? F(" selected='selected'") : F("")) << F(">PgmColor2") << F("</option>");
    server << F("<option value='") << 49 << F("'") << (EEPROM.read(414 + i) == 49 ? F(" selected='selected'") : F("")) << F(">PvwMedia1") << F("</option>");
    server << F("<option value='") << 50 << F("'") << (EEPROM.read(414 + i) == 50 ? F(" selected='selected'") : F("")) << F(">PgmMedia1") << F("</option>");
    server << F("<option value='") << 51 << F("'") << (EEPROM.read(414 + i) == 51 ? F(" selected='selected'") : F("")) << F(">PvwMedia2") << F("</option>");
    server << F("<option value='") << 52 << F("'") << (EEPROM.read(414 + i) == 52 ? F(" selected='selected'") : F("")) << F(">PgmMedia2") << F("</option>");
    server << F("<option value='") << 53 << F("'") << (EEPROM.read(414 + i) == 53 ? F(" selected='selected'") : F("")) << F(">PvwSSource") << F("</option>");
    server << F("<option value='") << 54 << F("'") << (EEPROM.read(414 + i) == 54 ? F(" selected='selected'") : F("")) << F(">PgmSSource") << F("</option>");

    for (uint8_t j = 1; j <= 8; j++)
    {
      server << F("<option value='") << 54 + j << F("'") << (EEPROM.read(414 + i) == 54 + j ? F(" selected='selected'") : F("")) << F(">GPI ") << j << F("</option>");
    }



    server << F("<option value='") << 255 << F("'") << (EEPROM.read(414 + i) == 255 ? F(" selected='selected'") : F("")) << F(">Default") << F("</option>");

    server << F("</select></td>");
  }
  server << F("</tr></n>");

  // Set second routing for Top buttons
  for (i = 1; i <= 2; ++i)
  {
    server << F("<td></p>");
    server << F("<select name='Buttons1F2") << i << F(":'div style='width:78px;>");
    server << F("<option value='0'></option>");

    server << F("<option value='") << 0 << F("'") << (EEPROM.read(514 + i) == 0 ? F(" selected='selected'") : F("")) << F("> \t") << F("</option>");

    // USK 1-4 on/off
    server << F("<option value='") << 1 << F("'") << (EEPROM.read(514 + i) == 1 ? F(" selected='selected'") : F("")) << F(">USK1") << F("</option>");
    server << F("<option value='") << 2 << F("'") << (EEPROM.read(514 + i) == 2 ? F(" selected='selected'") : F("")) << F(">USK2") << F("</option>");
    server << F("<option value='") << 3 << F("'") << (EEPROM.read(514 + i) == 3 ? F(" selected='selected'") : F("")) << F(">USK3") << F("</option>");
    server << F("<option value='") << 4 << F("'") << (EEPROM.read(514 + i) == 4 ? F(" selected='selected'") : F("")) << F(">USK4") << F("</option>");

    // DSK 1-2 on/off
    server << F("<option value='") << 5 << F("'") << (EEPROM.read(514 + i) == 5 ? F(" selected='selected'") : F("")) << F(">DSK1") << F("</option>");
    server << F("<option value='") << 6 << F("'") << (EEPROM.read(514 + i) == 6 ? F(" selected='selected'") : F("")) << F(">DSK2") << F("</option>");
    server << F("<option value='") << 7 << F("'") << (EEPROM.read(514 + i) == 7 ? F(" selected='selected'") : F("")) << F(">DSK1Auto") << F("</option>");
    server << F("<option value='") << 8 << F("'") << (EEPROM.read(514 + i) == 8 ? F(" selected='selected'") : F("")) << F(">DSK2Auto") << F("</option>");

    // Others
    server << F("<option value='") << 9 << F("'") << (EEPROM.read(514 + i) == 9 ? F(" selected='selected'") : F("")) << F(">Cut") << F("</option>");
    server << F("<option value='") << 10 << F("'") << (EEPROM.read(514 + i) == 10 ? F(" selected='selected'") : F("")) << F(">Auto") << F("</option>");
    server << F("<option value='") << 11 << F("'") << (EEPROM.read(514 + i) == 11 ? F(" selected='selected'") : F("")) << F(">FTB") << F("</option>");
    server << F("<option value='") << 12 << F("'") << (EEPROM.read(514 + i) == 12 ? F(" selected='selected'") : F("")) << F(">Keys Off") << F("</option>");

    for (uint8_t j = 1; j <= 8; j++)  {
      server << F("<option value='") << 12 + j << F("'") << (EEPROM.read(514 + i) == 12 + j ? F(" selected='selected'") : F("")) << F(">GPO ") << j << F("</option>");
    }
    for (uint8_t j = 1; j <= 8; j++)  {
      server << F("<option value='") << 20 + j << F("'") << (EEPROM.read(514 + i) == 20 + j ? F(" selected='selected'") : F("")) << F(">ToggleGPO ") << j << F("</option>");
    }

    server << F("</select></td>");
  }
  server << F("</tr></table><hr/>");

  ////////////////////////////////////////////////////
  // Set first routing for Side buttons
  server << F("<h1>Set functions for 2 side buttons:</h1><table border=0 cellspacing=3><tr>");
  server << F("<td><p>Top Button") << F( ":</p>");
  server << F("<td><p>Bottom Button") << F(":</p>");
  server << F("</tr></n>");
  for (i = 1; i <= 2; ++i)
  {
    server << F("<td></p>");
    server << F("<select name='Buttons2A1") << i << F(":'div style='width:78px;>");
    server << F("<option value='0'></option>");

    server << F("<option value='") << 0 << F("'") << (EEPROM.read(615 - i) == 0 ? F(" selected='selected'") : F("")) << F(">ATEM1ME") << F("</option>");
    server << F("<option value='") << 1 << F("'") << (EEPROM.read(615 - i) == 1 ? F(" selected='selected'") : F("")) << F(">ATEM2ME") << F("</option>");

    server << F("</select></td>");
  }
  server << F("</tr></n>");
  for (i = 1; i <= 2; ++i)
  {
    server << F("<td></p>");
    server << F("<select name='Buttons2F1") << i << F(":'div style='width:78px;>");
    server << F("<option value='0'></option>");

    server << F("<option value='") << 0 << F("'") << (EEPROM.read(415 - i) == 0 ? F(" selected='selected'") : F("")) << F("> \t") << F("</option>");

    // Preview bus inputs 1-20
    for (uint8_t j = 1; j <= 20; j++)  {
      server << F("<option value='") << j << F("'") << (EEPROM.read(415 - i) == j ? F(" selected='selected'") : F("")) << F(">PVW ") << j << F("</option>");
    }
    // Program bus inputs 1-16
    for (uint8_t j = 1; j <= 20; j++)  {
      server << F("<option value='") << j + 20 << F("'") << (EEPROM.read(415 - i) == j + 20 ? F(" selected='selected'") : F("")) << F(">PGM ") << j << F("</option>");
    }

    // Other input sources
    server << F("<option value='") << 41 << F("'") << (EEPROM.read(415 - i) == 41 ? F(" selected='selected'") : F("")) << F(">PvwBlack") << F("</option>");
    server << F("<option value='") << 42 << F("'") << (EEPROM.read(415 - i) == 42 ? F(" selected='selected'") : F("")) << F(">PgmBlack") << F("</option>");
    server << F("<option value='") << 43 << F("'") << (EEPROM.read(415 - i) == 43 ? F(" selected='selected'") : F("")) << F(">PvwBars") << F("</option>");
    server << F("<option value='") << 44 << F("'") << (EEPROM.read(415 - i) == 44 ? F(" selected='selected'") : F("")) << F(">PgmBars") << F("</option>");
    server << F("<option value='") << 45 << F("'") << (EEPROM.read(415 - i) == 45 ? F(" selected='selected'") : F("")) << F(">PvwColor1") << F("</option>");
    server << F("<option value='") << 46 << F("'") << (EEPROM.read(415 - i) == 46 ? F(" selected='selected'") : F("")) << F(">PgmColor1") << F("</option>");
    server << F("<option value='") << 47 << F("'") << (EEPROM.read(415 - i) == 47 ? F(" selected='selected'") : F("")) << F(">PvwColor2") << F("</option>");
    server << F("<option value='") << 48 << F("'") << (EEPROM.read(415 - i) == 48 ? F(" selected='selected'") : F("")) << F(">PgmColor2") << F("</option>");
    server << F("<option value='") << 49 << F("'") << (EEPROM.read(415 - i) == 49 ? F(" selected='selected'") : F("")) << F(">PvwMedia1") << F("</option>");
    server << F("<option value='") << 50 << F("'") << (EEPROM.read(415 - i) == 50 ? F(" selected='selected'") : F("")) << F(">PgmMedia1") << F("</option>");
    server << F("<option value='") << 51 << F("'") << (EEPROM.read(415 - i) == 51 ? F(" selected='selected'") : F("")) << F(">PvwMedia2") << F("</option>");
    server << F("<option value='") << 52 << F("'") << (EEPROM.read(415 - i) == 52 ? F(" selected='selected'") : F("")) << F(">PgmMedia2") << F("</option>");
    server << F("<option value='") << 53 << F("'") << (EEPROM.read(415 - i) == 53 ? F(" selected='selected'") : F("")) << F(">PvwSSource") << F("</option>");
    server << F("<option value='") << 54 << F("'") << (EEPROM.read(415 - i) == 54 ? F(" selected='selected'") : F("")) << F(">PgmSSource") << F("</option>");

    for (uint8_t j = 1; j <= 8; j++)
    {
      server << F("<option value='") << 54 + j << F("'") << (EEPROM.read(415 - i) == 54 + j ? F(" selected='selected'") : F("")) << F(">GPI ") << j << F("</option>");
    }

    server << F("<option value='") << 255 << F("'") << (EEPROM.read(415 - i) == 255 ? F(" selected='selected'") : F("")) << F(">Default") << F("</option>");

    server << F("</select></td>");
  }
  server << F("</tr></n>");

  // Set second routing for Side buttons
  for (i = 1; i <= 2; ++i)
  {
    server << F("<td></p>");
    server << F("<select name='Buttons2F2") << i << F(":'div style='width:78px;>");
    server << F("<option value='0'></option>");

    server << F("<option value='") << 0 << F("'") << (EEPROM.read(515 - i) == 0 ? F(" selected='selected'") : F("")) << F("> \t") << F("</option>");

    // USK 1-4 on/off
    server << F("<option value='") << 1 << F("'") << (EEPROM.read(515 - i) == 1 ? F(" selected='selected'") : F("")) << F(">USK1") << F("</option>");
    server << F("<option value='") << 2 << F("'") << (EEPROM.read(515 - i) == 2 ? F(" selected='selected'") : F("")) << F(">USK2") << F("</option>");
    server << F("<option value='") << 3 << F("'") << (EEPROM.read(515 - i) == 3 ? F(" selected='selected'") : F("")) << F(">USK3") << F("</option>");
    server << F("<option value='") << 4 << F("'") << (EEPROM.read(515 - i) == 4 ? F(" selected='selected'") : F("")) << F(">USK4") << F("</option>");

    // DSK 1-2 on/off
    server << F("<option value='") << 5 << F("'") << (EEPROM.read(515 - i) == 5 ? F(" selected='selected'") : F("")) << F(">DSK1") << F("</option>");
    server << F("<option value='") << 6 << F("'") << (EEPROM.read(515 - i) == 6 ? F(" selected='selected'") : F("")) << F(">DSK2") << F("</option>");
    server << F("<option value='") << 7 << F("'") << (EEPROM.read(515 - i) == 7 ? F(" selected='selected'") : F("")) << F(">DSK1Auto") << F("</option>");
    server << F("<option value='") << 8 << F("'") << (EEPROM.read(515 - i) == 8 ? F(" selected='selected'") : F("")) << F(">DSK2Auto") << F("</option>");

    // Others
    server << F("<option value='") << 9 << F("'") << (EEPROM.read(515 - i) == 9 ? F(" selected='selected'") : F("")) << F(">Cut") << F("</option>");
    server << F("<option value='") << 10 << F("'") << (EEPROM.read(515 - i) == 10 ? F(" selected='selected'") : F("")) << F(">Auto") << F("</option>");
    server << F("<option value='") << 11 << F("'") << (EEPROM.read(515 - i) == 11 ? F(" selected='selected'") : F("")) << F(">FTB") << F("</option>");
    server << F("<option value='") << 12 << F("'") << (EEPROM.read(515 - i) == 12 ? F(" selected='selected'") : F("")) << F(">Keys Off") << F("</option>");

    for (uint8_t j = 1; j <= 8; j++)  {
      server << F("<option value='") << 12 + j << F("'") << (EEPROM.read(515 - i) == 12 + j ? F(" selected='selected'") : F("")) << F(">GPO ") << j << F("</option>");
    }
    for (uint8_t j = 1; j <= 8; j++)  {
      server << F("<option value='") << 20 + j << F("'") << (EEPROM.read(515 - i) == 20 + j ? F(" selected='selected'") : F("")) << F(">ToggleGPO ") << j << F("</option>");
    }

    server << F("</select></td>");
  }
  server << F("</tr></table><hr/>");

  ///////////////////////////////////////////
  // Set first routing for Top row buttons
  server << F("<h1>Set functions for 6 top row buttons:</h1><table border=0 cellspacing=3><tr>");
  for (i = 1; i <= 6; ++i)
  {
    server << F("<td><p>Button ") << i << F(":</p>");
    server << F("<select name='Buttons3A1") << i << F(":'div style='width:78px;>");
    server << F("<option value='0'></option>");

    server << F("<option value='") << 0 << F("'") << (EEPROM.read(606 + i) == 0 ? F(" selected='selected'") : F("")) << F(">ATEM1ME") << F("</option>");
    server << F("<option value='") << 1 << F("'") << (EEPROM.read(606 + i) == 1 ? F(" selected='selected'") : F("")) << F(">ATEM2ME") << F("</option>");

    server << F("</select></td>");
  }
  server << F("</tr></n>");
  for (i = 1; i <= 6; ++i)
  {
    server << F("<td></p>");
    server << F("<select name='Buttons3F1") << i << F(":'div style='width:78px;>");
    server << F("<option value='0'></option>");

    server << F("<option value='") << 0 << F("'") << (EEPROM.read(406 + i) == 0 ? F(" selected='selected'") : F("")) << F("> \t") << F("</option>");

    // Preview bus inputs 1-16
    for (uint8_t j = 1; j <= 20; j++)  {
      server << F("<option value='") << j << F("'") << (EEPROM.read(406 + i) == j ? F(" selected='selected'") : F("")) << F(">PVW ") << j << F("</option>");
    }
    // Program bus inputs 1-16
    for (uint8_t j = 1; j <= 20; j++)  {
      server << F("<option value='") << j + 20 << F("'") << (EEPROM.read(406 + i) == j + 20 ? F(" selected='selected'") : F("")) << F(">PGM ") << j << F("</option>");
    }

    // Other input sources
    server << F("<option value='") << 41 << F("'") << (EEPROM.read(406 + i) == 41 ? F(" selected='selected'") : F("")) << F(">PvwBlack") << F("</option>");
    server << F("<option value='") << 42 << F("'") << (EEPROM.read(406 + i) == 42 ? F(" selected='selected'") : F("")) << F(">PgmBlack") << F("</option>");
    server << F("<option value='") << 43 << F("'") << (EEPROM.read(406 + i) == 43 ? F(" selected='selected'") : F("")) << F(">PvwBars") << F("</option>");
    server << F("<option value='") << 44 << F("'") << (EEPROM.read(406 + i) == 44 ? F(" selected='selected'") : F("")) << F(">PgmBars") << F("</option>");
    server << F("<option value='") << 45 << F("'") << (EEPROM.read(406 + i) == 45 ? F(" selected='selected'") : F("")) << F(">PvwColor1") << F("</option>");
    server << F("<option value='") << 46 << F("'") << (EEPROM.read(406 + i) == 46 ? F(" selected='selected'") : F("")) << F(">PgmColor1") << F("</option>");
    server << F("<option value='") << 47 << F("'") << (EEPROM.read(406 + i) == 47 ? F(" selected='selected'") : F("")) << F(">PvwColor2") << F("</option>");
    server << F("<option value='") << 48 << F("'") << (EEPROM.read(406 + i) == 48 ? F(" selected='selected'") : F("")) << F(">PgmColor2") << F("</option>");
    server << F("<option value='") << 49 << F("'") << (EEPROM.read(406 + i) == 49 ? F(" selected='selected'") : F("")) << F(">PvwMedia1") << F("</option>");
    server << F("<option value='") << 50 << F("'") << (EEPROM.read(406 + i) == 50 ? F(" selected='selected'") : F("")) << F(">PgmMedia1") << F("</option>");
    server << F("<option value='") << 51 << F("'") << (EEPROM.read(406 + i) == 51 ? F(" selected='selected'") : F("")) << F(">PvwMedia2") << F("</option>");
    server << F("<option value='") << 52 << F("'") << (EEPROM.read(406 + i) == 52 ? F(" selected='selected'") : F("")) << F(">PgmMedia2") << F("</option>");
    server << F("<option value='") << 53 << F("'") << (EEPROM.read(406 + i) == 53 ? F(" selected='selected'") : F("")) << F(">PvwSSource") << F("</option>");
    server << F("<option value='") << 54 << F("'") << (EEPROM.read(406 + i) == 54 ? F(" selected='selected'") : F("")) << F(">PgmSSource") << F("</option>");

    for (uint8_t j = 1; j <= 8; j++)
    {
      server << F("<option value='") << 54 + j << F("'") << (EEPROM.read(406 + i) == 54 + j ? F(" selected='selected'") : F("")) << F(">GPI ") << j << F("</option>");
    }

    server << F("<option value='") << 255 << F("'") << (EEPROM.read(406 + i) == 255 ? F(" selected='selected'") : F("")) << F(">Default") << F("</option>");

    server << F("</select></td>");
  }
  server << F("</tr></n>");

  // Set second routing for Top row buttons
  for (i = 1; i <= 6; ++i)
  {
    server << F("<td></p>");
    server << F("<select name='Buttons3F2") << i << F(":'div style='width:78px;>");
    server << F("<option value='0'></option>");

    server << F("<option value='") << 0 << F("'") << (EEPROM.read(506 + i) == 0 ? F(" selected='selected'") : F("")) << F("> \t") << F("</option>");

    // USK 1-4 on/off
    server << F("<option value='") << 1 << F("'") << (EEPROM.read(506 + i) == 1 ? F(" selected='selected'") : F("")) << F(">USK1") << F("</option>");
    server << F("<option value='") << 2 << F("'") << (EEPROM.read(506 + i) == 2 ? F(" selected='selected'") : F("")) << F(">USK2") << F("</option>");
    server << F("<option value='") << 3 << F("'") << (EEPROM.read(506 + i) == 3 ? F(" selected='selected'") : F("")) << F(">USK3") << F("</option>");
    server << F("<option value='") << 4 << F("'") << (EEPROM.read(506 + i) == 4 ? F(" selected='selected'") : F("")) << F(">USK4") << F("</option>");

    // DSK 1-2 on/off
    server << F("<option value='") << 5 << F("'") << (EEPROM.read(506 + i) == 5 ? F(" selected='selected'") : F("")) << F(">DSK1") << F("</option>");
    server << F("<option value='") << 6 << F("'") << (EEPROM.read(506 + i) == 6 ? F(" selected='selected'") : F("")) << F(">DSK2") << F("</option>");
    server << F("<option value='") << 7 << F("'") << (EEPROM.read(506 + i) == 7 ? F(" selected='selected'") : F("")) << F(">DSK1Auto") << F("</option>");
    server << F("<option value='") << 8 << F("'") << (EEPROM.read(506 + i) == 8 ? F(" selected='selected'") : F("")) << F(">DSK2Auto") << F("</option>");

    // Others
    server << F("<option value='") << 9 << F("'") << (EEPROM.read(506 + i) == 9 ? F(" selected='selected'") : F("")) << F(">Cut") << F("</option>");
    server << F("<option value='") << 10 << F("'") << (EEPROM.read(506 + i) == 10 ? F(" selected='selected'") : F("")) << F(">Auto") << F("</option>");
    server << F("<option value='") << 11 << F("'") << (EEPROM.read(506 + i) == 11 ? F(" selected='selected'") : F("")) << F(">FTB") << F("</option>");
    server << F("<option value='") << 12 << F("'") << (EEPROM.read(506 + i) == 12 ? F(" selected='selected'") : F("")) << F(">Keys Off") << F("</option>");

    for (uint8_t j = 1; j <= 8; j++)  {
      server << F("<option value='") << 12 + j << F("'") << (EEPROM.read(506 + i) == 12 + j ? F(" selected='selected'") : F("")) << F(">GPO ") << j << F("</option>");
    }
    for (uint8_t j = 1; j <= 8; j++)  {
      server << F("<option value='") << 20 + j << F("'") << (EEPROM.read(506 + i) == 20 + j ? F(" selected='selected'") : F("")) << F(">ToggleGPO ") << j << F("</option>");
    }

    server << F("</select></td>");
  }

  server << F("</tr></table><hr/>");

  ///////////////////////////////////////////
  // Set first routing for Bottom row buttons
  server << F("<h1>Set functions for 6 bottom row buttons:</h1><table border=0 cellspacing=3><tr>");
  for (i = 1; i <= 6; ++i)
  {
    server << F("<td><p>Button ") << i << F(":</p>");
    server << F("<select name='Buttons4A1") << i << F(":'div style='width:78px;>");
    server << F("<option value='0'></option>");

    server << F("<option value='") << 0 << F("'") << (EEPROM.read(600 + i) == 0 ? F(" selected='selected'") : F("")) << F(">ATEM1ME") << F("</option>");
    server << F("<option value='") << 1 << F("'") << (EEPROM.read(600 + i) == 1 ? F(" selected='selected'") : F("")) << F(">ATEM2ME") << F("</option>");

    server << F("</select></td>");
  }
  server << F("</tr></n>");
  for (i = 1; i <= 6; ++i)
  {
    server << F("<td></p>");
    server << F("<select name='Buttons4F1") << i << F(":'div style='width:78px;>");
    server << F("<option value='0'></option>");

    server << F("<option value='") << 0 << F("'") << (EEPROM.read(400 + i) == 0 ? F(" selected='selected'") : F("")) << F("> \t") << F("</option>");

    // Preview bus inputs 1-20
    for (uint8_t j = 1; j <= 20; j++)  {
      server << F("<option value='") << j << F("'") << (EEPROM.read(400 + i) == j ? F(" selected='selected'") : F("")) << F(">PVW ") << j << F("</option>");
    }
    // Program bus inputs 1-20
    for (uint8_t j = 1; j <= 20; j++)  {
      server << F("<option value='") << j + 20 << F("'") << (EEPROM.read(400 + i) == j + 20 ? F(" selected='selected'") : F("")) << F(">PGM ") << j << F("</option>");
    }

    // Other input sources
    server << F("<option value='") << 41 << F("'") << (EEPROM.read(400 + i) == 41 ? F(" selected='selected'") : F("")) << F(">PvwBlack") << F("</option>");
    server << F("<option value='") << 42 << F("'") << (EEPROM.read(400 + i) == 42 ? F(" selected='selected'") : F("")) << F(">PgmBlack") << F("</option>");
    server << F("<option value='") << 43 << F("'") << (EEPROM.read(400 + i) == 43 ? F(" selected='selected'") : F("")) << F(">PvwBars") << F("</option>");
    server << F("<option value='") << 44 << F("'") << (EEPROM.read(400 + i) == 44 ? F(" selected='selected'") : F("")) << F(">PgmBars") << F("</option>");
    server << F("<option value='") << 45 << F("'") << (EEPROM.read(400 + i) == 45 ? F(" selected='selected'") : F("")) << F(">PvwColor1") << F("</option>");
    server << F("<option value='") << 46 << F("'") << (EEPROM.read(400 + i) == 46 ? F(" selected='selected'") : F("")) << F(">PgmColor1") << F("</option>");
    server << F("<option value='") << 47 << F("'") << (EEPROM.read(400 + i) == 47 ? F(" selected='selected'") : F("")) << F(">PvwColor2") << F("</option>");
    server << F("<option value='") << 48 << F("'") << (EEPROM.read(400 + i) == 48 ? F(" selected='selected'") : F("")) << F(">PgmColor2") << F("</option>");
    server << F("<option value='") << 49 << F("'") << (EEPROM.read(400 + i) == 49 ? F(" selected='selected'") : F("")) << F(">PvwMedia1") << F("</option>");
    server << F("<option value='") << 50 << F("'") << (EEPROM.read(400 + i) == 50 ? F(" selected='selected'") : F("")) << F(">PgmMedia1") << F("</option>");
    server << F("<option value='") << 51 << F("'") << (EEPROM.read(400 + i) == 51 ? F(" selected='selected'") : F("")) << F(">PvwMedia2") << F("</option>");
    server << F("<option value='") << 52 << F("'") << (EEPROM.read(400 + i) == 52 ? F(" selected='selected'") : F("")) << F(">PgmMedia2") << F("</option>");
    server << F("<option value='") << 53 << F("'") << (EEPROM.read(400 + i) == 53 ? F(" selected='selected'") : F("")) << F(">PvwSSource") << F("</option>");
    server << F("<option value='") << 54 << F("'") << (EEPROM.read(400 + i) == 54 ? F(" selected='selected'") : F("")) << F(">PgmSSource") << F("</option>");

    for (uint8_t j = 1; j <= 8; j++)
    {
      server << F("<option value='") << 54 + j << F("'") << (EEPROM.read(400 + i) == 54 + j ? F(" selected='selected'") : F("")) << F(">GPI ") << j << F("</option>");
    }

    server << F("<option value='") << 255 << F("'") << (EEPROM.read(400 + i) == 255 ? F(" selected='selected'") : F("")) << F(">Default") << F("</option>");

    server << F("</select></td>");
  }
  server << F("</tr></n>");

  // Set second routing for Bottom row buttons
  for (i = 1; i <= 6; ++i)
  {
    server << F("<td></p>");
    server << F("<select name='Buttons4F2") << i << F(":'div style='width:78px;>");
    server << F("<option value='0'></option>");

    server << F("<option value='") << 0 << F("'") << (EEPROM.read(500 + i) == 0 ? F(" selected='selected'") : F("")) << F("> \t") << F("</option>");

    // USK 1-4 on/off
    server << F("<option value='") << 1 << F("'") << (EEPROM.read(500 + i) == 1 ? F(" selected='selected'") : F("")) << F(">USK1") << F("</option>");
    server << F("<option value='") << 2 << F("'") << (EEPROM.read(500 + i) == 2 ? F(" selected='selected'") : F("")) << F(">USK2") << F("</option>");
    server << F("<option value='") << 3 << F("'") << (EEPROM.read(500 + i) == 3 ? F(" selected='selected'") : F("")) << F(">USK3") << F("</option>");
    server << F("<option value='") << 4 << F("'") << (EEPROM.read(500 + i) == 4 ? F(" selected='selected'") : F("")) << F(">USK4") << F("</option>");

    // DSK 1-2 on/off
    server << F("<option value='") << 5 << F("'") << (EEPROM.read(500 + i) == 5 ? F(" selected='selected'") : F("")) << F(">DSK1") << F("</option>");
    server << F("<option value='") << 6 << F("'") << (EEPROM.read(500 + i) == 6 ? F(" selected='selected'") : F("")) << F(">DSK2") << F("</option>");
    server << F("<option value='") << 7 << F("'") << (EEPROM.read(500 + i) == 7 ? F(" selected='selected'") : F("")) << F(">DSK1Auto") << F("</option>");
    server << F("<option value='") << 8 << F("'") << (EEPROM.read(500 + i) == 8 ? F(" selected='selected'") : F("")) << F(">DSK2Auto") << F("</option>");

    // Others
    server << F("<option value='") << 9 << F("'") << (EEPROM.read(500 + i) == 9 ? F(" selected='selected'") : F("")) << F(">Cut") << F("</option>");
    server << F("<option value='") << 10 << F("'") << (EEPROM.read(500 + i) == 10 ? F(" selected='selected'") : F("")) << F(">Auto") << F("</option>");
    server << F("<option value='") << 11 << F("'") << (EEPROM.read(500 + i) == 11 ? F(" selected='selected'") : F("")) << F(">FTB") << F("</option>");
    server << F("<option value='") << 12 << F("'") << (EEPROM.read(500 + i) == 12 ? F(" selected='selected'") : F("")) << F(">Keys Off") << F("</option>");

    for (uint8_t j = 1; j <= 8; j++)  {
      server << F("<option value='") << 12 + j << F("'") << (EEPROM.read(500 + i) == 12 + j ? F(" selected='selected'") : F("")) << F(">GPO ") << j << F("</option>");
    }
    for (uint8_t j = 1; j <= 8; j++)  {
      server << F("<option value='") << 20 + j << F("'") << (EEPROM.read(500 + i) == 20 + j ? F(" selected='selected'") : F("")) << F(">ToggleGPO ") << j << F("</option>");
    }

    server << F("</select></td>");
  }

  server << F("</tr></table><hr/>");
  ///////////////////////////////////////////////////

  // Slider:
  server << F("<h1>Slider:</h1><table border=0 cellspacing=3><tr>");
  server << F("<td></p>");
  server << F("<select name='slider") << F(":'div style='width:78px;>");
  server << F("<option value='0'></option>");

  server << F("<option value='") << 0 << F("'") << (EEPROM.read(99) == 0 ? F(" selected='selected'") : F("")) << F(">ATEM1ME") << F("</option>");
  server << F("<option value='") << 1 << F("'") << (EEPROM.read(99) == 1 ? F(" selected='selected'") : F("")) << F(">ATEM2ME") << F("</option>");

  server << F("</select></td>");
  server << F("</tr></n>");

  server << F("</tr></table><hr/>");


  //////////////////////////////////////////////////

  // Set first routing for GPIs:
  server << F("<h1>Set functions when GPI pins are triggered:</h1><table border=0 cellspacing=3><tr>");
  for (i = 1; i <= 8; ++i)
  {
    server << F("<td><p>GPI ") << i << F(":</p>");
    server << F("<select name='GPI1AT") << i << F(":'div style='width:78px;>");
    server << F("<option value='0'></option>");

    server << F("<option value='") << 0 << F("'") << (EEPROM.read(100) == 0 ? F(" selected='selected'") : F("")) << F(">ATEM1ME") << F("</option>");
    server << F("<option value='") << 1 << F("'") << (EEPROM.read(100) == 1 ? F(" selected='selected'") : F("")) << F(">ATEM2ME") << F("</option>");

    server << F("</select></td>");
  }
  server << F("</tr></n>");

  for (i = 1; i <= 8; ++i)
  {
    server << F("<td></p>");
    server << F("<select name='GPI1") << i << F(":'div style='width:78px;>");
    server << F("<option value='0'></option>");

    server << F("<option value='") << 0 << F("'") << (EEPROM.read(200 + i) == 0 ? F(" selected='selected'") : F("")) << F("> \t") << F("</option>");

    // Preview 1-20
    for (uint8_t j = 1; j <= 20; j++)  {
      server << F("<option value='") << j << F("'") << (EEPROM.read(200 + i) == j ? F(" selected='selected'") : F("")) << F(">PVW ") << j << F("</option>");
    }
    // Program 1-16
    for (uint8_t j = 1; j <= 20; j++)  {
      server << F("<option value='") << j + 20 << F("'") << (EEPROM.read(200 + i) == j + 20 ? F(" selected='selected'") : F("")) << F(">PGM ") << j << F("</option>");
    }

    // Other input sources
    server << F("<option value='") << 41 << F("'") << (EEPROM.read(200 + i) == 41 ? F(" selected='selected'") : F("")) << F(">PvwBlack") << F("</option>");
    server << F("<option value='") << 42 << F("'") << (EEPROM.read(200 + i) == 42 ? F(" selected='selected'") : F("")) << F(">PgmBlack") << F("</option>");
    server << F("<option value='") << 43 << F("'") << (EEPROM.read(200 + i) == 43 ? F(" selected='selected'") : F("")) << F(">PvwBars") << F("</option>");
    server << F("<option value='") << 44 << F("'") << (EEPROM.read(200 + i) == 44 ? F(" selected='selected'") : F("")) << F(">PgmBars") << F("</option>");
    server << F("<option value='") << 45 << F("'") << (EEPROM.read(200 + i) == 45 ? F(" selected='selected'") : F("")) << F(">PvwColor1") << F("</option>");
    server << F("<option value='") << 46 << F("'") << (EEPROM.read(200 + i) == 46 ? F(" selected='selected'") : F("")) << F(">PgmColor1") << F("</option>");
    server << F("<option value='") << 47 << F("'") << (EEPROM.read(200 + i) == 47 ? F(" selected='selected'") : F("")) << F(">PvwColor2") << F("</option>");
    server << F("<option value='") << 48 << F("'") << (EEPROM.read(200 + i) == 48 ? F(" selected='selected'") : F("")) << F(">PgmColor2") << F("</option>");
    server << F("<option value='") << 49 << F("'") << (EEPROM.read(200 + i) == 49 ? F(" selected='selected'") : F("")) << F(">PvwMedia1") << F("</option>");
    server << F("<option value='") << 50 << F("'") << (EEPROM.read(200 + i) == 50 ? F(" selected='selected'") : F("")) << F(">PgmMedia1") << F("</option>");
    server << F("<option value='") << 51 << F("'") << (EEPROM.read(200 + i) == 51 ? F(" selected='selected'") : F("")) << F(">PvwMedia2") << F("</option>");
    server << F("<option value='") << 52 << F("'") << (EEPROM.read(200 + i) == 52 ? F(" selected='selected'") : F("")) << F(">PgmMedia2") << F("</option>");
    server << F("<option value='") << 53 << F("'") << (EEPROM.read(200 + i) == 53 ? F(" selected='selected'") : F("")) << F(">PvwSSource") << F("</option>");
    server << F("<option value='") << 54 << F("'") << (EEPROM.read(200 + i) == 54 ? F(" selected='selected'") : F("")) << F(">PgmSSource") << F("</option>");

    server << F("<option value='") << 53 << F("'") << (EEPROM.read(200 + i) == 53 ? F(" selected='selected'") : F("")) << F(">Cut") << F("</option>");
    server << F("<option value='") << 54 << F("'") << (EEPROM.read(200 + i) == 54 ? F(" selected='selected'") : F("")) << F(">Auto") << F("</option>");
    server << F("<option value='") << 55 << F("'") << (EEPROM.read(200 + i) == 55 ? F(" selected='selected'") : F("")) << F(">FTB") << F("</option>");

    server << F("</select></td>");
  }
  server << F("</tr></n>");

  // Set second routing for GPIs:
  //  server << F("<h1>Set secondary functions when GPI pins are triggered:</h1><table border=0 cellspacing=3><tr>");
  for (i = 1; i <= 8; ++i)
  {
    server << F("<td></p>");
    server << F("<select name='GPI2") << i << F(":'div style='width:78px;>");
    server << F("<option value='0'></option>");

    server << F("<option value='") << 0 << F("'") << (EEPROM.read(210 + i) == 0 ? F(" selected='selected'") : F("")) << F("> \t") << F("</option>");

    // USK 1-4 on/off
    server << F("<option value='") << 1 << F("'") << (EEPROM.read(210 + i) == 1 ? F(" selected='selected'") : F("")) << F(">USK1") << F("</option>");
    server << F("<option value='") << 2 << F("'") << (EEPROM.read(210 + i) == 2 ? F(" selected='selected'") : F("")) << F(">USK2") << F("</option>");
    server << F("<option value='") << 3 << F("'") << (EEPROM.read(210 + i) == 3 ? F(" selected='selected'") : F("")) << F(">USK3") << F("</option>");
    server << F("<option value='") << 4 << F("'") << (EEPROM.read(210 + i) == 4 ? F(" selected='selected'") : F("")) << F(">USK4") << F("</option>");

    // USK 1-4 on and off

    server << F("<option value='") << 5 << F("'") << (EEPROM.read(210 + i) == 5 ? F(" selected='selected'") : F("")) << F(">USK1 on") << F("</option>");
    server << F("<option value='") << 6 << F("'") << (EEPROM.read(210 + i) == 6 ? F(" selected='selected'") : F("")) << F(">USK1 off") << F("</option>");
    server << F("<option value='") << 7 << F("'") << (EEPROM.read(210 + i) == 7 ? F(" selected='selected'") : F("")) << F(">USK2 on") << F("</option>");
    server << F("<option value='") << 8 << F("'") << (EEPROM.read(210 + i) == 8 ? F(" selected='selected'") : F("")) << F(">USK2 off") << F("</option>");
    server << F("<option value='") << 9 << F("'") << (EEPROM.read(210 + i) == 9 ? F(" selected='selected'") : F("")) << F(">USK3 on") << F("</option>");
    server << F("<option value='") << 10 << F("'") << (EEPROM.read(210 + i) == 10 ? F(" selected='selected'") : F("")) << F(">USK3 off") << F("</option>");
    server << F("<option value='") << 11 << F("'") << (EEPROM.read(210 + i) == 11 ? F(" selected='selected'") : F("")) << F(">USK4 on") << F("</option>");
    server << F("<option value='") << 12 << F("'") << (EEPROM.read(210 + i) == 12 ? F(" selected='selected'") : F("")) << F(">USK4 off") << F("</option>");

    // DSK 1-2 on/off
    server << F("<option value='") << 13 << F("'") << (EEPROM.read(210 + i) == 13 ? F(" selected='selected'") : F("")) << F(">DSK1") << F("</option>");
    server << F("<option value='") << 14 << F("'") << (EEPROM.read(210 + i) == 14 ? F(" selected='selected'") : F("")) << F(">DSK2") << F("</option>");

    server << F("<option value='") << 15 << F("'") << (EEPROM.read(210 + i) == 15 ? F(" selected='selected'") : F("")) << F(">DSK1Auto") << F("</option>");
    server << F("<option value='") << 16 << F("'") << (EEPROM.read(210 + i) == 16 ? F(" selected='selected'") : F("")) << F(">DSK2Auto") << F("</option>");

    // DSK 1-2 on and off
    server << F("<option value='") << 17 << F("'") << (EEPROM.read(210 + i) == 17 ? F(" selected='selected'") : F("")) << F(">DSK1 on") << F("</option>");
    server << F("<option value='") << 18 << F("'") << (EEPROM.read(210 + i) == 18 ? F(" selected='selected'") : F("")) << F(">DSK1 off") << F("</option>");
    server << F("<option value='") << 19 << F("'") << (EEPROM.read(210 + i) == 19 ? F(" selected='selected'") : F("")) << F(">DSK2 on") << F("</option>");
    server << F("<option value='") << 20 << F("'") << (EEPROM.read(210 + i) == 20 ? F(" selected='selected'") : F("")) << F(">DSK2 off") << F("</option>");

    // Others
    server << F("<option value='") << 21 << F("'") << (EEPROM.read(210 + i) == 21 ? F(" selected='selected'") : F("")) << F(">Cut") << F("</option>");
    server << F("<option value='") << 22 << F("'") << (EEPROM.read(210 + i) == 22 ? F(" selected='selected'") : F("")) << F(">Auto") << F("</option>");
    server << F("<option value='") << 23 << F("'") << (EEPROM.read(210 + i) == 23 ? F(" selected='selected'") : F("")) << F(">FTB") << F("</option>");
    server << F("<option value='") << 24 << F("'") << (EEPROM.read(210 + i) == 24 ? F(" selected='selected'") : F("")) << F(">Keys Off") << F("</option>");

    server << F("</select></td>");
  }
  server << F("</tr></n>");

  // Set third routing for GPIs:
  //  server << F("<h1>Set tertiary functions when GPI pins are triggered:</h1><table border=0 cellspacing=3><tr>");
  for (i = 1; i <= 8; ++i)
  {
    server << F("<td></p>");
    server << F("<select name='GPI3") << i << F(":'div style='width:78px;>");
    server << F("<option value='0'></option>");

    server << F("<option value='") << 0 << F("'") << (EEPROM.read(220 + i) == 0 ? F(" selected='selected'") : F("")) << F("> \t") << F("</option>");

    // USK 1-4 on/off
    server << F("<option value='") << 1 << F("'") << (EEPROM.read(220 + i) == 1 ? F(" selected='selected'") : F("")) << F(">USK1") << F("</option>");
    server << F("<option value='") << 2 << F("'") << (EEPROM.read(220 + i) == 2 ? F(" selected='selected'") : F("")) << F(">USK2") << F("</option>");
    server << F("<option value='") << 3 << F("'") << (EEPROM.read(220 + i) == 3 ? F(" selected='selected'") : F("")) << F(">USK3") << F("</option>");
    server << F("<option value='") << 4 << F("'") << (EEPROM.read(220 + i) == 4 ? F(" selected='selected'") : F("")) << F(">USK4") << F("</option>");

    // USK 1-4 on and off

    server << F("<option value='") << 5 << F("'") << (EEPROM.read(220 + i) == 5 ? F(" selected='selected'") : F("")) << F(">USK1 on") << F("</option>");
    server << F("<option value='") << 6 << F("'") << (EEPROM.read(220 + i) == 6 ? F(" selected='selected'") : F("")) << F(">USK1 off") << F("</option>");
    server << F("<option value='") << 7 << F("'") << (EEPROM.read(220 + i) == 7 ? F(" selected='selected'") : F("")) << F(">USK2 on") << F("</option>");
    server << F("<option value='") << 8 << F("'") << (EEPROM.read(220 + i) == 8 ? F(" selected='selected'") : F("")) << F(">USK2 off") << F("</option>");
    server << F("<option value='") << 9 << F("'") << (EEPROM.read(220 + i) == 9 ? F(" selected='selected'") : F("")) << F(">USK3 on") << F("</option>");
    server << F("<option value='") << 10 << F("'") << (EEPROM.read(220 + i) == 10 ? F(" selected='selected'") : F("")) << F(">USK3 off") << F("</option>");
    server << F("<option value='") << 11 << F("'") << (EEPROM.read(220 + i) == 11 ? F(" selected='selected'") : F("")) << F(">USK4 on") << F("</option>");
    server << F("<option value='") << 12 << F("'") << (EEPROM.read(220 + i) == 12 ? F(" selected='selected'") : F("")) << F(">USK4 off") << F("</option>");

    // DSK 1-2 on/off
    server << F("<option value='") << 13 << F("'") << (EEPROM.read(220 + i) == 13 ? F(" selected='selected'") : F("")) << F(">DSK1") << F("</option>");
    server << F("<option value='") << 14 << F("'") << (EEPROM.read(220 + i) == 14 ? F(" selected='selected'") : F("")) << F(">DSK2") << F("</option>");
    server << F("<option value='") << 15 << F("'") << (EEPROM.read(220 + i) == 15 ? F(" selected='selected'") : F("")) << F(">DSK1Auto") << F("</option>");
    server << F("<option value='") << 16 << F("'") << (EEPROM.read(220 + i) == 16 ? F(" selected='selected'") : F("")) << F(">DSK2Auto") << F("</option>");

    // DSK 1-2 on and off
    server << F("<option value='") << 17 << F("'") << (EEPROM.read(220 + i) == 17 ? F(" selected='selected'") : F("")) << F(">DSK1 on") << F("</option>");
    server << F("<option value='") << 18 << F("'") << (EEPROM.read(220 + i) == 18 ? F(" selected='selected'") : F("")) << F(">DSK1 off") << F("</option>");
    server << F("<option value='") << 19 << F("'") << (EEPROM.read(220 + i) == 19 ? F(" selected='selected'") : F("")) << F(">DSK2 on") << F("</option>");
    server << F("<option value='") << 20 << F("'") << (EEPROM.read(220 + i) == 20 ? F(" selected='selected'") : F("")) << F(">DSK2 off") << F("</option>");

    // Others
    server << F("<option value='") << 21 << F("'") << (EEPROM.read(220 + i) == 21 ? F(" selected='selected'") : F("")) << F(">Cut") << F("</option>");
    server << F("<option value='") << 22 << F("'") << (EEPROM.read(220 + i) == 22 ? F(" selected='selected'") : F("")) << F(">Auto") << F("</option>");
    server << F("<option value='") << 23 << F("'") << (EEPROM.read(220 + i) == 23 ? F(" selected='selected'") : F("")) << F(">FTB") << F("</option>");
    server << F("<option value='") << 24 << F("'") << (EEPROM.read(220 + i) == 24 ? F(" selected='selected'") : F("")) << F(">Keys Off") << F("</option>");

    server << F("</select></td>");
  }
  server << F("</tr></n>");

  ////////////////////////

  // Aux 1
  server << F("<table border=0 cellspacing=3><tr>");
  server << F("<h2>Aux1</h2><p>");
  for (i = 1; i <= 8; ++i)
  {
    server << F("<td>");
    server << F("<select name='GPIA1") << i << F(":'div style='width:78px;>");
    server << F("<option value='0'></option>");

    server << F("<option value='") << 0 << F("'") << (EEPROM.read(230 + i) == 0 ? F(" selected='selected'") : F("")) << F("> \t") << F("</option>");
    server << F("<option value='") << 1 << F("'") << (EEPROM.read(230 + i) == 1 ? F(" selected='selected'") : F("")) << F(">Camera1") << F("</option>");
    server << F("<option value='") << 2 << F("'") << (EEPROM.read(230 + i) == 2 ? F(" selected='selected'") : F("")) << F(">Camera2") << F("</option>");
    server << F("<option value='") << 3 << F("'") << (EEPROM.read(230 + i) == 3 ? F(" selected='selected'") : F("")) << F(">Camera3") << F("</option>");
    server << F("<option value='") << 4 << F("'") << (EEPROM.read(230 + i) == 4 ? F(" selected='selected'") : F("")) << F(">Camera4") << F("</option>");
    server << F("<option value='") << 5 << F("'") << (EEPROM.read(230 + i) == 5 ? F(" selected='selected'") : F("")) << F(">Camera5") << F("</option>");
    server << F("<option value='") << 6 << F("'") << (EEPROM.read(230 + i) == 6 ? F(" selected='selected'") : F("")) << F(">Camera6") << F("</option>");
    server << F("<option value='") << 7 << F("'") << (EEPROM.read(230 + i) == 7 ? F(" selected='selected'") : F("")) << F(">Camera7") << F("</option>");
    server << F("<option value='") << 8 << F("'") << (EEPROM.read(230 + i) == 8 ? F(" selected='selected'") : F("")) << F(">Camera8") << F("</option>");
    server << F("<option value='") << 9 << F("'") << (EEPROM.read(230 + i) == 9 ? F(" selected='selected'") : F("")) << F(">Camera9") << F("</option>");
    server << F("<option value='") << 10 << F("'") << (EEPROM.read(230 + i) == 10 ? F(" selected='selected'") : F("")) << F(">Camera10") << F("</option>");
    server << F("<option value='") << 11 << F("'") << (EEPROM.read(230 + i) == 11 ? F(" selected='selected'") : F("")) << F(">Camera11") << F("</option>");
    server << F("<option value='") << 12 << F("'") << (EEPROM.read(230 + i) == 12 ? F(" selected='selected'") : F("")) << F(">Camera12") << F("</option>");
    server << F("<option value='") << 13 << F("'") << (EEPROM.read(230 + i) == 13 ? F(" selected='selected'") : F("")) << F(">Camera13") << F("</option>");
    server << F("<option value='") << 14 << F("'") << (EEPROM.read(230 + i) == 14 ? F(" selected='selected'") : F("")) << F(">Camera14") << F("</option>");
    server << F("<option value='") << 15 << F("'") << (EEPROM.read(230 + i) == 15 ? F(" selected='selected'") : F("")) << F(">Camera15") << F("</option>");
    server << F("<option value='") << 16 << F("'") << (EEPROM.read(230 + i) == 16 ? F(" selected='selected'") : F("")) << F(">Camera16") << F("</option>");
    server << F("<option value='") << 17 << F("'") << (EEPROM.read(230 + i) == 17 ? F(" selected='selected'") : F("")) << F(">Camera17") << F("</option>");
    server << F("<option value='") << 18 << F("'") << (EEPROM.read(230 + i) == 18 ? F(" selected='selected'") : F("")) << F(">Camera18") << F("</option>");
    server << F("<option value='") << 19 << F("'") << (EEPROM.read(230 + i) == 19 ? F(" selected='selected'") : F("")) << F(">Camera19") << F("</option>");
    server << F("<option value='") << 20 << F("'") << (EEPROM.read(230 + i) == 20 ? F(" selected='selected'") : F("")) << F(">Camera20") << F("</option>");
    server << F("<option value='") << 21 << F("'") << (EEPROM.read(230 + i) == 21 ? F(" selected='selected'") : F("")) << F(">Black") << F("</option>");
    server << F("<option value='") << 22 << F("'") << (EEPROM.read(230 + i) == 22 ? F(" selected='selected'") : F("")) << F(">Bars") << F("</option>");
    server << F("<option value='") << 23 << F("'") << (EEPROM.read(230 + i) == 23 ? F(" selected='selected'") : F("")) << F(">Color1") << F("</option>");
    server << F("<option value='") << 24 << F("'") << (EEPROM.read(230 + i) == 24 ? F(" selected='selected'") : F("")) << F(">Color2") << F("</option>");
    server << F("<option value='") << 25 << F("'") << (EEPROM.read(230 + i) == 25 ? F(" selected='selected'") : F("")) << F(">MPL1") << F("</option>");
    server << F("<option value='") << 26 << F("'") << (EEPROM.read(230 + i) == 26 ? F(" selected='selected'") : F("")) << F(">MPL1-K") << F("</option>");
    server << F("<option value='") << 27 << F("'") << (EEPROM.read(230 + i) == 27 ? F(" selected='selected'") : F("")) << F(">MPL2") << F("</option>");
    server << F("<option value='") << 28 << F("'") << (EEPROM.read(230 + i) == 28 ? F(" selected='selected'") : F("")) << F(">MPL2-K") << F("</option>");
    server << F("<option value='") << 29 << F("'") << (EEPROM.read(230 + i) == 29 ? F(" selected='selected'") : F("")) << F(">CleanFeed1") << F("</option>");
    server << F("<option value='") << 30 << F("'") << (EEPROM.read(230 + i) == 30 ? F(" selected='selected'") : F("")) << F(">CleanFeed2") << F("</option>");
    server << F("<option value='") << 31 << F("'") << (EEPROM.read(230 + i) == 31 ? F(" selected='selected'") : F("")) << F(">SuperSource") << F("</option>");
    server << F("<option value='") << 32 << F("'") << (EEPROM.read(230 + i) == 32 ? F(" selected='selected'") : F("")) << F(">Program") << F("</option>");
    server << F("<option value='") << 33 << F("'") << (EEPROM.read(230 + i) == 33 ? F(" selected='selected'") : F("")) << F(">Preview") << F("</option>");

    server << F("</select></td>");
  }
  server << F("</tr></n>");

  // Aux 2
  server << F("<table border=0 cellspacing=3><tr>");
  server << F("<h2>Aux2</h2><p>");
  for (i = 1; i <= 8; ++i)
  {
    server << F("<td>");
    server << F("<select name='GPIA2") << i << F(":'div style='width:78px;>");
    server << F("<option value='0'></option>");

    server << F("<option value='") << 0 << F("'") << (EEPROM.read(240 + i) == 0 ? F(" selected='selected'") : F("")) << F("> \t") << F("</option>");
    server << F("<option value='") << 1 << F("'") << (EEPROM.read(240 + i) == 1 ? F(" selected='selected'") : F("")) << F(">Camera1") << F("</option>");
    server << F("<option value='") << 2 << F("'") << (EEPROM.read(240 + i) == 2 ? F(" selected='selected'") : F("")) << F(">Camera2") << F("</option>");
    server << F("<option value='") << 3 << F("'") << (EEPROM.read(240 + i) == 3 ? F(" selected='selected'") : F("")) << F(">Camera3") << F("</option>");
    server << F("<option value='") << 4 << F("'") << (EEPROM.read(240 + i) == 4 ? F(" selected='selected'") : F("")) << F(">Camera4") << F("</option>");
    server << F("<option value='") << 5 << F("'") << (EEPROM.read(240 + i) == 5 ? F(" selected='selected'") : F("")) << F(">Camera5") << F("</option>");
    server << F("<option value='") << 6 << F("'") << (EEPROM.read(240 + i) == 6 ? F(" selected='selected'") : F("")) << F(">Camera6") << F("</option>");
    server << F("<option value='") << 7 << F("'") << (EEPROM.read(240 + i) == 7 ? F(" selected='selected'") : F("")) << F(">Camera7") << F("</option>");
    server << F("<option value='") << 8 << F("'") << (EEPROM.read(240 + i) == 8 ? F(" selected='selected'") : F("")) << F(">Camera8") << F("</option>");
    server << F("<option value='") << 9 << F("'") << (EEPROM.read(240 + i) == 9 ? F(" selected='selected'") : F("")) << F(">Camera9") << F("</option>");
    server << F("<option value='") << 10 << F("'") << (EEPROM.read(240 + i) == 10 ? F(" selected='selected'") : F("")) << F(">Camera10") << F("</option>");
    server << F("<option value='") << 11 << F("'") << (EEPROM.read(240 + i) == 11 ? F(" selected='selected'") : F("")) << F(">Camera11") << F("</option>");
    server << F("<option value='") << 12 << F("'") << (EEPROM.read(240 + i) == 12 ? F(" selected='selected'") : F("")) << F(">Camera12") << F("</option>");
    server << F("<option value='") << 13 << F("'") << (EEPROM.read(240 + i) == 13 ? F(" selected='selected'") : F("")) << F(">Camera13") << F("</option>");
    server << F("<option value='") << 14 << F("'") << (EEPROM.read(240 + i) == 14 ? F(" selected='selected'") : F("")) << F(">Camera14") << F("</option>");
    server << F("<option value='") << 15 << F("'") << (EEPROM.read(240 + i) == 15 ? F(" selected='selected'") : F("")) << F(">Camera15") << F("</option>");
    server << F("<option value='") << 16 << F("'") << (EEPROM.read(240 + i) == 16 ? F(" selected='selected'") : F("")) << F(">Camera16") << F("</option>");
    server << F("<option value='") << 17 << F("'") << (EEPROM.read(240 + i) == 17 ? F(" selected='selected'") : F("")) << F(">Camera17") << F("</option>");
    server << F("<option value='") << 18 << F("'") << (EEPROM.read(240 + i) == 18 ? F(" selected='selected'") : F("")) << F(">Camera18") << F("</option>");
    server << F("<option value='") << 19 << F("'") << (EEPROM.read(240 + i) == 19 ? F(" selected='selected'") : F("")) << F(">Camera19") << F("</option>");
    server << F("<option value='") << 20 << F("'") << (EEPROM.read(240 + i) == 20 ? F(" selected='selected'") : F("")) << F(">Camera20") << F("</option>");
    server << F("<option value='") << 21 << F("'") << (EEPROM.read(240 + i) == 21 ? F(" selected='selected'") : F("")) << F(">Black") << F("</option>");
    server << F("<option value='") << 22 << F("'") << (EEPROM.read(240 + i) == 22 ? F(" selected='selected'") : F("")) << F(">Bars") << F("</option>");
    server << F("<option value='") << 23 << F("'") << (EEPROM.read(240 + i) == 23 ? F(" selected='selected'") : F("")) << F(">Color1") << F("</option>");
    server << F("<option value='") << 24 << F("'") << (EEPROM.read(240 + i) == 24 ? F(" selected='selected'") : F("")) << F(">Color2") << F("</option>");
    server << F("<option value='") << 25 << F("'") << (EEPROM.read(240 + i) == 25 ? F(" selected='selected'") : F("")) << F(">MPL1") << F("</option>");
    server << F("<option value='") << 26 << F("'") << (EEPROM.read(240 + i) == 26 ? F(" selected='selected'") : F("")) << F(">MPL1-K") << F("</option>");
    server << F("<option value='") << 27 << F("'") << (EEPROM.read(240 + i) == 27 ? F(" selected='selected'") : F("")) << F(">MPL2") << F("</option>");
    server << F("<option value='") << 28 << F("'") << (EEPROM.read(240 + i) == 28 ? F(" selected='selected'") : F("")) << F(">MPL2-K") << F("</option>");
    server << F("<option value='") << 29 << F("'") << (EEPROM.read(240 + i) == 29 ? F(" selected='selected'") : F("")) << F(">CleanFeed1") << F("</option>");
    server << F("<option value='") << 30 << F("'") << (EEPROM.read(240 + i) == 30 ? F(" selected='selected'") : F("")) << F(">CleanFeed2") << F("</option>");
    server << F("<option value='") << 31 << F("'") << (EEPROM.read(240 + i) == 31 ? F(" selected='selected'") : F("")) << F(">SuperSource") << F("</option>");
    server << F("<option value='") << 32 << F("'") << (EEPROM.read(240 + i) == 32 ? F(" selected='selected'") : F("")) << F(">Program") << F("</option>");
    server << F("<option value='") << 33 << F("'") << (EEPROM.read(240 + i) == 33 ? F(" selected='selected'") : F("")) << F(">Preview") << F("</option>");

    server << F("</select></td>");
  }
  server << F("</tr></n>");

  // Aux 3
  server << F("<table border=0 cellspacing=3><tr>");
  server << F("<h2>Aux3</h2><p>");
  for (i = 1; i <= 8; ++i)
  {
    server << F("<td>");
    server << F("<select name='GPIA3") << i << F(":'div style='width:78px;>");
    server << F("<option value='0'></option>");

    server << F("<option value='") << 0 << F("'") << (EEPROM.read(250 + i) == 0 ? F(" selected='selected'") : F("")) << F("> \t") << F("</option>");
    server << F("<option value='") << 1 << F("'") << (EEPROM.read(250 + i) == 1 ? F(" selected='selected'") : F("")) << F(">Camera1") << F("</option>");
    server << F("<option value='") << 2 << F("'") << (EEPROM.read(250 + i) == 2 ? F(" selected='selected'") : F("")) << F(">Camera2") << F("</option>");
    server << F("<option value='") << 3 << F("'") << (EEPROM.read(250 + i) == 3 ? F(" selected='selected'") : F("")) << F(">Camera3") << F("</option>");
    server << F("<option value='") << 4 << F("'") << (EEPROM.read(250 + i) == 4 ? F(" selected='selected'") : F("")) << F(">Camera4") << F("</option>");
    server << F("<option value='") << 5 << F("'") << (EEPROM.read(250 + i) == 5 ? F(" selected='selected'") : F("")) << F(">Camera5") << F("</option>");
    server << F("<option value='") << 6 << F("'") << (EEPROM.read(250 + i) == 6 ? F(" selected='selected'") : F("")) << F(">Camera6") << F("</option>");
    server << F("<option value='") << 7 << F("'") << (EEPROM.read(250 + i) == 7 ? F(" selected='selected'") : F("")) << F(">Camera7") << F("</option>");
    server << F("<option value='") << 8 << F("'") << (EEPROM.read(250 + i) == 8 ? F(" selected='selected'") : F("")) << F(">Camera8") << F("</option>");
    server << F("<option value='") << 9 << F("'") << (EEPROM.read(250 + i) == 9 ? F(" selected='selected'") : F("")) << F(">Camera9") << F("</option>");
    server << F("<option value='") << 10 << F("'") << (EEPROM.read(250 + i) == 10 ? F(" selected='selected'") : F("")) << F(">Camera10") << F("</option>");
    server << F("<option value='") << 11 << F("'") << (EEPROM.read(250 + i) == 11 ? F(" selected='selected'") : F("")) << F(">Camera11") << F("</option>");
    server << F("<option value='") << 12 << F("'") << (EEPROM.read(250 + i) == 12 ? F(" selected='selected'") : F("")) << F(">Camera12") << F("</option>");
    server << F("<option value='") << 13 << F("'") << (EEPROM.read(250 + i) == 13 ? F(" selected='selected'") : F("")) << F(">Camera13") << F("</option>");
    server << F("<option value='") << 14 << F("'") << (EEPROM.read(250 + i) == 14 ? F(" selected='selected'") : F("")) << F(">Camera14") << F("</option>");
    server << F("<option value='") << 15 << F("'") << (EEPROM.read(250 + i) == 15 ? F(" selected='selected'") : F("")) << F(">Camera15") << F("</option>");
    server << F("<option value='") << 16 << F("'") << (EEPROM.read(250 + i) == 16 ? F(" selected='selected'") : F("")) << F(">Camera16") << F("</option>");
    server << F("<option value='") << 17 << F("'") << (EEPROM.read(250 + i) == 17 ? F(" selected='selected'") : F("")) << F(">Camera17") << F("</option>");
    server << F("<option value='") << 18 << F("'") << (EEPROM.read(250 + i) == 18 ? F(" selected='selected'") : F("")) << F(">Camera18") << F("</option>");
    server << F("<option value='") << 19 << F("'") << (EEPROM.read(250 + i) == 19 ? F(" selected='selected'") : F("")) << F(">Camera19") << F("</option>");
    server << F("<option value='") << 20 << F("'") << (EEPROM.read(250 + i) == 20 ? F(" selected='selected'") : F("")) << F(">Camera20") << F("</option>");
    server << F("<option value='") << 21 << F("'") << (EEPROM.read(250 + i) == 21 ? F(" selected='selected'") : F("")) << F(">Black") << F("</option>");
    server << F("<option value='") << 22 << F("'") << (EEPROM.read(250 + i) == 22 ? F(" selected='selected'") : F("")) << F(">Bars") << F("</option>");
    server << F("<option value='") << 23 << F("'") << (EEPROM.read(250 + i) == 23 ? F(" selected='selected'") : F("")) << F(">Color1") << F("</option>");
    server << F("<option value='") << 24 << F("'") << (EEPROM.read(250 + i) == 24 ? F(" selected='selected'") : F("")) << F(">Color2") << F("</option>");
    server << F("<option value='") << 25 << F("'") << (EEPROM.read(250 + i) == 25 ? F(" selected='selected'") : F("")) << F(">MPL1") << F("</option>");
    server << F("<option value='") << 26 << F("'") << (EEPROM.read(250 + i) == 26 ? F(" selected='selected'") : F("")) << F(">MPL1-K") << F("</option>");
    server << F("<option value='") << 27 << F("'") << (EEPROM.read(250 + i) == 27 ? F(" selected='selected'") : F("")) << F(">MPL2") << F("</option>");
    server << F("<option value='") << 28 << F("'") << (EEPROM.read(250 + i) == 28 ? F(" selected='selected'") : F("")) << F(">MPL2-K") << F("</option>");
    server << F("<option value='") << 29 << F("'") << (EEPROM.read(250 + i) == 29 ? F(" selected='selected'") : F("")) << F(">CleanFeed1") << F("</option>");
    server << F("<option value='") << 30 << F("'") << (EEPROM.read(250 + i) == 30 ? F(" selected='selected'") : F("")) << F(">CleanFeed2") << F("</option>");
    server << F("<option value='") << 31 << F("'") << (EEPROM.read(250 + i) == 31 ? F(" selected='selected'") : F("")) << F(">SuperSource") << F("</option>");
    server << F("<option value='") << 32 << F("'") << (EEPROM.read(250 + i) == 32 ? F(" selected='selected'") : F("")) << F(">Program") << F("</option>");
    server << F("<option value='") << 33 << F("'") << (EEPROM.read(250 + i) == 33 ? F(" selected='selected'") : F("")) << F(">Preview") << F("</option>");

    server << F("</select></td>");
  }
  server << F("</tr></n>");

  // Aux 4
  server << F("<table border=0 cellspacing=3><tr>");
  server << F("<h2>Aux4</h2><p>");
  for (i = 1; i <= 8; ++i)
  {
    server << F("<td>");
    server << F("<select name='GPIA4") << i << F(":'div style='width:78px;>");
    server << F("<option value='0'></option>");

    server << F("<option value='") << 0 << F("'") << (EEPROM.read(260 + i) == 0 ? F(" selected='selected'") : F("")) << F("> \t") << F("</option>");
    server << F("<option value='") << 1 << F("'") << (EEPROM.read(260 + i) == 1 ? F(" selected='selected'") : F("")) << F(">Camera1") << F("</option>");
    server << F("<option value='") << 2 << F("'") << (EEPROM.read(260 + i) == 2 ? F(" selected='selected'") : F("")) << F(">Camera2") << F("</option>");
    server << F("<option value='") << 3 << F("'") << (EEPROM.read(260 + i) == 3 ? F(" selected='selected'") : F("")) << F(">Camera3") << F("</option>");
    server << F("<option value='") << 4 << F("'") << (EEPROM.read(260 + i) == 4 ? F(" selected='selected'") : F("")) << F(">Camera4") << F("</option>");
    server << F("<option value='") << 5 << F("'") << (EEPROM.read(260 + i) == 5 ? F(" selected='selected'") : F("")) << F(">Camera5") << F("</option>");
    server << F("<option value='") << 6 << F("'") << (EEPROM.read(260 + i) == 6 ? F(" selected='selected'") : F("")) << F(">Camera6") << F("</option>");
    server << F("<option value='") << 7 << F("'") << (EEPROM.read(260 + i) == 7 ? F(" selected='selected'") : F("")) << F(">Camera7") << F("</option>");
    server << F("<option value='") << 8 << F("'") << (EEPROM.read(260 + i) == 8 ? F(" selected='selected'") : F("")) << F(">Camera8") << F("</option>");
    server << F("<option value='") << 9 << F("'") << (EEPROM.read(260 + i) == 9 ? F(" selected='selected'") : F("")) << F(">Camera9") << F("</option>");
    server << F("<option value='") << 10 << F("'") << (EEPROM.read(260 + i) == 10 ? F(" selected='selected'") : F("")) << F(">Camera10") << F("</option>");
    server << F("<option value='") << 11 << F("'") << (EEPROM.read(260 + i) == 11 ? F(" selected='selected'") : F("")) << F(">Camera11") << F("</option>");
    server << F("<option value='") << 12 << F("'") << (EEPROM.read(260 + i) == 12 ? F(" selected='selected'") : F("")) << F(">Camera12") << F("</option>");
    server << F("<option value='") << 13 << F("'") << (EEPROM.read(260 + i) == 13 ? F(" selected='selected'") : F("")) << F(">Camera13") << F("</option>");
    server << F("<option value='") << 14 << F("'") << (EEPROM.read(260 + i) == 14 ? F(" selected='selected'") : F("")) << F(">Camera14") << F("</option>");
    server << F("<option value='") << 15 << F("'") << (EEPROM.read(260 + i) == 15 ? F(" selected='selected'") : F("")) << F(">Camera15") << F("</option>");
    server << F("<option value='") << 16 << F("'") << (EEPROM.read(260 + i) == 16 ? F(" selected='selected'") : F("")) << F(">Camera16") << F("</option>");
    server << F("<option value='") << 17 << F("'") << (EEPROM.read(260 + i) == 17 ? F(" selected='selected'") : F("")) << F(">Camera17") << F("</option>");
    server << F("<option value='") << 18 << F("'") << (EEPROM.read(260 + i) == 18 ? F(" selected='selected'") : F("")) << F(">Camera18") << F("</option>");
    server << F("<option value='") << 19 << F("'") << (EEPROM.read(260 + i) == 19 ? F(" selected='selected'") : F("")) << F(">Camera19") << F("</option>");
    server << F("<option value='") << 20 << F("'") << (EEPROM.read(260 + i) == 20 ? F(" selected='selected'") : F("")) << F(">Camera20") << F("</option>");
    server << F("<option value='") << 21 << F("'") << (EEPROM.read(260 + i) == 21 ? F(" selected='selected'") : F("")) << F(">Black") << F("</option>");
    server << F("<option value='") << 22 << F("'") << (EEPROM.read(260 + i) == 22 ? F(" selected='selected'") : F("")) << F(">Bars") << F("</option>");
    server << F("<option value='") << 23 << F("'") << (EEPROM.read(260 + i) == 23 ? F(" selected='selected'") : F("")) << F(">Color1") << F("</option>");
    server << F("<option value='") << 24 << F("'") << (EEPROM.read(260 + i) == 24 ? F(" selected='selected'") : F("")) << F(">Color2") << F("</option>");
    server << F("<option value='") << 25 << F("'") << (EEPROM.read(260 + i) == 25 ? F(" selected='selected'") : F("")) << F(">MPL1") << F("</option>");
    server << F("<option value='") << 26 << F("'") << (EEPROM.read(260 + i) == 26 ? F(" selected='selected'") : F("")) << F(">MPL1-K") << F("</option>");
    server << F("<option value='") << 27 << F("'") << (EEPROM.read(260 + i) == 27 ? F(" selected='selected'") : F("")) << F(">MPL2") << F("</option>");
    server << F("<option value='") << 28 << F("'") << (EEPROM.read(260 + i) == 28 ? F(" selected='selected'") : F("")) << F(">MPL2-K") << F("</option>");
    server << F("<option value='") << 29 << F("'") << (EEPROM.read(260 + i) == 29 ? F(" selected='selected'") : F("")) << F(">CleanFeed1") << F("</option>");
    server << F("<option value='") << 30 << F("'") << (EEPROM.read(260 + i) == 30 ? F(" selected='selected'") : F("")) << F(">CleanFeed2") << F("</option>");
    server << F("<option value='") << 31 << F("'") << (EEPROM.read(260 + i) == 31 ? F(" selected='selected'") : F("")) << F(">SuperSource") << F("</option>");
    server << F("<option value='") << 32 << F("'") << (EEPROM.read(260 + i) == 32 ? F(" selected='selected'") : F("")) << F(">Program") << F("</option>");
    server << F("<option value='") << 33 << F("'") << (EEPROM.read(260 + i) == 33 ? F(" selected='selected'") : F("")) << F(">Preview") << F("</option>");

    server << F("</select></td>");
  }
  server << F("</tr></n>");

  // Aux 5
  server << F("<table border=0 cellspacing=3><tr>");
  server << F("<h2>Aux5</h2><p>");
  for (i = 1; i <= 8; ++i)
  {
    server << F("<td>");
    server << F("<select name='GPIA5") << i << F(":'div style='width:78px;>");
    server << F("<option value='0'></option>");

    server << F("<option value='") << 0 << F("'") << (EEPROM.read(270 + i) == 0 ? F(" selected='selected'") : F("")) << F("> \t") << F("</option>");
    server << F("<option value='") << 1 << F("'") << (EEPROM.read(270 + i) == 1 ? F(" selected='selected'") : F("")) << F(">Camera1") << F("</option>");
    server << F("<option value='") << 2 << F("'") << (EEPROM.read(270 + i) == 2 ? F(" selected='selected'") : F("")) << F(">Camera2") << F("</option>");
    server << F("<option value='") << 3 << F("'") << (EEPROM.read(270 + i) == 3 ? F(" selected='selected'") : F("")) << F(">Camera3") << F("</option>");
    server << F("<option value='") << 4 << F("'") << (EEPROM.read(270 + i) == 4 ? F(" selected='selected'") : F("")) << F(">Camera4") << F("</option>");
    server << F("<option value='") << 5 << F("'") << (EEPROM.read(270 + i) == 5 ? F(" selected='selected'") : F("")) << F(">Camera5") << F("</option>");
    server << F("<option value='") << 6 << F("'") << (EEPROM.read(270 + i) == 6 ? F(" selected='selected'") : F("")) << F(">Camera6") << F("</option>");
    server << F("<option value='") << 7 << F("'") << (EEPROM.read(270 + i) == 7 ? F(" selected='selected'") : F("")) << F(">Camera7") << F("</option>");
    server << F("<option value='") << 8 << F("'") << (EEPROM.read(270 + i) == 8 ? F(" selected='selected'") : F("")) << F(">Camera8") << F("</option>");
    server << F("<option value='") << 9 << F("'") << (EEPROM.read(270 + i) == 9 ? F(" selected='selected'") : F("")) << F(">Camera9") << F("</option>");
    server << F("<option value='") << 10 << F("'") << (EEPROM.read(270 + i) == 10 ? F(" selected='selected'") : F("")) << F(">Camera10") << F("</option>");
    server << F("<option value='") << 11 << F("'") << (EEPROM.read(270 + i) == 11 ? F(" selected='selected'") : F("")) << F(">Camera11") << F("</option>");
    server << F("<option value='") << 12 << F("'") << (EEPROM.read(270 + i) == 12 ? F(" selected='selected'") : F("")) << F(">Camera12") << F("</option>");
    server << F("<option value='") << 13 << F("'") << (EEPROM.read(270 + i) == 13 ? F(" selected='selected'") : F("")) << F(">Camera13") << F("</option>");
    server << F("<option value='") << 14 << F("'") << (EEPROM.read(270 + i) == 14 ? F(" selected='selected'") : F("")) << F(">Camera14") << F("</option>");
    server << F("<option value='") << 15 << F("'") << (EEPROM.read(270 + i) == 15 ? F(" selected='selected'") : F("")) << F(">Camera15") << F("</option>");
    server << F("<option value='") << 16 << F("'") << (EEPROM.read(270 + i) == 16 ? F(" selected='selected'") : F("")) << F(">Camera16") << F("</option>");
    server << F("<option value='") << 17 << F("'") << (EEPROM.read(270 + i) == 17 ? F(" selected='selected'") : F("")) << F(">Camera17") << F("</option>");
    server << F("<option value='") << 18 << F("'") << (EEPROM.read(270 + i) == 18 ? F(" selected='selected'") : F("")) << F(">Camera18") << F("</option>");
    server << F("<option value='") << 19 << F("'") << (EEPROM.read(270 + i) == 19 ? F(" selected='selected'") : F("")) << F(">Camera19") << F("</option>");
    server << F("<option value='") << 20 << F("'") << (EEPROM.read(270 + i) == 20 ? F(" selected='selected'") : F("")) << F(">Camera20") << F("</option>");
    server << F("<option value='") << 21 << F("'") << (EEPROM.read(270 + i) == 21 ? F(" selected='selected'") : F("")) << F(">Black") << F("</option>");
    server << F("<option value='") << 22 << F("'") << (EEPROM.read(270 + i) == 22 ? F(" selected='selected'") : F("")) << F(">Bars") << F("</option>");
    server << F("<option value='") << 23 << F("'") << (EEPROM.read(270 + i) == 23 ? F(" selected='selected'") : F("")) << F(">Color1") << F("</option>");
    server << F("<option value='") << 24 << F("'") << (EEPROM.read(270 + i) == 24 ? F(" selected='selected'") : F("")) << F(">Color2") << F("</option>");
    server << F("<option value='") << 25 << F("'") << (EEPROM.read(270 + i) == 25 ? F(" selected='selected'") : F("")) << F(">MPL1") << F("</option>");
    server << F("<option value='") << 26 << F("'") << (EEPROM.read(270 + i) == 26 ? F(" selected='selected'") : F("")) << F(">MPL1-K") << F("</option>");
    server << F("<option value='") << 27 << F("'") << (EEPROM.read(270 + i) == 27 ? F(" selected='selected'") : F("")) << F(">MPL2") << F("</option>");
    server << F("<option value='") << 28 << F("'") << (EEPROM.read(270 + i) == 28 ? F(" selected='selected'") : F("")) << F(">MPL2-K") << F("</option>");
    server << F("<option value='") << 29 << F("'") << (EEPROM.read(270 + i) == 29 ? F(" selected='selected'") : F("")) << F(">CleanFeed1") << F("</option>");
    server << F("<option value='") << 30 << F("'") << (EEPROM.read(270 + i) == 30 ? F(" selected='selected'") : F("")) << F(">CleanFeed2") << F("</option>");
    server << F("<option value='") << 31 << F("'") << (EEPROM.read(270 + i) == 31 ? F(" selected='selected'") : F("")) << F(">SuperSource") << F("</option>");
    server << F("<option value='") << 32 << F("'") << (EEPROM.read(270 + i) == 32 ? F(" selected='selected'") : F("")) << F(">Program") << F("</option>");
    server << F("<option value='") << 33 << F("'") << (EEPROM.read(270 + i) == 33 ? F(" selected='selected'") : F("")) << F(">Preview") << F("</option>");

    server << F("</select></td>");
  }
  server << F("</tr></n>");

  // Aux 6
  server << F("<table border=0 cellspacing=3><tr>");
  server << F("<h2>Aux6</h2><p>");
  for (i = 1; i <= 8; ++i)
  {
    server << F("<td>");
    server << F("<select name='GPIA6") << i << F(":'div style='width:78px;>");
    server << F("<option value='0'></option>");

    server << F("<option value='") << 0 << F("'") << (EEPROM.read(280 + i) == 0 ? F(" selected='selected'") : F("")) << F("> \t") << F("</option>");
    server << F("<option value='") << 1 << F("'") << (EEPROM.read(280 + i) == 1 ? F(" selected='selected'") : F("")) << F(">Camera1") << F("</option>");
    server << F("<option value='") << 2 << F("'") << (EEPROM.read(280 + i) == 2 ? F(" selected='selected'") : F("")) << F(">Camera2") << F("</option>");
    server << F("<option value='") << 3 << F("'") << (EEPROM.read(280 + i) == 3 ? F(" selected='selected'") : F("")) << F(">Camera3") << F("</option>");
    server << F("<option value='") << 4 << F("'") << (EEPROM.read(280 + i) == 4 ? F(" selected='selected'") : F("")) << F(">Camera4") << F("</option>");
    server << F("<option value='") << 5 << F("'") << (EEPROM.read(280 + i) == 5 ? F(" selected='selected'") : F("")) << F(">Camera5") << F("</option>");
    server << F("<option value='") << 6 << F("'") << (EEPROM.read(280 + i) == 6 ? F(" selected='selected'") : F("")) << F(">Camera6") << F("</option>");
    server << F("<option value='") << 7 << F("'") << (EEPROM.read(280 + i) == 7 ? F(" selected='selected'") : F("")) << F(">Camera7") << F("</option>");
    server << F("<option value='") << 8 << F("'") << (EEPROM.read(280 + i) == 8 ? F(" selected='selected'") : F("")) << F(">Camera8") << F("</option>");
    server << F("<option value='") << 9 << F("'") << (EEPROM.read(280 + i) == 9 ? F(" selected='selected'") : F("")) << F(">Camera9") << F("</option>");
    server << F("<option value='") << 10 << F("'") << (EEPROM.read(280 + i) == 10 ? F(" selected='selected'") : F("")) << F(">Camera10") << F("</option>");
    server << F("<option value='") << 11 << F("'") << (EEPROM.read(280 + i) == 11 ? F(" selected='selected'") : F("")) << F(">Camera11") << F("</option>");
    server << F("<option value='") << 12 << F("'") << (EEPROM.read(280 + i) == 12 ? F(" selected='selected'") : F("")) << F(">Camera12") << F("</option>");
    server << F("<option value='") << 13 << F("'") << (EEPROM.read(280 + i) == 13 ? F(" selected='selected'") : F("")) << F(">Camera13") << F("</option>");
    server << F("<option value='") << 14 << F("'") << (EEPROM.read(280 + i) == 14 ? F(" selected='selected'") : F("")) << F(">Camera14") << F("</option>");
    server << F("<option value='") << 15 << F("'") << (EEPROM.read(280 + i) == 15 ? F(" selected='selected'") : F("")) << F(">Camera15") << F("</option>");
    server << F("<option value='") << 16 << F("'") << (EEPROM.read(280 + i) == 16 ? F(" selected='selected'") : F("")) << F(">Camera16") << F("</option>");
    server << F("<option value='") << 17 << F("'") << (EEPROM.read(280 + i) == 17 ? F(" selected='selected'") : F("")) << F(">Camera17") << F("</option>");
    server << F("<option value='") << 18 << F("'") << (EEPROM.read(280 + i) == 18 ? F(" selected='selected'") : F("")) << F(">Camera18") << F("</option>");
    server << F("<option value='") << 19 << F("'") << (EEPROM.read(280 + i) == 19 ? F(" selected='selected'") : F("")) << F(">Camera19") << F("</option>");
    server << F("<option value='") << 20 << F("'") << (EEPROM.read(280 + i) == 20 ? F(" selected='selected'") : F("")) << F(">Camera20") << F("</option>");
    server << F("<option value='") << 21 << F("'") << (EEPROM.read(280 + i) == 21 ? F(" selected='selected'") : F("")) << F(">Black") << F("</option>");
    server << F("<option value='") << 22 << F("'") << (EEPROM.read(280 + i) == 22 ? F(" selected='selected'") : F("")) << F(">Bars") << F("</option>");
    server << F("<option value='") << 23 << F("'") << (EEPROM.read(280 + i) == 23 ? F(" selected='selected'") : F("")) << F(">Color1") << F("</option>");
    server << F("<option value='") << 24 << F("'") << (EEPROM.read(280 + i) == 24 ? F(" selected='selected'") : F("")) << F(">Color2") << F("</option>");
    server << F("<option value='") << 25 << F("'") << (EEPROM.read(280 + i) == 25 ? F(" selected='selected'") : F("")) << F(">MPL1") << F("</option>");
    server << F("<option value='") << 26 << F("'") << (EEPROM.read(280 + i) == 26 ? F(" selected='selected'") : F("")) << F(">MPL1-K") << F("</option>");
    server << F("<option value='") << 27 << F("'") << (EEPROM.read(280 + i) == 27 ? F(" selected='selected'") : F("")) << F(">MPL2") << F("</option>");
    server << F("<option value='") << 28 << F("'") << (EEPROM.read(280 + i) == 28 ? F(" selected='selected'") : F("")) << F(">MPL2-K") << F("</option>");
    server << F("<option value='") << 29 << F("'") << (EEPROM.read(280 + i) == 29 ? F(" selected='selected'") : F("")) << F(">CleanFeed1") << F("</option>");
    server << F("<option value='") << 30 << F("'") << (EEPROM.read(280 + i) == 30 ? F(" selected='selected'") : F("")) << F(">CleanFeed2") << F("</option>");
    server << F("<option value='") << 31 << F("'") << (EEPROM.read(280 + i) == 31 ? F(" selected='selected'") : F("")) << F(">SuperSource") << F("</option>");
    server << F("<option value='") << 32 << F("'") << (EEPROM.read(280 + i) == 32 ? F(" selected='selected'") : F("")) << F(">Program") << F("</option>");
    server << F("<option value='") << 33 << F("'") << (EEPROM.read(280 + i) == 33 ? F(" selected='selected'") : F("")) << F(">Preview") << F("</option>");

    server << F("</select></td>");
  }
  server << F("</tr></n>");


  server << F("</tr></table><hr/>");

  // Set routing for GPOs:
  server << F("<h1>Set GPO functions:</h1><table border=0 cellspacing=3><tr>");
  for (i = 1; i <= 8; ++i)
  {
    server << F("<td><p>GPO ") << i << F(":</p>");
    server << F("<select name='GPO1AT") << i << F(":'div style='width:78px;>");
    server << F("<option value='0'></option>");

    server << F("<option value='") << 0 << F("'") << (EEPROM.read(150 + i) == 0 ? F(" selected='selected'") : F("")) << F(">ATEM1ME") << F("</option>");
    server << F("<option value='") << 1 << F("'") << (EEPROM.read(150 + i) == 1 ? F(" selected='selected'") : F("")) << F(">ATEM2ME") << F("</option>");

    server << F("</select></td>");
  }
  server << F("</tr></n>");
  for (i = 1; i <= 8; ++i)
  {
    server << F("<td></p>");
    server << F("<select name='GPO1") << i << F(":'div style='width:78px;>");
    server << F("<option value='0'></option>");

    server << F("<option value='") << 0 << F("'") << (EEPROM.read(300 + i) == 0 ? F(" selected='selected'") : F("")) << F("> \t") << F("</option>");

    // Inputs 1-20
    for (uint8_t j = 1; j <= 20; j++)  {
      server << F("<option value='") << j << F("'") << (EEPROM.read(300 + i) == j ? F(" selected='selected'") : F("")) << F(">PVW ") << j << F("</option>");
    }
    for (uint8_t j = 1; j <= 20; j++)  {
      server << F("<option value='") << j + 20 << F("'") << (EEPROM.read(300 + i) == j + 20 ? F(" selected='selected'") : F("")) << F(">PGM ") << j << F("</option>");
    }
    // Other inputs
    server << F("<option value='") << 41 << F("'") << (EEPROM.read(300 + i) == 41 ? F(" selected='selected'") : F("")) << F(">PvwBlack") << F("</option>");
    server << F("<option value='") << 42 << F("'") << (EEPROM.read(300 + i) == 42 ? F(" selected='selected'") : F("")) << F(">PgmBlack") << F("</option>");
    server << F("<option value='") << 43 << F("'") << (EEPROM.read(300 + i) == 43 ? F(" selected='selected'") : F("")) << F(">PvwBars") << F("</option>");
    server << F("<option value='") << 44 << F("'") << (EEPROM.read(300 + i) == 44 ? F(" selected='selected'") : F("")) << F(">PgmBars") << F("</option>");
    server << F("<option value='") << 45 << F("'") << (EEPROM.read(300 + i) == 45 ? F(" selected='selected'") : F("")) << F(">PvwColor1") << F("</option>");
    server << F("<option value='") << 46 << F("'") << (EEPROM.read(300 + i) == 46 ? F(" selected='selected'") : F("")) << F(">PgmColor1") << F("</option>");
    server << F("<option value='") << 47 << F("'") << (EEPROM.read(300 + i) == 47 ? F(" selected='selected'") : F("")) << F(">PvwColor2") << F("</option>");
    server << F("<option value='") << 48 << F("'") << (EEPROM.read(300 + i) == 48 ? F(" selected='selected'") : F("")) << F(">PgmColor2") << F("</option>");
    server << F("<option value='") << 49 << F("'") << (EEPROM.read(300 + i) == 49 ? F(" selected='selected'") : F("")) << F(">PvwMedia1") << F("</option>");
    server << F("<option value='") << 50 << F("'") << (EEPROM.read(300 + i) == 50 ? F(" selected='selected'") : F("")) << F(">PgmMedia1") << F("</option>");
    server << F("<option value='") << 51 << F("'") << (EEPROM.read(300 + i) == 51 ? F(" selected='selected'") : F("")) << F(">PvwMedia2") << F("</option>");
    server << F("<option value='") << 52 << F("'") << (EEPROM.read(300 + i) == 52 ? F(" selected='selected'") : F("")) << F(">PgmMedia2") << F("</option>");
    server << F("<option value='") << 53 << F("'") << (EEPROM.read(300 + i) == 53 ? F(" selected='selected'") : F("")) << F(">PvwSSource") << F("</option>");
    server << F("<option value='") << 54 << F("'") << (EEPROM.read(300 + i) == 54 ? F(" selected='selected'") : F("")) << F(">PgmSSource") << F("</option>");

    // Other sources
    server << F("<option value='") << 53 << F("'") << (EEPROM.read(300 + i) == 53 ? F(" selected='selected'") : F("")) << F(">USK1") << F("</option>");
    server << F("<option value='") << 54 << F("'") << (EEPROM.read(300 + i) == 54 ? F(" selected='selected'") : F("")) << F(">USK2") << F("</option>");
    server << F("<option value='") << 55 << F("'") << (EEPROM.read(300 + i) == 55 ? F(" selected='selected'") : F("")) << F(">USK3") << F("</option>");
    server << F("<option value='") << 56 << F("'") << (EEPROM.read(300 + i) == 56 ? F(" selected='selected'") : F("")) << F(">USK4") << F("</option>");
    server << F("<option value='") << 57 << F("'") << (EEPROM.read(300 + i) == 57 ? F(" selected='selected'") : F("")) << F(">DSK1") << F("</option>");
    server << F("<option value='") << 58 << F("'") << (EEPROM.read(300 + i) == 58 ? F(" selected='selected'") : F("")) << F(">DSK2") << F("</option>");

    server << F("</select></td>");
  }
  server << F("</tr></n>");

  //server << F("<h1>Set GPO functions:</h1><table border=0 cellspacing=3><tr>");
  for (i = 1; i <= 8; ++i)
  {
    server << F("<td></p>");
    server << F("<select name='GPO2") << i << F(":'div style='width:78px;>");
    server << F("<option value='0'></option>");

    server << F("<option value='") << 0 << F("'") << (EEPROM.read(310 + i) == 0 ? F(" selected='selected'") : F("")) << F("> \t") << F("</option>");

    //Aux
    for (uint8_t j = 1; j <= 6; j++) {
      server << F("<option value='") << 1 + (j - 1) * 33 << F("'") << (EEPROM.read(310 + i) == 1 + (j - 1) * 33 ? F(" selected='selected'") : F("")) << F(">A") << j << F("-Cam1") << F("</option>");
      server << F("<option value='") << 2 + (j - 1) * 33 << F("'") << (EEPROM.read(310 + i) == 2 + (j - 1) * 33 ? F(" selected='selected'") : F("")) << F(">A") << j << F("-Cam2") << F("</option>");
      server << F("<option value='") << 3 + (j - 1) * 33 << F("'") << (EEPROM.read(310 + i) == 3 + (j - 1) * 33 ? F(" selected='selected'") : F("")) << F(">A") << j << F("-Cam3") << F("</option>");
      server << F("<option value='") << 4 + (j - 1) * 33 << F("'") << (EEPROM.read(310 + i) == 4 + (j - 1) * 33 ? F(" selected='selected'") : F("")) << F(">A") << j << F("-Cam4") << F("</option>");
      server << F("<option value='") << 5 + (j - 1) * 33 << F("'") << (EEPROM.read(310 + i) == 5 + (j - 1) * 33 ? F(" selected='selected'") : F("")) << F(">A") << j << F("-Cam5") << F("</option>");
      server << F("<option value='") << 6 + (j - 1) * 33 << F("'") << (EEPROM.read(310 + i) == 6 + (j - 1) * 33 ? F(" selected='selected'") : F("")) << F(">A") << j << F("-Cam6") << F("</option>");
      server << F("<option value='") << 7 + (j - 1) * 33 << F("'") << (EEPROM.read(310 + i) == 7 + (j - 1) * 33 ? F(" selected='selected'") : F("")) << F(">A") << j << F("-Cam7") << F("</option>");
      server << F("<option value='") << 8 + (j - 1) * 33 << F("'") << (EEPROM.read(310 + i) == 8 + (j - 1) * 33 ? F(" selected='selected'") : F("")) << F(">A") << j << F("-Cam8") << F("</option>");
      server << F("<option value='") << 9 + (j - 1) * 33 << F("'") << (EEPROM.read(310 + i) == 9 + (j - 1) * 33 ? F(" selected='selected'") : F("")) << F(">A") << j << F("-Cam9") << F("</option>");
      server << F("<option value='") << 10 + (j - 1) * 33 << F("'") << (EEPROM.read(310 + i) == 10 + (j - 1) * 33 ? F(" selected='selected'") : F("")) << F(">A") << j << F("-Cam10") << F("</option>");
      server << F("<option value='") << 11 + (j - 1) * 33 << F("'") << (EEPROM.read(310 + i) == 11 + (j - 1) * 33 ? F(" selected='selected'") : F("")) << F(">A") << j << F("-Cam11") << F("</option>");
      server << F("<option value='") << 12 + (j - 1) * 33 << F("'") << (EEPROM.read(310 + i) == 12 + (j - 1) * 33 ? F(" selected='selected'") : F("")) << F(">A") << j << F("-Cam12") << F("</option>");
      server << F("<option value='") << 13 + (j - 1) * 33 << F("'") << (EEPROM.read(310 + i) == 13 + (j - 1) * 33 ? F(" selected='selected'") : F("")) << F(">A") << j << F("-Cam13") << F("</option>");
      server << F("<option value='") << 14 + (j - 1) * 33 << F("'") << (EEPROM.read(310 + i) == 14 + (j - 1) * 33 ? F(" selected='selected'") : F("")) << F(">A") << j << F("-Cam14") << F("</option>");
      server << F("<option value='") << 15 + (j - 1) * 33 << F("'") << (EEPROM.read(310 + i) == 15 + (j - 1) * 33 ? F(" selected='selected'") : F("")) << F(">A") << j << F("-Cam15") << F("</option>");
      server << F("<option value='") << 16 + (j - 1) * 33 << F("'") << (EEPROM.read(310 + i) == 16 + (j - 1) * 33 ? F(" selected='selected'") : F("")) << F(">A") << j << F("-Cam16") << F("</option>");
      server << F("<option value='") << 17 + (j - 1) * 33 << F("'") << (EEPROM.read(310 + i) == 17 + (j - 1) * 33 ? F(" selected='selected'") : F("")) << F(">A") << j << F("-Cam17") << F("</option>");
      server << F("<option value='") << 18 + (j - 1) * 33 << F("'") << (EEPROM.read(310 + i) == 18 + (j - 1) * 33 ? F(" selected='selected'") : F("")) << F(">A") << j << F("-Cam18") << F("</option>");
      server << F("<option value='") << 19 + (j - 1) * 33 << F("'") << (EEPROM.read(310 + i) == 19 + (j - 1) * 33 ? F(" selected='selected'") : F("")) << F(">A") << j << F("-Cam19") << F("</option>");
      server << F("<option value='") << 20 + (j - 1) * 33 << F("'") << (EEPROM.read(310 + i) == 20 + (j - 1) * 33 ? F(" selected='selected'") : F("")) << F(">A") << j << F("-Cam20") << F("</option>");
      server << F("<option value='") << 21 + (j - 1) * 33 << F("'") << (EEPROM.read(310 + i) == 21 + (j - 1) * 33 ? F(" selected='selected'") : F("")) << F(">A") << j << F("-Black") << F("</option>");
      server << F("<option value='") << 22 + (j - 1) * 33 << F("'") << (EEPROM.read(310 + i) == 22 + (j - 1) * 33 ? F(" selected='selected'") : F("")) << F(">A") << j << F("-Bars") << F("</option>");
      server << F("<option value='") << 23 + (j - 1) * 33 << F("'") << (EEPROM.read(310 + i) == 23 + (j - 1) * 33 ? F(" selected='selected'") : F("")) << F(">A") << j << F("-Color1") << F("</option>");
      server << F("<option value='") << 24 + (j - 1) * 33 << F("'") << (EEPROM.read(310 + i) == 24 + (j - 1) * 33 ? F(" selected='selected'") : F("")) << F(">A") << j << F("-Color2") << F("</option>");
      server << F("<option value='") << 25 + (j - 1) * 33 << F("'") << (EEPROM.read(310 + i) == 25 + (j - 1) * 33 ? F(" selected='selected'") : F("")) << F(">A") << j << F("-MPL1") << F("</option>");
      server << F("<option value='") << 26 + (j - 1) * 33 << F("'") << (EEPROM.read(310 + i) == 26 + (j - 1) * 33 ? F(" selected='selected'") : F("")) << F(">A") << j << F("-MPL1-K") << F("</option>");
      server << F("<option value='") << 27 + (j - 1) * 33 << F("'") << (EEPROM.read(310 + i) == 27 + (j - 1) * 33 ? F(" selected='selected'") : F("")) << F(">A") << j << F("-MPL2") << F("</option>");
      server << F("<option value='") << 28 + (j - 1) * 33 << F("'") << (EEPROM.read(310 + i) == 28 + (j - 1) * 33 ? F(" selected='selected'") : F("")) << F(">A") << j << F("-MPL3-K") << F("</option>");
      server << F("<option value='") << 29 + (j - 1) * 33 << F("'") << (EEPROM.read(310 + i) == 29 + (j - 1) * 33 ? F(" selected='selected'") : F("")) << F(">A") << j << F("-ClFeed1") << F("</option>");
      server << F("<option value='") << 30 + (j - 1) * 33 << F("'") << (EEPROM.read(310 + i) == 30 + (j - 1) * 33 ? F(" selected='selected'") : F("")) << F(">A") << j << F("-ClFeed2") << F("</option>");
      server << F("<option value='") << 31 + (j - 1) * 33 << F("'") << (EEPROM.read(310 + i) == 31 + (j - 1) * 33 ? F(" selected='selected'") : F("")) << F(">A") << j << F("-SSource") << F("</option>");
      server << F("<option value='") << 32 + (j - 1) * 33 << F("'") << (EEPROM.read(310 + i) == 32 + (j - 1) * 33 ? F(" selected='selected'") : F("")) << F(">A") << j << F("-Program") << F("</option>");
      server << F("<option value='") << 33 + (j - 1) * 33 << F("'") << (EEPROM.read(310 + i) == 33 + (j - 1) * 33 ? F(" selected='selected'") : F("")) << F(">A") << j << F("-Preview") << F("</option>");
    }
    server << F("</select></td>");
  }

  server << F("</tr></table><hr/>");
  ///////////////////////////////////////////////////


  // End form and page:
  server << F("<input type='submit' value='Submit'/></form></div>");
  server << F("<br><i>(Reset / Pull the power after submitting the form successfully)</i>");
  server << F("</body></html>");
}

void formCmd(WebServer & server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  if (type == WebServer::POST)
  {
    bool repeat;
    char name[16], value[16];
    do
    {
      repeat = server.readPOSTparam(name, 16, value, 16);
      String Name = String(name);

      // C200 Panel IP:
      if (Name.startsWith("IP"))  {
        int addr = strtoul(name + 2, NULL, 10);
        int val = strtoul(value, NULL, 10);
        if (addr >= 0 && addr <= 3)  {
          EEPROM.write(addr + 2, val); // IP address stored in bytes 0-3
        }
      }

      // ATEM Switcher Panel IP:
      if (Name.startsWith("ATEM_IP"))  {
        int addr = strtoul(name + 7, NULL, 10);
        int val = strtoul(value, NULL, 10);
        if (addr >= 0 && addr <= 3)  {
          EEPROM.write(addr + 2 + 4, val); // IP address stored in bytes 4-7
        }
      }

      // functions
      if (Name.startsWith("Functions"))  {
        int inputNum = strtoul(name + 9, NULL, 10);
        int val = strtoul(value, NULL, 10);
        if (inputNum >= 0 && inputNum <= 1)  {
          EEPROM.write(400, val);
        }
      }

      // routing
      if (Name.startsWith("Buttons1A1"))  {
        int inputNum = strtoul(name + 10, NULL, 10);
        int val = strtoul(value, NULL, 10);
        if (inputNum >= 1 && inputNum <= 2)  {
          EEPROM.write(614 + inputNum, val);
        }
      }
      if (Name.startsWith("Buttons1F1"))  {
        int inputNum = strtoul(name + 10, NULL, 10);
        int val = strtoul(value, NULL, 10);
        if (inputNum >= 1 && inputNum <= 2)  {
          EEPROM.write(414 + inputNum, val);
        }
      }
      if (Name.startsWith("Buttons1F2"))  {
        int inputNum = strtoul(name + 10, NULL, 10);
        int val = strtoul(value, NULL, 10);
        if (inputNum >= 1 && inputNum <= 2)  {
          EEPROM.write(514 + inputNum, val);
        }
      }
      if (Name.startsWith("Buttons2A1"))  {
        int inputNum = strtoul(name + 10, NULL, 10);
        int val = strtoul(value, NULL, 10);
        if (inputNum >= 1 && inputNum <= 2)  {
          EEPROM.write(615 - inputNum, val);
        }
      }
      if (Name.startsWith("Buttons2F1"))  {
        int inputNum = strtoul(name + 10, NULL, 10);
        int val = strtoul(value, NULL, 10);
        if (inputNum >= 1 && inputNum <= 2)  {
          EEPROM.write(415 - inputNum, val);
        }
      }
      if (Name.startsWith("Buttons2F2"))  {
        int inputNum = strtoul(name + 10, NULL, 10);
        int val = strtoul(value, NULL, 10);
        if (inputNum >= 1 && inputNum <= 2)  {
          EEPROM.write(515 - inputNum, val);
        }
      }
      if (Name.startsWith("Buttons3A1"))  {
        int inputNum = strtoul(name + 10, NULL, 10);
        int val = strtoul(value, NULL, 10);
        if (inputNum >= 1 && inputNum <= 6)  {
          EEPROM.write(606 + inputNum, val);
        }
      }
      if (Name.startsWith("Buttons3F1"))  {
        int inputNum = strtoul(name + 10, NULL, 10);
        int val = strtoul(value, NULL, 10);
        if (inputNum >= 1 && inputNum <= 6)  {
          EEPROM.write(406 + inputNum, val);
        }
      }
      if (Name.startsWith("Buttons3F2"))  {
        int inputNum = strtoul(name + 10, NULL, 10);
        int val = strtoul(value, NULL, 10);
        if (inputNum >= 1 && inputNum <= 6)  {
          EEPROM.write(506 + inputNum, val);
        }
      }
      if (Name.startsWith("Buttons4A1"))  {
        int inputNum = strtoul(name + 10, NULL, 10);
        int val = strtoul(value, NULL, 10);
        if (inputNum >= 1 && inputNum <= 6)  {
          EEPROM.write(600 + inputNum, val);
        }
      }
      if (Name.startsWith("Buttons4F1"))  {
        int inputNum = strtoul(name + 10, NULL, 10);
        int val = strtoul(value, NULL, 10);
        if (inputNum >= 1 && inputNum <= 6)  {
          EEPROM.write(400 + inputNum, val);
        }
      }
      if (Name.startsWith("Buttons4F2"))  {
        int inputNum = strtoul(name + 10, NULL, 10);
        int val = strtoul(value, NULL, 10);
        if (inputNum >= 1 && inputNum <= 6)  {
          EEPROM.write(500 + inputNum, val);
        }
      }

      // Slider
      if (Name.startsWith("slider"))  {
        int inputNum = strtoul(name + 6, NULL, 10);
        int val = strtoul(value, NULL, 10);
        EEPROM.write(99, val);
      }

      // GPIO
      if (Name.startsWith("GPI1AT"))  {
        int inputNum = strtoul(name + 6, NULL, 10);
        int val = strtoul(value, NULL, 10);
        if (inputNum >= 1 && inputNum <= 8)  {
          EEPROM.write(100 + inputNum, val);
        }
      }
      if (Name.startsWith("GPI1"))  {
        int inputNum = strtoul(name + 4, NULL, 10);
        int val = strtoul(value, NULL, 10);
        if (inputNum >= 1 && inputNum <= 8)  {
          EEPROM.write(200 + inputNum, val);
        }
      }
      if (Name.startsWith("GPI2"))  {
        int inputNum = strtoul(name + 4, NULL, 10);
        int val = strtoul(value, NULL, 10);
        if (inputNum >= 1 && inputNum <= 8)  {
          EEPROM.write(210 + inputNum, val);
        }
      }
      if (Name.startsWith("GPI3"))  {
        int inputNum = strtoul(name + 4, NULL, 10);
        int val = strtoul(value, NULL, 10);
        if (inputNum >= 1 && inputNum <= 8)  {
          EEPROM.write(220 + inputNum, val);
        }
      }
      if (Name.startsWith("GPIA1"))  {
        int inputNum = strtoul(name + 5, NULL, 10);
        int val = strtoul(value, NULL, 10);
        if (inputNum >= 1 && inputNum <= 8)  {
          EEPROM.write(230 + inputNum, val);
        }
      }
      if (Name.startsWith("GPIA2"))  {
        int inputNum = strtoul(name + 5, NULL, 10);
        int val = strtoul(value, NULL, 10);
        if (inputNum >= 1 && inputNum <= 8)  {
          EEPROM.write(240 + inputNum, val);
        }
      }
      if (Name.startsWith("GPIA3"))  {
        int inputNum = strtoul(name + 5, NULL, 10);
        int val = strtoul(value, NULL, 10);
        if (inputNum >= 1 && inputNum <= 8)  {
          EEPROM.write(250 + inputNum, val);
        }
      }
      if (Name.startsWith("GPIA4"))  {
        int inputNum = strtoul(name + 5, NULL, 10);
        int val = strtoul(value, NULL, 10);
        if (inputNum >= 1 && inputNum <= 8)  {
          EEPROM.write(260 + inputNum, val);
        }
      }
      if (Name.startsWith("GPIA5"))  {
        int inputNum = strtoul(name + 5, NULL, 10);
        int val = strtoul(value, NULL, 10);
        if (inputNum >= 1 && inputNum <= 8)  {
          EEPROM.write(270 + inputNum, val);
        }
      }
      if (Name.startsWith("GPIA6"))  {
        int inputNum = strtoul(name + 5, NULL, 10);
        int val = strtoul(value, NULL, 10);
        if (inputNum >= 1 && inputNum <= 8)  {
          EEPROM.write(280 + inputNum, val);
        }
      }
      if (Name.startsWith("GPO1AT"))  {
        int inputNum = strtoul(name + 6, NULL, 10);
        int val = strtoul(value, NULL, 10);
        if (inputNum >= 1 && inputNum <= 8)  {
          EEPROM.write(150 + inputNum, val);
        }
      }
      if (Name.startsWith("GPO1"))  {
        int inputNum = strtoul(name + 4, NULL, 10);
        int val = strtoul(value, NULL, 10);
        if (inputNum >= 1 && inputNum <= 8)  {
          EEPROM.write(300 + inputNum, val);
        }
      }
      if (Name.startsWith("GPO2"))  {
        int inputNum = strtoul(name + 4, NULL, 10);
        int val = strtoul(value, NULL, 10);
        if (inputNum >= 1 && inputNum <= 8)  {
          EEPROM.write(310 + inputNum, val);
        }
      }

    }
    while (repeat);

    server.httpSeeOther(PREFIX "/form");
  }
  else
    webDefaultView(server, type);
}
void defaultCmd(WebServer & server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  webDefaultView(server, type);
}





























/*************************************************************


                       MAIN PROGRAM CODE AHEAD


 **********************************************************/



bool isConfigMode;  // If set, the system will run the Web Configurator, not the normal program
uint8_t buttonsATEM[16];
uint8_t buttons1function[16];
uint8_t buttons2function[16];
uint8_t slider;
uint8_t GPIATEM[8];
uint8_t GPI1[8];
uint8_t GPI2[8];
uint8_t GPI3[8];
uint8_t GPIA1[8];
uint8_t GPIA2[8];
uint8_t GPIA3[8];
uint8_t GPIA4[8];
uint8_t GPIA5[8];
uint8_t GPIA6[8];
uint8_t GPOATEM[8];
uint8_t GPO1[8];
uint8_t GPO2[8];


void setup() {

  // Start the Ethernet, Serial (debugging) and UDP:
  Serial.begin(115200);
  Serial << F("\n- - - - - - - -\nSerial Started\n");

  // *********************************
  // Mode of Operation (Normal / Configuration)
  // *********************************
  // Determine web config mode
  // This is done by:
  // -  either flipping a switch connecting A1 to GND
  // -  Holding the CUT button during start up.
  pinMode(A1, INPUT_PULLUP);
  delay(100);
  isConfigMode = (analogRead(A1) > 500) ? true : false;


  // *********************************
  // INITIALIZE EEPROM memory:
  // *********************************
  // Check if EEPROM has ever been initialized, if not, install default IP
  if (EEPROM.read(0) != 12 ||  EEPROM.read(1) != 232)  {  // Just randomly selected values which should be unlikely to be in EEPROM by default.
    // Set these random values so this initialization is only run once!
    EEPROM.write(0, 12);
    EEPROM.write(1, 232);

    // Set default IP address for Arduino/C100 panel (172.16.99.57)
    EEPROM.write(2, 172);
    EEPROM.write(3, 16);
    EEPROM.write(4, 99);
    EEPROM.write(5, 57); // Just some value I chose, probably below DHCP range?

    // Set default IP address for ATEM Switcher (172.16.99.54):
    EEPROM.write(6, 172);
    EEPROM.write(7, 16);
    EEPROM.write(8, 99);
    EEPROM.write(9, 54);
  }


  // *********************************
  // Setting up IP addresses, starting Ethernet
  // *********************************
  if (isConfigMode)  {
    // Setting the default ip address for configuration mode:
    ip[0] = default_ip[0];
    ip[1] = default_ip[1];
    ip[2] = default_ip[2];
    ip[3] = default_ip[3];
  }
  else {
    ip[0] = EEPROM.read(0 + 2);
    ip[1] = EEPROM.read(1 + 2);
    ip[2] = EEPROM.read(2 + 2);
    ip[3] = EEPROM.read(3 + 2);
  }

  // Setting the ATEM IP address:
  atem_ip[0] = EEPROM.read(0 + 2 + 4);
  atem_ip[1] = EEPROM.read(1 + 2 + 4);
  atem_ip[2] = EEPROM.read(2 + 2 + 4);
  atem_ip[3] = EEPROM.read(3 + 2 + 4);


  Serial << F("SKAARHOJ Device IP Address: ") << ip[0] << "." << ip[1] << "." << ip[2] << "." << ip[3] << "\n";
  Serial << F("ATEM Switcher IP Address: ") << atem_ip[0] << "." << atem_ip[1] << "." << atem_ip[2] << "." << atem_ip[3] << "\n";

  // Setting MAC address:
  mac[0] = EEPROM.read(10);
  mac[1] = EEPROM.read(11);
  mac[2] = EEPROM.read(12);
  mac[3] = EEPROM.read(13);
  mac[4] = EEPROM.read(14);
  mac[5] = EEPROM.read(15);
  char buffer[18];
  sprintf(buffer, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  Serial << F("SKAARHOJ Device MAC address: ") << buffer << F(" - Checksum: ")
         << ((mac[0] + mac[1] + mac[2] + mac[3] + mac[4] + mac[5]) & 0xFF);
  if ((uint8_t)EEPROM.read(16) != ((mac[0] + mac[1] + mac[2] + mac[3] + mac[4] + mac[5]) & 0xFF))  {
    Serial << F("MAC address not found in EEPROM memory!\n") <<
           F("Please load example sketch ConfigEthernetAddresses to set it.\n") <<
           F("The MAC address is found on the backside of your Ethernet Shield/Board\n (STOP)");
    while (true);

  }

  Ethernet.begin(mac, ip);




  // Always initialize Wire before setting up the SkaarhojBI8 class!
  Wire.begin();

  // Set up the SkaarhojBI8 boards:
  buttons.begin(1);

  buttons.setDefaultColor(3);  // Dimmed by default
  buttons.setButtonColorsToDefault();
  buttons.testSequence();

  // Initializing the slider:
  utils.uniDirectionalSlider_init();
  utils.uniDirectionalSlider_hasMoved();

  GPIOboard.begin();

  // Set:
  for (int i = 1; i <= 8; i++)  {
    GPIOboard.setOutput(i, HIGH);
    delay(100);
  }
  // Clear:
  for (int i = 1; i <= 8; i++)  {
    GPIOboard.setOutput(i, LOW);
    delay(100);
  }

  // Sets the Bi-color LED to off = "no connection"
  pinMode(greenled, OUTPUT);
  pinMode(redled, OUTPUT);
  digitalWrite(greenled, false);
  digitalWrite(redled, false);


  // *********************************
  // Final Setup based on mode
  // *********************************
  if (isConfigMode)  {
    Serial << F("\nConfig mode\n");
    webserver.begin();
    webserver.setDefaultCommand(&defaultCmd);
    webserver.addCommand("form", &formCmd);
    webserver.addCommand("logo.png", &logoCmd);
  }
  else {
    Serial << F("\nWork mode\n");
    // read buttons functions
    for (uint8_t i = 1; i <= 16; i++) {
      buttonsATEM[i - 1] = (EEPROM.read(600 + i));
      buttons1function[i - 1] = (EEPROM.read(400 + i));
      buttons2function[i - 1] = (EEPROM.read(500 + i));
    }
    for (uint8_t i = 1; i <= 8; i++) {
      GPIATEM[i - 1] = EEPROM.read(100 + i);
      GPI1[i - 1] = EEPROM.read(200 + i);
      GPI2[i - 1] = EEPROM.read(210 + i);
      GPI3[i - 1] = EEPROM.read(220 + i);
      GPIA1[i - 1] = EEPROM.read(230 + i);
      GPIA2[i - 1] = EEPROM.read(240 + i);
      GPIA3[i - 1] = EEPROM.read(250 + i);
      GPIA4[i - 1] = EEPROM.read(260 + i);
      GPIA5[i - 1] = EEPROM.read(270 + i);
      GPIA6[i - 1] = EEPROM.read(280 + i);
      GPOATEM[i - 1] = EEPROM.read(150 + i);
      GPO1[i - 1] = EEPROM.read(300 + i);
      GPO2[i - 1] = EEPROM.read(310 + i);
    }
    if (EEPROM.read(99) >1) {
      EEPROM.write(99, 0);
    }
    slider = EEPROM.read(99);


    // Initialize a connection to the switcher:
    AtemSwitcher.begin(IPAddress(atem_ip[0], atem_ip[1], atem_ip[2], atem_ip[3]), 56417);    // <= SETUP!
    //AtemSwitcher.serialOutput(true);
    AtemSwitcher.connect();

    // Set Bi-color LED orange - indicates "connecting...":
    digitalWrite(redled, true);
    digitalWrite(greenled, true);

    buttons.setDefaultColor(0);  // Off by default
    buttons.setButtonColorsToDefault();
  }
}

bool AtemOnline = false;

bool GPOstate[8] = {false, false, false, false, false, false, false, false};
bool GPOblock[8] = {false, false, false, false, false, false, false, false};

void loop() {
  if (isConfigMode)  {
    webserver.processConnection();
    digitalWrite(redled, (((unsigned long)millis() >> 3) & B11000000) ? true : false);
    if (millis() < 600000) {
    buttons.setDefaultColor((((unsigned long)millis() >> 3) & B11000000) ? 1 : 0); // Off by default
    buttons.setButtonColorsToDefault();
    } else {
      runTest();
    }
  }
  else {
    // Check for packets, respond to them etc. Keeping the connection alive!
    lDelay(0);

    // If the switcher has been initialized, check for button presses as reflect status of switcher in button lights:
    if (AtemSwitcher.hasInitialized())  {
      if (!AtemOnline)  {
        AtemOnline = true;
        // Set Bi-color LED to red or green depending on mode:
        digitalWrite(redled, false);
        digitalWrite(greenled, true);
        Serial << F("Turning on buttons\n");

        buttons.setDefaultColor(3);  // Dimmed by default
        buttons.setButtonColorsToDefault();
      }

      setButtonColors();
      commandDispatch();
      // GPIO
      setTally();    // This will reflect outputs according to the web interface
      checkGPI();  // This will select the functions according to the web interface.

    }
    else {
      if (AtemOnline)  {
        AtemOnline = false;

        // Set Bi-color LED off = "no connection"
        digitalWrite(redled, true);
        digitalWrite(greenled, false);

        Serial << F("Turning off buttons light\n");
        buttons.setDefaultColor(0);  // Off by default
        buttons.setButtonColorsToDefault();
      }
    }
  }
}



/*******************************
   Set buttons functions
 *******************************/

void commandDispatch() {
  // "T-bar" slider:
  if (utils.uniDirectionalSlider_hasMoved())  {
    AtemSwitcher.setTransitionPosition(slider, 10 * utils.uniDirectionalSlider_position());
    lDelay(20);
    if (utils.uniDirectionalSlider_isAtEnd())  {
      AtemSwitcher.setTransitionPosition(slider, 0);
      lDelay(5);
    }
  }

  uint16_t busSelection = buttons.buttonDownAll();
  uint16_t busSelectionUp = buttons.buttonUpAll();
  if (busSelection > 0) {
    Serial << (busSelection, BIN) << "\n";
  }
  for (int i = 1; i <= 16; i++) {
    if (buttons.isButtonIn(i, busSelection))  {
      switch (buttons1function[i - 1]) {
        case 1:
          AtemSwitcher.setPreviewInputVideoSource(buttonsATEM[i - 1], 1);
          break;
        case 2:
          AtemSwitcher.setPreviewInputVideoSource(buttonsATEM[i - 1], 2);
          break;
        case 3:
          AtemSwitcher.setPreviewInputVideoSource(buttonsATEM[i - 1], 3);
          break;
        case 4:
          AtemSwitcher.setPreviewInputVideoSource(buttonsATEM[i - 1], 4);
          break;
        case 5:
          AtemSwitcher.setPreviewInputVideoSource(buttonsATEM[i - 1], 5);
          break;
        case 6:
          AtemSwitcher.setPreviewInputVideoSource(buttonsATEM[i - 1], 6);
          break;
        case 7:
          AtemSwitcher.setPreviewInputVideoSource(buttonsATEM[i - 1], 7);
          break;
        case 8:
          AtemSwitcher.setPreviewInputVideoSource(buttonsATEM[i - 1], 8);
          break;
        case 9:
          AtemSwitcher.setPreviewInputVideoSource(buttonsATEM[i - 1], 9);
          break;
        case 10:
          AtemSwitcher.setPreviewInputVideoSource(buttonsATEM[i - 1], 10);
          break;
        case 11:
          AtemSwitcher.setPreviewInputVideoSource(buttonsATEM[i - 1], 11);
          break;
        case 12:
          AtemSwitcher.setPreviewInputVideoSource(buttonsATEM[i - 1], 12);
          break;
        case 13:
          AtemSwitcher.setPreviewInputVideoSource(buttonsATEM[i - 1], 13);
          break;
        case 14:
          AtemSwitcher.setPreviewInputVideoSource(buttonsATEM[i - 1], 14);
          break;
        case 15:
          AtemSwitcher.setPreviewInputVideoSource(buttonsATEM[i - 1], 15);
          break;
        case 16:
          AtemSwitcher.setPreviewInputVideoSource(buttonsATEM[i - 1], 16);
          break;
        case 17:
          AtemSwitcher.setPreviewInputVideoSource(buttonsATEM[i - 1], 17);
          break;
        case 18:
          AtemSwitcher.setPreviewInputVideoSource(buttonsATEM[i - 1], 18);
          break;
        case 19:
          AtemSwitcher.setPreviewInputVideoSource(buttonsATEM[i - 1], 19);
          break;
        case 20:
          AtemSwitcher.setPreviewInputVideoSource(buttonsATEM[i - 1], 20);
          break;
        case 21:
          AtemSwitcher.setProgramInputVideoSource(buttonsATEM[i - 1], 1);
          break;
        case 22:
          AtemSwitcher.setProgramInputVideoSource(buttonsATEM[i - 1], 2);
          break;
        case 23:
          AtemSwitcher.setProgramInputVideoSource(buttonsATEM[i - 1], 3);
          break;
        case 24:
          AtemSwitcher.setProgramInputVideoSource(buttonsATEM[i - 1], 4);
          break;
        case 25:
          AtemSwitcher.setProgramInputVideoSource(buttonsATEM[i - 1], 5);
          break;
        case 26:
          AtemSwitcher.setProgramInputVideoSource(buttonsATEM[i - 1], 6);
          break;
        case 27:
          AtemSwitcher.setProgramInputVideoSource(buttonsATEM[i - 1], 7);
          break;
        case 28:
          AtemSwitcher.setProgramInputVideoSource(buttonsATEM[i - 1], 8);
          break;
        case 29:
          AtemSwitcher.setProgramInputVideoSource(buttonsATEM[i - 1], 9);
          break;
        case 30:
          AtemSwitcher.setProgramInputVideoSource(buttonsATEM[i - 1], 10);
          break;
        case 31:
          AtemSwitcher.setProgramInputVideoSource(buttonsATEM[i - 1], 11);
          break;
        case 32:
          AtemSwitcher.setProgramInputVideoSource(buttonsATEM[i - 1], 12);
          break;
        case 33:
          AtemSwitcher.setProgramInputVideoSource(buttonsATEM[i - 1], 13);
          break;
        case 34:
          AtemSwitcher.setProgramInputVideoSource(buttonsATEM[i - 1], 14);
          break;
        case 35:
          AtemSwitcher.setProgramInputVideoSource(buttonsATEM[i - 1], 15);
          break;
        case 36:
          AtemSwitcher.setProgramInputVideoSource(buttonsATEM[i - 1], 16);
          break;
        case 37:
          AtemSwitcher.setProgramInputVideoSource(buttonsATEM[i - 1], 17);
          break;
        case 38:
          AtemSwitcher.setProgramInputVideoSource(buttonsATEM[i - 1], 18);
          break;
        case 39:
          AtemSwitcher.setProgramInputVideoSource(buttonsATEM[i - 1], 19);
          break;
        case 40:
          AtemSwitcher.setProgramInputVideoSource(buttonsATEM[i - 1], 20);
          break;
        case 41:
          AtemSwitcher.setPreviewInputVideoSource(buttonsATEM[i - 1], 0);
          break;
        case 42:
          AtemSwitcher.setProgramInputVideoSource(buttonsATEM[i - 1], 0);
          break;
        case 43:
          AtemSwitcher.setPreviewInputVideoSource(buttonsATEM[i - 1], 1000);
          break;
        case 44:
          AtemSwitcher.setProgramInputVideoSource(buttonsATEM[i - 1], 1000);
          break;
        case 45:
          AtemSwitcher.setPreviewInputVideoSource(buttonsATEM[i - 1], 2001);
          break;
        case 46:
          AtemSwitcher.setProgramInputVideoSource(buttonsATEM[i - 1], 2001);
          break;
        case 47:
          AtemSwitcher.setPreviewInputVideoSource(buttonsATEM[i - 1], 2002);
          break;
        case 48:
          AtemSwitcher.setProgramInputVideoSource(buttonsATEM[i - 1], 2002);
          break;
        case 49:
          AtemSwitcher.setPreviewInputVideoSource(buttonsATEM[i - 1], 3010);
          break;
        case 50:
          AtemSwitcher.setProgramInputVideoSource(buttonsATEM[i - 1], 3010);
          break;
        case 51:
          AtemSwitcher.setPreviewInputVideoSource(buttonsATEM[i - 1], 3020);
          break;
        case 52:
          AtemSwitcher.setProgramInputVideoSource(buttonsATEM[i - 1], 3020);
          break;
        case 53:
          AtemSwitcher.setPreviewInputVideoSource(buttonsATEM[i - 1], 6000);
        case 54:
          AtemSwitcher.setProgramInputVideoSource(buttonsATEM[i - 1], 6000);
          break;
        case 255:
          // The following 6 if-clauses detects if a button is pressed for input selection:
          switch (i) {
            case 1:
              AtemSwitcher.setPreviewInputVideoSource(buttonsATEM[i - 1], 1);
              break;
            case 2:
              AtemSwitcher.setPreviewInputVideoSource(buttonsATEM[i - 1], 2);
              break;
            case 3:
              AtemSwitcher.setPreviewInputVideoSource(buttonsATEM[i - 1], 3);
              break;
            case 4:
              AtemSwitcher.setPreviewInputVideoSource(buttonsATEM[i - 1], 4);
              break;
            case 5:
              AtemSwitcher.setPreviewInputVideoSource(buttonsATEM[i - 1], 5);
              break;
            case 6:
              AtemSwitcher.setPreviewInputVideoSource(buttonsATEM[i - 1], 6);
              break;


            // The following 6 if-clauses detects if a button is pressed for input selection:
            case 7:
              AtemSwitcher.setProgramInputVideoSource(buttonsATEM[i - 1], 1);
              break;
            case 8:
              AtemSwitcher.setProgramInputVideoSource(buttonsATEM[i - 1], 2);
              break;
            case 9:
              AtemSwitcher.setProgramInputVideoSource(buttonsATEM[i - 1], 3);
              break;
            case 10:
              AtemSwitcher.setProgramInputVideoSource(buttonsATEM[i - 1], 4);
              break;
            case 11:
              AtemSwitcher.setProgramInputVideoSource(buttonsATEM[i - 1], 5);
              break;
            case 12:
              AtemSwitcher.setProgramInputVideoSource(buttonsATEM[i - 1], 6);
              break;

            // The following detects if a button is pressed for either AUTO or CUT:
            case 13:
              buttons.setButtonColor(13, 1);   // Highlight CUT button
              AtemSwitcher.performCutME(buttonsATEM[i - 1]);
              break;
            case 14:
              AtemSwitcher.performAutoME(buttonsATEM[i - 1]);
              break;

            // Key1
            case 15:
              AtemSwitcher.setKeyerOnAirEnabled(buttonsATEM[i - 1], 0, !AtemSwitcher.getKeyerOnAirEnabled(buttonsATEM[i - 1], 0));
              break;

            // Media1:
            case 16:
              AtemSwitcher.setPreviewInputVideoSource(buttonsATEM[i - 1], 3010);
              break;
          }
          break;
      }
      switch (buttons2function[i - 1]) {
        case 1:
          AtemSwitcher.setKeyerOnAirEnabled(buttonsATEM[i - 1], 0, !AtemSwitcher.getKeyerOnAirEnabled(buttonsATEM[i - 1], 0)); // Toggle USK 1
          break;
        case 2:
          AtemSwitcher.setKeyerOnAirEnabled(buttonsATEM[i - 1], 1, !AtemSwitcher.getKeyerOnAirEnabled(buttonsATEM[i - 1], 1)); // Toggle USK 2
          break;
        case 3:
          AtemSwitcher.setKeyerOnAirEnabled(buttonsATEM[i - 1], 2, !AtemSwitcher.getKeyerOnAirEnabled(buttonsATEM[i - 1], 2)); // Toggle USK 3
          break;
        case 4:
          AtemSwitcher.setKeyerOnAirEnabled(buttonsATEM[i - 1], 3, !AtemSwitcher.getKeyerOnAirEnabled(buttonsATEM[i - 1], 3)); // Toggle USK 4
          break;
        case 5:
          AtemSwitcher.setDownstreamKeyerOnAir(0, !AtemSwitcher.getDownstreamKeyerOnAir(0)); // Toggle DSK 1
          break;
        case 6:
          AtemSwitcher.setDownstreamKeyerOnAir(1, !AtemSwitcher.getDownstreamKeyerOnAir(1)); // Toggle DSK 2
          break;
        case 7:
          AtemSwitcher.performDownstreamKeyerAutoKeyer(0); // DSK 1 Auto
          break;
        case 8:
          AtemSwitcher.performDownstreamKeyerAutoKeyer(1); // DSK 2 Auto
          break;
        case 9:
          AtemSwitcher.performCutME(buttonsATEM[i - 1]);
          buttons.setButtonColor(i, 1);   // Highlight CUT button
          break;
        case 10:
          AtemSwitcher.performAutoME(buttonsATEM[i - 1]);
          break;
        case 11:
          AtemSwitcher.performFadeToBlackME(buttonsATEM[i - 1]);
          break;
        case 12:
          for (int j = 1; j <= 4; j++) {
            AtemSwitcher.setKeyerOnAirEnabled(buttonsATEM[i - 1], j - 1, 0);
          }
          for (int j = 1; j <= 2; j++) {
            AtemSwitcher.setDownstreamKeyerOnAir(j - 1, 0);
          }
          break;
        case 13:
          GPOstate[0] = true;
          break;
        case 14:
          GPOstate[1] = true;
          break;
        case 15:
          GPOstate[2] = true;
          break;
        case 16:
          GPOstate[3] = true;
          break;
        case 17:
          GPOstate[4] = true;
          break;
        case 18:
          GPOstate[5] = true;
          break;
        case 19:
          GPOstate[6] = true;
          break;
        case 20:
          GPOstate[7] = true;
          break;
        case 21:
          GPOstate[0] = !GPOstate[0];
          break;
        case 22:
          GPOstate[1] = !GPOstate[1];
          break;
        case 23:
          GPOstate[2] = !GPOstate[2];
          break;
        case 24:
          GPOstate[3] = !GPOstate[3];
          break;
        case 25:
          GPOstate[4] = !GPOstate[4];
          break;
        case 26:
          GPOstate[5] = !GPOstate[5];
          break;
        case 27:
          GPOstate[6] = !GPOstate[6];
          break;
        case 28:
          GPOstate[7] = !GPOstate[7];
          break;
      }
    }
    if (buttons.isButtonIn(i, busSelectionUp))  {
      switch (buttons2function[i - 1]) {
        case 13:
          GPOstate[0] = false;
          break;
        case 14:
          GPOstate[1] = false;
          break;
        case 15:
          GPOstate[2] = false;
          break;
        case 16:
          GPOstate[3] = false;
          break;
        case 17:
          GPOstate[4] = false;
          break;
        case 18:
          GPOstate[5] = false;
          break;
        case 19:
          GPOstate[6] = false;
          break;
        case 20:
          GPOstate[7] = false;
          break;
      }
    }
    switch (buttons2function[i - 1]) {
      case 13:
      case 21:
        GPOblock[0] = true;
        GPIOboard.setOutput(1, GPOstate[0] ? HIGH : LOW);
        break;
      case 14:
      case 22:
        GPOblock[1] = true;
        GPIOboard.setOutput(2, GPOstate[1] ? HIGH : LOW);
        break;
      case 15:
      case 23:
        GPOblock[2] = true;
        GPIOboard.setOutput(3, GPOstate[2] ? HIGH : LOW);
        break;
      case 16:
      case 24:
        GPOblock[3] = true;
        GPIOboard.setOutput(4, GPOstate[3] ? HIGH : LOW);
        break;
      case 17:
      case 25:
        GPOblock[4] = true;
        GPIOboard.setOutput(5, GPOstate[4] ? HIGH : LOW);
        break;
      case 18:
      case 26:
        GPOblock[5] = true;
        GPIOboard.setOutput(6, GPOstate[5] ? HIGH : LOW);
        break;
      case 19:
      case 27:
        GPOblock[6] = true;
        GPIOboard.setOutput(7, GPOstate[6] ? HIGH : LOW);
        break;
      case 20:
      case 28:
        GPOblock[7] = true;
        GPIOboard.setOutput(8, GPOstate[7] ? HIGH : LOW);
        break;
    }
  }
}

/*********************************
   Set buttons colors
 *********************************/

void setButtonColors() {
  uint8_t busGPIOIn = GPIOboard.inputIsActiveAll();
  for (int i = 1; i <= 16; i++) {
    switch (buttons1function[i - 1]) {
      case 0:
        switch (buttons2function[i - 1]) {
          case 1:
            buttons.setButtonColor(i, AtemSwitcher.getKeyerOnAirEnabled(buttonsATEM[i - 1], 0) ? 1 : 3);
            break;
          case 2:
            buttons.setButtonColor(i, AtemSwitcher.getKeyerOnAirEnabled(buttonsATEM[i - 1], 1) ? 1 : 3);
            break;
          case 3:
            buttons.setButtonColor(i, AtemSwitcher.getKeyerOnAirEnabled(buttonsATEM[i - 1], 2) ? 1 : 3);
            break;
          case 4:
            buttons.setButtonColor(i, AtemSwitcher.getKeyerOnAirEnabled(buttonsATEM[i - 1], 3) ? 1 : 3);
            break;
          case 5:
            buttons.setButtonColor(i, AtemSwitcher.getDownstreamKeyerOnAir(0) ? 1 : 3);
            break;
          case 6:
            buttons.setButtonColor(i, AtemSwitcher.getDownstreamKeyerOnAir(1) ? 1 : 3);
            break;
          case 7:
            buttons.setButtonColor(i, AtemSwitcher.getDownstreamKeyerOnAir(0) ? 1 : 3);
            break;
          case 8:
            buttons.setButtonColor(i, AtemSwitcher.getDownstreamKeyerOnAir(1) ? 1 : 3);
            break;
          case 9:
            if (!buttons.buttonIsPressed(i))  {
              buttons.setButtonColor(i, 3);   // de-highlight CUT button
            }
            break;
          case 10:
            buttons.setButtonColor(i, AtemSwitcher.getTransitionInTransition(buttonsATEM[i - 1]) > 0 ? 1 : 3); // Auto button
            break;
          case 11:
            if (AtemSwitcher.getFadeToBlackStateInTransition(buttonsATEM[i - 1]))  { // Setting button color. This is a more complex example which includes blinking during execution:
              if (!AtemSwitcher.getFadeToBlackStateFullyBlack(buttonsATEM[i - 1]) && (AtemSwitcher.getFadeToBlackStateFramesRemaining(buttonsATEM[i - 1]) != AtemSwitcher.getFadeToBlackRate(buttonsATEM[i - 1])))  { // It's important to test if fadeToBlack time is more than zero because it's the kind of state from the ATEM which is usually not captured during initialization. Hopefull this will be solved in the future.
                // Blinking with AMBER color if Fade To Black is executing:
                if ((unsigned long)millis() & B10000000)  {
                  buttons.setButtonColor(i, 1);
                }
                else {
                  buttons.setButtonColor(i, 2);
                }
              }
            }
            else if (AtemSwitcher.getFadeToBlackStateFullyBlack(buttonsATEM[i - 1])) {
              buttons.setButtonColor(i, 1);  // Sets color of button to RED (2) if Fade To Black is activated
            }
            else {
              buttons.setButtonColor(i, 3);  // Dimmed background if no fade to black
            }
            break;
          case 12:
            if (AtemSwitcher.getKeyerOnAirEnabled(buttonsATEM[i - 1], 0) || AtemSwitcher.getKeyerOnAirEnabled(buttonsATEM[i - 1], 1) || AtemSwitcher.getKeyerOnAirEnabled(buttonsATEM[i - 1], 2) || AtemSwitcher.getKeyerOnAirEnabled(buttonsATEM[i - 1], 3) || AtemSwitcher.getDownstreamKeyerOnAir(0) || AtemSwitcher.getDownstreamKeyerOnAir(1)) {
              buttons.setButtonColor(i, 1);
            }
            else {
              buttons.setButtonColor(i, 3);
            }
            break;
        }
        break;
      case 1:
        buttons.setButtonColor(i, AtemSwitcher.getPreviewInputVideoSource(buttonsATEM[i - 1]) == 1 ? 1 : 3);
        break;
      case 2:
        buttons.setButtonColor(i, AtemSwitcher.getPreviewInputVideoSource(buttonsATEM[i - 1]) == 2 ? 1 : 3);
        break;
      case 3:
        buttons.setButtonColor(i, AtemSwitcher.getPreviewInputVideoSource(buttonsATEM[i - 1]) == 3 ? 1 : 3);
        break;
      case 4:
        buttons.setButtonColor(i, AtemSwitcher.getPreviewInputVideoSource(buttonsATEM[i - 1]) == 4 ? 1 : 3);
        break;
      case 5:
        buttons.setButtonColor(i, AtemSwitcher.getPreviewInputVideoSource(buttonsATEM[i - 1]) == 5 ? 1 : 3);
        break;
      case 6:
        buttons.setButtonColor(i, AtemSwitcher.getPreviewInputVideoSource(buttonsATEM[i - 1]) == 6 ? 1 : 3);
        break;
      case 7:
        buttons.setButtonColor(i, AtemSwitcher.getPreviewInputVideoSource(buttonsATEM[i - 1]) == 7 ? 1 : 3);
        break;
      case 8:
        buttons.setButtonColor(i, AtemSwitcher.getPreviewInputVideoSource(buttonsATEM[i - 1]) == 8 ? 1 : 3);
        break;
      case 9:
        buttons.setButtonColor(i, AtemSwitcher.getPreviewInputVideoSource(buttonsATEM[i - 1]) == 9 ? 1 : 3);
        break;
      case 10:
        buttons.setButtonColor(i, AtemSwitcher.getPreviewInputVideoSource(buttonsATEM[i - 1]) == 10 ? 1 : 3);
        break;
      case 11:
        buttons.setButtonColor(i, AtemSwitcher.getPreviewInputVideoSource(buttonsATEM[i - 1]) == 11 ? 1 : 3);
        break;
      case 12:
        buttons.setButtonColor(i, AtemSwitcher.getPreviewInputVideoSource(buttonsATEM[i - 1]) == 12 ? 1 : 3);
        break;
      case 13:
        buttons.setButtonColor(i, AtemSwitcher.getPreviewInputVideoSource(buttonsATEM[i - 1]) == 13 ? 1 : 3);
        break;
      case 14:
        buttons.setButtonColor(i, AtemSwitcher.getPreviewInputVideoSource(buttonsATEM[i - 1]) == 14 ? 1 : 3);
        break;
      case 15:
        buttons.setButtonColor(i, AtemSwitcher.getPreviewInputVideoSource(buttonsATEM[i - 1]) == 15 ? 1 : 3);
        break;
      case 16:
        buttons.setButtonColor(i, AtemSwitcher.getPreviewInputVideoSource(buttonsATEM[i - 1]) == 16 ? 1 : 3);
        break;
      case 17:
        buttons.setButtonColor(i, AtemSwitcher.getPreviewInputVideoSource(buttonsATEM[i - 1]) == 17 ? 1 : 3);
        break;
      case 18:
        buttons.setButtonColor(i, AtemSwitcher.getPreviewInputVideoSource(buttonsATEM[i - 1]) == 18 ? 1 : 3);
        break;
      case 19:
        buttons.setButtonColor(i, AtemSwitcher.getPreviewInputVideoSource(buttonsATEM[i - 1]) == 19 ? 1 : 3);
        break;
      case 20:
        buttons.setButtonColor(i, AtemSwitcher.getPreviewInputVideoSource(buttonsATEM[i - 1]) == 20 ? 1 : 3);
        break;
      case 21:
        buttons.setButtonColor(i, AtemSwitcher.getProgramInputVideoSource(buttonsATEM[i - 1]) == 1 ? 1 : 3);
        break;
      case 22:
        buttons.setButtonColor(i, AtemSwitcher.getProgramInputVideoSource(buttonsATEM[i - 1]) == 2 ? 1 : 3);
        break;
      case 23:
        buttons.setButtonColor(i, AtemSwitcher.getProgramInputVideoSource(buttonsATEM[i - 1]) == 3 ? 1 : 3);
        break;
      case 24:
        buttons.setButtonColor(i, AtemSwitcher.getProgramInputVideoSource(buttonsATEM[i - 1]) == 4 ? 1 : 3);
        break;
      case 25:
        buttons.setButtonColor(i, AtemSwitcher.getProgramInputVideoSource(buttonsATEM[i - 1]) == 5 ? 1 : 3);
        break;
      case 26:
        buttons.setButtonColor(i, AtemSwitcher.getProgramInputVideoSource(buttonsATEM[i - 1]) == 6 ? 1 : 3);
        break;
      case 27:
        buttons.setButtonColor(i, AtemSwitcher.getProgramInputVideoSource(buttonsATEM[i - 1]) == 7 ? 1 : 3);
        break;
      case 28:
        buttons.setButtonColor(i, AtemSwitcher.getProgramInputVideoSource(buttonsATEM[i - 1]) == 8 ? 1 : 3);
        break;
      case 29:
        buttons.setButtonColor(i, AtemSwitcher.getProgramInputVideoSource(buttonsATEM[i - 1]) == 9 ? 1 : 3);
        break;
      case 30:
        buttons.setButtonColor(i, AtemSwitcher.getProgramInputVideoSource(buttonsATEM[i - 1]) == 10 ? 1 : 3);
        break;
      case 31:
        buttons.setButtonColor(i, AtemSwitcher.getProgramInputVideoSource(buttonsATEM[i - 1]) == 11 ? 1 : 3);
        break;
      case 32:
        buttons.setButtonColor(i, AtemSwitcher.getProgramInputVideoSource(buttonsATEM[i - 1]) == 12 ? 1 : 3);
        break;
      case 33:
        buttons.setButtonColor(i, AtemSwitcher.getProgramInputVideoSource(buttonsATEM[i - 1]) == 13 ? 1 : 3);
        break;
      case 34:
        buttons.setButtonColor(i, AtemSwitcher.getProgramInputVideoSource(buttonsATEM[i - 1]) == 14 ? 1 : 3);
        break;
      case 35:
        buttons.setButtonColor(i, AtemSwitcher.getProgramInputVideoSource(buttonsATEM[i - 1]) == 15 ? 1 : 3);
        break;
      case 36:
        buttons.setButtonColor(i, AtemSwitcher.getProgramInputVideoSource(buttonsATEM[i - 1]) == 16 ? 1 : 3);
        break;
      case 37:
        buttons.setButtonColor(i, AtemSwitcher.getProgramInputVideoSource(buttonsATEM[i - 1]) == 17 ? 1 : 3);
        break;
      case 38:
        buttons.setButtonColor(i, AtemSwitcher.getProgramInputVideoSource(buttonsATEM[i - 1]) == 18 ? 1 : 3);
        break;
      case 39:
        buttons.setButtonColor(i, AtemSwitcher.getProgramInputVideoSource(buttonsATEM[i - 1]) == 19 ? 1 : 3);
        break;
      case 40:
        buttons.setButtonColor(i, AtemSwitcher.getProgramInputVideoSource(buttonsATEM[i - 1]) == 20 ? 1 : 3);
        break;
      case 41:
        buttons.setButtonColor(i, AtemSwitcher.getPreviewInputVideoSource(buttonsATEM[i - 1]) == 0 ? 1 : 3);
        break;
      case 42:
        buttons.setButtonColor(i, AtemSwitcher.getProgramInputVideoSource(buttonsATEM[i - 1]) == 0 ? 1 : 3);
        break;
      case 43:
        buttons.setButtonColor(i, AtemSwitcher.getPreviewInputVideoSource(buttonsATEM[i - 1]) == 1000 ? 1 : 3);
        break;
      case 44:
        buttons.setButtonColor(i, AtemSwitcher.getProgramInputVideoSource(buttonsATEM[i - 1]) == 1000 ? 1 : 3);
        break;
      case 45:
        buttons.setButtonColor(i, AtemSwitcher.getPreviewInputVideoSource(buttonsATEM[i - 1]) == 2001 ? 1 : 3);
        break;
      case 46:
        buttons.setButtonColor(i, AtemSwitcher.getProgramInputVideoSource(buttonsATEM[i - 1]) == 2001 ? 1 : 3);
        break;
      case 47:
        buttons.setButtonColor(i, AtemSwitcher.getPreviewInputVideoSource(buttonsATEM[i - 1]) == 2002 ? 1 : 3);
        break;
      case 48:
        buttons.setButtonColor(i, AtemSwitcher.getProgramInputVideoSource(buttonsATEM[i - 1]) == 2002 ? 1 : 3);
        break;
      case 49:
        buttons.setButtonColor(i, AtemSwitcher.getPreviewInputVideoSource(buttonsATEM[i - 1]) == 3010 ? 1 : 3);
        break;
      case 50:
        buttons.setButtonColor(i, AtemSwitcher.getProgramInputVideoSource(buttonsATEM[i - 1]) == 3010 ? 1 : 3);
        break;
      case 51:
        buttons.setButtonColor(i, AtemSwitcher.getPreviewInputVideoSource(buttonsATEM[i - 1]) == 3020 ? 1 : 3);
        break;
      case 52:
        buttons.setButtonColor(i, AtemSwitcher.getProgramInputVideoSource(buttonsATEM[i - 1]) == 3020 ? 1 : 3);
        break;
      case 53:
        buttons.setButtonColor(i, AtemSwitcher.getPreviewInputVideoSource(buttonsATEM[i - 1]) == 6000 ? 1 : 3);
        break;
      case 54:
        buttons.setButtonColor(i, AtemSwitcher.getProgramInputVideoSource(buttonsATEM[i - 1]) == 6000 ? 1 : 3);
        break;
      case 55:
        buttons.setButtonColor(i, GPIOboard.isInputIn(1, busGPIOIn) ? 1 : 3);
        break;
      case 56:
        buttons.setButtonColor(i, GPIOboard.isInputIn(2, busGPIOIn) ? 1 : 3);
        break;
      case 57:
        buttons.setButtonColor(i, GPIOboard.isInputIn(3, busGPIOIn) ? 1 : 3);
        break;
      case 58:
        buttons.setButtonColor(i, GPIOboard.isInputIn(4, busGPIOIn) ? 1 : 3);
        break;
      case 59:
        buttons.setButtonColor(i, GPIOboard.isInputIn(5, busGPIOIn) ? 1 : 3);
        break;
      case 60:
        buttons.setButtonColor(i, GPIOboard.isInputIn(6, busGPIOIn) ? 1 : 3);
        break;
      case 61:
        buttons.setButtonColor(i, GPIOboard.isInputIn(7, busGPIOIn) ? 1 : 3);
        break;
      case 62:
        buttons.setButtonColor(i, GPIOboard.isInputIn(8, busGPIOIn) ? 1 : 3);
        break;
      case 255:
        if (i <= 6) {
          buttons.setButtonColor(i, AtemSwitcher.getPreviewInputVideoSource(buttonsATEM[i - 1]) == i ? 1 : 3);
        }
        else if (i <= 12) {
          buttons.setButtonColor(i, AtemSwitcher.getProgramInputVideoSource(buttonsATEM[i - 1]) == (i - 6) ? 1 : 3);
        }
        else if (i == 13) {
          if (!buttons.buttonIsPressed(13))  {
            buttons.setButtonColor(13, 3);   // de-highlight CUT button
          }
        }
        else if (i == 14) {
          buttons.setButtonColor(14, AtemSwitcher.getTransitionInTransition(buttonsATEM[i - 1]) ? 1 : 3);     // Auto button
        }
        else if (i == 15) {
          buttons.setButtonColor(15, AtemSwitcher.getKeyerOnAirEnabled(buttonsATEM[i - 1], 0) ? 1 : 3);     // Key button
        }
        else if (i == 16) {
          if (AtemSwitcher.getProgramInputVideoSource(buttonsATEM[i - 1]) == 3010)  { // Media 1 button
            buttons.setButtonColor(16, 1);
          }
          else if (AtemSwitcher.getPreviewInputVideoSource(buttonsATEM[i - 1]) == 3010) {
            buttons.setButtonColor(16, 2);
          }
          else {
            buttons.setButtonColor(16, 3);
          }
        }

        break;
    }
  }
}


/*************************
   GPIO
 *************************/

void setTally()  {
  for (uint8_t i = 1; i <= 8; i++)  {
    if (!GPOblock[i - 1]) {
      switch (GPO1[i - 1]) {
        // second GPO
        case 0:
          switch (GPO2[i]) {
            // Aux1
            case 1:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(0) == 1 ? HIGH : LOW);
              break;
            case 2:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(0) == 2 ? HIGH : LOW);
              break;
            case 3:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(0) == 3 ? HIGH : LOW);
              break;
            case 4:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(0) == 4 ? HIGH : LOW);
              break;
            case 5:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(0) == 5 ? HIGH : LOW);
              break;
            case 6:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(0) == 6 ? HIGH : LOW);
              break;
            case 7:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(0) == 7 ? HIGH : LOW);
              break;
            case 8:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(0) == 8 ? HIGH : LOW);
              break;
            case 9:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(0) == 9 ? HIGH : LOW);
              break;
            case 10:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(0) == 10 ? HIGH : LOW);
              break;
            case 11:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(0) == 11 ? HIGH : LOW);
              break;
            case 12:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(0) == 12 ? HIGH : LOW);
              break;
            case 13:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(0) == 13 ? HIGH : LOW);
              break;
            case 14:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(0) == 14 ? HIGH : LOW);
              break;
            case 15:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(0) == 15 ? HIGH : LOW);
              break;
            case 16:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(0) == 16 ? HIGH : LOW);
              break;
            case 17:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(0) == 17 ? HIGH : LOW);
              break;
            case 18:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(0) == 18 ? HIGH : LOW);
              break;
            case 19:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(0) == 19 ? HIGH : LOW);
              break;
            case 20:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(0) == 20 ? HIGH : LOW);
              break;
            case 21:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(0) == 0 ? HIGH : LOW);
              break;
            case 22:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(0) == 1000 ? HIGH : LOW);
              break;
            case 23:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(0) == 2001 ? HIGH : LOW);
              break;
            case 24:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(0) == 2002 ? HIGH : LOW);
              break;
            case 25:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(0) == 3010 ? HIGH : LOW);
              break;
            case 26:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(0) == 3011 ? HIGH : LOW);
              break;
            case 27:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(0) == 3020 ? HIGH : LOW);
              break;
            case 28:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(0) == 3021 ? HIGH : LOW);
              break;
            case 29:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(0) == 7001 ? HIGH : LOW);
              break;
            case 30:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(0) == 7002 ? HIGH : LOW);
              break;
            case 31:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(0) == 6000 ? HIGH : LOW);
              break;
            case 32:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(0) == 10010 ? HIGH : LOW);
              break;
            case 33:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(0) == 10011 ? HIGH : LOW);
              break;
            // Aux 2
            case 34:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(1) == 1 ? HIGH : LOW);
              break;
            case 35:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(1) == 2 ? HIGH : LOW);
              break;
            case 36:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(1) == 3 ? HIGH : LOW);
              break;
            case 37:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(1) == 4 ? HIGH : LOW);
              break;
            case 38:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(1) == 5 ? HIGH : LOW);
              break;
            case 39:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(1) == 6 ? HIGH : LOW);
              break;
            case 40:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(1) == 7 ? HIGH : LOW);
              break;
            case 41:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(1) == 8 ? HIGH : LOW);
              break;
            case 42:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(1) == 9 ? HIGH : LOW);
              break;
            case 43:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(1) == 10 ? HIGH : LOW);
              break;
            case 44:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(1) == 11 ? HIGH : LOW);
              break;
            case 45:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(1) == 12 ? HIGH : LOW);
              break;
            case 46:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(1) == 13 ? HIGH : LOW);
              break;
            case 47:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(1) == 14 ? HIGH : LOW);
              break;
            case 48:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(1) == 15 ? HIGH : LOW);
              break;
            case 49:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(1) == 16 ? HIGH : LOW);
              break;
            case 50:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(1) == 17 ? HIGH : LOW);
              break;
            case 51:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(1) == 18 ? HIGH : LOW);
              break;
            case 52:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(1) == 19 ? HIGH : LOW);
              break;
            case 53:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(1) == 20 ? HIGH : LOW);
              break;
            case 54:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(1) == 0 ? HIGH : LOW);
              break;
            case 55:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(1) == 1000 ? HIGH : LOW);
              break;
            case 56:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(1) == 2001 ? HIGH : LOW);
              break;
            case 57:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(1) == 2002 ? HIGH : LOW);
              break;
            case 58:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(1) == 3010 ? HIGH : LOW);
              break;
            case 59:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(1) == 3011 ? HIGH : LOW);
              break;
            case 60:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(1) == 3020 ? HIGH : LOW);
              break;
            case 61:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(1) == 3021 ? HIGH : LOW);
              break;
            case 62:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(1) == 7001 ? HIGH : LOW);
              break;
            case 63:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(1) == 7002 ? HIGH : LOW);
              break;
            case 64:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(1) == 6000 ? HIGH : LOW);
              break;
            case 65:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(1) == 10010 ? HIGH : LOW);
              break;
            case 66:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(1) == 10011 ? HIGH : LOW);
              break;
            //Aux 3
            case 67:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(2) == 1 ? HIGH : LOW);
              break;
            case 68:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(2) == 2 ? HIGH : LOW);
              break;
            case 69:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(2) == 3 ? HIGH : LOW);
              break;
            case 70:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(2) == 4 ? HIGH : LOW);
              break;
            case 71:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(2) == 5 ? HIGH : LOW);
              break;
            case 72:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(2) == 6 ? HIGH : LOW);
              break;
            case 73:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(2) == 7 ? HIGH : LOW);
              break;
            case 74:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(2) == 8 ? HIGH : LOW);
              break;
            case 75:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(2) == 9 ? HIGH : LOW);
              break;
            case 76:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(2) == 10 ? HIGH : LOW);
              break;
            case 77:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(2) == 11 ? HIGH : LOW);
              break;
            case 78:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(2) == 12 ? HIGH : LOW);
              break;
            case 79:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(2) == 13 ? HIGH : LOW);
              break;
            case 80:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(2) == 14 ? HIGH : LOW);
              break;
            case 81:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(2) == 15 ? HIGH : LOW);
              break;
            case 82:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(2) == 16 ? HIGH : LOW);
              break;
            case 83:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(2) == 17 ? HIGH : LOW);
              break;
            case 84:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(2) == 18 ? HIGH : LOW);
              break;
            case 85:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(2) == 19 ? HIGH : LOW);
              break;
            case 86:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(2) == 20 ? HIGH : LOW);
              break;
            case 87:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(2) == 0 ? HIGH : LOW);
              break;
            case 88:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(2) == 1000 ? HIGH : LOW);
              break;
            case 89:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(2) == 2001 ? HIGH : LOW);
              break;
            case 90:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(2) == 2002 ? HIGH : LOW);
              break;
            case 91:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(2) == 3010 ? HIGH : LOW);
              break;
            case 92:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(2) == 3011 ? HIGH : LOW);
              break;
            case 93:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(2) == 3020 ? HIGH : LOW);
              break;
            case 94:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(2) == 3021 ? HIGH : LOW);
              break;
            case 95:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(2) == 7001 ? HIGH : LOW);
              break;
            case 96:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(2) == 7002 ? HIGH : LOW);
              break;
            case 97:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(2) == 6000 ? HIGH : LOW);
              break;
            case 98:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(2) == 10010 ? HIGH : LOW);
              break;
            case 99:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(2) == 10011 ? HIGH : LOW);
              break;
            // Aux 4
            case 100:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(3) == 1 ? HIGH : LOW);
              break;
            case 101:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(3) == 2 ? HIGH : LOW);
              break;
            case 102:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(3) == 3 ? HIGH : LOW);
              break;
            case 103:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(3) == 4 ? HIGH : LOW);
              break;
            case 104:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(3) == 5 ? HIGH : LOW);
              break;
            case 105:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(3) == 6 ? HIGH : LOW);
              break;
            case 106:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(3) == 7 ? HIGH : LOW);
              break;
            case 107:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(3) == 8 ? HIGH : LOW);
              break;
            case 108:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(3) == 9 ? HIGH : LOW);
              break;
            case 109:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(3) == 10 ? HIGH : LOW);
              break;
            case 110:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(3) == 11 ? HIGH : LOW);
              break;
            case 111:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(3) == 12 ? HIGH : LOW);
              break;
            case 112:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(3) == 13 ? HIGH : LOW);
              break;
            case 113:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(3) == 14 ? HIGH : LOW);
              break;
            case 114:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(3) == 15 ? HIGH : LOW);
              break;
            case 115:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(3) == 16 ? HIGH : LOW);
              break;
            case 116:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(3) == 17 ? HIGH : LOW);
              break;
            case 117:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(3) == 18 ? HIGH : LOW);
              break;
            case 118:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(3) == 19 ? HIGH : LOW);
              break;
            case 119:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(3) == 20 ? HIGH : LOW);
              break;
            case 120:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(3) == 0 ? HIGH : LOW);
              break;
            case 121:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(3) == 1000 ? HIGH : LOW);
              break;
            case 122:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(3) == 2001 ? HIGH : LOW);
              break;
            case 123:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(3) == 2002 ? HIGH : LOW);
              break;
            case 124:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(3) == 3010 ? HIGH : LOW);
              break;
            case 125:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(3) == 3011 ? HIGH : LOW);
              break;
            case 126:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(3) == 3020 ? HIGH : LOW);
              break;
            case 127:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(3) == 3021 ? HIGH : LOW);
              break;
            case 128:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(3) == 7001 ? HIGH : LOW);
              break;
            case 129:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(3) == 7002 ? HIGH : LOW);
              break;
            case 130:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(3) == 6000 ? HIGH : LOW);
              break;
            case 131:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(3) == 10010 ? HIGH : LOW);
              break;
            case 132:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(3) == 10011 ? HIGH : LOW);
              break;
            // Aux 5
            case 133:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(4) == 1 ? HIGH : LOW);
              break;
            case 134:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(4) == 2 ? HIGH : LOW);
              break;
            case 135:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(4) == 3 ? HIGH : LOW);
              break;
            case 136:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(4) == 4 ? HIGH : LOW);
              break;
            case 137:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(4) == 5 ? HIGH : LOW);
              break;
            case 138:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(4) == 6 ? HIGH : LOW);
              break;
            case 139:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(4) == 7 ? HIGH : LOW);
              break;
            case 140:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(4) == 8 ? HIGH : LOW);
              break;
            case 141:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(4) == 9 ? HIGH : LOW);
              break;
            case 142:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(4) == 10 ? HIGH : LOW);
              break;
            case 143:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(4) == 11 ? HIGH : LOW);
              break;
            case 144:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(4) == 12 ? HIGH : LOW);
              break;
            case 145:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(4) == 13 ? HIGH : LOW);
              break;
            case 146:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(4) == 14 ? HIGH : LOW);
              break;
            case 147:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(4) == 15 ? HIGH : LOW);
              break;
            case 148:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(4) == 16 ? HIGH : LOW);
              break;
            case 149:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(4) == 17 ? HIGH : LOW);
              break;
            case 150:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(4) == 18 ? HIGH : LOW);
              break;
            case 151:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(4) == 19 ? HIGH : LOW);
              break;
            case 152:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(4) == 20 ? HIGH : LOW);
              break;
            case 153:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(4) == 0 ? HIGH : LOW);
              break;
            case 154:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(4) == 1000 ? HIGH : LOW);
              break;
            case 155:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(4) == 2001 ? HIGH : LOW);
              break;
            case 156:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(4) == 2002 ? HIGH : LOW);
              break;
            case 157:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(4) == 3010 ? HIGH : LOW);
              break;
            case 158:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(4) == 3011 ? HIGH : LOW);
              break;
            case 159:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(4) == 3020 ? HIGH : LOW);
              break;
            case 160:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(4) == 3021 ? HIGH : LOW);
              break;
            case 161:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(4) == 7001 ? HIGH : LOW);
              break;
            case 162:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(4) == 7002 ? HIGH : LOW);
              break;
            case 163:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(4) == 6000 ? HIGH : LOW);
              break;
            case 164:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(4) == 10010 ? HIGH : LOW);
              break;
            case 165:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(4) == 10011 ? HIGH : LOW);
              break;
            // Aux 6
            case 166:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(5) == 1 ? HIGH : LOW);
              break;
            case 167:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(5) == 2 ? HIGH : LOW);
              break;
            case 168:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(5) == 3 ? HIGH : LOW);
              break;
            case 169:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(5) == 4 ? HIGH : LOW);
              break;
            case 170:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(5) == 5 ? HIGH : LOW);
              break;
            case 171:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(5) == 6 ? HIGH : LOW);
              break;
            case 172:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(5) == 7 ? HIGH : LOW);
              break;
            case 173:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(5) == 8 ? HIGH : LOW);
              break;
            case 174:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(5) == 9 ? HIGH : LOW);
              break;
            case 175:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(5) == 10 ? HIGH : LOW);
              break;
            case 176:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(5) == 11 ? HIGH : LOW);
              break;
            case 177:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(5) == 12 ? HIGH : LOW);
              break;
            case 178:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(5) == 13 ? HIGH : LOW);
              break;
            case 179:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(5) == 14 ? HIGH : LOW);
              break;
            case 180:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(5) == 15 ? HIGH : LOW);
              break;
            case 181:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(5) == 16 ? HIGH : LOW);
              break;
            case 182:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(5) == 17 ? HIGH : LOW);
              break;
            case 183:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(5) == 18 ? HIGH : LOW);
              break;
            case 184:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(5) == 19 ? HIGH : LOW);
              break;
            case 185:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(5) == 20 ? HIGH : LOW);
              break;
            case 186:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(5) == 0 ? HIGH : LOW);
              break;
            case 187:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(5) == 1000 ? HIGH : LOW);
              break;
            case 188:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(5) == 2001 ? HIGH : LOW);
              break;
            case 189:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(5) == 2002 ? HIGH : LOW);
              break;
            case 190:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(5) == 3010 ? HIGH : LOW);
              break;
            case 191:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(5) == 3011 ? HIGH : LOW);
              break;
            case 192:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(5) == 3020 ? HIGH : LOW);
              break;
            case 193:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(5) == 3021 ? HIGH : LOW);
              break;
            case 194:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(5) == 7001 ? HIGH : LOW);
              break;
            case 195:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(5) == 7002 ? HIGH : LOW);
              break;
            case 196:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(5) == 6000 ? HIGH : LOW);
              break;
            case 197:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(5) == 10010 ? HIGH : LOW);
              break;
            case 198:
              GPIOboard.setOutput(i, AtemSwitcher.getAuxSourceInput(5) == 10011 ? HIGH : LOW);
              break;
          }
          break;
        // Preview input
        case 1:
          GPIOboard.setOutput(i, AtemSwitcher.getPreviewInputVideoSource(GPOATEM[i - 1]) == 1 ? HIGH : LOW);
          break;
        case 2:
          GPIOboard.setOutput(i, AtemSwitcher.getPreviewInputVideoSource(GPOATEM[i - 1]) == 2 ? HIGH : LOW);
          break;
        case 3:
          GPIOboard.setOutput(i, AtemSwitcher.getPreviewInputVideoSource(GPOATEM[i - 1]) == 3 ? HIGH : LOW);
          break;
        case 4:
          GPIOboard.setOutput(i, AtemSwitcher.getPreviewInputVideoSource(GPOATEM[i - 1]) == 4 ? HIGH : LOW);
          break;
        case 5:
          GPIOboard.setOutput(i, AtemSwitcher.getPreviewInputVideoSource(GPOATEM[i - 1]) == 5 ? HIGH : LOW);
          break;
        case 6:
          GPIOboard.setOutput(i, AtemSwitcher.getPreviewInputVideoSource(GPOATEM[i - 1]) == 6 ? HIGH : LOW);
          break;
        case 7:
          GPIOboard.setOutput(i, AtemSwitcher.getPreviewInputVideoSource(GPOATEM[i - 1]) == 7 ? HIGH : LOW);
          break;
        case 8:
          GPIOboard.setOutput(i, AtemSwitcher.getPreviewInputVideoSource(GPOATEM[i - 1]) == 8 ? HIGH : LOW);
          break;
        case 9:
          GPIOboard.setOutput(i, AtemSwitcher.getPreviewInputVideoSource(GPOATEM[i - 1]) == 9 ? HIGH : LOW);
          break;
        case 10:
          GPIOboard.setOutput(i, AtemSwitcher.getPreviewInputVideoSource(GPOATEM[i - 1]) == 10 ? HIGH : LOW);
          break;
        case 11:
          GPIOboard.setOutput(i, AtemSwitcher.getPreviewInputVideoSource(GPOATEM[i - 1]) == 11 ? HIGH : LOW);
          break;
        case 12:
          GPIOboard.setOutput(i, AtemSwitcher.getPreviewInputVideoSource(GPOATEM[i - 1]) == 12 ? HIGH : LOW);
          break;
        case 13:
          GPIOboard.setOutput(i, AtemSwitcher.getPreviewInputVideoSource(GPOATEM[i - 1]) == 13 ? HIGH : LOW);
          break;
        case 14:
          GPIOboard.setOutput(i, AtemSwitcher.getPreviewInputVideoSource(GPOATEM[i - 1]) == 14 ? HIGH : LOW);
          break;
        case 15:
          GPIOboard.setOutput(i, AtemSwitcher.getPreviewInputVideoSource(GPOATEM[i - 1]) == 15 ? HIGH : LOW);
          break;
        case 16:
          GPIOboard.setOutput(i, AtemSwitcher.getPreviewInputVideoSource(GPOATEM[i - 1]) == 16 ? HIGH : LOW);
          break;
        case 17:
          GPIOboard.setOutput(i, AtemSwitcher.getPreviewInputVideoSource(GPOATEM[i - 1]) == 17 ? HIGH : LOW);
          break;
        case 18:
          GPIOboard.setOutput(i, AtemSwitcher.getPreviewInputVideoSource(GPOATEM[i - 1]) == 18 ? HIGH : LOW);
          break;
        case 19:
          GPIOboard.setOutput(i, AtemSwitcher.getPreviewInputVideoSource(GPOATEM[i - 1]) == 19 ? HIGH : LOW);
          break;
        case 20:
          GPIOboard.setOutput(i, AtemSwitcher.getPreviewInputVideoSource(GPOATEM[i - 1]) == 20 ? HIGH : LOW);
          break;

        // program input
        case 21:
          GPIOboard.setOutput(i, AtemSwitcher.getProgramInputVideoSource(GPOATEM[i - 1]) == 1 ? HIGH : LOW);
          break;
        case 22:
          GPIOboard.setOutput(i, AtemSwitcher.getProgramInputVideoSource(GPOATEM[i - 1]) == 2 ? HIGH : LOW);
          break;
        case 23:
          GPIOboard.setOutput(i, AtemSwitcher.getProgramInputVideoSource(GPOATEM[i - 1]) == 3 ? HIGH : LOW);
          break;
        case 24:
          GPIOboard.setOutput(i, AtemSwitcher.getProgramInputVideoSource(GPOATEM[i - 1]) == 4 ? HIGH : LOW);
          break;
        case 25:
          GPIOboard.setOutput(i, AtemSwitcher.getProgramInputVideoSource(GPOATEM[i - 1]) == 5 ? HIGH : LOW);
          break;
        case 26:
          GPIOboard.setOutput(i, AtemSwitcher.getProgramInputVideoSource(GPOATEM[i - 1]) == 6 ? HIGH : LOW);
          break;
        case 27:
          GPIOboard.setOutput(i, AtemSwitcher.getProgramInputVideoSource(GPOATEM[i - 1]) == 7 ? HIGH : LOW);
          break;
        case 28:
          GPIOboard.setOutput(i, AtemSwitcher.getProgramInputVideoSource(GPOATEM[i - 1]) == 8 ? HIGH : LOW);
          break;
        case 29:
          GPIOboard.setOutput(i, AtemSwitcher.getProgramInputVideoSource(GPOATEM[i - 1]) == 9 ? HIGH : LOW);
          break;
        case 30:
          GPIOboard.setOutput(i, AtemSwitcher.getProgramInputVideoSource(GPOATEM[i - 1]) == 10 ? HIGH : LOW);
          break;
        case 31:
          GPIOboard.setOutput(i, AtemSwitcher.getProgramInputVideoSource(GPOATEM[i - 1]) == 11 ? HIGH : LOW);
          break;
        case 32:
          GPIOboard.setOutput(i, AtemSwitcher.getProgramInputVideoSource(GPOATEM[i - 1]) == 12 ? HIGH : LOW);
          break;
        case 33:
          GPIOboard.setOutput(i, AtemSwitcher.getProgramInputVideoSource(GPOATEM[i - 1]) == 13 ? HIGH : LOW);
          break;
        case 34:
          GPIOboard.setOutput(i, AtemSwitcher.getProgramInputVideoSource(GPOATEM[i - 1]) == 14 ? HIGH : LOW);
          break;
        case 35:
          GPIOboard.setOutput(i, AtemSwitcher.getProgramInputVideoSource(GPOATEM[i - 1]) == 15 ? HIGH : LOW);
          break;
        case 36:
          GPIOboard.setOutput(i, AtemSwitcher.getProgramInputVideoSource(GPOATEM[i - 1]) == 16 ? HIGH : LOW);
          break;
        case 37:
          GPIOboard.setOutput(i, AtemSwitcher.getProgramInputVideoSource(GPOATEM[i - 1]) == 17 ? HIGH : LOW);
          break;
        case 38:
          GPIOboard.setOutput(i, AtemSwitcher.getProgramInputVideoSource(GPOATEM[i - 1]) == 18 ? HIGH : LOW);
          break;
        case 39:
          GPIOboard.setOutput(i, AtemSwitcher.getProgramInputVideoSource(GPOATEM[i - 1]) == 19 ? HIGH : LOW);
          break;
        case 40:
          GPIOboard.setOutput(i, AtemSwitcher.getProgramInputVideoSource(GPOATEM[i - 1]) == 20 ? HIGH : LOW);
          break;

        // Other inputs
        case 41:
          GPIOboard.setOutput(i, AtemSwitcher.getPreviewInputVideoSource(GPOATEM[i - 1]) == 0 ? HIGH : LOW);
          break;
        case 42:
          GPIOboard.setOutput(i, AtemSwitcher.getProgramInputVideoSource(GPOATEM[i - 1]) == 0 ? HIGH : LOW);
          break;
        case 43:
          GPIOboard.setOutput(i, AtemSwitcher.getPreviewInputVideoSource(GPOATEM[i - 1]) == 1000 ? HIGH : LOW);
          break;
        case 44:
          GPIOboard.setOutput(i, AtemSwitcher.getProgramInputVideoSource(GPOATEM[i - 1]) == 1000 ? HIGH : LOW);
          break;
        case 45:
          GPIOboard.setOutput(i, AtemSwitcher.getPreviewInputVideoSource(GPOATEM[i - 1]) == 2001 ? HIGH : LOW);
          break;
        case 46:
          GPIOboard.setOutput(i, AtemSwitcher.getProgramInputVideoSource(GPOATEM[i - 1]) == 2001 ? HIGH : LOW);
          break;
        case 47:
          GPIOboard.setOutput(i, AtemSwitcher.getPreviewInputVideoSource(GPOATEM[i - 1]) == 2002 ? HIGH : LOW);
          break;
        case 48:
          GPIOboard.setOutput(i, AtemSwitcher.getProgramInputVideoSource(GPOATEM[i - 1]) == 2002 ? HIGH : LOW);
          break;
        case 49:
          GPIOboard.setOutput(i, AtemSwitcher.getPreviewInputVideoSource(GPOATEM[i - 1]) == 3010 ? HIGH : LOW);
          break;
        case 50:
          GPIOboard.setOutput(i, AtemSwitcher.getProgramInputVideoSource(GPOATEM[i - 1]) == 3010 ? HIGH : LOW);
          break;
        case 51 :
          GPIOboard.setOutput(i, AtemSwitcher.getPreviewInputVideoSource(GPOATEM[i - 1]) == 3020 ? HIGH : LOW);
          break;
        case 52 :
          GPIOboard.setOutput(i, AtemSwitcher.getProgramInputVideoSource(GPOATEM[i - 1]) == 3020 ? HIGH : LOW);
          break;
        // USK 1-4
        case 53:
          GPIOboard.setOutput(i, AtemSwitcher.getKeyerOnAirEnabled(GPOATEM[i - 1], 0) ? HIGH : LOW);
          break;
        case 54:
          GPIOboard.setOutput(i, AtemSwitcher.getKeyerOnAirEnabled(GPOATEM[i - 1], 1) ? HIGH : LOW);
          break;
        case 55:
          GPIOboard.setOutput(i, AtemSwitcher.getKeyerOnAirEnabled(GPOATEM[i - 1], 2) ? HIGH : LOW);
          break;
        case 56:
          GPIOboard.setOutput(i, AtemSwitcher.getKeyerOnAirEnabled(GPOATEM[i - 1], 3) ? HIGH : LOW);
          break;

        // DSK 1-2
        case 57:
          GPIOboard.setOutput(i, AtemSwitcher.getDownstreamKeyerOnAir(0) ? HIGH : LOW);
          break;
        case 58:
          GPIOboard.setOutput(i, AtemSwitcher.getDownstreamKeyerOnAir(1) ? HIGH : LOW);
          break;
      }
    }
  }
}


void checkGPI()  {
  for (int i = 1; i <= 8; i++)  {
    if (GPIOboard.inputDown(i))  {
      switch (GPI1[i - 1]) {
        case 1:
          AtemSwitcher.setPreviewInputVideoSource(GPIATEM[i - 1], 1); // Change preview input to 1
          break;
        case 2:
          AtemSwitcher.setPreviewInputVideoSource(GPIATEM[i - 1], 2); // Change preview input to 2
          break;
        case 3:
          AtemSwitcher.setPreviewInputVideoSource(GPIATEM[i - 1], 3); // Change preview input to 3
          break;
        case 4:
          AtemSwitcher.setPreviewInputVideoSource(GPIATEM[i - 1], 4); // Change preview input to 4
          break;
        case 5:
          AtemSwitcher.setPreviewInputVideoSource(GPIATEM[i - 1], 5); // Change preview input to 5
          break;
        case 6:
          AtemSwitcher.setPreviewInputVideoSource(GPIATEM[i - 1], 6); // Change preview input to 6
          break;
        case 7:
          AtemSwitcher.setPreviewInputVideoSource(GPIATEM[i - 1], 7); // Change preview input to 7
          break;
        case 8:
          AtemSwitcher.setPreviewInputVideoSource(GPIATEM[i - 1], 8); // Change preview input to 8
          break;
        case 9:
          AtemSwitcher.setPreviewInputVideoSource(GPIATEM[i - 1], 9); // Change preview input to 9
          break;
        case 10:
          AtemSwitcher.setPreviewInputVideoSource(GPIATEM[i - 1], 10); // Change preview input to 10
          break;
        case 11:
          AtemSwitcher.setPreviewInputVideoSource(GPIATEM[i - 1], 11); // Change preview input to 11
          break;
        case 12:
          AtemSwitcher.setPreviewInputVideoSource(GPIATEM[i - 1], 12); // Change preview input to 12
          break;
        case 13:
          AtemSwitcher.setPreviewInputVideoSource(GPIATEM[i - 1], 13); // Change preview input to 13
          break;
        case 14:
          AtemSwitcher.setPreviewInputVideoSource(GPIATEM[i - 1], 14); // Change preview input to 14
          break;
        case 15:
          AtemSwitcher.setPreviewInputVideoSource(GPIATEM[i - 1], 15); // Change preview input to 15
          break;
        case 16:
          AtemSwitcher.setPreviewInputVideoSource(GPIATEM[i - 1], 16); // Change preview input to 16
          break;
        case 17:
          AtemSwitcher.setPreviewInputVideoSource(GPIATEM[i - 1], 17); // Change preview input to 17
          break;
        case 18:
          AtemSwitcher.setPreviewInputVideoSource(GPIATEM[i - 1], 18); // Change preview input to 18
          break;
        case 19:
          AtemSwitcher.setPreviewInputVideoSource(GPIATEM[i - 1], 19); // Change preview input to 19
          break;
        case 20:
          AtemSwitcher.setPreviewInputVideoSource(GPIATEM[i - 1], 20); // Change preview input to 20
          break;

        case 21:
          AtemSwitcher.setProgramInputVideoSource(GPIATEM[i - 1], 1); // Change program input to 1
          break;
        case 22:
          AtemSwitcher.setProgramInputVideoSource(GPIATEM[i - 1], 2); // Change program input to 2
          break;
        case 23:
          AtemSwitcher.setProgramInputVideoSource(GPIATEM[i - 1], 3); // Change program input to 3
          break;
        case 24:
          AtemSwitcher.setProgramInputVideoSource(GPIATEM[i - 1], 4); // Change program input to 4
          break;
        case 25:
          AtemSwitcher.setProgramInputVideoSource(GPIATEM[i - 1], 5); // Change program input to 5
          break;
        case 26:
          AtemSwitcher.setProgramInputVideoSource(GPIATEM[i - 1], 6); // Change program input to 6
          break;
        case 27:
          AtemSwitcher.setProgramInputVideoSource(GPIATEM[i - 1], 7); // Change program input to 7
          break;
        case 28:
          AtemSwitcher.setProgramInputVideoSource(GPIATEM[i - 1], 8); // Change program input to 8
          break;
        case 29:
          AtemSwitcher.setProgramInputVideoSource(GPIATEM[i - 1], 9); // Change program input to 9
          break;
        case 30:
          AtemSwitcher.setProgramInputVideoSource(GPIATEM[i - 1], 10); // Change program input to 10
          break;
        case 31:
          AtemSwitcher.setProgramInputVideoSource(GPIATEM[i - 1], 11); // Change program input to 11
          break;
        case 32:
          AtemSwitcher.setProgramInputVideoSource(GPIATEM[i - 1], 12); // Change program input to 12
          break;
        case 33:
          AtemSwitcher.setProgramInputVideoSource(GPIATEM[i - 1], 13); // Change program input to 13
          break;
        case 34:
          AtemSwitcher.setProgramInputVideoSource(GPIATEM[i - 1], 14); // Change program input to 14
          break;
        case 35:
          AtemSwitcher.setProgramInputVideoSource(GPIATEM[i - 1], 15); // Change program input to 15
          break;
        case 36:
          AtemSwitcher.setProgramInputVideoSource(GPIATEM[i - 1], 16); // Change program input to 16
          break;
        case 37:
          AtemSwitcher.setProgramInputVideoSource(GPIATEM[i - 1], 17); // Change program input to 17
          break;
        case 38:
          AtemSwitcher.setProgramInputVideoSource(GPIATEM[i - 1], 18); // Change program input to 18
          break;
        case 39:
          AtemSwitcher.setProgramInputVideoSource(GPIATEM[i - 1], 19); // Change program input to 19
          break;
        case 40:
          AtemSwitcher.setProgramInputVideoSource(GPIATEM[i - 1], 20); // Change program input to 20
          break;

        case 41:
          AtemSwitcher.setPreviewInputVideoSource(GPIATEM[i - 1], 0); // Change preview input to black
          break;
        case 42:
          AtemSwitcher.setProgramInputVideoSource(GPIATEM[i - 1], 0); // Change program input to black
          break;
        case 43:
          AtemSwitcher.setPreviewInputVideoSource(GPIATEM[i - 1], 1000); // Change preview input to bars
          break;
        case 44:
          AtemSwitcher.setProgramInputVideoSource(GPIATEM[i - 1], 1000); // Change program input to bars
          break;
        case 45:
          AtemSwitcher.setPreviewInputVideoSource(GPIATEM[i - 1], 2001); // Change preview input to color 1
          break;
        case 46:
          AtemSwitcher.setProgramInputVideoSource(GPIATEM[i - 1], 2001); // Change program input to color 1
          break;
        case 47:
          AtemSwitcher.setPreviewInputVideoSource(GPIATEM[i - 1], 2002); // Change preview input to color 2
          break;
        case 48:
          AtemSwitcher.setProgramInputVideoSource(GPIATEM[i - 1], 2002); // Change program input to color 2
          break;
        case 49:
          AtemSwitcher.setPreviewInputVideoSource(GPIATEM[i - 1], 3010); // Change preview input to media 1
          break;
        case 50:
          AtemSwitcher.setProgramInputVideoSource(GPIATEM[i - 1], 3010); // Change program input to media 1
          break;
        case 51:
          AtemSwitcher.setPreviewInputVideoSource(GPIATEM[i - 1], 3020); // Change preview input to media 2
          break;
        case 52:
          AtemSwitcher.setProgramInputVideoSource(GPIATEM[i - 1], 3020); // Change program input to media 2
          break;

        case 53:
          AtemSwitcher.performCutME(GPIATEM[i - 1]);
          break;
        case 54:
          AtemSwitcher.performAutoME(GPIATEM[i - 1]);
          break;
        case 55:
          AtemSwitcher.performFadeToBlackME(GPIATEM[i - 1]);
          break;
      }
      switch (GPI2[i - 1]) {
        case 1:
          AtemSwitcher.setKeyerOnAirEnabled(GPIATEM[i - 1], 0, AtemSwitcher.getKeyerOnAirEnabled(GPIATEM[i - 1], 0) ? false : true); // Toggle USK 1
          break;
        case 2:
          AtemSwitcher.setKeyerOnAirEnabled(GPIATEM[i - 1], 1, AtemSwitcher.getKeyerOnAirEnabled(GPIATEM[i - 1], 1) ? false : true); // Toggle USK 2
          break;
        case 3:
          AtemSwitcher.setKeyerOnAirEnabled(GPIATEM[i - 1], 2, AtemSwitcher.getKeyerOnAirEnabled(GPIATEM[i - 1], 2) ? false : true); // Toggle USK 3
          break;
        case 4:
          AtemSwitcher.setKeyerOnAirEnabled(GPIATEM[i - 1], 3, AtemSwitcher.getKeyerOnAirEnabled(GPIATEM[i - 1], 3) ? false : true); // Toggle USK 4
          break;

        case 5:
          AtemSwitcher.setKeyerOnAirEnabled(GPIATEM[i - 1], 0, 1); // Turn USK 1 on
          break;
        case 6:
          AtemSwitcher.setKeyerOnAirEnabled(GPIATEM[i - 1], 0, 0); // Turn USK 1 off
          break;
        case 7:
          AtemSwitcher.setKeyerOnAirEnabled(GPIATEM[i - 1], 1, 1); // Turn USK 2 on
          break;
        case 8:
          AtemSwitcher.setKeyerOnAirEnabled(GPIATEM[i - 1], 1, 0); // Turn USK 2 off
          break;
        case 9:
          AtemSwitcher.setKeyerOnAirEnabled(GPIATEM[i - 1], 2, 1); // Turn USK 3 on
          break;
        case 10:
          AtemSwitcher.setKeyerOnAirEnabled(GPIATEM[i - 1], 2, 0); // Turn USK 3 off
          break;
        case 11:
          AtemSwitcher.setKeyerOnAirEnabled(GPIATEM[i - 1], 3, 1); // Turn USK 4 on
          break;
        case 12:
          AtemSwitcher.setKeyerOnAirEnabled(GPIATEM[i - 1], 3, 0); // Turn USK 4 off
          break;

        case 13:
          AtemSwitcher.setDownstreamKeyerOnAir(1, AtemSwitcher.getDownstreamKeyerOnAir(0) ? false : true); // Toggle DSK 1
          break;
        case 14:
          AtemSwitcher.setDownstreamKeyerOnAir(2, AtemSwitcher.getDownstreamKeyerOnAir(1) ? false : true); // Toggle DSK 2
          break;
        case 15:
          AtemSwitcher.performDownstreamKeyerAutoKeyer(0); // DSK 1 Auto
          break;
        case 16:
          AtemSwitcher.performDownstreamKeyerAutoKeyer(1); // DSK 2 Auto
          break;

        case 17:
          AtemSwitcher.setDownstreamKeyerOnAir(0, 1); // Turn DSK 1 on
          break;
        case 18:
          AtemSwitcher.setDownstreamKeyerOnAir(0, 0); // Turn DSK 1 off
          break;
        case 19:
          AtemSwitcher.setDownstreamKeyerOnAir(1, 1); // Turn DSK 2 on
          break;
        case 20:
          AtemSwitcher.setDownstreamKeyerOnAir(0, 0); // Turn DSK 2 off
          break;

        case 21:
          if (GPI1[i - 1] != 53 && GPI1[i - 1] != 54 && GPI1[i - 1] != 55) {
            AtemSwitcher.performCutME(GPIATEM[i - 1]);
          }
          break;
        case 22:
          if (GPI1[i - 1] != 53 && GPI1[i - 1] != 54 && GPI1[i - 1] != 55) {
            AtemSwitcher.performAutoME(GPIATEM[i - 1]);
          }
          break;
        case 23:
          if (GPI1[i - 1] != 53 && GPI1[i - 1] != 54 && GPI1[i - 1] != 55) {
            AtemSwitcher.performFadeToBlackME(GPIATEM[i - 1]);
          }
          break;
        case 24:
          for (int j = 1; j <= 4; j++) {
            AtemSwitcher.setKeyerOnAirEnabled(GPIATEM[i - 1], j - 1, 0);
          }
          for (int j = 1; j <= 2; j++) {
            AtemSwitcher.setDownstreamKeyerOnAir(j - 1, 0);
          }
          break;
      }
      switch (GPI3[i - 1]) {
        case 1:
          if (GPI2[i - 1] != 1 && GPI2[i - 1] != 5 && GPI2[i - 1] != 6 && GPI2[i - 1] != 24) {
            AtemSwitcher.setKeyerOnAirEnabled(GPIATEM[i - 1], 0, AtemSwitcher.getKeyerOnAirEnabled(GPIATEM[i - 1], 0) ? false : true);
          } // Toggle USK 1
          break;
        case 2:
          if (GPI2[i - 1] != 2 && GPI2[i - 1] != 7 && GPI2[i - 1] != 8 && GPI2[i - 1] != 24) {
            AtemSwitcher.setKeyerOnAirEnabled(GPIATEM[i - 1], 1, AtemSwitcher.getKeyerOnAirEnabled(GPIATEM[i - 1], 1) ? false : true);
          } // Toggle USK 2
          break;
        case 3:
          if (GPI2[i - 1] != 3 && GPI2[i - 1] != 9 && GPI2[i - 1] != 10 && GPI2[i - 1] != 24) {
            AtemSwitcher.setKeyerOnAirEnabled(GPIATEM[i - 1], 2, AtemSwitcher.getKeyerOnAirEnabled(GPIATEM[i - 1], 2) ? false : true);
          } // Toggle USK 3
          break;
        case 4:
          if (GPI2[i - 1] != 4 && GPI2[i - 1] != 11 && GPI2[i - 1] != 12 && GPI2[i - 1] != 24) {
            AtemSwitcher.setKeyerOnAirEnabled(GPIATEM[i - 1], 3, AtemSwitcher.getKeyerOnAirEnabled(GPIATEM[i - 1], 3) ? false : true);
          } // Toggle USK 4
          break;

        case 5:
          if (GPI2[i - 1] != 5 && GPI2[i - 1] != 6 && GPI2[i - 1] != 1 && GPI2[i - 1] != 24) {
            AtemSwitcher.setKeyerOnAirEnabled(GPIATEM[i - 1], 0, 1);
          } // Turn USK 1 on
          break;
        case 6:
          if (GPI2[i - 1] != 5 && GPI2[i - 1] != 6 && GPI2[i - 1] != 1 && GPI2[i - 1] != 24) {
            AtemSwitcher.setKeyerOnAirEnabled(GPIATEM[i - 1], 0, 0);
          } // Turn USK 1 off
          break;
        case 7:
          if (GPI2[i - 1] != 7 && GPI2[i - 1] != 8 && GPI2[i - 1] != 2 && GPI2[i - 1] != 24) {
            AtemSwitcher.setKeyerOnAirEnabled(GPIATEM[i - 1], 1, 1);
          } // Turn USK 2 on
          break;
        case 8:
          if (GPI2[i - 1] != 7 && GPI2[i - 1] != 8 && GPI2[i - 1] != 2 && GPI2[i - 1] != 24) {
            AtemSwitcher.setKeyerOnAirEnabled(GPIATEM[i - 1], 1, 0);
          } // Turn USK 2 off
          break;
        case 9:
          if (GPI2[i - 1] != 9 && GPI2[i - 1] != 10 && GPI2[i - 1] != 3 && GPI2[i - 1] != 24) {
            AtemSwitcher.setKeyerOnAirEnabled(GPIATEM[i - 1], 2, 1);
          } // Turn USK 3 on
          break;
        case 10:
          if (GPI2[i - 1] != 9 && GPI2[i - 1] != 10 && GPI2[i - 1] != 3 && GPI2[i - 1] != 24) {
            AtemSwitcher.setKeyerOnAirEnabled(GPIATEM[i - 1], 2, 0);
          } // Turn USK 3 off
          break;
        case 11:
          if (GPI2[i - 1] != 11 && GPI2[i - 1] != 12 && GPI2[i - 1] != 4 && GPI2[i - 1] != 24) {
            AtemSwitcher.setKeyerOnAirEnabled(GPIATEM[i - 1], 3, 1);
          } // Turn USK 4 on
          break;
        case 12:
          if (GPI2[i - 1] != 11 && GPI2[i - 1] != 12 && GPI2[i - 1] != 4 && GPI2[i - 1] != 24) {
            AtemSwitcher.setKeyerOnAirEnabled(GPIATEM[i - 1], 3, 0);
          } // Turn USK 4 off
          break;

        case 13:
          if (GPI2[i - 1] != 13 && GPI2[i - 1] != 15 && GPI2[i - 1] != 16 && GPI2[i - 1] != 24) {
            AtemSwitcher.setDownstreamKeyerOnAir(0, AtemSwitcher.getDownstreamKeyerOnAir(0) ? false : true);
          } // Toggle DSK 1
          break;
        case 14:
          if (GPI2[i - 1] != 14 && GPI2[i - 1] != 17 && GPI2[i - 1] != 18 && GPI2[i - 1] != 24) {
            AtemSwitcher.setDownstreamKeyerOnAir(1, AtemSwitcher.getDownstreamKeyerOnAir(1) ? false : true);
          } // Toggle DSK 2
          break;
        case 15:
          AtemSwitcher.performDownstreamKeyerAutoKeyer(0); // DSK 1 Auto
          break;
        case 16:
          AtemSwitcher.performDownstreamKeyerAutoKeyer(1); // DSK 2 Auto
          break;

        case 17:
          if (GPI2[i - 1] != 15 && GPI2[i - 1] != 16 && GPI2[i - 1] != 13 && GPI2[i - 1] != 24) {
            AtemSwitcher.setDownstreamKeyerOnAir(0, 1);
          } // Turn DSK 1 on
          break;
        case 18:
          if (GPI2[i - 1] != 15 && GPI2[i - 1] != 16 && GPI2[i - 1] != 13 && GPI2[i - 1] != 24) {
            AtemSwitcher.setDownstreamKeyerOnAir(0, 0);
          } // Turn DSK 1 off
          break;
        case 19:
          if (GPI2[i - 1] != 17 && GPI2[i - 1] != 18 && GPI2[i - 1] != 14 && GPI2[i - 1] != 24) {
            AtemSwitcher.setDownstreamKeyerOnAir(1, 1);
          } // Turn DSK 2 on
          break;
        case 20:
          if (GPI2[i - 1] != 17 && GPI2[i - 1] != 18 && GPI2[i - 1] != 14 && GPI2[i - 1] != 24) {
            AtemSwitcher.setDownstreamKeyerOnAir(1, 0);
          } // Turn DSK 2 off
          break;

        case 21:
          if (GPI1[i - 1] != 53 && GPI1[i - 1] != 54 && GPI1[i - 1] != 55 && GPI2[i - 1] != 21 && GPI2[i - 1] != 22 && GPI2[i - 1] != 23) {
            AtemSwitcher.performCutME(GPIATEM[i - 1]);
          }
          break;
        case 22:
          if (GPI1[i - 1] != 53 && GPI1[i - 1] != 54 && GPI1[i - 1] != 55 && GPI2[i - 1] != 21 && GPI2[i - 1] != 22 && GPI2[i - 1] != 23) {
            AtemSwitcher.performAutoME(GPIATEM[i - 1]);
          }
          break;
        case 23:
          if (GPI1[i - 1] != 53 && GPI1[i - 1] != 54 && GPI1[i - 1] != 55 && GPI2[i - 1] != 21 && GPI2[i - 1] != 22 && GPI2[i - 1] != 23) {
            AtemSwitcher.performFadeToBlackME(GPIATEM[i - 1]);
          }
          break;
        case 24:
          for (int j = 1; j <= 4; j++) {
            AtemSwitcher.setKeyerOnAirEnabled(GPIATEM[i - 1], j - 1, 0);
          }
          for (int j = 1; j <= 2; j++) {
            AtemSwitcher.setDownstreamKeyerOnAir(j - 1, 0);
          }
          break;
      }
      // Aux 1
      switch (GPIA1[i - 1]) {
        case 1:
          AtemSwitcher.setAuxSourceInput(0, 1);
          break;
        case 2:
          AtemSwitcher.setAuxSourceInput(0, 2);
          break;
        case 3:
          AtemSwitcher.setAuxSourceInput(0, 3);
          break;
        case 4:
          AtemSwitcher.setAuxSourceInput(0, 4);
          break;
        case 5:
          AtemSwitcher.setAuxSourceInput(0, 5);
          break;
        case 6:
          AtemSwitcher.setAuxSourceInput(0, 6);
          break;
        case 7:
          AtemSwitcher.setAuxSourceInput(0, 7);
          break;
        case 8:
          AtemSwitcher.setAuxSourceInput(0, 8);
          break;
        case 9:
          AtemSwitcher.setAuxSourceInput(0, 9);
          break;
        case 10:
          AtemSwitcher.setAuxSourceInput(0, 10);
          break;
        case 11:
          AtemSwitcher.setAuxSourceInput(0, 11);
          break;
        case 12:
          AtemSwitcher.setAuxSourceInput(0, 12);
          break;
        case 13:
          AtemSwitcher.setAuxSourceInput(0, 13);
          break;
        case 14:
          AtemSwitcher.setAuxSourceInput(0, 14);
          break;
        case 15:
          AtemSwitcher.setAuxSourceInput(0, 15);
          break;
        case 16:
          AtemSwitcher.setAuxSourceInput(0, 16);
          break;
        case 17:
          AtemSwitcher.setAuxSourceInput(0, 17);
          break;
        case 18:
          AtemSwitcher.setAuxSourceInput(0, 18);
          break;
        case 19:
          AtemSwitcher.setAuxSourceInput(0, 19);
          break;
        case 20:
          AtemSwitcher.setAuxSourceInput(0, 20);
          break;
        case 21:
          AtemSwitcher.setAuxSourceInput(0, 0);
          break;
        case 22:
          AtemSwitcher.setAuxSourceInput(0, 1000);
          break;
        case 23:
          AtemSwitcher.setAuxSourceInput(0, 2001);
          break;
        case 24:
          AtemSwitcher.setAuxSourceInput(0, 2002);
          break;
        case 25:
          AtemSwitcher.setAuxSourceInput(0, 3010);
          break;
        case 26:
          AtemSwitcher.setAuxSourceInput(0, 3011);
          break;
        case 27:
          AtemSwitcher.setAuxSourceInput(0, 3020);
          break;
        case 28:
          AtemSwitcher.setAuxSourceInput(0, 3021);
          break;
        case 29:
          AtemSwitcher.setAuxSourceInput(0, 7001);
          break;
        case 30:
          AtemSwitcher.setAuxSourceInput(0, 7002);
          break;
        case 31:
          AtemSwitcher.setAuxSourceInput(0, 6000);
          break;
        case 32:
          AtemSwitcher.setAuxSourceInput(0, 10010);
          break;
        case 33:
          AtemSwitcher.setAuxSourceInput(0, 10011);
          break;
      }
      // Aux 2
      switch (GPIA2[i - 1]) {
        case 1:
          AtemSwitcher.setAuxSourceInput(1, 1);
          break;
        case 2:
          AtemSwitcher.setAuxSourceInput(1, 2);
          break;
        case 3:
          AtemSwitcher.setAuxSourceInput(1, 3);
          break;
        case 4:
          AtemSwitcher.setAuxSourceInput(1, 4);
          break;
        case 5:
          AtemSwitcher.setAuxSourceInput(1, 5);
          break;
        case 6:
          AtemSwitcher.setAuxSourceInput(1, 6);
          break;
        case 7:
          AtemSwitcher.setAuxSourceInput(1, 7);
          break;
        case 8:
          AtemSwitcher.setAuxSourceInput(1, 8);
          break;
        case 9:
          AtemSwitcher.setAuxSourceInput(1, 9);
          break;
        case 10:
          AtemSwitcher.setAuxSourceInput(1, 10);
          break;
        case 11:
          AtemSwitcher.setAuxSourceInput(1, 11);
          break;
        case 12:
          AtemSwitcher.setAuxSourceInput(1, 12);
          break;
        case 13:
          AtemSwitcher.setAuxSourceInput(1, 13);
          break;
        case 14:
          AtemSwitcher.setAuxSourceInput(1, 14);
          break;
        case 15:
          AtemSwitcher.setAuxSourceInput(1, 15);
          break;
        case 16:
          AtemSwitcher.setAuxSourceInput(1, 16);
          break;
        case 17:
          AtemSwitcher.setAuxSourceInput(1, 17);
          break;
        case 18:
          AtemSwitcher.setAuxSourceInput(1, 18);
          break;
        case 19:
          AtemSwitcher.setAuxSourceInput(1, 19);
          break;
        case 20:
          AtemSwitcher.setAuxSourceInput(1, 20);
          break;
        case 21:
          AtemSwitcher.setAuxSourceInput(1, 0);
          break;
        case 22:
          AtemSwitcher.setAuxSourceInput(1, 1000);
          break;
        case 23:
          AtemSwitcher.setAuxSourceInput(1, 2001);
          break;
        case 24:
          AtemSwitcher.setAuxSourceInput(1, 2002);
          break;
        case 25:
          AtemSwitcher.setAuxSourceInput(1, 3010);
          break;
        case 26:
          AtemSwitcher.setAuxSourceInput(1, 3011);
          break;
        case 27:
          AtemSwitcher.setAuxSourceInput(1, 3020);
          break;
        case 28:
          AtemSwitcher.setAuxSourceInput(1, 3021);
          break;
        case 29:
          AtemSwitcher.setAuxSourceInput(1, 7001);
          break;
        case 30:
          AtemSwitcher.setAuxSourceInput(1, 7002);
          break;
        case 31:
          AtemSwitcher.setAuxSourceInput(1, 6000);
          break;
        case 32:
          AtemSwitcher.setAuxSourceInput(1, 10010);
          break;
        case 33:
          AtemSwitcher.setAuxSourceInput(1, 10011);
          break;
      }
      // Aux 3
      switch (GPIA3[i - 1]) {
        case 1:
          AtemSwitcher.setAuxSourceInput(2, 1);
          break;
        case 2:
          AtemSwitcher.setAuxSourceInput(2, 2);
          break;
        case 3:
          AtemSwitcher.setAuxSourceInput(2, 3);
          break;
        case 4:
          AtemSwitcher.setAuxSourceInput(2, 4);
          break;
        case 5:
          AtemSwitcher.setAuxSourceInput(2, 5);
          break;
        case 6:
          AtemSwitcher.setAuxSourceInput(2, 6);
          break;
        case 7:
          AtemSwitcher.setAuxSourceInput(2, 7);
          break;
        case 8:
          AtemSwitcher.setAuxSourceInput(2, 8);
          break;
        case 9:
          AtemSwitcher.setAuxSourceInput(2, 9);
          break;
        case 10:
          AtemSwitcher.setAuxSourceInput(2, 10);
          break;
        case 11:
          AtemSwitcher.setAuxSourceInput(2, 11);
          break;
        case 12:
          AtemSwitcher.setAuxSourceInput(2, 12);
          break;
        case 13:
          AtemSwitcher.setAuxSourceInput(2, 13);
          break;
        case 14:
          AtemSwitcher.setAuxSourceInput(2, 14);
          break;
        case 15:
          AtemSwitcher.setAuxSourceInput(2, 15);
          break;
        case 16:
          AtemSwitcher.setAuxSourceInput(2, 16);
          break;
        case 17:
          AtemSwitcher.setAuxSourceInput(2, 17);
          break;
        case 18:
          AtemSwitcher.setAuxSourceInput(2, 18);
          break;
        case 19:
          AtemSwitcher.setAuxSourceInput(2, 19);
          break;
        case 20:
          AtemSwitcher.setAuxSourceInput(2, 20);
          break;
        case 21:
          AtemSwitcher.setAuxSourceInput(2, 0);
          break;
        case 22:
          AtemSwitcher.setAuxSourceInput(2, 1000);
          break;
        case 23:
          AtemSwitcher.setAuxSourceInput(2, 2001);
          break;
        case 24:
          AtemSwitcher.setAuxSourceInput(2, 2002);
          break;
        case 25:
          AtemSwitcher.setAuxSourceInput(2, 3010);
          break;
        case 26:
          AtemSwitcher.setAuxSourceInput(2, 3011);
          break;
        case 27:
          AtemSwitcher.setAuxSourceInput(2, 3020);
          break;
        case 28:
          AtemSwitcher.setAuxSourceInput(2, 3021);
          break;
        case 29:
          AtemSwitcher.setAuxSourceInput(2, 7001);
          break;
        case 30:
          AtemSwitcher.setAuxSourceInput(2, 7002);
          break;
        case 31:
          AtemSwitcher.setAuxSourceInput(2, 6000);
          break;
        case 32:
          AtemSwitcher.setAuxSourceInput(2, 10010);
          break;
        case 33:
          AtemSwitcher.setAuxSourceInput(2, 10011);
          break;
      }
      // Aux 4
      switch (GPIA4[i - 1]) {
        case 1:
          AtemSwitcher.setAuxSourceInput(3, 1);
          break;
        case 2:
          AtemSwitcher.setAuxSourceInput(3, 2);
          break;
        case 3:
          AtemSwitcher.setAuxSourceInput(3, 3);
          break;
        case 4:
          AtemSwitcher.setAuxSourceInput(3, 4);
          break;
        case 5:
          AtemSwitcher.setAuxSourceInput(3, 5);
          break;
        case 6:
          AtemSwitcher.setAuxSourceInput(3, 6);
          break;
        case 7:
          AtemSwitcher.setAuxSourceInput(3, 7);
          break;
        case 8:
          AtemSwitcher.setAuxSourceInput(3, 8);
          break;
        case 9:
          AtemSwitcher.setAuxSourceInput(3, 9);
          break;
        case 10:
          AtemSwitcher.setAuxSourceInput(3, 10);
          break;
        case 11:
          AtemSwitcher.setAuxSourceInput(3, 11);
          break;
        case 12:
          AtemSwitcher.setAuxSourceInput(3, 12);
          break;
        case 13:
          AtemSwitcher.setAuxSourceInput(3, 13);
          break;
        case 14:
          AtemSwitcher.setAuxSourceInput(3, 14);
          break;
        case 15:
          AtemSwitcher.setAuxSourceInput(3, 15);
          break;
        case 16:
          AtemSwitcher.setAuxSourceInput(3, 16);
          break;
        case 17:
          AtemSwitcher.setAuxSourceInput(3, 17);
          break;
        case 18:
          AtemSwitcher.setAuxSourceInput(3, 18);
          break;
        case 19:
          AtemSwitcher.setAuxSourceInput(3, 19);
          break;
        case 20:
          AtemSwitcher.setAuxSourceInput(3, 20);
          break;
        case 21:
          AtemSwitcher.setAuxSourceInput(3, 0);
          break;
        case 22:
          AtemSwitcher.setAuxSourceInput(3, 1000);
          break;
        case 23:
          AtemSwitcher.setAuxSourceInput(3, 2001);
          break;
        case 24:
          AtemSwitcher.setAuxSourceInput(3, 2002);
          break;
        case 25:
          AtemSwitcher.setAuxSourceInput(3, 3010);
          break;
        case 26:
          AtemSwitcher.setAuxSourceInput(3, 3011);
          break;
        case 27:
          AtemSwitcher.setAuxSourceInput(3, 3020);
          break;
        case 28:
          AtemSwitcher.setAuxSourceInput(3, 3021);
          break;
        case 29:
          AtemSwitcher.setAuxSourceInput(3, 7001);
          break;
        case 30:
          AtemSwitcher.setAuxSourceInput(3, 7002);
          break;
        case 31:
          AtemSwitcher.setAuxSourceInput(3, 6000);
          break;
        case 32:
          AtemSwitcher.setAuxSourceInput(3, 10010);
          break;
        case 33:
          AtemSwitcher.setAuxSourceInput(3, 10011);
          break;
      }
      // Aux 5
      switch (GPIA5[i - 1]) {
        case 1:
          AtemSwitcher.setAuxSourceInput(4, 1);
          break;
        case 2:
          AtemSwitcher.setAuxSourceInput(4, 2);
          break;
        case 3:
          AtemSwitcher.setAuxSourceInput(4, 3);
          break;
        case 4:
          AtemSwitcher.setAuxSourceInput(4, 4);
          break;
        case 5:
          AtemSwitcher.setAuxSourceInput(4, 5);
          break;
        case 6:
          AtemSwitcher.setAuxSourceInput(4, 6);
          break;
        case 7:
          AtemSwitcher.setAuxSourceInput(4, 7);
          break;
        case 8:
          AtemSwitcher.setAuxSourceInput(4, 8);
          break;
        case 9:
          AtemSwitcher.setAuxSourceInput(4, 9);
          break;
        case 10:
          AtemSwitcher.setAuxSourceInput(4, 10);
          break;
        case 11:
          AtemSwitcher.setAuxSourceInput(4, 11);
          break;
        case 12:
          AtemSwitcher.setAuxSourceInput(4, 12);
          break;
        case 13:
          AtemSwitcher.setAuxSourceInput(4, 13);
          break;
        case 14:
          AtemSwitcher.setAuxSourceInput(4, 14);
          break;
        case 15:
          AtemSwitcher.setAuxSourceInput(4, 15);
          break;
        case 16:
          AtemSwitcher.setAuxSourceInput(4, 16);
          break;
        case 17:
          AtemSwitcher.setAuxSourceInput(4, 17);
          break;
        case 18:
          AtemSwitcher.setAuxSourceInput(4, 18);
          break;
        case 19:
          AtemSwitcher.setAuxSourceInput(4, 19);
          break;
        case 20:
          AtemSwitcher.setAuxSourceInput(4, 20);
          break;
        case 21:
          AtemSwitcher.setAuxSourceInput(4, 0);
          break;
        case 22:
          AtemSwitcher.setAuxSourceInput(4, 1000);
          break;
        case 23:
          AtemSwitcher.setAuxSourceInput(4, 2001);
          break;
        case 24:
          AtemSwitcher.setAuxSourceInput(4, 2002);
          break;
        case 25:
          AtemSwitcher.setAuxSourceInput(4, 3010);
          break;
        case 26:
          AtemSwitcher.setAuxSourceInput(4, 3011);
          break;
        case 27:
          AtemSwitcher.setAuxSourceInput(4, 3020);
          break;
        case 28:
          AtemSwitcher.setAuxSourceInput(4, 3021);
          break;
        case 29:
          AtemSwitcher.setAuxSourceInput(4, 7001);
          break;
        case 30:
          AtemSwitcher.setAuxSourceInput(4, 7002);
          break;
        case 31:
          AtemSwitcher.setAuxSourceInput(4, 6000);
          break;
        case 32:
          AtemSwitcher.setAuxSourceInput(4, 10010);
          break;
        case 33:
          AtemSwitcher.setAuxSourceInput(4, 10011);
          break;
      }
      // Aux 6
      switch (GPIA6[i - 1]) {
        case 1:
          AtemSwitcher.setAuxSourceInput(5, 1);
          break;
        case 2:
          AtemSwitcher.setAuxSourceInput(5, 2);
          break;
        case 3:
          AtemSwitcher.setAuxSourceInput(5, 3);
          break;
        case 4:
          AtemSwitcher.setAuxSourceInput(5, 4);
          break;
        case 5:
          AtemSwitcher.setAuxSourceInput(5, 5);
          break;
        case 6:
          AtemSwitcher.setAuxSourceInput(5, 6);
          break;
        case 7:
          AtemSwitcher.setAuxSourceInput(5, 7);
          break;
        case 8:
          AtemSwitcher.setAuxSourceInput(5, 8);
          break;
        case 9:
          AtemSwitcher.setAuxSourceInput(5, 9);
          break;
        case 10:
          AtemSwitcher.setAuxSourceInput(5, 10);
          break;
        case 11:
          AtemSwitcher.setAuxSourceInput(5, 11);
          break;
        case 12:
          AtemSwitcher.setAuxSourceInput(5, 12);
          break;
        case 13:
          AtemSwitcher.setAuxSourceInput(5, 13);
          break;
        case 14:
          AtemSwitcher.setAuxSourceInput(5, 14);
          break;
        case 15:
          AtemSwitcher.setAuxSourceInput(5, 15);
          break;
        case 16:
          AtemSwitcher.setAuxSourceInput(5, 16);
          break;
        case 17:
          AtemSwitcher.setAuxSourceInput(5, 17);
          break;
        case 18:
          AtemSwitcher.setAuxSourceInput(5, 18);
          break;
        case 19:
          AtemSwitcher.setAuxSourceInput(5, 19);
          break;
        case 20:
          AtemSwitcher.setAuxSourceInput(5, 20);
          break;
        case 21:
          AtemSwitcher.setAuxSourceInput(5, 0);
          break;
        case 22:
          AtemSwitcher.setAuxSourceInput(5, 1000);
          break;
        case 23:
          AtemSwitcher.setAuxSourceInput(5, 2001);
          break;
        case 24:
          AtemSwitcher.setAuxSourceInput(5, 2002);
          break;
        case 25:
          AtemSwitcher.setAuxSourceInput(5, 3010);
          break;
        case 26:
          AtemSwitcher.setAuxSourceInput(5, 3011);
          break;
        case 27:
          AtemSwitcher.setAuxSourceInput(5, 3020);
          break;
        case 28:
          AtemSwitcher.setAuxSourceInput(5, 3021);
          break;
        case 29:
          AtemSwitcher.setAuxSourceInput(5, 7001);
          break;
        case 30:
          AtemSwitcher.setAuxSourceInput(5, 7002);
          break;
        case 31:
          AtemSwitcher.setAuxSourceInput(5, 6000);
          break;
        case 32:
          AtemSwitcher.setAuxSourceInput(5, 10010);
          break;
        case 33:
          AtemSwitcher.setAuxSourceInput(5, 10011);
          break;
      }

      lDelay(100);  // to catch any rebounce from the switch
      break;  // Only send one command at a time!
    }
  }
}

void runTest() {
  buttons.testProgramme(65535);
  lDelay(20);
}


/**
   Local delay function
*/
void lDelay(unsigned long timeout)  {
  unsigned long thisTime = millis();
  do {
    if (isConfigMode)  {
      webserver.processConnection();
    } else {
      AtemSwitcher.runLoop();
    }
    Serial << F(".");
    static int k = 1;
    k++;
    if (k > 100) {
      k = 1;
      Serial << F("\n");
    }
  }
  while (!sTools.hasTimedOut(thisTime, timeout));
}
