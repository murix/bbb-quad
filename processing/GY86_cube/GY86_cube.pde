
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
float[] mag_angles=new float[3];
float[] gyro_angles=new float[3];
float[] acc_angles=new float[3];
float hz=0;
float magN=0;
float accN=0;
float[] mag=new float[3];
float[] acc=new float[3];
float[] gyro=new float[3];
double hz_net;

void sendrecv() {
  (new Thread() {
    public void run() {

      
      long t_now = System.currentTimeMillis();
      long t_back = t_now;
      
      while (true) { 
        String str="";
        try {
          sleep( (1/30)*1000 );
          t_back = t_now;
          t_now = System.nanoTime();
          hz_net = 1/((double)(t_now - t_back)/(1000*1000*1000));

          //
          DatagramSocket clientSocket = new DatagramSocket();  
          //
          clientSocket.setSoTimeout(1000); 

          //
          InetAddress IPAddress = InetAddress.getByName("beaglebone.local");
          String command="g";
          byte[] command_bin=command.getBytes();
          DatagramPacket sendPacket = new DatagramPacket(command_bin, command_bin.length, IPAddress, 32000);       
          clientSocket.send(sendPacket);       

          //
          byte[] buf=new byte[2048];
          DatagramPacket receivePacket = new DatagramPacket(buf, buf.length);       
          clientSocket.receive(receivePacket);      

          //
          str=new String(receivePacket.getData());
          //println(str);

          //
          JSONObject json = JSONObject.parse(str);

          hz=json.getFloat("hz");

          gyro_angles[0]=json.getFloat("gyro_pitch");
          gyro_angles[1]=json.getFloat("gyro_roll");
          gyro_angles[2]=json.getFloat("gyro_yaw");

          acc_angles[0]=json.getFloat("acc_pitch");
          acc_angles[1]=json.getFloat("acc_roll");
          acc_angles[2]=json.getFloat("acc_yaw");

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
         
          accN=json.getFloat("accn");
          magN=json.getFloat("magn");

          //
          clientSocket.close();
        } 
        catch(Exception ex) {
          println(ex);
          println(str);
        }
      }
    }
  }
  ).start();
}

void setup() 
{
  //
  sendrecv();
  
  //
  size(1024, 600, P3D);
 
  //font
  axisLabelFont = createFont( "Arial", 14 );
  textFont(axisLabelFont, 4);
  
  //camera
  cam = new PeasyCam(this, 150);
}

void buildBoxShape(float half_width, float half_height, float half_depth) {

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


  ///////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////

  background(#000000);
  
  ///////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////


  pushMatrix();
  stroke(100, 0, 0);
  line(0,0,0,-60,0,0);
  text("-X",-60,0,0);
  stroke(255, 0, 0);
  line(0,0,0,60,0,0);
  text("+X",60,0,0);
  stroke(0, 100, 0);
  line(0,0,0,0,-60,0);
  text("-Y",0,-60,0);
  stroke(0, 255, 0);
  line(0,0,0,0,60,0);
  text("+Y",0,60,0);
  stroke(0, 0, 100);
  line(0,0,0,0,0,-60);
  text("-Z",0,0,-60);
  stroke(0, 0, 255);
  line(0,0,0,0,0,60);
  text("+Z",0,0,60);
  popMatrix();

  ///////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////


  pushMatrix();
  translate(-25, -25, +25);
  strokeWeight(3);

  float gyro_scale=20;
  
  stroke( 255, 0, 0 );     
  line( 0,0,0,     gyro[0]/gyro_scale,0,0 );
  text("gX",       gyro[0]/gyro_scale,0,0);
  
  stroke(   0, 255, 0 );
  line( 0,0,0,     0,gyro[1]/gyro_scale,0 );
  text( "gY",      0,gyro[1]/gyro_scale,0 );
  
  stroke(   0, 0, 255 );
  line( 0,0,0,     0,0,gyro[2]/gyro_scale );
  text( "gZ",      0,0,gyro[2]/gyro_scale );
  
  popMatrix();

  pushMatrix();
  text("Gyroscope\nimu(hz): "+hz+"\nnet(hz): "+hz_net+"\nPitch (degrees): " + degrees(gyro_angles[0]) + "\nRoll (degrees): " + degrees(gyro_angles[1])+"\nYaw (degrees): " + degrees(gyro_angles[2]), -70, -70,-25);
  popMatrix();

  pushMatrix();
  translate(-25, -25, -25);
  rotateX(gyro_angles[0]); // screen x -> sensor x
  rotateY(gyro_angles[1]); // screen y -> sensor z
  rotateZ(gyro_angles[2]); // screen z -> sensor y
  buildBoxShape(15, 5, 10);
  popMatrix();
  

  ///////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////


  pushMatrix();
  translate(25, -25, +25);
  strokeWeight(3);
  
  stroke( 255, 0, 0 );     
  line( 0,0,0,    acc[0]/accN*25,0,0 );
  text( "aX",     acc[0]/accN*25,0,0);
  
  stroke(   0, 255, 0 );
  line( 0,0,0,    0,acc[1]/accN*25,0 );
  text( "aY",     0,acc[1]/accN*25,0);
  
  stroke(   0, 0, 255 );
  line( 0,0,0,    0,0,acc[2]/accN*25 );
  text( "aZ",     0,0,acc[2]/accN*25);
  
  stroke(   0, 255, 255 );
  line( 0,0,0,        acc[0]/accN*25,acc[1]/accN*25,acc[2]/accN*25);
  text("aN="+accN,    acc[0]/accN*25,acc[1]/accN*25,acc[2]/accN*25);
  popMatrix();

  pushMatrix();
  text("Accelerometer rate(hz): "+hz+"\nPitch (degrees): " + degrees(acc_angles[0]) + "\nRoll (degrees): " + degrees(acc_angles[1])+"\nYaw (degrees): " + degrees(acc_angles[2]), +20, -70,-25);
  popMatrix();

  pushMatrix();
  translate(25, -25, -25);
  rotateX(acc_angles[0]); // screen x -> sensor x
  rotateY(acc_angles[1]); // screen y -> sensor z
  rotateZ(acc_angles[2]); // screen z -> sensor y
  buildBoxShape(15, 5, 10);
  popMatrix();

  ///////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////
  
  pushMatrix();
  translate(-25, +25, +25);
  box(1, 1, 1);
  strokeWeight(3);
  
  stroke( 255, 0, 0 );     
  line( 0,0,0,       mag[0]/magN*25,0,0 );
  text( "mX",        mag[0]/magN*25,0,0 );
  
  stroke(   0, 255, 0 );
  line( 0,0,0,       0,mag[1]/magN*25,0 );
  text( "mY",        0,mag[1]/magN*25,0 );
  
  stroke(   0, 0, 255 );
  line( 0,0,0,       0,0,mag[2]/magN*25 );
  text( "mZ",        0,0,mag[2]/magN*25 );
  
  stroke(   0, 255, 255 );
  line( 0,0,0,          mag[0]/magN*25,mag[1]/magN*25,mag[2]/magN*25 );
  text("mN="+magN,      mag[0]/magN*25,mag[1]/magN*25,mag[2]/magN*25);
  
  //stroke(#ffffff);
  //noFill();
  //sphere(25);
  popMatrix();

  pushMatrix();
  text("Magnetometer\nPitch (degrees): " + degrees(mag_angles[0]) + "\nRoll (degrees): " + degrees(mag_angles[1])+"\nYaw (degrees): " + degrees(mag_angles[2]), -70, +50,-25);
  popMatrix();
  
  pushMatrix();
  translate(-25, +25, -25);
  rotateX(mag_angles[0]); 
  rotateY(mag_angles[1]); 
  rotateZ(mag_angles[2]); 
  buildBoxShape(15, 5, 10);
  popMatrix();

  ///////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////
  
}

