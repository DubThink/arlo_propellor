/*
  Blank Simple Project.c
  http://learn.parallax.com/propeller-c-tutorials 
*/
#include "simpletools.h"                      // Include simple tools
#include "arlodrive.h"                        // Include arlo drive
#include "fdserial.h"
#include "ping.h"

#define FRONT_LEFT 17
#define FRONT_RIGHT 16
#define BACK_LEFT 15
#define BACK_RIGHT 14

#define BUTTON_A 1
#define BUTTON_B 2
#define BUTTON_C 3
#define BUTTON_D 4

#define FRONT_BUMP 10
#define BACK_BUMP 11

fdserial *term;

int main()                                    // Main function
{
  // Add startup code here.
  term=fdserial_open(31,30,0,9600);
  drive_feedback(0);
  drive_speed(0,0);
  dprint(term,"HELLO WOERL\n");
  char c;
  char* s;
  short x_desired=0;
  short y_desired=0;
  long last_speed_ticks=0;
  long current_ticks=0;
  int ping_thresh=20;
  int ping_front=0;
  int ping_back=0;
  // Drive settings
  int speed=5;
  //drive_feedback(0);
  
  int r_active=1;
  int r_speed=0;
   
  int speedL=0;
  int speedR=0;
  freqout(0, 400, 100);                     // Beep -> program starting
  while(1)
  { 
  
    // Remote Buttons
    int sigA = input(BUTTON_A);
    int sigB = input(BUTTON_B);
    //int sigC = input(BUTTON_C);
    //int sigD = input(BUTTON_D);
    if(sigA)r_active=1;
    if(sigB)r_active=0;
    //if(sigC)r_speed=1;
    //if(sigD)r_speed=0;
    if(r_active)high(26);else low(26);
    if(r_speed)high(27);else low(27);
    // ping detection
    //ping ping_front = ping_thresh>=ping_cm(FRONT_LEFT)||ping_thresh>=ping_cm(FRONT_RIGHT)||ping_thresh>=ping_cm(BACK_LEFT);  
    if(fdserial_rxReady(term)){
      c=fdserial_rxCheck(term);
      // procedure:
      // take in byte
      // if first bit is 1, it's a measurement byte
      // otherwise it's a command byte (in general, a letter corresponding to a command)
      // 0b1xxxyyyy
//      dprint(term,"%d, %d, %d\n",(int)c,(c&0x80)==128,c&0x7f);
      //dprint(term,"recieved %d\n",c);
      if((c&0x80)==128){
        // if first bit is 1
        x_desired=((c>>4)&0x7)-3;
        y_desired=(c&0xf)-4;
        //ping if(ping_front&&x_desired>0)x_desired=0;
        //dprint(term,"x_desired: %d  y_desired:%d",x_desired,y_desired);
        //ping if(ping_front)dprint(term, " HHHHHHHHHH\n");
        //ping else dprint(term, "-------\n");
        last_speed_ticks=current_ticks;
      }else{      
        if(c=='b'){
          freqout(0, 400, 100);                     // Beep -> program starting
          dprint(term,"beeped\n");
        }
      }      
    }
    // Add main loop code here.
    if(current_ticks>last_speed_ticks+10){
      // stop if no speed in the last second
      dprint(term,"autostop\n");
      x_desired=0;
      y_desired=0;
    }
    if(r_active==0){
      x_desired=0;
      y_desired=0;
    }  
    

    //drive_clearTicks();
    speedL=speed*(2*x_desired-y_desired);
    speedR=speed*(2*x_desired+y_desired);
    if(input(FRONT_BUMP)!=0){
      speedL=speedL>0?0:speedL;
      speedR=speedR>0?0:speedR;
    } 
    if(input(BACK_BUMP)!=0){
      speedL=speedL<0?0:speedL;
      speedR=speedR<0?0:speedR;
    } 
    drive_speed(speedL,speedR);
    
    if(current_ticks%10==0){
      dprint(term, "running motors at %d, %d\n",speedL,speedR);
      s = dhb10_com("SPD\r");                // Request hardware version
      dprint(term,"Speed  SPD = %s\n", s);
    }   
    //drive_speed(16*(x_desired-y_desired),16*(x_desired+y_desired)); //spin-2-win
    pause(10); // throttle loop speed
    current_ticks++;
  }  
}
