/*
 Name:		CubeCell Capsule Sender
 Created:	31-Mar-22 12:32:35
 Author:	Zhelyazkov (unrealbg)
*/

#include <LoRaWan_APP.h>
#include <Arduino.h>
#include <Wire.h>
#include <BH1750.h>

BH1750 lightMeter;

/*
 * set LoraWan_RGB to 1,the RGB active in loraWan
 * RGB red means sending;
 * RGB green means received done;
 */
#ifndef LoraWan_RGB
#define LoraWan_RGB 0
#endif

#define RF_FREQUENCY                                868000000 // Hz

#define TX_OUTPUT_POWER                             14        // dBm

#define LORA_BANDWIDTH                              0         // [0: 125 kHz,
 //  1: 250 kHz,
 //  2: 500 kHz,
 //  3: Reserved]
#define LORA_SPREADING_FACTOR                       7         // [SF7..SF12]
#define LORA_CODINGRATE                             1         // [1: 4/5,
                                                              //  2: 4/6,
                                                              //  3: 4/7,
                                                              //  4: 4/8]
#define LORA_PREAMBLE_LENGTH                        8         // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT                         0         // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define LORA_IQ_INVERSION_ON                        false


#define RX_TIMEOUT_VALUE                            1000
#define BUFFER_SIZE                                 30 // Define the payload size here

char txpacket[BUFFER_SIZE];
char rxpacket[BUFFER_SIZE];

static RadioEvents_t RadioEvents;

double txNumber;

int16_t rssi, rxSize;
void  DoubleToString(char* str, double double_num, unsigned int len);

void setup() {
    Serial.begin(115200);

    txNumber = 0;
    rssi = 0;

    Radio.Init(&RadioEvents);
    Radio.SetChannel(RF_FREQUENCY);
    Radio.SetTxConfig(MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
        LORA_SPREADING_FACTOR, LORA_CODINGRATE,
        LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
        true, 0, 0, LORA_IQ_INVERSION_ON, 3000);

    pinMode(Vext, OUTPUT);
    digitalWrite(Vext, LOW);

    lightMeter.begin();

    Serial.println(F("BH1750 Test begin"));
}



void loop()
{
    delay(1000);
    txNumber += 0.01;
    float lux = lightMeter.readLightLevel();
    sprintf(txpacket, "%s", "Lux: ");  //start a package
  //  sprintf(txpacket+strlen(txpacket),"%d",lux); //add to the end of package

    DoubleToString(txpacket, lux, 1);     //add to the end of package

    turnOnRGB(COLOR_SEND, 0); //change rgb color

    Serial.printf("\r\nsending packet \"%s\"", txpacket, strlen(txpacket));

    Radio.Send((uint8_t*)txpacket, strlen(txpacket)); //send the package out 
}

/**
  * @brief  Double To String
  * @param  str: Array or pointer for storing strings
  * @param  double_num: Number to be converted
  * @param  len: Fractional length to keep
  * @retval None
  */
void  DoubleToString(char* str, double double_num, unsigned int len) {
    double fractpart, intpart;
    fractpart = modf(double_num, &intpart);
    fractpart = fractpart * (pow(10, len));
    sprintf(str + strlen(str), "%d", (int)(intpart)); //Integer part
    sprintf(str + strlen(str), ".%d", (int)(fractpart)); //Decimal part
}