/* queue.c --- 
 * 
 * Filename: queue.c
 * Description: 
 * Author: Bryce Himebaugh
 * Maintainer: Ching Yuen Ng/ No partner
 * Created: Thu Feb  7 19:49:26 2013 (-0500)
 * Version: 
 * Last-Updated: Ching Yuen Ng
 *           By: 11-21-2013
 *     Update #: 2
 * URL: 
 * Doc URL: 
 * Keywords: 
 * Compatibility: 
 * 
 */

/* Commentary: 
 * 
 * 
 * 
 */

/* Change Log:
 * 
 * 
 */

/* This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301, USA.
 */

/* Code: */

#include "queue.h"



int isFull(queue_t *buf){
  return (((buf->tail+1) % 32) == buf->head);

}

void init_queue(queue_t *buf) {
  buf->head=0;
  buf->tail=0;
}

int enqueue (queue_t *buf, int data) {
  if(isFull(buf)){return 0;}

  else{
    buf->tail+=1;
    
   

    buf->buffer[buf->tail%32]=data;
    
    return 1;
  }
}

int dequeue (queue_t *buf) {
  char d;
  if(queue_empty(buf)){return 0;}

  else{
    buf->head+=1;

    d=buf->buffer[buf->head % 32];

    return d;

  }
  
}

int queue_empty(queue_t *buf) {

  return(buf->head==buf->tail);

  
}


/* queue.c ends here */
