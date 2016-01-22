
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

#define nodeID 01
#define TO_MASTER 00

//used for the outgoing data from this node
struct payload_t {
  unsigned long ms;
  unsigned long data;
  char data_type;
};

struct command_t {
  bool motorLeft_on;
  bool motorRight_on;
  int velocityLeft_motor;
  int velocityRight_motor;
};

void setup() {
  Serial.begin(115200);
  mesh.setNodeID(nodeID);
  Serial.println(F("Connecting to the mesh..."));
  mesh.begin();
}

void loop() {
  mesh.update();

// Debugging of the network

//  Serial.print("This node address: ");
//  Serial.println(mesh.getAddress(nodeID));
//  Serial.print("This node ID: ");
//  Serial.println(nodeID);
//  Serial.print("This node connection: ");
//  Serial.println(mesh.checkConnection());
//  Serial.print("This network: ");
//  Serial.println(network.available());
  
  if (!mesh.checkConnection()){
    mesh.renewAddress();
    Serial.println("Renewing Address"); //for debugging
  }
  //receive commands from the master
  delay(10); //necessary so we can get the message to this node
  while(network.available()){
    RF24NetworkHeader header;
    network.peek(header);
    command_t data;
    switch(header.type){
      case 65: network.read(header, &data, sizeof(data));
               Serial.println("Message_type --- from --- data"); //for debugging
               Serial.print(char(header.type)); Serial.print("           --- "); 
               Serial.print(header.from_node); Serial.print("  --- ");
               Serial.print("r-motor: ");
               Serial.print(data.motorRight_on);
               Serial.print("   l-motor: ");
               Serial.print(data.motorLeft_on);
               Serial.print("  velocity-r: ");
               Serial.print(data.velocityRight_motor);
               Serial.print("  velocity-l: ");
               Serial.print(data.velocityLeft_motor);
               Serial.println("\n");
                
      default:
      break;
    }
  }
  
  //send data back to the master using fragmentation
//  payload_t payload;
//  payload.data = 0b10101010101010101010101010101010 ;/*data from the sensor*/
//  payload.ms = millis(); //send the time of data packet
//  payload.data_type = IMU; 
//  RF24NetworkHeader header(TO_MASTER, DATA_FROM_CENTER);
//  while(!network.write(header, &payload, sizeof(payload))){
//    if (!mesh.checkConnection()){
//      mesh.renewAddress();
//    }
//  }

}

