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
  
  
#include <RF24Network.h>
#include "RF24.h"
#include "RF24Mesh.h"
#include <SPI.h>
//Include eeprom.h for AVR (Uno, Nano) etc. except ATTiny
#include <EEPROM.h>


/***** Configure the chosen CE,CS pins *****/
RF24 radio(9,10);
RF24Network network(radio);
RF24Mesh mesh(radio,network);

//used for incoming data form the nodes
struct payload_t {
  unsigned long ms;
  unsigned long data;
  char data_type;
};

//used for sending commands to the nodes
struct command_t {
  bool motorEnable;
  int motorVelocity;
};

//global variables
char m_type = 0;
int velocity = 200;
command_t content;
RF24NetworkHeader header;

//nodes definition
#define CENTER 01
#define LEFT_CENTER 02
#define LEFT_TOP 03
#define RIGHT_CENTER 04
#define RIGHT_TOP 05 

//message types for nodeID identification
#define DATA_FROM_CENTER 'a'
#define DATA_FROM_LEFT_CENTER 'b'
#define DATA_FROM_LEFT_TOP 'c'
#define DATA_FROM_RIGHT_CENTER 'd'
#define DATA_FROM_RIGHT_TOP 'e'

//types of data to be sent
#define IMU 'I'

void setup() {
  Serial.begin(115200);

  // Set the nodeID to 0 for the master node
  mesh.setNodeID(0);
  Serial.println(mesh.getNodeID());
  // Connect to the mesh
  mesh.begin();
  Serial.println("Initializing the master");
  
  radio.setRetries(15,15);
}

void loop() {    

  // Call mesh.update to keep the network updated
  mesh.update();
  
  // In addition, keep the 'DHCP service' running on the master node so addresses will
  // be assigned to the sensor nodes
  mesh.DHCP();

  //protocol to send data form the serial monitor  
  // This line gets everything that is on the buffer, thus it is possible to send a phrase and process it all
  m_type = char(Serial.read());
  
  //use ASCII 65-127 (7 bits) to send the type in the header;  user-defined
  uint8_t node;  
  switch(m_type){
    
	/* Begining cases for the center ************************************************************/
	/*
					      W: goes forward					    1:increase velovity			3: pre programed movement
	A:turns left	S (stops)  D:turns right		2:descrese velocity
					      X:goes backward
		
	*/
 
	case 'A': 
  		//Constructing the header
  		header.to_node = mesh.getAddress(CENTER);       //destination 
  		header.type = m_type;                           //type of message
  	  
  		Serial.println("Sending spin command to center...");       //debugging
  		content.motorEnable = true;
  		content.motorVelocity = velocity;
  		while(!network.write(header, &content, sizeof(content))){Serial.println("Write Failed");}
		m_type = 0;
		break;
  		//debugging
  		//Serial.println(mesh.addrList[0].nodeID);
  		//Serial.println(mesh.addrList[0].address);
	
    case 'D':
	    header.to_node = mesh.getAddress(CENTER);       //destination 
	    header.type = m_type;                           //type of message
		
	    Serial.println("Sending spin command to center...");       //debugging
	    content.motorEnable = true;
	    content.motorVelocity = velocity;
	    while(!network.write(header, &content, sizeof(content))){Serial.println("Write Failed");}
	    m_type = 0;
		break;

	case 'W':
        header.to_node = mesh.getAddress(CENTER);       //destination 
        header.type = m_type;                           //type of message
        
        Serial.println("Sending go command to center...");       //debugging
        content.motorEnable = true;
  	    content.motorVelocity = velocity;
        while(!network.write(header, &content, sizeof(content))){Serial.println("Write Failed");}
        m_type = 0;
		break;

	case 'X':
        header.to_node = mesh.getAddress(CENTER);       //destination 
        header.type = m_type;                           //type of message
        
        Serial.println("Sending go back command to center...");       //debugging
        content.motorEnable = true;
  	    content.motorVelocity = velocity;
        while(!network.write(header, &content, sizeof(content))){Serial.println("Write Failed");}
        m_type = 0;
		break;
		
	case 'S':
  		header.to_node = mesh.getAddress(CENTER);       //destination 
  		header.type = m_type;                           //type of message
  		  
  		Serial.println("Sending stop command to center...");       //debugging
  		content.motorEnable = false;
  		content.motorVelocity = 0;
  		while(!network.write(header, &content, sizeof(content))){Serial.println("Write Failed");}
  		m_type = 0;
		break;
		
	case '1':
		if ((velocity + 50) <= 255) {
			velocity = velocity + 50;
			Serial.print("Velocity power increased to:");
			Serial.println(velocity);
		}
		break;
		
	case '2':
		if ((velocity - 50) >= 0) {
			velocity = velocity - 50;
			Serial.print("Velocity power decrease to:");
			Serial.println(velocity);
		}
		break;
		
	case '3':
		Serial.println("Sending pre programed command");
		header.to_node = mesh.getAddress(CENTER); 
		header.type = 'W';
		content.motorEnable = true;
		content.motorVelocity = velocity;
		while(!network.write(header, &content, sizeof(content))){Serial.println("Write Failed 1");}
		delay(1000);

    header.to_node = mesh.getAddress(CENTER);
		header.type = 'A';
		content.motorEnable = true;
		content.motorVelocity = velocity;
		while(!network.write(header, &content, sizeof(content))){Serial.println("Write Failed 2");}
		delay(1000);
    
    header.to_node = mesh.getAddress(CENTER);
		header.type = 'W';
		content.motorEnable = false;
		content.motorVelocity = velocity;
		while(!network.write(header, &content, sizeof(content))){Serial.println("Write Failed 3");}
		delay(1000);

    header.to_node = mesh.getAddress(CENTER);
		header.type = 'D';
		content.motorEnable = true;
		content.motorVelocity = velocity;
		while(!network.write(header, &content, sizeof(content))){Serial.println("Write Failed 4");}
		delay(1000);
    
    header.to_node = mesh.getAddress(CENTER);
		header.type = 'S';
		content.motorEnable = true;
		content.motorVelocity = velocity;
		while(!network.write(header, &content, sizeof(content))){Serial.println("Write Failed 5");}

    m_type = 0;
		break;
		
	/* Begining cases for the center ************************************************************/
	
	
	/* Begining cases for the center_left *******************************************************/
  	case 'F':
  		header.to_node = mesh.getAddress(LEFT_CENTER);       //destination 
  		header.type = m_type;                           //type of message
  		 
  		Serial.println("Sending spin command to left center...");       //debugging
  		content.motorEnable = true;
  		content.motorVelocity = 200;
  		network.write(header, &content, sizeof(content));
  		m_type = 0;
		break;
		
  	case 'G':
  		header.to_node = mesh.getAddress(LEFT_CENTER);       //destination 
  		header.type = m_type;                           //type of message
  		  
  		Serial.println("Sending stop command to left center...");       //debugging
  		content.motorEnable = false;
  		content.motorVelocity = 0;
  		network.write(header, &content, sizeof(content));
  		m_type = 0;
		break;
	
	
		
		
    default: 
		break;
  }

  //protocol to collect data from nodes
//  if (network.available()){
//    RF24NetworkHeader header;
//    network.peek(header);
//    payload_t payload;
//    
//    //nodes are OCT related, how to handle this in the case switch?
//    switch(header.type){
//      //creat a routine to save or send data to computer
//      case DATA_FROM_CENTER: network.read(header,&payload,sizeof(payload));
//                             Serial.println("Getting data form the Center");
//                             Serial.println(payload.data,HEX);
//                             Serial.println(payload.ms);
//                             Serial.println(payload.data_type);
//                             Serial.println(char(header.type)); // show message type
//                             delay(1000);
//      default: network.read(header,0,0);
//      break;
//    }
//  }
  
}


