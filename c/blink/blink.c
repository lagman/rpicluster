//============================================================================
// Name         : blink.c
// Author       : Josh Kiepert
// Description  : C WiringPi based routing for blinking an RGB LED
//                Raspberry Pis
//
//  wiringPi pin numbers:
//  pin 0 = (Header pin 11) (Red)
//  pin 1 = (Header pin 12) (Blue)
//  pin 2 = (Header pin 13)
//  pin 3 = (Header pin 15)
//  pin 4 = (Header pin 16)
//  pin 5 = (Header pin 18)
//  pin 6 = (Header pin 22)
//  pin 7 = (Header pin 7)  (Green)
//
// Author       : Joshua Kiepert
// Date         : 2013-03-08
//============================================================================

#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <wiringPi.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>

const int R_PIN = 0;
const int B_PIN = 1;
const int G_PIN = 7;
const int R_MSK = 0x1;
const int B_MSK = 0x2;
const int G_MSK = 0x4;

const int OFF = 1;
const int ON = 0;

static bool keepRunning = true;

void intHandler(int dummy){
  keepRunning = false;  
}

bool timeout(int *countdown){
  bool timed = true;
  
  if(*countdown < 0){
    timed = false;
  }
  
  if(timed){
    if(*countdown == 0){
      keepRunning = false;
    }else{
      printf("\rTimeout in: %5d", *countdown);
      fflush(stdout);
      (*countdown)--;
    }
  }
  
  if(!keepRunning){
    printf("\n");
    fflush(stdout);
    return true;
  }
  
  return false;
}

void led_set(uint8_t mask){
  
  if(mask & R_MSK){
    digitalWrite(R_PIN,ON);
  }else{
    digitalWrite(R_PIN,OFF);
  }
  
  if(mask & B_MSK){
    digitalWrite(B_PIN,ON);
  }else{
    digitalWrite(B_PIN,OFF);
  }
  
  if(mask & G_MSK){
    digitalWrite(G_PIN,ON);
  }else{
    digitalWrite(G_PIN,OFF);
  }
}

void led_toggle(uint8_t mask){
  int state = 0;
  
  if(mask & R_MSK){
    state = digitalRead(R_PIN);
    digitalWrite(R_PIN,state ^ 1);
  }else{
    digitalWrite(R_PIN,OFF);
  }
  
  if(mask & G_MSK){
    state = digitalRead(G_PIN);
    digitalWrite(G_PIN,state ^ 1);
  }else{
    digitalWrite(G_PIN,OFF);
  }
  
  if(mask & B_MSK){
    state = digitalRead(B_PIN);
    digitalWrite(B_PIN,state ^ 1);
  }else{
    digitalWrite(B_PIN,OFF);
  }
}

void blink(int mask, int rate){
  int us = 1000*rate;
  
  if(mask & R_MSK){
    digitalWrite(R_PIN,ON);
    usleep(us);
    digitalWrite(R_PIN,OFF);
    usleep(us);
  }else{
    digitalWrite(R_PIN,OFF);
  }
  
  if(mask & G_MSK){
    digitalWrite(G_PIN,ON);
    usleep(us);
    digitalWrite(G_PIN,OFF);
    usleep(us);
  }else{
    digitalWrite(G_PIN,OFF);
  }
  
  if(mask & B_MSK){
    digitalWrite(B_PIN,ON);
    usleep(us);
    digitalWrite(B_PIN,OFF);
    usleep(us);
  }else{
    digitalWrite(B_PIN,OFF);
  }
  
}

void led_setter(int mask, int ms){
  static int countdown = -1;
  countdown = ms;
  
  led_set(mask);
  
  while(!timeout(&countdown)){   
    usleep(1000);
  }
}

void led_toggler(int mask, int rate, int iterations){
  static int countdown = -1;
  int ms = 1000*rate;
  
  countdown = iterations;  
  
  while(!timeout(&countdown)){
    led_toggle(mask);
    usleep(ms);
  }
}

void led_blinker(int mask, int rate, int iterations){
  static int countdown = -1;
  countdown = iterations;
  
  while(!timeout(&countdown)){
    blink(mask, rate);
  }
}

void run(int mode, int mask, int rate, int iterations){
  
  switch(mode){
    case 1:
      printf("Mode: %d (constant), mask: %d, iterations(ms): %d\n", mode, mask, iterations);
      fflush(stdout); 
      led_setter(mask, iterations);
      break;
    case 2:
      printf("Mode: %d (toggle), rate: %d, mask: %d, iterations: %d\n", mode, rate, mask, iterations);
      fflush(stdout); 
      led_toggler(mask, rate, iterations);
    default:
      printf("Mode: %d (blink), rate: %d, mask: %d, iterations: %d\n", mode, rate, mask, iterations);
      fflush(stdout); 
      led_blinker(mask, rate, iterations);
      break;
  }
  
  digitalWrite(R_PIN,OFF);
  digitalWrite(G_PIN,OFF);
  digitalWrite(B_PIN,OFF);
  
  return;
}

void usage(char *progname){
  fprintf(stdout, "Usage: %s [mode] [mask] [iterations] [blinkrate]\n" , progname);
}

int main(int argc, char **argv)
{
  static int blinkrate = 50;
  static int mode = 3;
  static int mask = 0x7;
  static int iterations = -1;
  int valid = 0;
  
  if (argc > 5) {
    usage(argv[0]);
    exit(1);
  }
  
  if(argc >= 2){
    valid = sscanf(argv[1],"%d",&mode);
    if(!valid){
      usage(argv[0]);
      exit(1);
    }
  }
  
  if(argc >= 3){
    valid = sscanf(argv[2],"%d",&mask);
     if(!valid){
      usage(argv[0]);
      exit(1);
    }
  }
  
  if(argc >= 4){
    valid = sscanf(argv[3],"%d",&iterations);
    if(!valid){
      usage(argv[0]);
      exit(1);
    }
  }
  
  if(argc == 5){
    valid = sscanf(argv[4],"%d",&blinkrate);
    if(!valid){
      usage(argv[0]);
      exit(1);
    }
  }
    
  if (wiringPiSetup() == -1){
    printf("Error opening GPIO!\n");
    fflush(stdout);
    exit (1);
  }
  
  pinMode(R_PIN, OUTPUT);
  digitalWrite(R_PIN,OFF);
  pinMode(G_PIN, OUTPUT);
  digitalWrite(R_PIN,OFF);
  pinMode(B_PIN, OUTPUT);
  digitalWrite(R_PIN,OFF);
  
  signal(SIGINT, intHandler);
  
  run(mode, mask, blinkrate, iterations);
  
  exit(0);
}
