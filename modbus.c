/*****************************************************************************
 
 *   MODBUS.c: MODBUS functions
 
******************************************************************************/

INT8U rxidx;
INT8U rx_buff[256];
INT8U tx_buff[256];
/******************************************************************************
** Function name:		RDA
**
** Descriptions:		Fills 7 bytes from FIFO into rx_buff so that FIFO doesn’t get empty
**                      
** parameters:			None 
** Returned value:		None
** 
******************************************************************************/
void RDA(void)
{
	INT8U i;
	for(i=0;i<7;i++) 
	{
		rx_buff[rxidx]=UART0_RBR;
		rxidx++;
	}
}

INT8U packetfull;
/******************************************************************************
** Function name:		CTI
**
** Descriptions:		copies received data till FIFO gets empty
**                      
** parameters:			None 
** Returned value:		None
** 
******************************************************************************/
void CTI(void)
{
    while((UART0_LSR & 0x01)!=0)
    {
    	rx_buff[rxidx]=UART0_RBR;
        rxidx++;		
    }
    packetfull=1;
}

#define MYADDR   0x01

const INT8U auchCRCHi[] = {
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40
} ;

const INT8U auchCRCLo[] = {
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
0x40
} ;
/******************************************************************************
** Function name:		calc_crc
**
** Descriptions:		Calculates CRC accotding to MODBUS RTU framing rules 
**                      
** parameters:			pointer to CRC array,no. of  characters of which CRC is to be found 
** Returned value:		CRC
** 
******************************************************************************/
INT16U calc_crc(INT8U* puchMsg,INT16U usDataLen)
{
  INT8U uchCRCHi = 0xFF ; /* high byte of CRC initialized */
  INT8U uchCRCLo = 0xFF ; /* low byte of CRC initialized */
  INT8U uIndex ; 
  while (usDataLen) /* pass through message buffer */
  {
    uIndex = uchCRCHi ^ *puchMsg++ ; /* calculate the CRC */
    uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex] ;
    uchCRCLo = auchCRCLo[uIndex] ;
    usDataLen--;
  }
  return (uchCRCHi << 8 | uchCRCLo) ;
}


INT8U tx_length;
/******************************************************************************
** Function name:		combine
**
** Descriptions:		Combines 2 data bytes into a 16 bit big endian Data Word
**                      
** parameters:			pointer to array,index of the 1st among the 2 bytes to be combined
** Returned value:		combined Word
** 
******************************************************************************/
INT16U combine(INT8U *p,INT8U index)
{
   INT16U x;
   x=(INT16U)p[index];
   x<<=8;
   x+=p[index+1];
   return x;
}

INT8U txidx;
/******************************************************************************
** Function name:		transmit
**
** Descriptions:		transmits one character and then enables transmitter interrupts
**                      
** parameters:			None 
** Returned value:		None
** 
******************************************************************************/
void transmit(void)
{
  UART0_THR=tx_buff[0]; 
  txidx = 1;
  UART0_IER |= 2;
}
/******************************************************************************
** Function name:		trans
**
** Descriptions:		fills values from tx_buff into UART transmit register
**                      
** parameters:			None 
** Returned value:		None
** 
******************************************************************************/
void trans(void)
{
  if(txidx<tx_length)
  {
    UART0_THR=tx_buff[txidx];
    txidx++;
  }
  else UART0_IER&=~2;
}

INT8U Batch_start;
/******************************************************************************
** Function name:		response
**
** Descriptions:		Processes MODBUS commands coming from master for function codes 3,4,5,16
**                      Sends exception response in case of errors
**                      Sends data response in case of no errors  
** parameters:			None 
** Returned value:		None
** 
******************************************************************************/
void response(void)
{
  INT8U  count,*q,i,func ;
  INT16U m,n ;
  INT16U CRC1,CRC2 ;
 
  if ( rx_buff[0] != MYADDR ) 
  {
    packetfull=0;
    rxidx=0;
    return;
  }

  CRC1=combine(rx_buff,rxidx-2);
  CRC2=calc_crc(rx_buff,(INT16U)(rxidx-2));

  if(CRC1!=CRC2)
  {
    packetfull=0;
    rxidx=0;
    return;
  }
  func=rx_buff[1];
  if((func!=3) && (func!=4) && (func!=5) && (func!=16))
  {
    tx_buff[0]=MYADDR;
    tx_buff[1]=rx_buff[1] | 0x80;
    tx_buff[2]=0x01;
    CRC2=calc_crc(tx_buff,3);
    tx_buff[3]=CRC2>>8;
    tx_buff[4]=(INT8U)CRC2;
    tx_length=5;
    transmit();
    packetfull=0;
    rxidx=0;
    return;
  }
  m = combine(rx_buff,2);
  n = combine(rx_buff,4);
  if(func==16)
  {
    if (((m & 0x01)!= 0) ||((n & 0x01)!= 0) ||((m+n)>10))
    {
      tx_buff[0]=MYADDR;
      tx_buff[1]=rx_buff[1] | 0x80;
      tx_buff[2]=0x02;
      CRC2=calc_crc(tx_buff,3);
      tx_buff[3]=CRC2>>8;
      tx_buff[4]=(INT8U)CRC2;
      tx_length=5;
      transmit();
      packetfull=0;
      rxidx=0;
      return;
    }
    count = 0;
    while(n)
    {
      switch(m)
      {
        case 0:q=(INT8U*)&L;break;
        case 2:q=(INT8U*)&DIA;break;
        case 4:q=(INT8U*)&k;break;
        case 6:q=(INT8U*)&Batch_limit1;break;
        case 8:q=(INT8U*)&Batch_limit2;break;
      }
      for(i=4;i>0;i--)
      {
        *q=rx_buff[6+i+2*count];
        q++;
      }
      count+=2;
      m+=2;
      n-=2;
    }
    
    for(i=0;i<6;i++)
    {
      tx_buff[i]=rx_buff[i];
    }
    CRC2=calc_crc(tx_buff,6);
    tx_buff[6]=CRC2>>8;
    tx_buff[7]=(INT8U)CRC2;
    tx_length=8;
    transmit();
    packetfull=0;
    rxidx=0;
    return;
  }
  else if (func==3)
  {
    if (((m & 0x01)!= 0) ||((n & 0x01)!= 0) ||((m+n)>10))
    {
      tx_buff[0]=MYADDR;
      tx_buff[1]=rx_buff[1] | 0x80;
      tx_buff[2]=0x02;
      CRC2=calc_crc(tx_buff,3);
      tx_buff[3]=CRC2>>8;
      tx_buff[4]=(INT8U)CRC2;
      tx_length=5;
      transmit();
      packetfull=0;
      rxidx=0;
      return;
    }
    count= 0 ;
    tx_buff[0]=MYADDR;
    tx_buff[1]=func;
    tx_buff[2]=n*2;
    while(n)
    {
      
      switch(m)
      {
        case 0:q=(INT8U*)&L;break;
        case 2:q=(INT8U*)&DIA;break;
        case 4:q=(INT8U*)&k;break;
        case 6:q=(INT8U*)&Batch_limit1;break;
        case 8:q=(INT8U*)&Batch_limit2;break;
      }
      
      for(i=4;i>0;i--)
      {
        tx_buff[(2*count)+2+i]=*q;
        q++;
      }
      count+=2;
      m+=2;
      n-=2;
    }

    CRC2=calc_crc(tx_buff,2*count+3);
    tx_buff[2*count+3]=CRC2>>8;
    tx_buff[2*count+4]=(INT8U)CRC2;
    tx_length=2*count+5;
    transmit();
    packetfull=0;
    rxidx=0;
    return;
 }
 else if (func==4)
 {
    if (((m & 0x01)!= 0) ||((n & 0x01)!= 0) ||((m+n)>8))
    {
      tx_buff[0]=MYADDR;
      tx_buff[1]=rx_buff[1] | 0x80;
      tx_buff[2]=0x02;
      CRC2=calc_crc(tx_buff,3);
      tx_buff[3]=CRC2>>8;
      tx_buff[4]=(INT8U)CRC2;
      tx_length=5;
      transmit();
      packetfull=0;
      rxidx=0;
      return;
    }
    count= 0 ;
    tx_buff[0]=MYADDR;
    tx_buff[1]=func;
    tx_buff[2]=n*2;
    while(n)
    {
      
      switch(m)
      {
        case 0:q=(INT8U*)&Final_Flow_velocity;break;
        case 2:q=(INT8U*)&Final_Flow_Rate;break;
        case 4:q=(INT8U*)&Totalized_Flow;break;
        case 6:q=(INT8U*)&Batch;break;
      }
      
      for(i=4;i>0;i--)
      {
        tx_buff[(2*count)+2+i]=*q;
        q++;
      }
      count+=2;
      m+=2;
      n-=2;
    }  
    CRC2=calc_crc(tx_buff,2*count+3);
    tx_buff[2*count+3]=CRC2>>8;
    tx_buff[2*count+4]=(INT8U)CRC2;
    tx_length=2*count+5;
    transmit();
    packetfull=0;
    rxidx=0;
    return;
  }
  if(func==5)
  {
    if (m>2)
    {
      tx_buff[0]=MYADDR;
      tx_buff[1]=rx_buff[1] | 0x80;
      tx_buff[2]=0x02;
      CRC2=calc_crc(tx_buff,3);
      tx_buff[3]=CRC2>>8;
      tx_buff[4]=(INT8U)CRC2;
      tx_length=5;
      transmit();
      packetfull=0;
      rxidx=0;
      return;
    }
    switch(m)
    {
       case 0:Totalized_Flow=0;break;
       case 1:Batch=0;break;
       case 2:Batch_limit1_HIGH; Batch_limit2_HIGH; Batch_start=1; break; 
    }   
    tx_buff[0]=MYADDR;
    tx_buff[1]=func;
    tx_buff[2]=rx_buff[2];
    tx_buff[3]=rx_buff[3];
    tx_buff[4]=rx_buff[4];
    tx_buff[5]=rx_buff[5];
    CRC2=calc_crc(tx_buff,6);
    tx_buff[6]=CRC2>>8;
    tx_buff[7]=(INT8U)CRC2;
    transmit();
    packetfull=0;
    rxidx=0;
    return; 
  }
}
/******************************************************************************
** Function name:	    UART0_int_Init
**
** Descriptions:		Initializes UART0 interrupts with 3rd priority   
**                      
** parameters:			None 
** Returned value:		None
** 
******************************************************************************/
void UART0_int_Init( void )
{
       VICIntSelect &= ~0x40;		 
       VICIntEnable  =  0x40;		 
       VICVectCntl2 = 0x26; 
       VICVectAddr2 = ( INT32U )UART0_isr;
       //UART0_IER=1;
       rxidx=0;
}
/******************************************************************************
** Function(IRQ) name:	UART0_isr
**
** Descriptions:		calls RDA/CTI/trans based of which interrupt has been asserted   
**                      
** parameters:			None 
** Returned value:		None
** 
******************************************************************************/
void UART0_isr( void )__attribute__((interrupt("IRQ")));

void UART0_isr( void )
{
  INT32U IIR = UART0_IIR & 0x0E;
  if(IIR == 4) RDA();
  else if(IIR == 0x0C)CTI();
  else if(IIR == 2)trans();
  VICVectAddr=0;
}










