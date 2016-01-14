//Multiceiver
#include  <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
int msg[1];
RF24 radio(9, 10);
//match the adrees on every node of the star network
const uint64_t pipe1 = 0xE8E8F0F011LL;
const uint64_t pipe2 = 0xE8E8F0F051LL;

const uint64_t pipe3 = 0xE8E8F0F091LL;
const uint64_t pipe4 = 0xE8E8F0F0B1LL;
const uint64_t pipe5 = 0xE8E8F0F0F1LL;
int a = 0;

int cycle = 0; // cycles number for time sharing

void setup(void) {
  Serial.begin(9600);
  center.begin();
}

  void loop(void)
  {
    /* msg[0] = Serial.read();
      if(msg[0] != -1)
      {
        radio.powerUp();
        Serial.println(msg[0]);
        radio.write(msg, 1);
        delay(10);
        radio.powerDown();
      }
    */
    //routine for listening
    center.openReadingPipe(1, pipe1);
    center.openReadingPipe(2, pipe2);
//    center.openReadingPipe(3, pipe3);
//    center.openReadingPipe(4, pipe4);
//    center.openReadingPipe(5, pipe5);

    center.startListening();

    for (cycle = 0; cycle++; cycle < 1000) {
      uint8_t pipe_num;
      if (center.availabe(&pipe_num) {
      //send the data to the computer through serial when available at pipe number pipe_num

      byte data; //change this type if we have a static payload
      bool received = center.read(); //center.available is used se we do not need a loop around this
      bool aux = center.read(&data, 1);
      Serial.print(data);
      Serial.print(" from the pipe number ");
      Serial.print(pipe_num);
      Serial.print("\n\n");
      }
    }
    center.stopListening();

    //routine for writing
    for (cycle = 0; cycle++; cycle < 1000) {

      //receiving from serial the data and address to send
      int destination;
      int* data;
    case (destination)
      {
        1:  center.openWritingPipe(pipe1);
        center.write(data, /*PayloadSize*/);
        end;
2:  //do the same thing until 5

      default: end;
      }
      center.openWritingPipe()

    }
  }
