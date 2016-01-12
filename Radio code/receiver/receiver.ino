//receiver
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
RF24 radio(9, 10);
const uint64_t pipe = 0xE8E8F0F0E1LL;

void setup(void)
{
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(1, pipe);
  radio.startListening();
}

void loop(void)
{
  if (radio.available())
  {
    boolean done = false;
    while (!done)
    {
      byte msg;
      done = radio.read(&msg, 1); 
      Serial.println(msg);
    }
  }
  else
  {
    //Serial.println("No Radio Available");
  }
}
