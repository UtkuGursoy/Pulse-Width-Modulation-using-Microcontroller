#include "ee308.h"
#include <xc.h>
#include <libpic30.h>

#pragma config JTAGEN=OFF, FWDTEN = 0

const unsigned char LookUp[16] = "0123456789ABCDEF";
unsigned char msgbuff[3] = "  0";
unsigned char rcvdata; 

int tintflag = 0;

void __attribute__ ( ( __interrupt__ , auto_psv ) ) _T1Interrupt ( void )
{
    LATAbits.LATA10 = ~LATAbits.LATA10;
    IFS0bits.T1IF = 0;
    //LATCbits.LATC5= ~LATCbits.LATC5;  // Open the LED
    //TRISCbits.TRISC5=0; 
    tintflag = ~tintflag;
}

void __attribute__ ((__interrupt__, auto_psv)) _T2Interrupt(void)
{
    IFS0bits.T2IF = 0;
} 

int main() { 
    init_IO();
    init_timer();
    init_I2C();
    //init_LCD();
    init_Serial();
   
    // PERIPHERAL PIN SELECT OUTPUT REGISTER 10
    RPOR10bits.RP21R0 = 1;  // RP21 Output Pin Mapping bits, set to OC1 (Output Compare 1)
    RPOR10bits.RP21R2 = 1;  // 
    RPOR10bits.RP21R3 = 1;  // 
    
    /**** Single Output Pulse Mode Setup and Interrupt Servicing ****/
   
    //OC1CON = 0x0000;      // Turn off Output Compare 1 Module
    //OC1CON = 0x0004;      // Load new compare mode to OC1CON
     
    OC1CON1 = 0x0000;       //
    OC1R    = 0x0000;       // Initialize Compare Register1 with 0
    OC1RS   = 0x0000;       // Initialize Secondary Compare Register1 with 0
    OC1CON1 = 0x0006;       // Comparison for red light
    PR2 = 1023;             // PWM 500 ms
    
    IPC1bits.T2IP = 1;      // Setup Output Compare 1 interrupt for
    //IPC0bits.OC1IP1 = 0;  // desired priority level
    //IPC0bits.OC1IP2 = 0;  // (this example assigns level 1 priority)
    IFS0bits.T2IF = 0;      // Clear Output Compare 1 interrupt flag
    IEC0bits.T2IE = 1;      // Enable Output Compare 1 interrupts
    T2CONbits.TON = 1;      // Start Timer2 with assumed settings
    TRISBbits.TRISB10 = 1;  //   
    
    /**** Converting One Channel, Manual Sample Start, Manual Conversion Start Code ****/
    
    int ADCValue = 1;       // get the ADV value, initialize to an arbitrary value
    AD1CON1 = 0;            // SAMP bit = 0 ends sampling
                            // and starts converting
    AD1CHS = 0x000A;        // Connect AN10 as CH0 input
                            // in this example AN10 is the input
    AD1CSSL = 0;
    AD1CON3 = 0x0002;       // Manual Sample, Tad = 2 Tcy
    AD1CON2 = 0;
    AD1CON1bits.ADON = 1;   // turn ADC ON
    
    // Color Adjustments
    //TRISCbits.TRISC5 = 0;  //Red
    //TRISCbits.TRISC6 = 0;  //Green
    //TRISCbits.TRISC7 = 0;  //Blue
    
    
    unsigned char thousands,hundreds,tens,ones,end_line;
    
    
    while (1) // repeat continuously
    {
        if(tintflag){
            AD1CON1bits.SAMP = 1;       // start sampling...
            __delay32(500);             // Ensure the correct sampling time has elapsed
                                        // before starting conversion.
            AD1CON1bits.SAMP = 0;       // start Converting
            while (!AD1CON1bits.DONE);  // conversion done?
            ADCValue = ADC1BUF0;        // yes then get ADC value
            
            
            thousands = ((unsigned char)(ADCValue / 1000)) + '0';
            hundreds  = ((unsigned char)((ADCValue % 1000) / 100)) + '0';
            tens      = ((unsigned char)((ADCValue % 100) / 10  )) + '0';
            ones      = ((unsigned char)((ADCValue % 10) / 1    )) + '0';
            end_line  = '\n';
            
            U1TXREG = thousands;
            U1TXREG = hundreds;
            U1TXREG = tens;
            U1TXREG = ones;
            U1TXREG = end_line;
            
            OC1R = (ADCValue % (PR2+1));    // reset the value of OC1R when the ADCValue is more than PR2
            while(tintflag);            // wait until flag is down
        }
    }
    return 0;
}
