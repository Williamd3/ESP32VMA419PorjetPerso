/*--------------------------------------------------------------------------------------

 dmd_clock_readout.cpp 
   Demo and example project for the Freetronics DMD, a 512 LED matrix display
   panel arranged in a 32 x 16 layout.

This LIibrary (DMD32) and example are  fork of original DMD library  was modified to work on ESP32 
Modified by: Khudhur Alfarhan  // Qudoren@gmail.com
1/Oct./2020

 See http://www.freetronics.com/dmd for resources and a getting started guide.

 Note that the DMD32 library uses the VSPI port for the fastest, low overhead writing to the
 display. Keep an eye on conflicts if there are any other devices running from the same
 SPI port, and that the chip select on those devices is correctly set to be inactive
 when the DMD is being written to.

 USAGE NOTES
 -----------
 - Place the DMD library folder into the "arduino/libraries/" folder of your Arduino installation.
 - Restart the IDE.
 - In the Arduino IDE, go to Tools > Board > and choose any ESP32 board
 - In the Arduino IDE, you can open File > Examples > DMD > dmd_demo, or dmd_clock_readout, and get it
   running straight away!

 * See the documentation on Github or attached images to find the pins that should be connected to the DMD LED display
  

 This example code is in the public domain.
 The DMD32 library is open source (GPL), for more see DMD32.cpp and DMD32.h

--------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------
  Includes
--------------------------------------------------------------------------------------*/

#define DISPLAYS_ACROSS 2
#define DISPLAYS_DOWN 2
#define FRAMERATE 60

#include "DMD32.h"        //
#include "fonts/SystemFont5x7.h"

//Fire up the DMD library as dmd
DMD dmd(DISPLAYS_ACROSS,DISPLAYS_DOWN);

//Timer setup
//create a hardware timer  of ESP32
hw_timer_t * timer = NULL;
volatile bool frameUpdateFlag = false;

uint8_t tag = 0;

/*--------------------------------------------------------------------------------------
  Interrupt handler for Timer1 (TimerOne) driven DMD refresh scanning, this gets
  called at the period set in Timer1.initialize();
--------------------------------------------------------------------------------------*/
void IRAM_ATTR triggerScan()
{
  frameUpdateFlag = true;
}

/*--------------------------------------------------------------------------------------
  setup
  Called by the Arduino architecture before the main loop begins
--------------------------------------------------------------------------------------*/
void setup(void)
{
    Serial.begin(9600);
    //return the clock speed of the CPU
    uint8_t cpuClock = ESP.getCpuFreqMHz();
  
    // Use 1st timer of 4 
    // devide cpu clock speed on its speed value by MHz to get 1us for each signal  of the timer
    timer = timerBegin(0, cpuClock, true);
    // Attach triggerScan function to our timer 
    timerAttachInterrupt(timer, &triggerScan, true);
    // Set alarm to call triggerScan function  
    // Repeat the alarm (third parameter) 
    timerAlarmWrite(timer, ((int)500000/FRAMERATE) , true);
    // Start an alarm 
    timerAlarmEnable(timer);
}

/*--------------------------------------------------------------------------------------
  loop
  Arduino architecture main loop
--------------------------------------------------------------------------------------*/
void loop(void)
{
   if (frameUpdateFlag){

    
      // Remet tout les pixels à 0
     dmd.clearScreen(true);

     // Séelectionne la police d'écriture
     dmd.selectFont(SystemFont5x7);
     // Affiche du texte
     dmd.drawString(1,1,"1", 1, GRAPHICS_NORMAL);
     dmd.drawString(35,1,"2", 1, GRAPHICS_NORMAL);
     dmd.drawString(1,23,"3", 1, GRAPHICS_NORMAL);
     dmd.drawString(35,20,"4", 1, GRAPHICS_NORMAL);
     
     // Affiche le pixel vaquant et le déplace
     dmd.writePixel(tag, 9, GRAPHICS_NORMAL, 1);
     tag = (tag+1) % (32*DISPLAYS_ACROSS);

     // Affiche le cercle
     dmd.drawCircle(30, 12, 10, GRAPHICS_NORMAL);



     // Remet le flag à 0
     frameUpdateFlag = false;
   
   
   }
   // Scan de l'affichage
   dmd.scanDisplayBySPI();
}
