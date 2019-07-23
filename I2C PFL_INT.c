/*****************************************************************************
 
 *   I2C PFL_INT.c: Power fail module
 
******************************************************************************/


/******************************************************************************
** Function name:		Init_I2C
**
** Descriptions:		Initializes I2C clock and configuration registers
**
** parameters:			None 
** Returned value:		None
** 
******************************************************************************/
void Init_I2C( void )
{
       I2C_I2SCLH=370;
       I2C_I2SCLL=370;
       I2C_I2CONSET=0x40;
       I2C_I2CONSET=0x10;
}

INT16U sector_number=0;
INT8U n=25;
INT8U buff[25];
INT8U *buffer;
/******************************************************************************
** Function name:		Fill_buff
**
** Descriptions:		Gives signature and copies L,DIA,K,Totalized_Flow,Batch 
**                      from SRAM into buffer stores Checksum 
** parameters:		    None
** Returned value:		None
** 
******************************************************************************/
void Fill_buff(void)
{
  buff[0]=0x55;
  buff[1]=0xAA;
  buff[2]=0x5A;
  buff[3]=0xA5;
  
  INT8U *x,i;
  x=(INT8U*)&L;
  for(i=4;i<8;i++)
  {
    buff[i]=*x;
    x++;
  }
  
  x=(INT8U*)&DIA;
  for(i=8;i<12;i++)
  {
    buff[i]=*x;
    x++;
  }

  x=(INT8U*)&k;
  for(i=12;i<16;i++)
  {
    buff[i]=*x;
    x++;
  }
  
  x=(INT8U*)&Totalized_Flow;
  for(i=16;i<20;i++)
  {
    buff[i]=*x;
    x++;
  }
  
  x=(INT8U*)&Batch;
  for(i=20;i<24;i++)
  {
    buff[i]=*x;
    x++;
  }
  buff[24] = 0 ;

  for(i=0;i<24;i++)
  {
    buff[24]+=buff[i];
  }
}
/******************************************************************************
** Function(IRQ) name:  PFL_isr
**
** Descriptions:		On assertion of power failure interrupt,it Writes data into 
**                      EEPROM(flash memory) by I2C Page write Cycle
** parameters:			None 
** Returned value:		None
** 
******************************************************************************/
void PFL_isr( void )__attribute__((interrupt("IRQ")));

void PFL_isr( void )
{
        Fill_buff();
        
        I2C_I2CONSET = 0x20;
        
        while( I2C_I2STAT != 0x08 )
	{}
        I2C_I2CONCLR = 0x28;
	I2C_I2DAT=0xA0;
        	
	while( I2C_I2STAT != 0x18 )
        {}
        I2C_I2CONCLR = 0x08;
	INT8U h_addr,l_addr;
	INT16U temp_cast;
	temp_cast=(sector_number << 7);
	h_addr=(INT8U)(temp_cast >> 8);
	l_addr=(INT8U)(temp_cast);
	I2C_I2DAT=h_addr;
	
	while( I2C_I2STAT != 0x28 )
	{}
        I2C_I2CONCLR = 0x08;
	I2C_I2DAT=l_addr;
        
        INT8U i;
	for(i=0;i<n;i++)
        {
	    while( I2C_I2STAT != 0x28 )
	    {}
            I2C_I2CONCLR = 0x08;
	    I2C_I2DAT=buff[i];

	}
         
        while( I2C_I2STAT != 0x28 );
        I2C_I2CONCLR = 0x08;
	I2C_I2CONSET=0x50;
        SCB_EXTINT=0x08;
	VICVectAddr=0;
}
/******************************************************************************
** Function name:		PFL_Init
**
** Descriptions:		Initializes power failure interrupt with 1st priority 
**
** parameters:			None 
** Returned value:		None
** 
******************************************************************************/
void PFL_Init( void )
{
       VICIntSelect &= ~0x20000;		//select EINT3 interrup as IRQ 
       VICIntEnable  =  0x20000;		//enable EINT3 interrupt 
       VICVectCntl0 = 0x31; 
       VICVectAddr0 = ( INT32U )PFL_isr;
}
/******************************************************************************
** Function name:		Flash_read 
**
** Descriptions:		At power on,reads the values stored in Flash using I2C Random read Cycle   
**                      
** parameters:			None 
** Returned value:		None
** 
******************************************************************************/
void Flash_read( void )
{    
        I2C_I2CONSET = 0x20;
       
        while( I2C_I2STAT != 0x08 )
	{}
        I2C_I2CONCLR = 0x28; 
        I2C_I2DAT = 0xA0 ;
 
        while ( I2C_I2STAT != 0x18)
        {}
        I2C_I2CONCLR = 0x08;
	INT8U h_addr,l_addr;
	INT16U temp_cast;
	temp_cast=(sector_number << 7);
	h_addr=(INT8U)(temp_cast >> 8);
	l_addr=(INT8U)(temp_cast);
	I2C_I2DAT=h_addr;
	
        while( I2C_I2STAT != 0x28 )                 
	{}
        I2C_I2CONCLR = 0x08;
	I2C_I2DAT=l_addr;
	
        while( I2C_I2STAT != 0x28 )
	{}
        I2C_I2CONCLR = 0x08;
	I2C_I2CONSET = 0x60;
	
        while( I2C_I2STAT != 0x10 )
	{}
        I2C_I2CONCLR = 0x28;
        I2C_I2DAT = 0xA1;
	
        while( I2C_I2STAT != 0x40 )
	{}
        I2C_I2CONCLR = 0x08;
        I2C_I2CONSET = 0x04;
        INT8U i;
        
	for(i=0; i<n ;i++)
        {
	    while( I2C_I2STAT != 0x50 )
	    {}
            I2C_I2CONCLR = 0x08;
	    buff[i]=I2C_I2DAT;
	}
	I2C_I2CONCLR = 0x04;
	while( I2C_I2STAT !=0x58 )
	{}
        I2C_I2CONCLR = 0x08;
	I2C_I2CONSET=0x50;
}
/******************************************************************************
** Function name:		Write_var 
**
** Descriptions:		It copies the values in buffer and stores them in L,DIA,k,Totalized_Flow,Batch   
**                      
** parameters:			None 
** Returned value:		None
** 
******************************************************************************/
void Write_var(void)
{
    INT8U *y,i;
    y=(INT8U*)&L;
    for(i=4;i<8;i++)
    {
      *y=buff[i];
      y++;
    }

    y=(INT8U*)&DIA;
    for(i=8;i<12;i++)
    {
      *y=buff[i];
      y++;
    }

    y=(INT8U*)&k;
    for(i=12;i<16;i++)
    {
      *y=buff[i];
      y++;
    }

    y=(INT8U*)&Totalized_Flow;
    for(i=16;i<20;i++)
    {
      *y=buff[i];
      y++;
    }

    y=(INT8U*)&Batch;
    for(i=20;i<24;i++)
    {
      *y=buff[i];
      y++;
    }  
}
/******************************************************************************
** Function name:		def_par
**
** Descriptions:		stores default values of L,DIA,k,Totalized_Flow,Batch into SRAM
**                      
** parameters:			None 
** Returned value:		None
** 
******************************************************************************/
void def_par(void)
{
    L = 0.072 ;
    DIA = 0.018 ;
    k = 0.98 ;
    Totalized_Flow = 0.0 ;
    Batch = 0.0 ;
}









