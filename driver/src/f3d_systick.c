/* f3d_systick.c --- 
 * 
 * Filename: f3d_systick.c
 * Description: 
 * Author: Bryce Himebaugh
 * Maintainer: Ching Yuen Ng/ No partner
 * Created: Thu Nov 14 07:57:37 2013
 * Last-Updated: 11-21-2013
 *           By: Ching Yuen Ng
 *     Update #: 2
 * Keywords: 
 * Compatibility: 
 * 
 */

/* Commentary: 
 * 
 * 
 * 
 */

/* Change log:
 * 
 * 
 */

/* Copyright (c) 2004-2007 The Trustees of Indiana University and 
 * Indiana University Research and Technology Corporation.  
 * 
 * All rights reserved. 
 * 
 * Additional copyrights may follow 
 */

/* Code: */


#include <f3d_systick.h>
#include <f3d_led.h>
#include <f3d_uart.h>
//#include <stdio.h>
#include <queue.h>

volatile int systick_flag = 0;
int button_state;
int n=0;
int count=0;

queue_t txbuf;


void f3d_systick_init(void) {
  SysTick_Config(SystemCoreClock/100);
  //init for user button
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}



void SysTick_Handler(void) {
  /*  
  button_state = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0);
 
 

  //if the button is being pressed, the leds cycle at 100ms
  if(button_state){
   
      if(count==10){
f3d_led_off(n);
n+=1;
if(n==8){n=0;}
f3d_led_on(n);
count=0;
      }
      count+=1;
  }

  //if the botton is not being pressed, the led cycle at 10 ms
  else if(!button_state){
    f3d_led_off(n);
    n+=1;
    if(n==8){n=0;}
    f3d_led_on(n);
  }
  
  */
   if (!queue_empty(&txbuf)) {
    flush_uart();
  }



  
}



/* f3d_systick.c ends here */
