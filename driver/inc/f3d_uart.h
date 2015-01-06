/* f3d_uart.h --- 
 * 
 * Filename: f3d_uart.h
 * Description: 
 * Author: Bryce Himebaugh
 * Maintainer: Ching Yuen Ng, No partner
 * Created: Thu Sep 26 07:05:43 2013
 * Last-Updated: 11/21/2013
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

#include <stm32f30x.h>

void f3d_uart_init(void);
int putchar(int);
int getchar(void);
//void putstring(char *);

/* f3d_uart.h ends here */
