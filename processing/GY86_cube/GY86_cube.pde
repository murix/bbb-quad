

import processing.serial.*;
import processing.opengl.*;
import processing.net.*; 
import java.io.*; 
import java.net.*;


float[] position=new float[3];
float[] gyro_angles=new float[3];
float[] acc_angles=new float[3];
float dt=0;
float hz=0;

void sendrecv() {
  (new Thread() {
    public void run() {

      while (true) { 
        try {
          
          //
          DatagramSocket clientSocket = new DatagramSocket();  
          //
          clientSocket.setSoTimeout(1000); 
          //
          InetAddress IPAddress = InetAddress.getByName("beaglebone.local");
          int imu_port = 32000;

          //
          byte[] sendData = new byte[1024];     
          String command = "get";       
          sendData = command.getBytes();       
          //
          DatagramPacket sendPacket = new DatagramPacket(sendData, sendData.length, IPAddress, imu_port);       
          clientSocket.send(sendPacket);       

          //
          byte[] receiveData = new byte[1024];       
          DatagramPacket receivePacket = new DatagramPacket(receiveData, receiveData.length);       
          clientSocket.receive(receivePacket);      
          
          //
          String str=new String(receivePacket.getData());
          println(str);
          
          //
          JSONObject json = JSONObject.parse(str);
          dt=json.getFloat("dt");
          hz=json.getFloat("hz");
          gyro_angles[0]=json.getFloat("gyro_pitch");
          gyro_angles[1]=json.getFloat("gyro_roll");
          gyro_angles[2]=json.getFloat("gyro_yaw");
          acc_angles[0]=json.getFloat("acc_pitch");
          acc_angles[1]=json.getFloat("acc_roll");


          //
          clientSocket.close();
        } 
        catch(Exception ex) {
          println(ex);
        }
      }
    }
  }
  ).start();
}


PFont font;
final int VIEW_SIZE_X = 800, VIEW_SIZE_Y = 600;
void setup() 
{
  size(VIEW_SIZE_X, VIEW_SIZE_Y, P3D);
  
  sendrecv();

  font = loadFont("CourierNew36.vlw"); 
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


void draw() {

  //
  background(#000000);
  //
  fill(#ffffff);

  //  
  textFont(font, 20);
  textAlign(LEFT, TOP);
  text("update rate(hz): "+hz+"\nPitch (degrees): " + degrees(gyro_angles[0]) + "\nRoll (degrees): " + degrees(gyro_angles[1])+"\nYaw (degrees): " + degrees(gyro_angles[2]),20,20);
  text("update rate(hz): "+hz+"\nPitch (degrees): " + degrees(acc_angles[0]) + "\nRoll (degrees): " + degrees(acc_angles[1])+"\nYaw (degrees): " + degrees(acc_angles[2]),400,20);
  
  
  
  //
  pushMatrix();
  translate((VIEW_SIZE_X/2), (VIEW_SIZE_Y/2)+30, 0);
  scale(scale, scale, scale);
  box(100, 2, 100);
  popMatrix();

  pushMatrix();
  translate(VIEW_SIZE_X/2 -100 , VIEW_SIZE_Y/2 -50 , 0);
  scale(scale, scale, scale);
  rotateX(gyro_angles[0]); // screen x -> sensor x
  rotateY(gyro_angles[2]); // screen y -> sensor z
  rotateZ(gyro_angles[1]); // screen z -> sensor y
  buildBoxShape();
  popMatrix();
  
  pushMatrix();
  translate(VIEW_SIZE_X/2 +100 , VIEW_SIZE_Y/2 -50 , 0);
  scale(scale, scale, scale);
  rotateX(acc_angles[0]);
  rotateY(acc_angles[2]);
  rotateZ(acc_angles[1]);
  buildBoxShape();
  popMatrix();
  
}


