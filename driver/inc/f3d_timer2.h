/* f3d_timer2.h --- 
 * 
 * Filename: f3d_timer2.h
 * Description: 
 * Author: Bryce Himebaugh
 * Maintainer: Ching Yuen Ng/ No Partner
 * Created: Mon Nov 18 14:11:49 2013
 * Last-Updated: 11-30-2013
 *           By: Ching Yuen ng
 *     Update #: 1
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

void f3d_timer2_init(void);
void f3d_timer2_setperiod(uint16_t period);
void f3d_timer2_enable(uint32_t samplerate);
void f3d_timer2_disable(void);

/* f3d_timer2.h ends here */
