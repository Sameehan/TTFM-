/*****************************************************************************
 
 *   SPI.c: SPI functions
 
******************************************************************************/

#define CS_TDC7200_HIGH         GPIO0_IOSET=0x01000000
#define CS_TDC7200_LOW          GPIO0_IOCLR=0x01000000
#define CS_TDC1000_HIGH         GPIO0_IOSET=0x00800000
#define CS_TDC1000_LOW          GPIO0_IOCLR=0x00800000

/******************************************************************************
** Function name:	    SPI_write 
**
** Descriptions:		writes into the slave registers   
**                      
** parameters:			address,data
** Returned value:		None
** 
******************************************************************************/
void SPI_Write(INT8U addr,INT8U dat)
{
	addr |= 0x40;
	SPI1_SPDR = addr;
	while((SPI1_SPSR & 0x80)!=0x80);
	SPI1_SPDR = dat;
	while((SPI1_SPSR & 0x80)!=0x80);
}
/******************************************************************************
** Function name:	    SPI_Read0
**
** Descriptions:		reads data from 8-bit slave registers  
**                      
** parameters:			address
** Returned value:		None
** 
******************************************************************************/
INT32U SPI_Read0(INT8U addr)
{
	addr &= 0x3f;
	SPI1_SPDR = addr;
	while((SPI1_SPSR & 0x80)!=0x80);
	SPI1_SPDR = 0xff;
	while((SPI1_SPSR & 0x80)!=0x80);
	
	return (INT32U)SPI1_SPDR; 
}
/******************************************************************************
** Function name:	    SPI_Read1
**
** Descriptions:		reads data from 24-bit slave registers
**                      
** parameters:			address
** Returned value:		None
** 
******************************************************************************/
INT32U SPI_Read1(INT8U addr)
{
        INT32U readdata ;

	addr &= 0x3f;
	SPI1_SPDR = addr;
	while((SPI1_SPSR & 0x80)!=0x80);
	SPI1_SPDR = 0xff;
	while((SPI1_SPSR & 0x80)!=0x80);
	readdata = SPI1_SPDR;
        readdata <<= 8 ;
	SPI1_SPDR = 0xff;
	while((SPI1_SPSR & 0x80)!=0x80);
	readdata |= SPI1_SPDR;
        readdata <<= 8 ;
	SPI1_SPDR = 0xff;
	while((SPI1_SPSR & 0x80)!=0x80);
	readdata |= SPI1_SPDR;
	return readdata;
}
/******************************************************************************
** Function name:	    TDC7200_Write
**
** Descriptions:		calls SPI_Write and fills data in programmed register   
**                      
** parameters:			address,data
** Returned value:		None
** 
******************************************************************************/
void TDC7200_Write(INT8U addr,INT8U dat)
{
	CS_TDC7200_LOW;
	SPI_Write(addr,dat);
	CS_TDC7200_HIGH;
}
/******************************************************************************
** Function name:	    TDC7200_Read
**
** Descriptions:		calls SPI_Read0 or SPI_Read1 depending on the MSB register address  
**                      
** parameters:			None 
** Returned value:		None
** 
******************************************************************************/
INT32U TDC7200_Read(INT8U addr)
{
        INT32U Reg7200_Data ;
	CS_TDC7200_LOW;
        if ( ( addr & 0xf0 ) == 0 ) 
	Reg7200_Data=SPI_Read0(addr);
        else Reg7200_Data=SPI_Read1(addr);
	CS_TDC7200_HIGH;
	return Reg7200_Data;
}
/******************************************************************************
** Function name:	    TDC1000_Write 
**
** Descriptions:		calls SPI_Write and fills data in programmed register   
**                      
** parameters:			None 
** Returned value:		None
** 
******************************************************************************/
void TDC1000_Write(INT8U addr,INT8U dat)
{
	CS_TDC1000_LOW;
	SPI_Write(addr,dat);
	CS_TDC1000_HIGH;
}
/******************************************************************************
** Function name:	    TDC1000_Read 
**
** Descriptions:		performs SPI_Read0 at given register address   
**                      
** parameters:			None 
** Returned value:		None
** 
******************************************************************************/
INT32U TDC1000_Read(INT8U addr)
{
	CS_TDC1000_LOW;
	INT32U Reg1000_Data=SPI_Read0(addr);
	CS_TDC1000_HIGH;
	return Reg1000_Data;
}
