
/** RF24Mesh_Example.ino by TMRh20
 *
 * This example sketch shows how to manually configure a node via RF24Mesh, and send data to the
 * master node.
 * The nodes will refresh their network address as soon as a single write fails. This allows the
 * nodes to change position in relation to each other and the master node.
 */


#include "RF24.h"
#include "RF24Network.h"
#include "RF24Mesh.h"
#include <SPI.h>
#include <EEPROM.h>
//#include <printf.h>


/**** Configure the nrf24l01 CE and CS pins ****/
RF24 radio(9, 10);
RF24Network network(radio);
RF24Mesh mesh(radio, network);

/**
 * User Configuration: nodeID - A unique identifier for each radio. Allows addressing
 * to change dynamically with physical changes to the mesh.
 *
 * In this example, configuration takes place below, prior to uploading the sketch to the device
 * A unique value from 1-255 must be configured for each node.
 * This will be stored in EEPROM on AVR devices, so remains persistent between further uploads, loss of power, etc.
 *
 **/
 
//nodes definition
#define CENTER 1
#define LEFT_CENTER 2
#define LEFT_TOP 3
#define RIGHT_CENTER 4
#define RIGHT_TOP 5 

#define nodeID CENTER


uint32_t displayTimer = 0;

struct payload_t {
  unsigned long ms;
  unsigned long data;
};

void setup() {

  Serial.begin(115200);
  //printf_begin();
  // Set the nodeID manually
  mesh.setNodeID(nodeID);
  // Connect to the mesh
  Serial.println(F("Connecting to the mesh..."));
  mesh.begin();
}



void loop() {

  mesh.update();

  // Send to the master node every second
  if (millis()-displayTimer >= 1000) {
    displayTimer = millis();
    uint32_t phrase = 0b00000000000000000000000011111111;
    // Send an 'M' type message containing the current millis()
    if (!mesh.write(&phrase, 'M', sizeof(phrase))) {

      // If a write fails, check connectivity to the mesh network
      if ( ! mesh.checkConnection() ) {
        //refresh the network address
        Serial.println("Renewing Address");
        mesh.renewAddress();
      } else {
        Serial.println("Send fail, Test OK");
      }
    } else {
      Serial.print("Send OK: "); Serial.println(displayTimer);
    }
  }

//  while (network.available()) {
//    RF24NetworkHeader header;
//    payload_t payload;
//    network.read(header, &payload, sizeof(payload));
//    Serial.print("Received packet #");
//    Serial.print(payload.counter);
//    Serial.print(" at ");
//    Serial.println(payload.ms);
//  }

  //receive commands from the master
  if (network.available()){
    RF24NetworkHeader header;
    network.peek(header);

    uint32_t data;
    //test whether the nodes can receive a message that is not supposed to go to them
    switch(header.type){
      case 'A': //network.read(header, &data, sizeof(data)); do we need to do this?
                //do the protocal for message A type (walk for example)

      default:
      break;
    }
  }


  //send data back to the master using fragmentation
  payload_t payload;
  payload.data =  ;/*data from the sensor*/
  payload.ms = millis(); //send the time of data packet

  /*
   * For enabling sending paylaod up to 120 bytes we need to make changes in RF24Network_config.h
   * so we can work with fragmentation (not done yet!!!)
   */

  /* 
   * if the header type under network layer does not support _from_node class, use the message
   * type instead to differentiate the data from all the nodes
   */
  
  RF24NetworkHeader header(00, 'S'); //header to send to master, message type may vary
  while(!network.write(header, &payload, sizeof(payload))){
    //goes out when it send succesfully or when it run out of time???
  }
}






