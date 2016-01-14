//transmitter
#include  <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
int msg[1];
RF24 radio(9,10);
const uint64_t pipe1 = 0xE8E8F0F011LL;
int a = 0;

void setup(void){
 Serial.begin(9600);
 radio.begin();
 radio.openWritingPipe(pipe1);}

void loop(void){ 
 msg[0] = Serial.read();
 if(msg[0] != -1)
 {
    radio.powerUp();
    Serial.println(msg[0]);
    radio.write(msg, 1);
    delay(10);
    radio.powerDown();
  }
}
