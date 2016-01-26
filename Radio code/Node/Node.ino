
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

#define PWR 255
#define STOP 0
 
//nodes definition
#define CENTER 1
#define LEFT_CENTER 2
#define LEFT_TOP 3
#define RIGHT_CENTER 4
#define RIGHT_TOP 5 

//Motors pin definitions
#define PIN_FRONT_MOTOR_DIR 4
#define PIN_FRONT_MOTOR_VEL 5
#define PIN_BACK_MOTOR_DIR 7
#define PIN_BACK_MOTOR_VEL 6

//Configure manually the motor direction after assembly
#define FRONT_MOTOR_FWD LOW
#define BACK_MOTOR_FWD HIGH
#define FRONT_MOTOR_BCK HIGH
#define BACK_MOTOR_BCK LOW

//LEDs pin definition
#define BLUE_LED A2
#define RED_LED A3

//message types for nodeID identification
#define DATA_FROM_CENTER 'a'
#define DATA_FROM_LEFT_CENTER 'b'
#define DATA_FROM_LEFT_TOP 'c'
#define DATA_FROM_RIGHT_CENTER 'd'
#define DATA_FROM_RIGHT_TOP 'e'

//types of data to be sent
#define IMU 'I'

#define nodeID CENTER
#define TO_MASTER 00

//used for the outgoing data from this node
struct payload_t {
  unsigned long ms;
  unsigned long data;
  char data_type;
};

//used for the incoming command
struct command_t {
  bool motorEnable;
  int motorVelocity;
};

void setup() {
  Serial.begin(115200);
  mesh.setNodeID(nodeID);
  Serial.println(F("Connecting to the mesh..."));
  mesh.begin();
  
  radio.setRetries(15,15);
  
  //Pins for the control of the motor
  pinMode(PIN_FRONT_MOTOR_DIR, OUTPUT);
  pinMode(PIN_FRONT_MOTOR_VEL, OUTPUT);
  pinMode(PIN_BACK_MOTOR_DIR, OUTPUT);
  pinMode(PIN_BACK_MOTOR_VEL, OUTPUT);
  
  //Set initial direction
  digitalWrite(PIN_FRONT_MOTOR_DIR, FRONT_MOTOR_FWD);
  digitalWrite(PIN_BACK_MOTOR_DIR, BACK_MOTOR_FWD);

  //Settign the LEDs as output
  pinMode(BLUE_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT); 

  //showing that the program went ON
  analogWrite(BLUE_LED,255);
  delay(500);
  analogWrite(BLUE_LED,0);
  analogWrite(RED_LED,255);
  delay(500);
  analogWrite(RED_LED,0);
  
}

void actuateMotor(bool actuate, int velocity) {
  if (actuate) {
	  analogWrite(PIN_BACK_MOTOR_VEL, velocity);
	  analogWrite(PIN_FRONT_MOTOR_VEL, velocity);
  }
  else {
	  analogWrite(PIN_BACK_MOTOR_VEL, STOP);
	  analogWrite(PIN_FRONT_MOTOR_VEL, STOP);
  }
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
    switch(char(header.type)){
      case 'D': 
        network.read(header, &data, sizeof(data));
        digitalWrite(PIN_FRONT_MOTOR_DIR, FRONT_MOTOR_FWD);
        digitalWrite(PIN_BACK_MOTOR_DIR, BACK_MOTOR_BCK);
		    actuateMotor(data.motorEnable, data.motorVelocity);
        analogWrite(BLUE_LED,255);
        delay(500);
        analogWrite(BLUE_LED,0);
				
				//code for debbuging incoming message
				// Serial.println("Message_type --- from --- data"); //for debugging
        // Serial.print(char(header.type)); Serial.print("           --- "); 
        // Serial.print(header.from_node); Serial.print("  --- ");
        // Serial.print("r-motor: ");
        // Serial.print(data.motorRight_on);
        // Serial.print("   l-motor: ");
        // Serial.print(data.motorLeft_on);
        // Serial.print("  velocity-r: ");
        // Serial.print(data.velocityRight_motor);
        // Serial.print("  velocity-l: ");
        // Serial.print(data.velocityLeft_motor);
        // Serial.println("\n");
        break;
		
	  case 'S': 
	      network.read(header, &data, sizeof(data));
		    analogWrite(PIN_FRONT_MOTOR_VEL, 0);
        analogWrite(PIN_BACK_MOTOR_VEL, 0);
        analogWrite(RED_LED,255);
        delay(500);
        analogWrite(RED_LED,0);
        break;

    case 'A': 
        network.read(header, &data, sizeof(data));
        digitalWrite(PIN_BACK_MOTOR_DIR, BACK_MOTOR_FWD);
        digitalWrite(PIN_FRONT_MOTOR_DIR, FRONT_MOTOR_BCK);
		    actuateMotor(data.motorEnable, data.motorVelocity);
        //Serial.println("Received A");
        analogWrite(BLUE_LED,255);
        delay(500);
        analogWrite(BLUE_LED,0);
        break;

    case 'W': 
        network.read(header, &data, sizeof(data));
        digitalWrite(PIN_FRONT_MOTOR_DIR, FRONT_MOTOR_FWD);
        digitalWrite(PIN_BACK_MOTOR_DIR, BACK_MOTOR_FWD);
		    actuateMotor(data.motorEnable, data.motorVelocity);
        analogWrite(RED_LED,255);
        delay(500);
        analogWrite(RED_LED,0);
        analogWrite(BLUE_LED,255);
        delay(500);
        analogWrite(BLUE_LED,0);
        break;

    case 'X': 
        network.read(header, &data, sizeof(data));
        digitalWrite(PIN_FRONT_MOTOR_DIR, FRONT_MOTOR_BCK);
        digitalWrite(PIN_BACK_MOTOR_DIR, BACK_MOTOR_BCK);
		    actuateMotor(data.motorEnable, data.motorVelocity);
        analogWrite(RED_LED,255);
        delay(500);
        analogWrite(RED_LED,0);
        analogWrite(BLUE_LED,255);
        delay(500);
        analogWrite(BLUE_LED,0);
        break;
        
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

