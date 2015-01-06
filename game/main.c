/* main.c --- 
 * 
 * Filename: main.c
 * Description: 
 * Author: 
 * Maintainer: Ching Yuen Ng
 * Created: Nov 10 11:23:43 2013
 * Last-Updated: 11-30-2013
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
/* Code: */

#include <stm32f30x.h>  // Pull in include files for F30x standard drivers 
#include <f3d_led.h>     // Pull in include file for the local drivers
#include <f3d_uart.h>
#include <queue.h>
#include <f3d_gyro.h>
#include <f3d_lcd_sd.h>
#include <f3d_i2c.h>
#include <f3d_accel.h>
#include <f3d_mag.h>
#include <f3d_nunchuk.h>
#include <f3d_rtc.h>
#include <f3d_systick.h>
#include <f3d_timer2.h>
#include <f3d_dac.h>
#include <ff.h>
#include <diskio.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


///////////////////////////////bmp file structures/////////////////////////////
struct bmpfile_magic {
  unsigned char magic [2];
};

struct bmpfile_header {
  uint32_t filesz ;
  uint16_t creator1 ;
  uint16_t creator2 ;
  uint32_t bmp_offset ;
};

typedef struct {
  uint32_t header_sz ;
  int32_t width ;
  int32_t height ;
  uint16_t nplanes ;
  uint16_t bitspp ;
  uint32_t compress_type ;
  uint32_t bmp_bytesz ;
  int32_t hres;
  int32_t vres;
  uint32_t ncolors ;
  uint32_t nimpcolors ;
} body ;

typedef struct{ // little endian byte order
  uint8_t b;
  uint8_t g;
  uint8_t r;
} pixel;
//////////////////////////////////////////////////////////////////////////////////

FATFS Fatfs;		
FIL Fil;


struct bmpfile_magic magic;
struct bmpfile_header header ;
body info;
uint16_t rgb16=0;

pixel rgb24;
char frameb[]="frameb.bmp";
char zom[]="zombie.bmp";
char blood[]="blood.bmp";
//char hp[]="heart.bmp";
char boss[]="tyy.bmp";
//char licker[]="lickerr.bmp";

int score=0;
int tmpscore=0;

char num[]={'0','1','2','3','4','5','6','7','8','9'};

uint16_t pic[1000];
int p=0;


void die (FRESULT rc) {
  printf("Failed with rc=%u.\n", rc);
  while (1);
}


/////////////////print_bmp function////////////////////////////////////////////

void print_bmp(char name[],int ox,int oy){
  int tmp=ox;
 
  int count_w=0;
  char footer[20];
  int count=0;
  int i=0;

  FRESULT rc;			/* Result code */
  DIR dir;			/* Directory object */
  FILINFO fno;			/* File information object */
  UINT bw, br;
  unsigned int retval;
 	
  f_mount(0, &Fatfs);

  //printf("\nOpen an existing file.\n");
  rc = f_open(&Fil, name, FA_READ);
  if (rc) die(rc);
  
  //printf("\nOpened\n");
  
  
  //printf("\nprocessing to LCD\n");
  
  //reading the trush at the beginning
  rc = f_read(&Fil, &magic, sizeof magic, &br);   
    
  //reading the header
  rc = f_read(&Fil, &header, sizeof header, &br);   
    
  //reading the bmp body
  rc = f_read(&Fil, &info, sizeof info, &br);
 
  BYTE Buff_p[info.width*3];

  for (;;) {
    
    //reading rgb
    
    //reading 1 line each time
    rc = f_read(&Fil, Buff_p, sizeof Buff_p, &br);
    
    if (rc || !br){
      break; 
    }

    for (i = 0; i < br; i+=3){		       
    
      //each element in Buff is a BYTE, total of rgb is 3 BYTE 
      //so each time we are pulling 3 elements from the buff and convert them into one single 16bit color
      rgb24.b=Buff_p[i];
      rgb24.g=Buff_p[i+1];
      rgb24.r=Buff_p[i+2];

      rgb24.r&=0xF8; //getting the most singafigant 5 bits
      rgb24.g&=0xFC; //getting the most singafigant 6 bits
      rgb24.b&=0xF8; //getting the most singafigant 5 bits
      

      //shifting and or, to pack them into one 16 bit color
      rgb16=0;
      rgb24.r = rgb24.r >> 3;
      rgb16|=rgb24.r;
      rgb16 = rgb16 << 6;
      rgb24.g = rgb24.g >> 2;
      rgb16|= rgb24.g;
      rgb16 = rgb16 << 5;
      rgb24.b = rgb24.b >> 3;
      rgb16 |= rgb24.b;
 
      //print the picture depanding on the orientation
      
      
      pic[p]=rgb16;
      p+=1;
      if(p==info.width){
	print_line(ox,oy,pic,info.width);
	oy-=1;
	p=0;
      }
      
      /*
      f3d_lcd_drawPixel(ox,oy,rgb16);
      ox+=1;
      count_w+=1;
      if(count_w==info.width){ox=tmp; oy-=1;count_w=0;}
      */
    }
  }
  
  if (rc) die(rc);
  
  //printf("finished\n");

}

//draw circle funtion 
void draw_circle(int radius, int ox, int oy, uint16_t color){
  int x,y;
  for (x=-radius; x<radius; x++){
    int height=(int)sqrt(radius*radius-x*x);
    for(y=-height;y<height;y++){
      f3d_lcd_drawPixel(x+ox,y+oy,color);
    }
  }
}



///////////////////////////////////sound///////////////////////////////
#define TIMER 20000
#define AUDIOBUFSIZE 128

extern uint8_t Audiobuf[AUDIOBUFSIZE];
extern int audioplayerHalf;
extern int audioplayerWhole;

int order[]={1,0,0};
int k=0;

char shot[]="shot.wav";
char cocking[]="cocking.wav";
char mob[]="mob.wav";
char re[]="re.wav";




//////////////////////sound////////////////////////////
FATFS Fatfs;		
FIL fid;		
BYTE Buff_sound[512];	       
int ret;

struct ckhd {
  uint32_t ckID;
  uint32_t cksize;
};

struct fmtck {
  uint16_t wFormatTag;      
  uint16_t nChannels;
  uint32_t nSamplesPerSec;
  uint32_t nAvgBytesPerSec;
  uint16_t nBlockAlign;
  uint16_t wBitsPerSample;
};

void readckhd(FIL *fid, struct ckhd *hd, uint32_t ckID) {
  f_read(fid, hd, sizeof(struct ckhd), &ret);
  if (ret != sizeof(struct ckhd))
    exit(-1);
  if (ckID && (ckID != hd->ckID))
    exit(-1);
}


static int sound_state =1;



int play(char name[]){

  FRESULT rc;		
  DIR dir;			
  FILINFO fno;			
  UINT bw, br;
  unsigned int retval;
  int bytesread;


  
 

  
  f_mount(0, &Fatfs);
  //printf("\nOpen wav file\n");
  rc = f_open(&fid, name , FA_READ);

 

  if (!rc) {
    struct ckhd hd;
    uint32_t  waveid;
    struct fmtck fck;

   
       printf("%d\n",sound_state);
      readckhd(&fid, &hd, 'FFIR');
      f_read(&fid, &waveid, sizeof(waveid), &ret);
      if ((ret != sizeof(waveid)) || (waveid != 'EVAW'))
	return -1;
      
      readckhd(&fid, &hd, ' tmf');
      
      f_read(&fid, &fck, sizeof(fck), &ret);
      
      // skip over extra info
      
      if (hd.cksize != 16) {
	//printf("extra header info %d\n", hd.cksize - 16);
	f_lseek(&fid, hd.cksize - 16);
      }
      
      while(1){
	readckhd(&fid, &hd, 0);
	if (hd.ckID == 'atad')
	  break;
	f_lseek(&fid, hd.cksize);
      }
      
      f_read(&fid, Audiobuf, AUDIOBUFSIZE, &ret);
      hd.cksize -= ret;
      audioplayerStart();

     

   
      
      while (hd.cksize) {
	int next = hd.cksize > AUDIOBUFSIZE/2 ? AUDIOBUFSIZE/2 : hd.cksize;
	
 
     
	if (audioplayerHalf) {
	  if (next < AUDIOBUFSIZE/2)
	    bzero(Audiobuf, AUDIOBUFSIZE/2);
	  f_read(&fid, Audiobuf, next, &ret);
	  hd.cksize -= ret;
	  audioplayerHalf = 0;
	}
	
	if (audioplayerWhole) {
	  if (next < AUDIOBUFSIZE/2)
	    bzero(&Audiobuf[AUDIOBUFSIZE/2], AUDIOBUFSIZE/2);
	  f_read(&fid, &Audiobuf[AUDIOBUFSIZE/2], next, &ret);
	  hd.cksize -= ret;
	  audioplayerWhole = 0;
	}
	
	



      }

      audioplayerStop();

   
  }
  

 
  //printf("\nClose the file.\n"); 
  rc = f_close(&fid);
  
  if (rc) die(rc);
  


  
}
  




///////////////////////////////////sound ends here/////////////////////////////////////





/*
void change(int i){
  if(i==0){
    
    draw_circle(5,10,15,GREEN);
    play(thermo);
    draw_circle(5,10,15,WHITE);
  }

  if(i==1){
    draw_circle(5,10,35,GREEN);
    play(mkk);
    draw_circle(5,10,35,WHITE);
  }

  if(i==2){
    draw_circle(5,10,55,GREEN);
    play(joyy);
    draw_circle(5,10,55,WHITE);
  }
}


*/
/////////////////////////////////////////////////////////////////////////////////



int zom_timer=0;
int a=0;
int i=0;
int speed=2;

int life_timer=0;
int l=7;

int bu=8;

int oxa[1000];
int oya[1000];
int state[1000];
int tmpox;
int tmpoy;
int tmpstate;
int startx=64;
int starty=80;
nunchuk_t nunchuk_data;


void gun(nunchuk_t nunchuk_data,int *startx, int *starty,int speed){

    if(nunchuk_data.jx==125&&nunchuk_data.jy==123){
      draw_circle(3,*startx,*starty,RED);
    }

    if(nunchuk_data.jy>126&&nunchuk_data.jx==125){//up
      draw_circle(3,*startx,*starty,WHITE);
      *starty-=speed;
      if(*startx<=10){*startx=10;}
      if(*startx>=117){*startx=117;}
      if(*starty<=30){*starty=30;}
      if(*starty>=149){*starty=149;}
      draw_circle(3,*startx,*starty,RED);
    }
    if(nunchuk_data.jy<120&&nunchuk_data.jx==125){//down
      draw_circle(3,*startx,*starty,WHITE);
      *starty+=speed;
      if(*startx<=10){*startx=10;}
      if(*startx>=117){*startx=117;}
      if(*starty<=30){*starty=30;}
      if(*starty>=149){*starty=149;}
      draw_circle(3,*startx,*starty,RED);
    }
    if(nunchuk_data.jx<122&&nunchuk_data.jy>=123){//left
      draw_circle(3,*startx,*starty,WHITE);
      *startx-=speed;
      if(*startx<=10){*startx=10;}
      if(*startx>=117){*startx=117;}
      if(*starty<=30){*starty=30;}
      if(*starty>=149){*starty=149;}
      draw_circle(3,*startx,*starty,RED);
    }
    if(nunchuk_data.jx>128&&nunchuk_data.jy>=123){//right
      draw_circle(3,*startx,*starty,WHITE);
      *startx+=speed;
      if(*startx<=10){*startx=10;}
      if(*startx>=117){*startx=117;}
      if(*starty<=30){*starty=30;}
      if(*starty>=149){*starty=149;}
      draw_circle(3,*startx,*starty,RED);
    }
    if(nunchuk_data.jx>128&&nunchuk_data.jy>126){//up right
      draw_circle(3,*startx,*starty,WHITE);
      *startx+=speed;*starty-=speed;
      if(*startx<=10){*startx=10;}
      if(*startx>=117){*startx=117;}
      if(*starty<=30){*starty=30;}
      if(*starty>=149){*starty=149;}
      draw_circle(3,*startx,*starty,RED);
    }
     if(nunchuk_data.jx>128&&nunchuk_data.jy<120){//down right
       draw_circle(3,*startx,*starty,WHITE);
      *startx+=2;*starty+=2;
      if(*startx<=10){*startx=10;}
      if(*startx>=117){*startx=117;}
      if(*starty<=30){*starty=30;}
      if(*starty>=149){*starty=149;}
      draw_circle(3,*startx,*starty,RED);
    }
     if(nunchuk_data.jx<122&&nunchuk_data.jy>126){//up left
       draw_circle(3,*startx,*starty,WHITE);
       *startx-=speed;*starty-=speed;
       if(*startx<=10){*startx=10;}
       if(*startx>=117){*startx=117;}
       if(*starty<=30){*starty=30;}
       if(*starty>=149){*starty=149;}
       draw_circle(3,*startx,*starty,RED);
     }
     if(nunchuk_data.jx<122&&nunchuk_data.jy<120){//down left
       draw_circle(3,*startx,*starty,WHITE);
       *startx-=speed;*starty+=speed;
       if(*startx<=10){*startx=10;}
       if(*startx>=117){*startx=117;}
       if(*starty<=30){*starty=30;}
       if(*starty>=149){*starty=149;}
       draw_circle(3,*startx,*starty,RED);
     }
}

void clear(int ox, int oy,int w,int h){
  int tmpx=ox;
  int tmpy=oy;
  while(oy>tmpy-h){
    while(ox<w+tmpx){
      f3d_lcd_drawPixel(ox,oy,WHITE);
      ox+=1;
    }
    oy-=1;
    ox=tmpx;
  }
}

int match(int startx,int starty,int w,int h){
  int k=sizeof oxa/sizeof oxa[0];
  int i;
  for(i=0;i<k ;i++){
      if(startx>oxa[i] && startx<oxa[i]+w && starty>oya[i]-h && starty<oya[i]){
	tmpox=oxa[i];
	tmpoy=oya[i];
	tmpstate=i;
	if(state[i]){
	  print_bmp(zom,oxa[i],oya[i]);
	  return 1;
	}
      }
  }
  return 0;
}

void kill(nunchuk_t data,int startx, int starty, int ox, int oy,int w, int h, int *a,int *zom_timer,int *i,int *speed,int *bu){
  if(*zom_timer==50){
    *speed=2;
    print_bmp(zom,ox,oy);
    oxa[*i]=ox;
    oya[*i]=oy;
    *i+=1;
    *zom_timer=0;
    *a+=1;
    state[*i]=1;
  }
  if(data.z!=1&&data.c==1&&*bu>0&&!match(startx,starty,w,h)){
    *bu-=1;
    f3d_led_off(*bu);
    play(shot);
  }
  if(match(startx,starty,w,h)){
    *speed=6;
    if(a!=0){
      if(data.z!=1&&data.c==1&&*bu>0){
	*bu-=1;
	f3d_led_off(*bu);
	print_bmp(blood,tmpox,tmpoy);
	play(shot);
	//delay(30);
	clear(tmpox,tmpoy,w,h);
	state[tmpstate]=0;
	*a-=1;
	score+=30;
      }
      if(data.z!=1&&data.c==1&&*bu<=0){
	play(re);
      }
    }
    *zom_timer+=1;
  } 
  else {
    *speed=2;
    *zom_timer+=1;
  }
}


int hh=3;
int hhx=37;

void life(int *a,int *life_timer,int *hh, int *hhx){
  if(*a>=3 && *life_timer>=200){
    //f3d_led_off(l);
    *life_timer=0;
    *hh-=1;
    clear(*hhx,22,8,7);
    *hhx-=7;
  }
  if(*a>=3){
    *life_timer+=1;
  }
  if(*hh==0){
   f3d_lcd_fillScreen(BLACK);
   f3d_lcd_drawString(40,50,"YOU LOSE",WHITE,BLACK);
  }
}




void reload(nunchuk_t data,int *bu){
  if(data.c==1&data.z==1){
    *bu=8;
    f3d_led_all_on();
    play(cocking);
  }
}




void points(int score, char num[]){
  int tmp1=score%1000;
  int tmp2=score%100;
  int k= (score-tmp1)/1000;
  clear(90,23,5,8);
  f3d_lcd_drawChar(90,15,num[k],BLACK,WHITE);
  int i= (score-tmp2-k*1000)/100;
  clear(96,23,5,8);
  f3d_lcd_drawChar(96,15,num[i],BLACK,WHITE);
  int j= (score-k*1000-i*100)/10;
  clear(102,23,5,8);
  f3d_lcd_drawChar(102,15,num[j],BLACK,WHITE);
}


uint16_t bar[100];
uint16_t bar_2[20];
uint16_t bar_3[20];

void set_color(uint16_t c[]){
  int i;
  for(i=0;i<40;i++){
    c[i]=BLUE;
  }
  for(i=41;i<60;i++){
    c[i]=GREEN;
  }
  for(i=61;i<100;i++){
    c[i]=BLUE;
  }
}

void set_sm_color(uint16_t c[],uint16_t color){
  int i;
  for(i=0;i<20;i++){
    c[i]=color;
  }
}


void bos_bar(uint16_t bar[]){
  int l;
  for(l=145;l>140;l--){
    print_line(15,l,bar,100);
  }
}


int boss_pointerx=23;
int tmpboss_pointerx;
int boss_di=1;
int boss_life=5;

void boss_move_right(int *boss_life,int *boss_pointerx,int *boss_di,nunchuk_t data,int *hh,int *hhx){
  int l;
  for(*boss_pointerx=23;*boss_pointerx<57;*boss_pointerx+=1){
    draw_circle(3,*boss_pointerx,144,RED);
    f3d_nunchuk_read(&data);
    if(data.c==1){
      play(shot);
      *hh-=1;
      clear(*hhx,22,8,7);
      *hhx-=7;
    }
    delay(*boss_di);
    draw_circle(3,*boss_pointerx-1,144,BLUE);
  }
  for(l=146;l>140;l--){
    print_line(58,l,bar_3,20);
  }
  for(*boss_pointerx=60;*boss_pointerx<74;*boss_pointerx+=1){//78
    draw_circle(3,*boss_pointerx,144,RED);
    f3d_nunchuk_read(&data);
    if(data.c==1){
      play(shot);
      *boss_life-=1;
    }
    delay(*boss_di);
    draw_circle(3,*boss_pointerx-1,144,YELLOW);
  }
  for(l=146;l>140;l--){
    print_line(58,l,bar_2,20);
  }
  for(*boss_pointerx=81;*boss_pointerx<110;*boss_pointerx+=1){
    draw_circle(3,*boss_pointerx,144,RED);
    f3d_nunchuk_read(&data);
    if(data.c==1){
      play(shot);
      *hh-=1;
      clear(*hhx,22,8,7);
      *hhx-=7;
    }
    delay(*boss_di);
    draw_circle(3,*boss_pointerx-1,144,BLUE);
  }
}





void boss_move_left(int *boss_life,int *boss_pointerx,int *boss_di,nunchuk_t data,int *hh,int *hhx){
  int l;
  for(*boss_pointerx=110;*boss_pointerx>74;*boss_pointerx-=1){
    draw_circle(3,*boss_pointerx,144,RED);
    f3d_nunchuk_read(&data);
    if(data.c==1){
      play(shot);
      *hh-=1;
      clear(*hhx,22,8,7);
      *hhx-=7;
    }
    delay(*boss_di);
    draw_circle(3,*boss_pointerx+1,144,BLUE);
  }
  for(l=146;l>140;l--){
    print_line(58,l,bar_3,20);
  }
  for(*boss_pointerx=74;*boss_pointerx>60;*boss_pointerx-=1){
    draw_circle(3,*boss_pointerx,144,RED);
    f3d_nunchuk_read(&data);
    if(data.c==1){
      play(shot);
      *boss_life-=1;
    }
    delay(*boss_di);
    draw_circle(3,*boss_pointerx+1,144,YELLOW);
  }
  for(l=146;l>140;l--){
    print_line(58,l,bar_2,20);
  }
  for(*boss_pointerx=53;*boss_pointerx>23;*boss_pointerx-=1){
    draw_circle(3,*boss_pointerx,144,RED);
    f3d_nunchuk_read(&data);
    if(data.c==1){
      play(shot);
      *hh-=1;
      clear(*hhx,22,8,7);
      *hhx-=7;
    }
    delay(*boss_di);
    draw_circle(3,*boss_pointerx+1,144,BLUE);
  }
}




//void boss_fi(int *boss_life, int *boss_pointerx){
// boss_move_right(boss_life,boss_pointerx,&boss_di,nunchuk_data,hh);
// boss_move_left(boss_life,boss_pointerx,&boss_di,nunchuk_data,hh);
  
//}


int state_m=0;

int main(void) { 
  
  setvbuf(stdin, NULL, _IONBF, 0);
  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stderr, NULL, _IONBF, 0);

  f3d_led_init();
  delay(10);
  f3d_uart_init();
  delay(10);
  f3d_timer2_init();
  delay(10);
  f3d_dac_init();
  delay(10);
  f3d_delay_init();
  delay(10);
  f3d_rtc_init();
  delay(10);
  f3d_i2c1_init();
  delay(10);
  f3d_systick_init();
  delay(10);
  f3d_lcd_init();	
  delay(10);
  f3d_nunchuk_init();
 
 
  set_color(bar);

  set_sm_color(bar_2,GREEN);

  set_sm_color(bar_3,YELLOW);

  state[0]=1;

  f3d_led_all_on();


  print_bmp(frameb,0,159);
  
  draw_circle(3,64,80,RED);

  f3d_lcd_drawString(55,15,"SCORE",BLACK,WHITE);
  
  f3d_lcd_drawChar(90,15,num[0],BLACK,WHITE);
  f3d_lcd_drawChar(96,15,num[0],BLACK,WHITE);
  f3d_lcd_drawChar(102,15,num[0],BLACK,WHITE);
  f3d_lcd_drawChar(108,15,num[0],BLACK,WHITE);

  uint16_t health[20];

  set_sm_color(health,RED);

  for(l=22;l>15;l--){
    print_line(25,l,health,20);
  }

  while(1){

    life(&a,&life_timer,&hh,&hhx);

    if(hh==0){break;}
     

    if(a>=3){life_timer+=1;}
    
    if(score!=tmpscore){points(score,num); tmpscore=score;}
    
   
    reload(nunchuk_data,&bu);

    kill(nunchuk_data,startx,starty,rand()%80+20,rand()%75+70,20,30,&a,&zom_timer,&i,&speed,&bu);

    //read nunchuk
    f3d_nunchuk_read(&nunchuk_data);

    //printf("%d %d %d\n",nunchuk_data.jx,nunchuk_data.jy,nunchuk_data.c);
 
    gun(nunchuk_data,&startx,&starty,speed);

    if(score>220){break;}
  }


  print_bmp(boss,15,130);
  play(mob);
  bos_bar(bar);

  while(1){
    
    

    f3d_nunchuk_read(&nunchuk_data);

    f3d_led_all_on();

    while(boss_life>0){
      if(hh<=0)
	break;
      boss_move_right(&boss_life,&boss_pointerx,&boss_di,nunchuk_data,&hh,&hhx);
      boss_move_left(&boss_life,&boss_pointerx,&boss_di,nunchuk_data,&hh,&hhx);
      
    }


    break;
  }

  if(hh==0){
    f3d_lcd_fillScreen(BLACK);
    f3d_lcd_drawString(40,50,"YOU LOSE",WHITE,BLACK);
  }

  else{
    f3d_lcd_fillScreen(BLACK);
    f3d_lcd_drawString(40,50,"YOU WIN",WHITE,BLACK);
  }
     
    

  
}



#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line) {
  /* Infinite loop */
  /* Use GDB to find out why we're here */
  while (1);
}
#endif


/* main.c ends here */
