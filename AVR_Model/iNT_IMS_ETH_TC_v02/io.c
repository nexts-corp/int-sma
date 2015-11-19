#include <mega2560.h>
#include <delay.h>
#include "io.h"
#include "debug.h"

unsigned char   d_out[USE_DOUT] = {0xFF,0xFF};

/*============================================================*/
void init_io(void)
{
    // Input/Output Ports initialization
    // Port A initialization
    // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=Out Func1=In Func0=In 
    // State7=T State6=T State5=T State4=T State3=T State2=0 State1=T State0=T 
    PORTA=0x00;
    DDRA=0x04;

    // Port B initialization
    // Func7=Out Func6=Out Func5=Out Func4=Out Func3=In Func2=Out Func1=Out Func0=Out 
    // State7=1 State6=1 State5=1 State4=1 State3=P State2=1 State1=1 State0=1 
    PORTB=0xFF;
    DDRB=0xF7;
                                             
    // Port C initialization
    // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
    // State7=P State6=P State5=T State4=T State3=T State2=T State1=T State0=T 
    PORTC=0xC0;
    DDRC=0x00;                 
    
    // Port C initialization
    // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
    // State7=P State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
    //PORTC=0x80;
    //DDRC=0x00;

    // Port D initialization
    // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
    // State7=P State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
    PORTD=0x80;
    DDRD=0x00;

    // Port E initialization
    // Func7=Out Func6=Out Func5=Out Func4=Out Func3=Out Func2=Out Func1=In Func0=In 
    // State7=1 State6=1 State5=1 State4=1 State3=1 State2=P State1=T State0=T 
    PORTE=0xFC;
    DDRE=0xFC;

    // Port F initialization
    // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
    // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
    PORTF=0x00;
    DDRF=0x00;

    // Port G initialization
    // Func5=Out Func4=Out Func3=In Func2=In Func1=In Func0=In 
    // State5=1 State4=1 State3=T State2=T State1=T State0=T 
    //PORTG=0x20;
    //DDRG=0x20;
    PORTG=0x30;
    DDRG=0x30;

    // Port H initialization
    // Func7=In Func6=Out Func5=Out Func4=Out Func3=Out Func2=In Func1=In Func0=In 
    // State7=P State6=1 State5=1 State4=1 State3=1 State2=T State1=T State0=T 
    PORTH=0xF8;
    DDRH=0xF8;

//    // Port J initialization
//    // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
//    // State7=P State6=P State5=P State4=P State3=P State2=P State1=T State0=T 
//    PORTJ=0xFC;
//    DDRJ=0x00;

    // Port J initialization
    // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
    // State7=P State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
    PORTJ=0x80;
    DDRJ=0x00;
          
    if((TEMP_SEL == TEMP_TYPE_K) || (TEMP_SEL == TEMP_RESERVED)){
        // Port K initialization
        // Func7=In Func6=In Func5=In Func4=In Func3=Out Func2=Out Func1=Out Func0=In 
        // State7=P State6=T State5=T State4=P State3=0 State2=0 State1=0 State0=T 
        
        //PORTK=0x90;
        //DDRK=0x0E;    
        
        /*bongkot edit*/ 
        PORTK=0x80;
        DDRK=0x00;
    }
    else if((TEMP_SEL == TEMP_TMEC) || (TEMP_SEL == TEMP_PT100)){
        // Port K initialization
        // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
        // State7=P State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
        PORTK=0x80;
        DDRK=0x00;
    }

    // Port L initialization
    // Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
    // State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
    PORTL=0x00;
    DDRL=0x00;                      
    
}
/*============================================================*/
void IO_dout(int ch, char state)
{
    switch(ch){
        case 0  :   DOUT1       =   !state;
                    d_out[0]    =   state;         
                    //printDebug("<IO_dout> dout0 %d\r\n",d_out[0]);
                    break;
        case 1  :   DOUT2       =   !state;
                    d_out[1]    =   state;                        
                    //printDebug("<IO_dout> dout1 %d\r\n",d_out[1]);
                    break;
        default :   break;
    }
}
/*============================================================*/
char IO_keyCheck(void)
{
    static char flag_hold_MODE   = 0;
    static char flag_hold_UP     = 0;
    static char flag_hold_DOWN   = 0;
    static char flag_hold_OK     = 0;                 

    if( KEY_PRESS_CHK_MODE && !flag_hold_MODE )
    {       
        delay_ms(KEY_DELAY);      
        
        if( KEY_PRESS_CHK_MODE && !flag_hold_MODE )
        {
            flag_hold_MODE = 1;
            return 0;
        }
    }
    else if( KEY_RELEASE_CHK_MODE && flag_hold_MODE)
    {                                                  
        printDebug("<IO_keyCheck> key MODE\r\n");
        flag_hold_MODE = 0;          
        return KEY_MODE;
    }                 
    
    if( KEY_PRESS_CHK_UP && !flag_hold_UP )
    {       
        delay_ms(KEY_DELAY);      
        
        if( KEY_PRESS_CHK_UP && !flag_hold_UP )
        {
            flag_hold_UP = 1;
            return 0;
        }
    }
    else if( KEY_RELEASE_CHK_UP && flag_hold_UP)
    {                                                  
        printDebug("<IO_keyCheck> key UP\r\n");
        flag_hold_UP = 0;            
        return KEY_UP;
    }
    
    if( KEY_PRESS_CHK_DOWN && !flag_hold_DOWN )
    {       
        delay_ms(KEY_DELAY);      
        
        if( KEY_PRESS_CHK_DOWN && !flag_hold_DOWN )
        {
            flag_hold_DOWN = 1;
            return 0;
        }
    }
    else if( KEY_RELEASE_CHK_DOWN && flag_hold_DOWN)
    {                                                  
        printDebug("<IO_keyCheck> key DOWN\r\n");
        flag_hold_DOWN = 0;          
        return KEY_DOWN;
    }                 
    
    if( KEY_PRESS_CHK_OK && !flag_hold_OK )
    {       
        delay_ms(KEY_DELAY);      
        
        if( KEY_PRESS_CHK_OK && !flag_hold_OK )
        {
            flag_hold_OK = 1;
            return 0;
        }
    }
    else if( KEY_RELEASE_CHK_OK && flag_hold_OK)
    {                                               
        printDebug("<IO_keyCheck> key OK\r\n");
        flag_hold_OK = 0;                                     
        return KEY_OK;
    }
        
    return 0;
}

