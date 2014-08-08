import peasy.*;

PeasyCam cam;
PFont    axisLabelFont;
PVector  axisXHud;
PVector  axisYHud;
PVector  axisZHud;
PVector  axisOrgHud;

// ------------------------------------------------------------------------ //
void setup()
{
   size( 640, 480, P3D );
   
   cam           = new PeasyCam(this, 100);
   axisLabelFont = createFont( "Arial", 14 );
   axisXHud      = new PVector();
   axisYHud      = new PVector();
   axisZHud      = new PVector();
   axisOrgHud    = new PVector();
}

// ------------------------------------------------------------------------ //
void draw()
{
   background(0);

   box( 10,10,10 );
   calculateAxis(50);

   cam.beginHUD();
      drawAxis( 2 );
   cam.endHUD();
}

// ------------------------------------------------------------------------ //
void calculateAxis( float length )
{
   // Store the screen positions for the X, Y, Z and origin
   axisXHud.set( screenX(length,0,0), screenY(length,0,0), 0 );
   axisYHud.set( screenX(0,length,0), screenY(0,length,0), 0 );     
   axisZHud.set( screenX(0,0,length), screenY(0,0,length), 0 );
   axisOrgHud.set( screenX(0,0,0), screenY(0,0,0), 0 );
}

// ------------------------------------------------------------------------ //
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


