
//
import processing.opengl.*;

//
import java.io.*; 
import java.net.*;

//
import peasy.*;

//
PeasyCam cam;
PFont    axisLabelFont;
PVector  axisXHud;
PVector  axisYHud;
PVector  axisZHud;
PVector  axisOrgHud;

float[] mag_angles=new float[3];
float[] gyro_angles=new float[3];
float[] acc_angles=new float[3];
float hz=0;
float[] mag=new float[3];
float[] acc=new float[3];
float[] gyro=new float[3];

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
          
          hz=json.getFloat("hz");
          
          gyro_angles[0]=json.getFloat("gyro_pitch");
          gyro_angles[1]=json.getFloat("gyro_roll");
          gyro_angles[2]=json.getFloat("gyro_yaw");
          
          acc_angles[0]=json.getFloat("acc_pitch");
          acc_angles[1]=json.getFloat("acc_roll");

          mag_angles[0]=json.getFloat("mag_pitch");
          mag_angles[1]=json.getFloat("mag_roll");
          mag_angles[2]=json.getFloat("mag_yaw");

          mag[0]=json.getFloat("mx");
          mag[1]=json.getFloat("my");
          mag[2]=json.getFloat("mz");

          acc[0]=json.getFloat("ax");
          acc[1]=json.getFloat("ay");
          acc[2]=json.getFloat("az");

          gyro[0]=json.getFloat("gx");
          gyro[1]=json.getFloat("gy");
          gyro[2]=json.getFloat("gz");         
          
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


void calculateAxis( float length )
{
   // Store the screen positions for the X, Y, Z and origin
   axisXHud.set( screenX(length,0,0), screenY(length,0,0), 0 );
   axisYHud.set( screenX(0,length,0), screenY(0,length,0), 0 );     
   axisZHud.set( screenX(0,0,length), screenY(0,0,length), 0 );
   axisOrgHud.set( screenX(0,0,0), screenY(0,0,0), 0 );
}

void drawAxis( float weight )
{
   pushStyle();   // Store the current style information

     strokeWeight( weight );      // Line width

     stroke( 255,   0,   0 );     // X axis color (Red)
     line( axisOrgHud.x, axisOrgHud.y, axisXHud.x, axisXHud.y );
 
     stroke(   0, 255,   0 );
     line( axisOrgHud.x, axisOrgHud.y, axisYHud.x, axisYHud.y );

     stroke(   0,   0, 255 );
     line( axisOrgHud.x, axisOrgHud.y, axisZHud.x, axisZHud.y );


      fill(255);                   // Text color
      textFont( axisLabelFont );   // Set the text font

      text( "X+", axisXHud.x, axisXHud.y );
      text( "Y+", axisYHud.x, axisYHud.y );
      text( "Z+", axisZHud.x, axisZHud.y );

   popStyle();    // Recall the previously stored style information
}



void setup() 
{
  //
  sendrecv();
  //
  size(1024, 600, P3D);
  //
  axisXHud      = new PVector();
  axisYHud      = new PVector();
  axisZHud      = new PVector();
  axisOrgHud    = new PVector();
  axisLabelFont = createFont( "Arial", 14 );
  cam = new PeasyCam(this, 150);
}




void buildBoxShape(float half_width,float half_height,float half_depth) {
  
  noStroke();
  beginShape(QUADS);

  //Z+ 
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
  
  background(#000000);
  fill(#ffffff);
  textFont(axisLabelFont,4);
  textAlign(LEFT, TOP);
  
  text("Gyroscope rate(hz): "+hz+"\nPitch (degrees): " + degrees(gyro_angles[0]) + "\nRoll (degrees): " + degrees(gyro_angles[1])+"\nYaw (degrees): " + degrees(gyro_angles[2]),-70,-70);
  
  text("Accelerometer rate(hz): "+hz+"\nPitch (degrees): " + degrees(acc_angles[0]) + "\nRoll (degrees): " + degrees(acc_angles[1])+"\nYaw (degrees): " + degrees(acc_angles[2]),+20,-70);
  
  text("Magnetometer rate(hz): "+hz+"\nPitch (degrees): " + degrees(mag_angles[0]) + "\nRoll (degrees): " + degrees(mag_angles[1])+"\nYaw (degrees): " + degrees(mag_angles[2]),-70,+50);


/////////////////////////////////  
/////////////////////////////////  

  calculateAxis(60);  
   cam.beginHUD();
  drawAxis(1);
  cam.endHUD();

/////////////////////////////////  
/////////////////////////////////  

  pushMatrix();
  fill(0,0,255);
  box(2, 2, 100);
  popMatrix();
  
  pushMatrix();
  fill(255,0,0);
  box(100, 2, 2);
  popMatrix();

  pushMatrix();
  fill(0,255,0);
  box(2, 100, 2);
  popMatrix();

///////////////////////////

  pushMatrix();
  translate(-25, -25, -25);
  rotateX(gyro_angles[0]); // screen x -> sensor x
  rotateY(gyro_angles[2]); // screen y -> sensor z
  rotateZ(gyro_angles[1]); // screen z -> sensor y
  buildBoxShape(15,5,10);
  popMatrix();

  pushMatrix();
  translate(-25, -25, +25);
  float gyroN= 200;//sqrt(gyro[0]*gyro[0]+gyro[1]*gyro[1]+gyro[2]*gyro[2]);
  box(gyro[0]/gyroN*40, 1, 1);
  box(1, gyro[2]/gyroN*40, 1);
  box(1, 1, gyro[1]/gyroN*40);
  popMatrix();
  
///////////////////////////

  pushMatrix();
  translate(25, -25, -25);
  rotateX(acc_angles[0]); // screen x -> sensor x
  rotateY(acc_angles[2]); // screen y -> sensor z
  rotateZ(acc_angles[1]); // screen z -> sensor y
  buildBoxShape(15,5,10);
  popMatrix();

  pushMatrix();
  translate(25, -25, +25);
  float accN= sqrt(acc[0]*acc[0]+acc[1]*acc[1]+acc[2]*acc[2]);
  box(acc[0]/accN*40, 1, 1);
  box(1, acc[2]/accN*40, 1);
  box(1, 1, acc[1]/accN*40);
  popMatrix();

/////////////////////////////

  pushMatrix();
  translate(-25, +25, -25);
  rotateX(mag_angles[0]); // screen x -> sensor x
  rotateY(mag_angles[2]); // screen y -> sensor z
  rotateZ(mag_angles[1]); // screen z -> sensor y
  buildBoxShape(15,5,10);
  popMatrix();

  pushMatrix();
  translate(-25, +25, +25);
  float magN= sqrt(mag[0]*mag[0]+mag[1]*mag[1]+mag[2]*mag[2]);
  box(mag[0]/magN*40, 1, 1);
  box(1, mag[2]/magN*40, 1);
  box(1, 1, mag[1]/magN*40);
  popMatrix();
  
}


