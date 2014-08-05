

import processing.serial.*;
import processing.opengl.*;
import processing.net.*; 
import java.io.*; 
import java.net.*;

void sendrecv(){
  (new Thread() {
  public void run() {

   while(true){ 
  try {
  DatagramSocket clientSocket = new DatagramSocket();  
  clientSocket.setSoTimeout(1000); 
  InetAddress IPAddress = InetAddress.getByName("beaglebone.local");       
  byte[] sendData = new byte[1024];       
  byte[] receiveData = new byte[1024];       
  String sentence = "g";       
  sendData = sentence.getBytes();       
  DatagramPacket sendPacket = new DatagramPacket(sendData, sendData.length, IPAddress, 32000);       
  clientSocket.send(sendPacket);       
  DatagramPacket receivePacket = new DatagramPacket(receiveData, receiveData.length);       
  clientSocket.receive(receivePacket);       
  String modifiedSentence = new String(receivePacket.getData());       
  //System.out.println("FROM SERVER:" + modifiedSentence);  
  decode_string(modifiedSentence);  
  clientSocket.close();
  } catch(Exception ex){
  }
  
   }
    }
 }).start();

}


Client myClient; 
Serial myPort;  
final String serialPort = "/dev/vmodem"; // replace this with your serial port. On windows you will need something like "COM1".
float [] Euler = new float [3]; 
float [] position = new float [3];
PFont font;
final int VIEW_SIZE_X = 800, VIEW_SIZE_Y = 600;
String title="testing";

void setup() 
{
  size(VIEW_SIZE_X, VIEW_SIZE_Y, OPENGL);

  
  sendrecv();

  //myPort = new Serial(this, serialPort, 115200);
  // The font must be located in the sketch's "data" directory to load successfully
  font = loadFont("CourierNew36.vlw"); 
  //myPort.clear();
}

void decode_string(String inputString){
    if (inputString != null && inputString.length() > 0) {
      //println(inputString);
      String [] inputStringArr = split(inputString, "|");
      if(inputStringArr.length >= 7) {
        title=inputStringArr[0];
        Euler[0]=Float.parseFloat(inputStringArr[1]);
        Euler[1]=Float.parseFloat(inputStringArr[2]);
        Euler[2]=Float.parseFloat(inputStringArr[3]);
        position[0]=Float.parseFloat(inputStringArr[4]);
        position[1]=Float.parseFloat(inputStringArr[5]);
        position[2]=Float.parseFloat(inputStringArr[6]);
      }
    }
}

void serialEvent(Serial p) {
   
    String inputString = p.readStringUntil('\r');
    decode_string(inputString);
}


int scale=5;
int half_width=15;
int half_height=5;
int half_depth=10;

void buildBoxShape() {
  //box(60, 10, 40);
  noStroke();
  beginShape(QUADS);
  

  
  //Z+ (to the drawing area)
  fill(#00ff00);
  vertex(-half_width, -half_height, half_depth);
  vertex(half_width, -half_height, half_depth);
  vertex(half_width, half_height, half_depth);
  vertex(-half_width, half_height, half_depth);
  
  //Z-
  fill(#0000ff);
  vertex(-half_width, -half_height, -half_depth);
  vertex(half_width, -half_height, -half_depth);
  vertex(half_width, half_height, -half_depth);
  vertex(-half_width, half_height, -half_depth);
  
  //X-
  fill(#ff0000);
  vertex(-half_width, -half_height, -half_depth);
  vertex(-half_width, -half_height, half_depth);
  vertex(-half_width, half_height, half_depth);
  vertex(-half_width, half_height, -half_depth);
  
  //X+
  fill(#ffff00);
  vertex(half_width, -half_height, -half_depth);
  vertex(half_width, -half_height, half_depth);
  vertex(half_width, half_height, half_depth);
  vertex(half_width, half_height, -half_depth);
  
  //Y-
  fill(#ff00ff);
  vertex(-half_width, -half_height, -half_depth);
  vertex(half_width, -half_height, -half_depth);
  vertex(half_width, -half_height, half_depth);
  vertex(-half_width, -half_height, half_depth);
  
  //Y+
  fill(#00ffff);
  vertex(-half_width, half_height, -half_depth);
  vertex(half_width, half_height, -half_depth);
  vertex(half_width, half_height, half_depth);
  vertex(-half_width, half_height, half_depth);
  
  endShape();
}


void drawCube() { 
 
  
  pushMatrix();
  translate((VIEW_SIZE_X/2),(VIEW_SIZE_Y/2)+30,0);
  scale(scale,scale,scale);
  box(100,2,100);
  popMatrix();
 
  pushMatrix();
    translate(VIEW_SIZE_X/2 - position[0], VIEW_SIZE_Y/2 - position[2], -position[1]);
    scale(scale,scale,scale);
     //x=x
   //y=z
   //z=y
    rotateX(-Euler[0]);
    rotateY(Euler[2]);
    rotateZ(Euler[1]);
    
    buildBoxShape();
  
    
  popMatrix();
}


void draw() {
  /*
  if(myClient!=null){
    myClient.write("g");
  }
  */
  
  background(#000000);
  fill(#ffffff);
  textFont(font, 20);
  textAlign(LEFT, TOP);
  text(title+"\nPitch (degrees): " + degrees(Euler[0]) + "\nRoll (degrees): " + degrees(Euler[1])+"\nYaw (degrees): " + degrees(Euler[2])+ "\nFront/Rear (cm): "+position[0]+ "\nLeft/Right (cm): "+position[1]+ "\nAltimeter (cm): "+position[2], 20, 20);
  drawCube();
}


