
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

//message types for nodeID identification
#define DATA_FROM_CENTER 'a'
#define DATA_FROM_LEFT_CENTER 'b'
#define DATA_FROM_LEFT_TOP 'c'
#define DATA_FROM_RIGHT_CENTER 'd'
#define DATA_FROM_RIGHT_TOP 'e'

//types of data to be sent
#define IMU 'I'

#define nodeID CENTER
#define to_master 00

uint32_t displayTimer = 0;

//used for the outgoing data from this node
struct payload_t {
  unsigned long ms;
  unsigned long data;
  char data_type;
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

  if (!mesh.checkConnection()){
    mesh.renewAddress();
    Serial.println("Renewing Address"); //for debugging
  }

  //receive commands from the master
  if (network.available()){
    RF24NetworkHeader header;
    network.peek(header);
    
    uint32_t data;
    //test whether the nodes can receive a message that is not supposed to go to them
    switch(header.type){
      case 'A': network.read(header, &data, sizeof(data));
                Serial.println("M_type --- from --- data"); //for debugging
                Serial.print(header.type); Serial.print(" --- "); 
                Serial.print(header.from_node); Serial.print(" --- ");
                Serial.println(data); Serial.println("\n");
                //do the protocal for message A type (walk for example)
      default:
      break;
    }
  }


  //send data back to the master using fragmentation
  payload_t payload;
  payload.data = 0 ;/*data from the sensor*/
  payload.ms = millis(); //send the time of data packet
  payload.data_type = IMU; 

  /* For enabling sending paylaod up to 120 bytes we need to make changes in RF24Network_config.h
   * so we can work with fragmentation (not done yet!!!)
   */
  RF24NetworkHeader header(to_master, DATA_FROM_CENTER);
  while(!network.write(header, &payload, sizeof(payload))){
    if (!mesh.checkConnection()){
      mesh.renewAddress();
    }
  }
}

