INT16S temperature;

void get_temp( void )
{    
        I2C_I2CONSET = 0x20;
       
        while( I2C_I2STAT != 0x08 )
	{}
        I2C_I2CONCLR = 0x28; 
        I2C_I2DAT = 0x91 ;
 
        while ( I2C_I2STAT != 0x18)
        {}
        I2C_I2CONCLR = 0x08;	
        
	
        while( I2C_I2STAT != 0x40 )
	{}
        I2C_I2CONCLR = 0x08;
        I2C_I2CONSET = 0x04;
        INT8U i;
        
	    while( I2C_I2STAT != 0x50 )
	    {}
            I2C_I2CONCLR = 0x08;
	    temperature=I2C_I2DAT;
	    temperature<<=8;
	    
		I2C_I2CONCLR = 0x04;
		
		while( I2C_I2STAT != 0x58 )
	    {}
            I2C_I2CONCLR = 0x08;
	    temperature+=I2C_I2DAT;

	I2C_I2CONSET=0x50;
}
