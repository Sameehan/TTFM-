/*****************************************************************************
 
 *   Measure.c: Measurement Block
 
******************************************************************************/


/******************************************************************************
** Function name:		set_gain
**
** Descriptions:		Sets the PGA/PGA+LNA gain to get an appropriate echo profile    
**                      
** parameters:			None 
** Returned value:		Gain/Error
** 
******************************************************************************/
INT8S set_gain(void)
{
  INT8U temp;
  INT8U n,i,gain=0;
  //UARTReadChar(); 
  temp=TDC1000_Read(0x05)& 0x1F;
  MEASURE_TOF();
  while(1)
  {
      //UARTReadChar();
      MEASURE_TOF();
      n=0;
      for(i=0;i<5;i++)
      {
        if((TOF_upStream[i]>8192000)||(TOF_upStream[i]<1))break;
        if((TOF_downStream[i]>8192000)||(TOF_downStream[i]<1))break;
      }
      n=i;
      //printf("n=%d\n",(INT32U)n);
     
    	
    if (n<5) 
    {
      if(gain<7) 
      {
        gain++;
        //printf("gain = %d\n",gain);
        TDC1000_Write(0x05,((gain<<5)+temp));
      }
      else 
      {
        temp=0;
        gain=0;
        TDC1000_Write(0x05,((gain<<5)+temp));
      }
    }
    else break;
  }
}
/******************************************************************************
** Function name:       measure
**
** Descriptions:	Carries out measurement cycles and finds deltat 
**			Function Not available for Public Use. Hence extern.    
**          
** parameters:			None 
** Returned value:		None
** 
******************************************************************************/
extern void measure(void);
