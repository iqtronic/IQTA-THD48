



/***************************************************************************
* File Name: IQTA_THD48.ino
* Processor/Platform: Arduino Uno R3 (tested)
* Development Environment: Arduino 1.6.5
* This code is for use with combined quad 1-wire sensor IQTA_THD48 made by IQtronic technologies Europe s.r.o. 
* it can be used free of charge
* Standard OneWire Adruino Library  is used and needed for correct function.
* Author: IQtronic technologies Europe s.r.o. Libor Konecny  (c) 2020 V1.0
* Circuit:
*    Arduino Uno        THD48 wire color
*    DATA    DIGITAL8   YELLOW
*    GND:    GND        BLACK
*    5V:     5V         RED
*    Important note: 4k7 resistor must be connect between DATA and 5V pins if SWITCH8 is OFF, otherwise not used.
*    
* Example of  UART Output: 
* SENSORS DATA:
* TEMPERATURE  is: 22.94 Deg C
* RELATIVE HUMIDITY  is :43.94 %
* ABSOLUTE HUMIDITY  is :9.00 g/m3
* DEW POINT is :10.00 deg C    
* 
* Decription:
* IQtronic IQTA_THD48 is high precicion sensor combined 4 sensors in one.
* Easy to use with similar 1-Wire drivers due to similar protocol as DS18BXXXX
* It has address manual selector for using 8 combined sensor for using at one 1-Wire bus (total 4x8 values)
* 1-Wire Temperature sensor, Relative humuidity sensor, Absolute humidity sensor and Dew point sensor. 
* Accuracy: Temperature: up to ±0.1 °C , Humidity: up to ±1.5 %RH
* Resulution: 0.125
***************************************************************************/

#define OFF 0
#define ON  1

//THD48 SWITCH STATUS, you can find this switch inside THD48 sensor,and it can be set to ON or OFF 
#define  SWITCH1    OFF   //address bit A2 SWITCH is set to OFF
#define  SWITCH2    OFF   //address bit A1 SWITCH is set to OFF
#define  SWITCH3    OFF   //address bit A0 SWITCH is set to OFF


/*
ALL SWICTHES 1 - 4 must be ON to get all values from sensr THD48 otherwise it cannot be read from sensor THD48
SWICTH 4 - TEMPERATURE IN CELSIUS
SWICTH 5 - RELATIVE HUMIDITY %
SWITCH 6 - ABSOLUTE HUMIDITY g/m3
SWITCH 7 - DEW POINT IN CELSIUS
SWITCH 8 -  4k7 bus resistor OFF/ON
*/
//Ident of each sensor of THD48
#define TEMPERATURE_ID  1
#define HUMIDITYREL_ID  2
#define HUMIDITYABS_ID  3
#define DEWPOINT_ID     4

#include <OneWire.h>
OneWire THDWire(8) ; //digital pin 8

//1-wire ADDRESS for first sensor
uint8_t MAIN_ROM_NO[8]={0x22,0x76,0x19,0x17,0x62,0x49,0x05,0x01};

void setup() { 
  Serial.begin(115200);
  Serial.println("IQTA_THD48 sensor by IQtronic technologies Europe s.r.o. demostration");
}


//GET real ROM ADDRESS according ADDRESS SWITCH SETTINGS and sensor ident (1 - 4)
uint8_t get_sensor_addr(uint8_t ident)
{
uint8_t adr;
adr=0;
if (ident>4) return 0x00; 
if (!ident) return 0x00;
if (SWITCH3==ON) adr=adr+4;
if (SWITCH2==ON) adr=adr+8;
if (SWITCH1==ON) adr=adr+16;
ident--;
adr+=ident;
return adr;
}

uint8_t  THD48_readval(float *val, uint8_t sensor_id)
{
     uint8_t i,DATA[8],CRC;
     int16_t value;
     float result;

     //rewrite address acconrding sensor_id and ADDRESS SWITCH SETTINGS
     MAIN_ROM_NO[7]=get_sensor_addr(sensor_id);
                   
     if (!THDWire.reset()) return 1;      
     THDWire.write(0x55, 1); 
     for (i = 0; i < 8; i++) THDWire.write(MAIN_ROM_NO[i]);
     
     THDWire.write(0x44, 1); 
     delay(150); //wait for conversion
     if (!THDWire.reset()) return 1;     
     THDWire.write(0x55, 1);
     for (i = 0; i < 8; i++) THDWire.write(MAIN_ROM_NO[i]);
     
     THDWire.write(0xBE,1);
     for(i=0;i<8;i++)   DATA[i]=THDWire.read();              
     CRC=THDWire.read();

     if( CRC == THDWire.crc8(DATA,sizeof(DATA))  ) {
            
            value=(DATA[1]<<8);
            value|=DATA[0];                 
            result=value*10;
            result=result/160;
            *val=result;                                    
            return 0;                      
     }
    *val=0xFFFF;    
    return 1;                      
}

#define TEMPERATURE_ID  1
#define HUMIDITYREL_ID  2
#define HUMIDITYABS_ID  3
#define DEWPOINT_ID     4


void loop() {
     float val;
     Serial.print("\r\n\r\nSENSORS DATA:\r\n");
     
     if (!THD48_readval(&val, TEMPERATURE_ID))  {
        Serial.print("TEMPERATURE  is: ");
        Serial.print(val, 2);        
        Serial.print(" Deg C\r\n");
     }
     if (!THD48_readval(&val, HUMIDITYREL_ID))  {
        Serial.print("RELATIVE HUMIDITY  is :");
        Serial.print(val, 2);        
        Serial.print(" %\r\n");
     }
     if (!THD48_readval(&val, HUMIDITYABS_ID))  {
        Serial.print("ABSOLUTE HUMIDITY  is :");
        Serial.print(val, 2);        
        Serial.print(" g/m3\r\n");
     }
     if (!THD48_readval(&val, DEWPOINT_ID))  {
        Serial.print("DEW POINT is :");
        Serial.print(val, 2);        
        Serial.print(" deg C\r\n");
     }


     
     delay(2000);
     
}
