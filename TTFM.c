/*****************************************************************************
 
 *   TTFM.c: Display and Multitasking Block
 
******************************************************************************/
#include<stdio.h>
#include"LPC210X.h"
#include"Times_New_Roman31h.c"



typedef unsigned char INT8U ;
typedef unsigned short  INT16U ;
typedef unsigned int  INT32U ;
typedef signed int  INT32S ;
typedef signed char INT8S ;
typedef signed short  INT16S ;

#include"SPI.c"



#define  OSCILLATOR_CLOCK_FREQUENCY 14745600 

#define NOP 0x00
#define SOFT_RESET 0x01
#define SET_PLL 0xE0
#define SET_PIXEL_DATA_INTERFACE 0xF0
#define SET_LCD_MODE_ 0xB0
#define SET_PLL_MN 0xE2
#define SET_LSHIFT_FREQ 0xE6
#define SET_HORI_PERIOD 0xB4
#define SET_VERT_PERIOD 0xB6
#define SET_COLUMN_ADDRESS 0x2A
#define SET_PAGE_ADDRESS 0x2B
#define SET_DISPLAY_ON 0x29
#define WRITE_MEMORY_START 0x2C
#define RESERVED 0x3A

#define CS_HIGH         GPIO1_IOSET=0x00100000
#define CS_LOW          GPIO1_IOCLR=0x00100000
#define D               GPIO1_IOSET=0x00800000
#define C               GPIO1_IOCLR=0x00800000
#define WR_HIGH         GPIO1_IOSET=0x00400000
#define WR_LOW          GPIO1_IOCLR=0x00400000
#define RD_HIGH         GPIO1_IOSET=0x00080000
#define RD_LOW          GPIO1_IOCLR=0x00080000
#define RESET_HIGH      GPIO1_IOSET=0x00040000
#define RESET_LOW       GPIO1_IOCLR=0x00040000

#define CHSEL_HIGH       GPIO0_IOSET=0x00000800
#define CHSEL_LOW        GPIO0_IOCLR=0x00000800

#define TDC7200EN_HIGH       GPIO0_IOSET=0x20000000
#define TDC7200EN_LOW        GPIO0_IOCLR=0x20000000
#define TDC1000EN_HIGH       GPIO0_IOSET=0x08000000
#define TDC1000EN_LOW        GPIO0_IOCLR=0x08000000
#define TDC1000_RESET_HIGH   GPIO0_IOSET=0x10000000
#define TDC1000_RESET_LOW    GPIO0_IOCLR=0x10000000

#define Batch_limit1_HIGH         GPIO1_IOSET=0x00100000
#define Batch_limit1_LOW          GPIO1_IOCLR=0x00100000

#define Batch_limit2_HIGH         GPIO1_IOSET=0x00200000
#define Batch_limit2_LOW          GPIO1_IOCLR=0x00200000

/******************************************************************************
** Function name:		UARTInitialize
**
** Descriptions:		Initializes UART 8 bit, 1 stop bit, no parity
**
** parameters:			Baudrate 
** Returned value:		None
** 
******************************************************************************/
UARTInitialize(unsigned int baud)
{
  INT16U divisor ;
  if ( baud == 115200 ) divisor = 32 ;
  else if ( baud == 9600 ) divisor = 384 ;

  UART0_LCR = 0x87 ;                                      /* 8 bit, 1 stop bit, no parity, enable DLAB */
  UART0_DLL = divisor & 0xFF;
  UART0_DLM = (divisor >> 8) & 0xFF;
  UART0_LCR &= ~0x80;                                     /* Disable DLAB */
  PCB_PINSEL0 = PCB_PINSEL0 & ~0xF | 0x5;
  UART0_FCR=0x87;
}

/******************************************************************************
** Function name:		UARTReadChar
**
** Descriptions:		Waits for a character to be received on UART 
**
** parameters:			None 
** Returned value:		Character read from UART
** 
******************************************************************************/
static unsigned char UARTReadChar(void)
{
  while ((UART0_LSR & 0x01) == 0);
  return UART0_RBR;
}
/******************************************************************************
** Function name:		Dly_1ms
**
** Descriptions:		Gives a delay of 1 ms with 14.7456 MHz crystal and PLL enabled
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void Dly_1ms()
{
int x;
for(x=0;x<180;x++)
{}
}
/******************************************************************************
** Function name:		Delay_ms
**
** Descriptions:	    Provides a software delay of given ms
**
** parameters:			number of ms delay 
** Returned value:		None
** 
******************************************************************************/
void Delay_ms(int time)
{
  int x;
  for(x=0;x<time;x++)
  {Dly_1ms();}
}
/******************************************************************************
** Function name:		Init_Hardware
**
** Descriptions:		Initializes GPIO pins,SPI,I2C ports
**
** parameters:			None 
** Returned value:		None
** 
******************************************************************************/
void Init_Hardware()
{
  PCB_PINSEL2 &= 0xFFFFFFF3;
  PCB_PINSEL0 |= 0x00000050; 
  PCB_PINSEL1 |= 0x000002A8;
  PCB_PINSEL1 &= 0xC03FFFFF;
  PCB_PINSEL1 &= 0xCFFFFFFF;
  PCB_PINSEL1 |= 0x20000000;
  PCB_PINSEL1 &= 0xFFF0FFFF;
  SCB_EXTMODE = 0x08;
  GPIO1_IODIR = 0xFFFFFFFF;
  GPIO0_IODIR = 0xFFFFF9FF;
  GPIO0_IOSET = 0x0000000C;
  RESET_HIGH;
  RESET_LOW;
  Delay_ms(1);
  RESET_HIGH;
  TDC1000_RESET_HIGH;
  Delay_ms(1);
  TDC1000_RESET_LOW;

  CS_HIGH;
  WR_HIGH;
  RD_HIGH;

  CS_TDC7200_HIGH;
  CS_TDC1000_HIGH;

  SPI1_SPCR = 0x0820;
  SPI1_SPCCR = 20;

  TDC7200EN_LOW;
  TDC7200EN_HIGH;
  TDC1000EN_LOW;

  Batch_limit1_LOW;
  Batch_limit2_LOW;
}
/******************************************************************************
** Function name:		Write_Parameter
**
** Descriptions:		Write given parameter to SSD1963 registers
**
** parameters:			Parameter to be written 
** Returned value:		None
** 
******************************************************************************/
void Write_Parameter(INT8U Parameter)
{
  INT32U temp ;
  temp = (((INT32U)Parameter)<<24);
  GPIO1_IOCLR=0xFF000000;
  GPIO1_IOSET=temp;
  D;
  CS_LOW;
  WR_LOW;
  WR_HIGH;
  CS_HIGH;
}
/******************************************************************************
** Function name:		Write_Command
**
** Descriptions:		Write given command to SSD1963 registers
**
** parameters:			Command code to be written
** Returned value:		None
** 
******************************************************************************/
void Write_Command(INT8U Command)
{
  INT32U temp;
  temp = (((INT32U)Command)<<24);
  GPIO1_IOCLR=0xFF000000;
  GPIO1_IOSET=temp;
  C;  
  CS_LOW;
  WR_LOW;
  WR_HIGH;
  CS_HIGH;
}

/*define LCDCMD (volatile INT8U*)0x80000000
#define LCDPAR (volatile INT8U*)0x80000004

void Write_Parameter(INT8U parameter)
{
	LCDPAR=parameter;
}

void Write_Command(INT8U command)
{
	LCDCMD=command;
}*/



INT8U *p1="FLOWRATE";									

INT8U f1_colour[3]={0,0,0};
INT8U b1_colour[3]={0xE0,0xE0,0xE0};
INT8U f2_colour[3]={0xFF,0,0};
INT8U b2_colour[3]={0,0xCC,0};
INT8U f3_colour[3]={0,0,0xCC};
INT8U b3_colour[3]={0xE0,0xE0,0xE0};                 //Colour Array with elements for R,G,B
/******************************************************************************
** Function name:		set_window
**
** Descriptions:		Set up SSD1963 to write following data within a given window
**
** parameters:			left top corner x,left top corner y,figure width
** Returned value:		None
** 
******************************************************************************/
void set_window(INT16U xs,INT16U ys,INT8U width)
{
  if(xs>319)return;
  if(ys>239)return;
  
  Write_Command(SET_COLUMN_ADDRESS);
  Write_Parameter((INT8U)(xs>>8));
  Write_Parameter((INT8U)(xs & 0x00FF));
  if((xs+width-1)>319)
  {
    Write_Parameter(0x01);
    Write_Parameter(0x3f);
  }
  else
  {
    Write_Parameter((INT8U)((xs+width-1)>>8));
    Write_Parameter((INT8U)((xs+width-1) & 0x00FF));
  }
  Write_Command(SET_PAGE_ADDRESS);
  Write_Parameter((INT8U)(ys>>8));
  Write_Parameter((INT8U)(ys & 0x00FF));
  if((ys+30)>239)
  {
    Write_Parameter(0x00);
    Write_Parameter(0xef);
  }
  else
  {
    Write_Parameter((INT8U)((ys+30)>>8));
    Write_Parameter((INT8U)((ys+30) & 0x00FF));
  }

  Write_Command(WRITE_MEMORY_START);
}
/******************************************************************************
** Function name:	    fill_colour
**
** Descriptions:		write one pixel with it's RGB values
**
** parameters:			r,g,b
** Returned value:		None
** 
******************************************************************************/
void fill_colour(INT8U r,INT8U g,INT8U b)
{
  Write_Parameter(r);
  Write_Parameter(g);
  Write_Parameter(b);
}
/******************************************************************************
** Function name:		drawFont
**
** Descriptions:		Render a font in SSD1963 Frame Buffer,the source array contains a bitmap font in 1 bit/pixel form 
**                      0=background colour,1=foreground colour
** parameters:			x,y,width of font,offset in Times Roman,pointer to foreground colour array,
**                      pointer to background colour array
** Returned value:		None
** 
******************************************************************************/
void drawFont(INT16U x, INT16U y, INT8U width,INT16U offset, INT8U *foreg ,INT8U *backg )
{
	INT8U bytelength=(width/8);									//Calculating bytelength
	if(width%8)
	{
		bytelength++;
	}

	set_window(x,y,width);
	INT8U row;
	for(row=0;row<31;row++)
	{
		INT8U column,pixel;
		INT8U limit;
		for(column=0;column<bytelength;column++)
		{
			INT8U fData=__Times_New_Roman31h[offset];
			if(column==(bytelength-1))
			{
				if(width%8==0)
				{
					limit=8;
				}
				else
				{
					limit=(width%8);
				}
			}
			else
			{
				limit=8;
			}
			INT8U i;
			for(i=0;i<limit;i++)								
			{
                		pixel= fData & 0x01;							//Selecting bit individually of byte                               
                          if(pixel==1)fill_colour(foreg[0],foreg[1],foreg[2]);
                          else fill_colour(backg[0],backg[1],backg[2]);                                 
                                fData>>=1;
				x++;									////Incrementing pixel column ADDRESS
	 		}
			offset++;
		}
		y++ ;											//Incrementing pixel row ADDRESS
	}	
}
/******************************************************************************
** Function name:	    drawString
**
** Descriptions:		Render fonts of null terminated array in Frame buffer
**
** parameters:			starting x,starting y,pointer to foreground colour array,pointer to background colour array,
**                      pointer to null terminated string
** Returned value:		None
** 
******************************************************************************/
void drawString(INT16U x1,INT16U y1,INT8U *f,INT8U *b ,INT8U *p)						
{
	INT8U width , hoffset , loffset ;
	INT16U offset ;
	while(*p)											//Selecting one letter of string
	{
		width=__Times_New_Roman31h[8+((*p)-32)*4];						//Calculating offset 
		hoffset=__Times_New_Roman31h[10+((*p)-32)*4];
		loffset=__Times_New_Roman31h[9+((*p)-32)*4];

		offset=((INT16U)(hoffset)<<8)+(loffset);

		drawFont(x1,y1,width,offset,f,b);
		p++;x1+=width;										//Incrementing letter of string
	}
}
/******************************************************************************
** Function name:	    InitDisplay
**
** Descriptions:		Initializes SSD1963 for the given graphics panel
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void InitDisplay (void)
{
  	Write_Command(SOFT_RESET);
  	Delay_ms(5);
  	Write_Command(SOFT_RESET); 
  	Delay_ms(5);
  	Write_Command(SOFT_RESET);
  	Delay_ms(5);

  	Write_Command(SET_PLL);
  	Delay_ms(5);
  	Write_Parameter(0x01);

  	Write_Command(SET_PLL); 
  	Delay_ms(5);
  	Write_Parameter(0x03);

  	Write_Command(SET_LCD_MODE_); 
  	Delay_ms(5);
  	Write_Parameter(0x0C);
  	Write_Parameter(0x80);
  	Write_Parameter(0x01);
  	Write_Parameter(0x3f);
  	Write_Parameter(0x00);
  	Write_Parameter(0xef);
  	Write_Parameter(0x00);
 
  	Delay_ms(5);

  	Write_Command(SET_PIXEL_DATA_INTERFACE); 
  	Delay_ms(5);
  	Write_Parameter(0x00);
  	Delay_ms(5);

  	Write_Command(RESERVED); 
  	Delay_ms(5);
  	Write_Parameter(0x20);
  	Delay_ms(5);

  	Write_Command(SET_PLL_MN); 
  	Delay_ms(5);
  	Write_Parameter(0x22);            // N[7:0] : Multiplier (N) of PLL. (POR = 00101101) b00100011
  	Write_Parameter(0x03);             // M[3:0] : Divider (M) of PLL. (POR = 0011)
  	Write_Parameter(0x04);             // C[2] : Effectuate MN value (POR = 100) - Effectuate the multiplier and divider value
  
  	Delay_ms(5);

  	Write_Command(SET_LSHIFT_FREQ); 
  	Delay_ms(5);
  	Write_Parameter(0x00);
  	Write_Parameter(0xe7);
  	Write_Parameter(0x4f);

  	Delay_ms(5);

  	Write_Command(SET_HORI_PERIOD); 
  	Delay_ms(5);
  	Write_Parameter(0x01);
  	Write_Parameter(0xb8);
  	Write_Parameter(0x00);
  	Write_Parameter(0x44);
  	Write_Parameter(0x0f);
  	Write_Parameter(0x00);
  	Write_Parameter(0x00);
  	Write_Parameter(0x00);

  	Delay_ms(5);

  	Write_Command(SET_VERT_PERIOD); 
  	Delay_ms(5);
  	Write_Parameter(0x01);
  	Write_Parameter(0x08);
  	Write_Parameter(0x00);
  	Write_Parameter(0x13);
  	Write_Parameter(0x07);
  	Write_Parameter(0x00);
  	Write_Parameter(0x00);
  
  	Delay_ms(5);

  	Write_Command(SET_DISPLAY_ON); 
}
/******************************************************************************
** Function name:	    Fill_Rectangle
**
** Descriptions:		Fill rectangle with given colour
**
** parameters:			left top x,left top y,right bottom x,right bottom y,r,g,b
** Returned value:		None
** 
******************************************************************************/
void Fill_Retangle(INT16U x1,INT16U y1,INT16U x2,INT16U y2,INT8U r,INT8U g,INT8U b ){
  
  Write_Command(SET_COLUMN_ADDRESS);
  Write_Parameter((INT8U)(x1>>8));
  Write_Parameter((INT8U)(x1 & 0x00FF));
  Write_Parameter((INT8U)(x2>>8));
  Write_Parameter((INT8U)(x2 & 0x00FF));
  Write_Command(SET_PAGE_ADDRESS);
  Write_Parameter((INT8U)(y1>>8));
  Write_Parameter((INT8U)(y1 & 0x00FF));
  Write_Parameter((INT8U)(y2>>8));
  Write_Parameter((INT8U)(y2 & 0x00FF));

  Write_Command(WRITE_MEMORY_START);

  INT32U fill_flag=(INT32U)(x2-x1+1)*(INT32U)(y2-y1+1);
  INT32U count;
  for(count=0;count<fill_flag;count++)
  {
    Write_Parameter(r);
    Write_Parameter(g);
    Write_Parameter(b);
  }
}
/******************************************************************************
** Function name:	    Init_Main_Screen
**
** Descriptions:		Initializes main screen with default colour
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void Init_Main_Screen(void) 
{
  Fill_Retangle(0,0,319,239,0xE0,0xE0,0xE0);

  drawString(75,0,f3_colour,b3_colour,"FLOWRATE");
  drawString(35,32,f1_colour,b1_colour,"VEL");
  drawString(90,64,f1_colour,b1_colour,"m/s");
  drawString(200,32,f1_colour,b1_colour,"VOL");
  drawString(265,64,f1_colour,b1_colour,"lit/s");

  drawString(75,96,f1_colour,b1_colour,"TOTALIZER");
  drawString(185,128,f1_colour,b1_colour,"L");
  drawString(95,160,f1_colour,b1_colour,"BATCH");
  drawString(185,192,f1_colour,b1_colour,"lit");
}

INT8U value[9];
/******************************************************************************
** Function name:	    dispn
**
** Descriptions:		puts decimal point at given position in a number string 
**
** parameters:			number string,position of decimal point,length of number string
** Returned value:		None
** 
******************************************************************************/
char* dispn(INT32U n,INT8U dp_pos,INT8U length)
{
  value[8]='\0';
  INT8S i;
  for(i=0;i<dp_pos;i++)
  {
    value[7-i]=(n%10)+'0';
    n=n/10;
  }
  value[7-dp_pos]='.';
  for(i=6-dp_pos;i>=0;i--)
  {
    value[i]=(n%10)+'0';
    n=n/10;
  }
  return value+8-length;
}
/******************************************************************************
** Function name:	    TDC7200_reg_init
**
** Descriptions:		Initializing configuration registers of TDC7200  
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void TDC7200_reg_init(void)
{
  INT8U i;
  INT8U TDC7200_reg_local_copy[10] = {0x02, 0x44, 0x04, 0x07, 0xFF, 0xFF, 0xFF, 0xFF, 0x0, 0x0 };

  for (i= 0; i< 10; i++)
  {
	TDC7200_Write(i, TDC7200_reg_local_copy[i]); 	// write in TDC1000 Registers
  }

}
/******************************************************************************
** Function name:	    TDC1000_reg_init
**
** Descriptions:		Initializing configuration registers of TDC1000  
**
** parameters:			None
** Returned value:		None
** 
******************************************************************************/
void TDC1000_reg_init(void)
{
  INT8U i;
  INT8U TDC1000_reg_local_copy[10] = {0x45, 0x45, 0x0A, 0x05, 0x1F, 0x08, 0x19, 0x01, 0x23, 0x01};

  for (i= 0; i< 10; i++)
  {
	TDC1000_Write(i, TDC1000_reg_local_copy[i]); 	// write in TDC1000 Registers
  }

}

float exp_time,min_exp_time,max_exp_time;
INT8U del_t_count;
/******************************************************************************
** Function name:	    abs_diff
**
** Descriptions:		Finds absolute difference between 2 numbers  
**
** parameters:			number 1,number 2
** Returned value:		absolute difference of number 1 & number 2 
** 
******************************************************************************/
float abs_diff(float a,float b)
{
  if(a-b>0) return a-b;
  else return b-a;
}

float TOF_upStream[5];
float TOF_downStream[5];
float velocity;
float time_upStream[6];
float time_downStream[6];
float clk_upStream[5];
float clk_downStream[5];
float CALIB1,CALIB2;
/******************************************************************************
** Function name:	    CALCULATE_TOF
**
** Descriptions:		Finds ultrasonic tof according to TDC7200 Mode 2 calculations.time of flights of n stop pulses found   
**                      by reading TDC7200 time and clock count registers and stores upstream and downstream tofs into arrays 
**
** parameters:			Channel Select
** Returned value:		None 
** 
******************************************************************************/
void CALCULATE_TOF(INT8U Ch_sel)
{	 
        CALIB1=TDC7200_Read(0x1B);
        CALIB2=TDC7200_Read(0x1C);
	   
        float CALIB2_period=10;   
	  
        float cal_count = (CALIB2-CALIB1)/(CALIB2_period-1);
        float norm_lsb; 
	if ((CALIB1-CALIB2)==0)norm_lsb=0;
        else norm_lsb= 125.0/cal_count;

	INT8U i;
	if(Ch_sel==1) 
        {
          CHSEL_HIGH;
          TDC7200_Write(0x00,0x03);
	
	  while((GPIO0_IOPIN & 0x00000200)==0x00000200)
	  {
       	  }
      
          for(i=0;i<6;i++)
          {
		time_upStream[i]=TDC7200_Read(16+(2*i));
          }
	
          for(i=0;i<5;i++)
          {
		clk_upStream[i]=TDC7200_Read(17+(2*i));
          }
          for(i=0;i<5;i++)
          {
              TOF_upStream[i] = ( norm_lsb*(time_upStream[0]-time_upStream[i+1]) ) + ( clk_upStream[i]*125.0 );
          }
        }


	if(Ch_sel==0)
        {
          CHSEL_LOW;
          TDC7200_Write(0x00,0x03);
	
	  while((GPIO0_IOPIN & 0x00000200)==0x00000200)
	  {
       	  }
      
          for(i=0;i<6;i++)
          {
		time_downStream[i]=TDC7200_Read(16+(2*i));
          }
	
          for(i=0;i<5;i++)
          {
		clk_downStream[i]=TDC7200_Read(17+(2*i));
          }
          for(i=0;i<5;i++)
	  {
		TOF_downStream[i] = ( norm_lsb*(time_downStream[0]-time_downStream[i+1]) ) + ( clk_downStream[i]*125.0 );
	  }
        }
}
/******************************************************************************
** Function name:	    MEASURE_TOF
**
** Descriptions:		1.Sets acceptance time window for generating stop pulses   
**                      2.gives a call to TOF measurement function with proper channel selected
**
** parameters:			None
** Returned value:		None 
** 
******************************************************************************/
void MEASURE_TOF(void)
{	 
        INT8U i;
        INT16U min_clks=min_exp_time/125;
        INT16U max_clks=max_exp_time/125;
        //printf("min_clks=%d\n",(INT32U)min_clks);
        //printf("max_clks=%d\n",(INT32U)max_clks);
        /*TDC7200_Write(0x07,(INT8U)max_clks);
        TDC7200_Write(0x06,(INT8U)(max_clks>>8));
        TDC7200_Write(0x09,(INT8U)min_clks);
	TDC7200_Write(0x08,(INT8U)(min_clks>>8));*/
        CALCULATE_TOF(0);
        CALCULATE_TOF(1);

        /*for(i=0;i<5;i++)
        {
              printf("TOF_downStream[%d]=%u\n",i,(INT32U)TOF_downStream[i]);
        }
	for(i=0;i<5;i++)
        {
              printf("TOF_upStream[%d]=%u\n",i,(INT32U)TOF_upStream[i]);
        }*/
	 
}
/******************************************************************************
** Function name:	    expected_time_sensor
**
** Descriptions:		Finds expected time,min_expected_time and max_expected_time by calculating sound 
**                      velocity from temp and velocity arrays(temperature read from LM75 sensor)  
** parameters:			length,diameter,temperature
** Returned value:	    None 
** 
******************************************************************************/
void expected_time_sensor(float l,float d,float temp)
{
	const float tempArray[]= {0,5,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80,85,90,95,100} ;
	const float soundVel[] = {1403,1427,1447,1465,1481,1495,1507,1517,1526,1534,1541,1547,1552,1554,1555,1556,1555,1553,1550,1547,1543};
	float temperature;
        INT8U i;
        INT8U lowLimit;
       
	for(i=0;i<21;i++)
        {
          if(tempArray[i]>temperature)
          {
                lowLimit=i-1;
                break;
          }
       	
        }
	velocity = soundVel[lowLimit] + (((soundVel[lowLimit+1]-soundVel[lowLimit]) / (tempArray[lowLimit+1]-tempArray[lowLimit])) * (temperature-tempArray[lowLimit]) );
        
        exp_time = (l+d)/velocity;
        min_exp_time = 0.96*exp_time;
        max_exp_time = (exp_time+9000)*1.04;
}
/******************************************************************************
** Function name:	    expected_time
**
** Descriptions:		Finds expected time,min_expected_time and max_expected_time by calculating sound 
**                      velocity from sum of two corresponding TOFs(no temperature sensor required) 
** parameters:			length,diameter
** Returned value:	    None 
** 
******************************************************************************/
void expected_time(float l,float d)
{
        INT8U i,j;
        MEASURE_TOF();
        for(i=0;i<5;i++)
        {
          for(j=0;j<5;j++)
          {
              if(abs_diff(TOF_upStream[i],TOF_downStream[j])<200)
              {
                  velocity=(2*(l+d))/(TOF_upStream[i]+TOF_downStream[j]);
                  break;
              }
          }
        }
        exp_time = (l+d)/velocity;
        min_exp_time = 0.96*exp_time;
        max_exp_time = (exp_time+9000)*1.04;
}


float del_t[9];
float sum_t[9];
float Final_Flow_Rate;
float Previous_Flow_Rate=0;
float Totalized_Flow=0;
float Final_Flow_velocity;
float Batch=0;
float Final_del_t=0;

float D_Final_Flow_Rate;
float D_Totalized_Flow;
float D_Final_Flow_Velocity;
float D_Batch;

float Batch_limit1;
float Batch_limit2;

float L=0.072;
float DIA=0.18;
float k=0.98;

/******************************************************************************
** Function name:	    UARTWriteChar
**
** Descriptions:		Transmits a character on UART
**
** parameters:			Character to be written
** Returned value:		None 
** 
******************************************************************************/
static void UARTWriteChar(INT8U ch)
{
  UART0_THR = ch;
  while ((UART0_LSR & 0x40) == 0);
}
/******************************************************************************
** Function name:	    __putchar
**
** Descriptions:		Sends a character to UART
**
** parameters:			Character to be sent
** Returned value:		None 
** 
******************************************************************************/
void __putchar(int ch)
{
  if (ch == '\n')
      UARTWriteChar('\r');
  UARTWriteChar(ch);
}


#include"I2C PFL_INT.c"
#include"modbus.c"
/******************************************************************************
** Function name:	    calculate
**
** Descriptions:	    Gives flow velocity,volumetric flow rate,totalized flow rate and batch
**                          flow rate;after batch is completes it resets the relay control GPIOs
**			    Function Not Available for public use.
** parameters:			None
** Returned value:		None 
** 
******************************************************************************/
extern void calculate(void);

#include"Measure.c"

/******************************************************************************
** Function name:	    copy
**
** Descriptions:		copys the flow parameters into variables that are going to be displayed
**
** parameters:			None
** Returned value:		None 
** 
******************************************************************************/
void copy()
{
  D_Final_Flow_Rate=Final_Flow_Rate;
  D_Final_Flow_Velocity=Final_Flow_velocity;
  D_Totalized_Flow=Totalized_Flow;
  D_Batch=Batch;
}
/******************************************************************************
** Function name:	    Display_Flow_Velociy
**
** Descriptions:		Displays flow velocity
**
** parameters:			None
** Returned value:		None 
** 
******************************************************************************/
void Display_Flow_Velocity()
{
  drawString(20,64,f2_colour,b2_colour,dispn((D_Final_Flow_Velocity*1000),3,5));
}
/******************************************************************************
** Function name:	    Display_Volume_Flow_Rate
**
** Descriptions:		Displays volumetric flow rate
**
** parameters:			None
** Returned value:		None 
** 
******************************************************************************/
void Display_Volume_Flow_Rate()
{
  drawString(180,64,f2_colour,b2_colour,dispn((D_Final_Flow_Rate*10000),4,6));
}
/******************************************************************************
** Function name:	    Display_Totalized_Flow
**
** Descriptions:		Displays Totalized flow 
**
** parameters:			None
** Returned value:		None 
** 
******************************************************************************/
void Display_Totalised_Flow() 
{
  drawString(95,128,f2_colour,b2_colour,dispn((D_Totalized_Flow),0,6));
}
/******************************************************************************
** Function name:	    Display_Batch_Flow
**
** Descriptions:		Displays Batch flow
**
** parameters:			None
** Returned value:		None 
** 
******************************************************************************/
void Display_Batch_Flow()
{
  drawString(95,192,f2_colour,b2_colour,dispn((D_Batch),0,6));
}
/******************************************************************************
** Function name:	    timer1start
**
** Descriptions:		starts on-chip timer
**
** parameters:			None
** Returned value:		None 
** 
******************************************************************************/
void timer1start(void)
{
	T1_TCR = 0 ;
	T1_TC = 0;
	T1_TCR = 1 ;                //enable timer;
}
/******************************************************************************
** Function name:	    timer1stop
**
** Descriptions:		stops on-chip timer
**
** parameters:			None
** Returned value:		None 
** 
******************************************************************************/
void timer1stop (void)
{
  T1_TCR = 0 ;              //disable timer;
}

INT8U   MilliSec = 0 , CentiSec = 0 ;
INT8U   CalcFlag = 0 , DispFlag = 0 , Display_State = 0 ;
/******************************************************************************
** Function(IRQ) name:	timer1Isr                      
**
** Descriptions:		sets CalcFlag every 100 ms,sets DispFlag every 0.5 s
**
** parameters:			None
** Returned value:		None 
** 
******************************************************************************/

void Timer1Isr( void )__attribute__ ((interrupt ("IRQ"))) ;

void Timer1Isr ( void )
{
        T1_IR = 0xff ; /* clear timer interrupt */
        MilliSec++ ;
	if ( MilliSec == 100 ) 
	{
		MilliSec = 0 ;
		CentiSec++ ;
		CalcFlag = 1 ; 

	        if ( CentiSec == 5 ) 
	        { 
                  CentiSec = 0 ;
                  DispFlag = 1 ;
                  Display_State = 0 ;	
                }
	}
        
        VICVectAddr = 0 ;
}
/******************************************************************************
** Function name:	    Timer1Init                      
**
** Descriptions:		initializes Timer1 to give an interrupt every 1 ms with 2nd priority
**
** parameters:			None
** Returned value:		None 
** 
******************************************************************************/
void Timer1init(void)
{
    T1_TCR = 2 ;                                 // Reset Timer Counter
    T1_PC =  0 ;                                 // Prescaler = 1(pclk)
    T1_MCR = 3 ;                                 // Generate Interrupt on Match Value
    
    SCB_VPBDIV = 1 ;
    
    T1_MR0 = 73728 ; // for 1 ms
    
    VICIntSelect &= ~0x20;		//select Timer1 interrup as IRQ 
    VICIntEnable  =  0x20;		//enable timer1 interrupt
    
    VICVectCntl1 = 0x25;  
    VICVectAddr1 = ( INT32U )Timer1Isr;
}
/******************************************************************************
** Function name:	    main                      
**
** Descriptions:		main function.Initializes all system parameters.Reads settings from flash at power on.
**                      Does co-operative multitasking.
**
** parameters:			None
** Returned value:		None 
** 
******************************************************************************/

int main(void)
{
  UARTInitialize( 9600 ) ;
  INT8U i;

  Init_Hardware();
  UART0_int_Init();
  PFL_Init();
  Init_I2C();
  Flash_read();
  if(buff[0]== 0x55 && buff[1]==0xAA && buff[2]== 0x5A && buff[3]==0xA5)
  {
   INT8U check_sum=0,i;
   for(i=0;i<24;i++) check_sum+=buff[i];
   if(check_sum==buff[24])Write_var();
   else def_par();
  }
  else def_par();
  InitDisplay();
 
  Timer1init();
  timer1start();

  Init_Main_Screen() ;

  __ARMLIB_enableIRQ();
  TDC7200_Read(1);
  TDC7200_reg_init() ;
  TDC1000_Read(1);
  TDC1000_reg_init() ;
  TDC1000EN_HIGH;
  def_par();
  /*printf("Length=%d Diameter=%d k=%d\n",(INT32U)(L*1000),(INT32U)(DIA*1000),(INT32U)(k*1000));
  for(i=0;i<10;i++)
  {
    printf("TDC1000 reg(%d)=%x\n",i,TDC1000_Read(i));
  }
  for(i=0;i<10;i++)
  {
    printf("TDC7200 reg(%d)=%x\n",i,TDC7200_Read(i));
  }*/
  
  INT8S init_gain=set_gain();
  
  while ( 1 ) 
  {
    if ( CalcFlag ) 
    {
        measure() ;
	CalcFlag = 0 ;
    }
    if ( DispFlag ) 
    {
	switch ( Display_State )
	{
		case 0 : copy(); Display_Flow_Velocity() ; Display_State = 1 ; break ;
                case 1 : Display_Volume_Flow_Rate() ; Display_State = 2 ; break ;
		case 2 : Display_Totalised_Flow() ; Display_State = 3 ; break ;
		case 3 : Display_Batch_Flow() ; Display_State = 4 ; break ;
                case 4 : DispFlag = 0 ; Display_State = 0 ; break ;
	}  
    }    
    if ( packetfull )
    {
        response();
    }
  }
}


void pabort(void)
{
    //printf( "\npabort" ) ;
    while(1) ;
}

void dabort( void )
{
    //printf( "\ndabort" ) ;
    while(1) ;
}

void undef( void )
{
    //printf( "\nundef" ) ;
    while(1) ;
}


