 /** RF24Mesh_Example_Master.ino by TMRh20
  * 
  *
  * This example sketch shows how to manually configure a node via RF24Mesh as a master node, which
  * will receive all data from sensor nodes.
  *
  * The nodes can change physical or logical position in the network, and reconnect through different
  * routing nodes as required. The master node manages the address assignments for the individual nodes
  * in a manner similar to DHCP.
  *
  */
  
  
#include "RF24Network.h"
#include "RF24.h"
#include "RF24Mesh.h"
#include <SPI.h>
//Include eeprom.h for AVR (Uno, Nano) etc. except ATTiny
#include <EEPROM.h>

/***** Configure the chosen CE,CS pins *****/
RF24 radio(9,10);
RF24Network network(radio);
RF24Mesh mesh(radio,network);

uint32_t displayTimer = 0;

//nodes definition
#define CENTER 1
#define LEFT_CENTER 2
#define LEFT_TOP 3
#define RIGHT_CENTER 4
#define RIGHT_TOP 5 

struct payload_t {
  unsigned long ms;
  unsigned long data;
};

void setup() {
  Serial.begin(115200);

  // Set the nodeID to 0 for the master node
  mesh.setNodeID(0);
  Serial.println(mesh.getNodeID());
  // Connect to the mesh
  mesh.begin();

}


void loop() {    

  // Call mesh.update to keep the network updated
  mesh.update();
  
  // In addition, keep the 'DHCP service' running on the master node so addresses will
  // be assigned to the sensor nodes
  mesh.DHCP();

  //protocol to send data form the serial monitor
  if (network.available()){
    uint8_t m_type;
    m_type = Serial.read();
    
    //use ASCII 65-127 (7 bits) to send the type in the header;  user-defined
    uint8_t node;
    uint32_t content; //maximum size-wise content
    
    /* case A: center goes forward for t seconds
     * case B:
     * case C:
     * 
     */
    switch(m_type){
      case 'A': node = CENTER;
                content = 0b00000001;
                mesh.write(&content, m_type, sizeof(content), node);
      default:
      break;
    }
  }

  //protocol to collect data from nodes
  if (network.available()){
    RF24NetworkHeader header;
    network.peek(header);
    
    payload_t payload;
    
    //nodes are OCT related, how to handle this in the case switch?
    switch(header.from_node){
      //creat a routine to save or send data to computer
      case mesh.getAddress(CENTER): network.read(header,&payload,sizeof(payload));
                                    Serial.println(payload.data);
                                    Serial.println(paylaod.ms);
                                    Serial.println(header.type); // show message type
      default: network.read(header,0,0);
      break;
    }
  }
  
}
