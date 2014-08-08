

import processing.serial.*;
import processing.opengl.*;
import processing.net.*; 
import java.io.*; 
import java.net.*;



//Notation "w" stands for one of the axes, so for example RwAcc[0],RwAcc[1],RwAcc[2] means RxAcc,RyAcc,RzAcc
//Variables below must be global (their previous value is used in getEstimatedInclination)
float[] RwEst=new float[3];     //Rw estimated from combining RwAcc and RwGyro
long lastMicros;  
//Variables below don't need to be global but we expose them for debug purposes
long interval; //interval since previous analog samples
float[] RwAcc=new float[3];         //projection of normalized gravitation force vector on x/y/z axis, as measured by accelerometer
float[] RwGyro=new float[3];        //Rw obtained from last estimated value and gyro movement
float[] Awz=new float[2];           //angles between projection of R on XZ/YZ plane and Z axis (deg)
float wGyro;  
int firstSample;



float[] position=new float[3];
float[] gyro_angles=new float[3];
float[] acc_angles=new float[3];
float[] acc=new float[3];
float[] gyro=new float[3];
float[] mag=new float[3];
float dt=0;
float hz=0;

float[] Rest_back = new float[3];
float[] Rest_now = new float[3];

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
          
          acc[0]=json.getFloat("ax");
          acc[1]=json.getFloat("ay");
          acc[2]=json.getFloat("az");

          gyro[0]=json.getFloat("gx");
          gyro[1]=json.getFloat("gy");
          gyro[2]=json.getFloat("gz");

          mag[0]=json.getFloat("mx");
          mag[1]=json.getFloat("my");
          mag[2]=json.getFloat("mz");

  float Rx=acc[0];
  float Ry=acc[1];
  float Rz=acc[2];
  float R = (float)Math.sqrt(Math.pow(Rx,2)+Math.pow(Ry,2)+Math.pow(Rz,2));
  print("R="); println(R);
  float Axr= (float)Math.acos(Rx/R);
  float Ayr= (float)Math.acos(Ry/R);
  float Azr= (float)Math.acos(Rz/R);
  //
  print("Axr="); println(Axr);
  print("Ayr="); println(Ayr);
  print("Azr="); println(Azr);
  //
  print("cos(Axr)="); println(cos(Axr));
  print("cos(Ayr)="); println(cos(Ayr));
  print("cos(Azr)="); println(cos(Azr));
  //must be 1 - to be ok
  print("unit vector test="); println(Math.sqrt(Math.pow(cos(Axr),2)+Math.pow(cos(Ayr),2)+Math.pow(cos(Azr),2)));
  
  //normalize acc
  float[] Raccn = new float[3];
  Raccn[0] = Rx/R;
  Raccn[1] = Ry/R;
  Raccn[2] = Rz/R;
  
  // estimate
  Rest_back[0]=Rest_now[0];
  Rest_back[1]=Rest_now[1];
  Rest_back[2]=Rest_now[2];
  Rest_now[0]=0;
  Rest_now[1]=0;
  Rest_now[2]=0;


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



int i,w;
float tmpf,tmpf2;  
long newMicros; //new timestamp
int signRzGyro;  
void getEstimatedInclination(){





}

PFont font;
final int VIEW_SIZE_X = 1020, VIEW_SIZE_Y = 600;
void setup() 
{
  size(VIEW_SIZE_X, VIEW_SIZE_Y, P3D);
  
  sendrecv();

  font = loadFont("CourierNew36.vlw"); 
  
  
  ///////////////////////////////////////
  wGyro = 10;
  firstSample = 1;
  
  /////////////////////////////////////////
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
  text("Gyroscope rate(hz): "+hz+"\nPitch (degrees): " + degrees(gyro_angles[0]) + "\nRoll (degrees): " + degrees(gyro_angles[1])+"\nYaw (degrees): " + degrees(gyro_angles[2]),20,20);
  text("Accelerometer rate(hz): "+hz+"\nPitch (degrees): " + degrees(acc_angles[0]) + "\nRoll (degrees): " + degrees(acc_angles[1])+"\nYaw (degrees): " + degrees(acc_angles[2]),400,20);
  
  
  
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
  
  rotateX(acc_angles[0]); // screen x -> sensor x
  rotateY(acc_angles[2]); // screen y -> sensor z
  rotateZ(acc_angles[1]); // screen z -> sensor y
  
  /*
  rotateX(Ayr); // screen x -> sensor x
  rotateY(Azr); // screen y -> sensor z
  rotateZ(Axr); // screen z -> sensor y
  */
  buildBoxShape();
  popMatrix();
  
}


