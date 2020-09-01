#include "led-matrix.h"
#include "graphics.h"
#include "TetrisMatrixDraw.h"

#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <functional>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <ctime>
#include <signal.h>
#include <syslog.h>
#include <sys/time.h>

using namespace std;

using rgb_matrix::GPIO;
using rgb_matrix::RGBMatrix;
using rgb_matrix::Canvas;


volatile bool interrupt_received = false;
static void InterruptHandler(int signo) {
	 syslog( LOG_NOTICE, "interrupt handler ");
	  interrupt_received = true;
}


void DrawSecondFrame(Canvas* canvas, int milliseconds, int hour)
{
    int red   = 0x10;
    int green = 0x10;
    int blue  = 0x10;

    int frame_leds = ((milliseconds * 124.0) / 60000.0) + 1;
    //printf("seconds: $%d number of leds: %d\n", milliseconds/1000, frame_leds); 
    for( int topx = 16; topx < 32 && frame_leds > 0; topx++ )
    {
	canvas->SetPixel(topx, 0, red, green, blue );
	frame_leds--;
    }
    for( int rightY = 1; rightY < 32 && frame_leds > 0; rightY++ )
    {
	canvas->SetPixel( 31, rightY, red,green, blue);
	frame_leds--;
    }
    for( int bottomX = 30; bottomX >=0 && frame_leds > 0; bottomX--)
    {
	   canvas->SetPixel(bottomX, 31, red, green, blue);
	  frame_leds--;
    }
   for ( int leftY = 30; leftY >= 0 && frame_leds > 0; leftY-- )
   {
	  canvas->SetPixel(0, leftY, red, green, blue);
	 frame_leds--;
   }
   for ( int topX1 = 1; topX1 < 16 && frame_leds > 0; topX1++)
   {
	   canvas->SetPixel(topX1, 0, red,green,blue);
	   frame_leds--;
   }

}

int main(int argc, char *argv[]) 
{
  setlogmask( LOG_UPTO (LOG_NOTICE));
  openlog ("tetrisclock", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);

  syslog( LOG_NOTICE, "Tetrisclock started pid: %d", getuid());
  GPIO io;
  if (!io.Init())
    return 1;

  srand((unsigned int)time(NULL));
  float brightness = 0.2;

  Canvas *canvas = new RGBMatrix(&io, 32, 1);

  TetrisMatrixDraw tetris(canvas, brightness);


  signal(SIGTERM, InterruptHandler);
  signal(SIGINT, InterruptHandler);
	
  time_t t = time(0);
  struct tm* now = localtime(&t);

  bool cont = true;

  tetris.setText("");

  while(cont)
  {
	// draw time
	t = time(0);
	now = localtime(&t);

	timeval tv;
	gettimeofday(&tv, NULL);

	int ms = tv.tv_usec / 1000;
	ms += now->tm_sec * 1000;

	canvas->Clear();
	tetris.drawText(3,20);

	DrawSecondFrame(canvas, ms, now->tm_hour);

	// wait a bit
	usleep(125000);
	if ( interrupt_received )
		cont = false;

        now = localtime(&t);

        std::string strHour = std::to_string(now->tm_hour);
        if( strHour.length() == 1) strHour.insert(0, 1, '0');
        std::string strMinute = std::to_string(now->tm_min);
        if( strMinute.length() == 1) strMinute.insert(0,1,'0');

	string strtime(strHour + strMinute);
        tetris.setText(strtime);
  }

  syslog(LOG_NOTICE, "end of tetrisclock,natural end");


  canvas->Clear();
  delete canvas;
  return 0;
}

