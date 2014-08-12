
//
import processing.opengl.*;

//
import java.io.*; 
import java.net.*;

//
import peasy.*;

//
PeasyCam cam;
float hz_imu;
double hz_net;
float mag_x,mag_y,mag_z,mag_n,mag_head;
float acc_x,acc_y,acc_z,acc_n,acc_pitch,acc_roll,acc_yaw;
float gyro_x,gyro_y,gyro_z,gyro_pitch,gyro_roll,gyro_yaw;
float fusion_pitch,fusion_roll;

void sendrecv() {
  (new Thread() {
    public void run() {

      
      long t_now = System.currentTimeMillis();
      long t_back = t_now;
      
      while (true) { 
        String str="";
        try {
          //sleep( 16 );
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

          //
          hz_imu=json.getFloat("time_hz");

          //
          gyro_x=json.getFloat("gyro_x");
          gyro_y=json.getFloat("gyro_y");
          gyro_z=json.getFloat("gyro_z");        
          gyro_pitch=json.getFloat("pitch_gyro");
          gyro_roll=json.getFloat("roll_gyro");
          gyro_yaw=json.getFloat("yaw_gyro");

          //
          acc_x=json.getFloat("acc_x");
          acc_y=json.getFloat("acc_y");
          acc_z=json.getFloat("acc_z");
          acc_n=json.getFloat("acc_n");
          acc_pitch=json.getFloat("pitch_acc");
          acc_roll=json.getFloat("roll_acc");

          //
          mag_x=json.getFloat("mag_x");
          mag_y=json.getFloat("mag_y");
          mag_z=json.getFloat("mag_z");
          mag_n=json.getFloat("mag_n");
          mag_head=json.getFloat("mag_head");

          fusion_pitch=json.getFloat("fusion_pitch");
          fusion_roll=json.getFloat("fusion_roll");

         

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
 
  
  //camera
  cam = new PeasyCam(this, 300);
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
  text("Gyroscope\nPitch (degrees): " + degrees(gyro_pitch) + "\nRoll (degrees): " + degrees(gyro_roll)+"\nYaw (degrees): " + degrees(gyro_yaw), -300, -150,-25);
  popMatrix();
  pushMatrix();
  text("Fusion\nPitch: " + degrees(acc_pitch) + "\nRoll: " + degrees(acc_roll)+"\n", +100, -150,-25);
  popMatrix();
  pushMatrix();
  text("HMC5883\nHeading (degrees): " + degrees(mag_head), -200, +100,-25);
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
  line( 0,0,0,     gyro_x/gyro_scale,0,0 );
  text("gX",       gyro_x/gyro_scale,0,0);
  
  stroke(   0, 255, 0 );
  line( 0,0,0,     0,gyro_z/gyro_scale,0 );
  text( "gZ",      0,gyro_z/gyro_scale,0 );
  
  stroke(   0, 0, 255 );
  line( 0,0,0,     0,0,gyro_y/gyro_scale );
  text( "gY",      0,0,gyro_y/gyro_scale );
  
  popMatrix();


  pushMatrix();
  translate(-25, -25, -25);
  rotateX(gyro_pitch); // screen x -> sensor x
  rotateY(gyro_yaw); // screen y -> sensor z
  rotateZ(gyro_roll); // screen z -> sensor y
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
  line( 0,0,0,    acc_x/acc_n*25,0,0 );
  text( "aX",     acc_x/acc_n*25,0,0);
  
  stroke(   0, 255, 0 );
  line( 0,0,0,    0,acc_z/acc_n*25,0 );
  text( "aZ",     0,acc_z/acc_n*25,0);
  
  stroke(   0, 0, 255 );
  line( 0,0,0,    0,0,acc_y/acc_n*25 );
  text( "aY",     0,0,acc_y/acc_n*25);
  
  stroke(   0, 255, 255 );
  line( 0,0,0,         acc_x/acc_n*25,acc_z/acc_n*25,acc_y/acc_n*25);
  text("aN"       ,    acc_x/acc_n*25,acc_z/acc_n*25,acc_y/acc_n*25);
  popMatrix();


  pushMatrix();
  translate(25, -25, -25);
  rotateX(acc_pitch); // screen x -> sensor x
  rotateY(0); // screen y -> sensor z
  rotateZ(acc_roll); // screen z -> sensor y
  buildBoxShape(15, 5, 10);
  popMatrix();

  pushMatrix();
  translate(75, -25, -25);
  rotateX(fusion_pitch); // screen x -> sensor x
  rotateY(0); // screen y -> sensor z
  rotateZ(fusion_roll); // screen z -> sensor y
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
  line( 0,0,0,       mag_x/mag_n*25,0,0 );
  text( "mX",        mag_x/mag_n*25,0,0 );
  
  stroke(   0, 255, 0 );
  line( 0,0,0,       0,mag_z/mag_n*25,0 );
  text( "mZ",        0,mag_z/mag_n*25,0 );
  
  stroke(   0, 0, 255 );
  line( 0,0,0,       0,0,mag_y/mag_n*25 );
  text( "mY",        0,0,mag_y/mag_n*25 );
  
  stroke(   0, 255, 255 );
  line( 0,0,0,           mag_x/mag_n*25,mag_z/mag_n*25,mag_y/mag_n*25 );
  text("mN",             mag_x/mag_n*25,mag_z/mag_n*25,mag_y/mag_n*25);
  
  popMatrix();

  
  pushMatrix();
  translate(-25, +25, -25);
  rotateY(mag_head); 
  buildBoxShape(15, 5, 10);
  popMatrix();

  ///////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////
  
}

