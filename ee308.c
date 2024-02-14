#include "ee308.h"
#include <xc.h>

void init_IO(void)
{
    LATCbits.LATC4 = 1; // TTP229 SCLK HIGH
    LATAbits.LATA9 = 0;
    LATAbits.LATA10 = 0;
    TRISCbits.TRISC4 = 0;
    TRISAbits.TRISA9 = 0;
    TRISAbits.TRISA10 = 0;
}

void init_timer(void)
{
	//_T1IP = 4;	// set Timer1 priority, (4 is the default value)
  	T1CONbits.TCKPS0 = 1;
	T1CONbits.TCKPS1 = 1;	// prescaler set to 256 
    T1CONbits.TCS = 0;
    
	TMR1	= 0x0000;
	PR1	= 500;		// set timer1 period to 3906 -> f = 2e6/256/3096 = 2Hz
//	_T1IF = 0;      // clear timer1 IF
//	_T1IE = 1;      // enable timer1 interrupt
    IFS0bits.T1IF = 0;
    IEC0bits.T1IE = 1;
    
	T1CONbits.TON = 1;
}
// ########################
// ### Serial functions ###
// ########################

void init_Serial(void) {
    U1BRG = 12;     // 2 MHz/(16*(12+1)) = 9615 (%0.16 error)
    RPINR18 = (RPINR18 & 0xFF00) | 0x0005; // RB5 : UART1 RX
    RPOR3 = (RPOR3 & 0xFF00)| 0x0003; // RB6 : UART1 TX
    
    U1MODEbits.UARTEN = 1;  // enable UART1
    U1STAbits.UTXEN = 1;    // enable UART1 TX
    U1STAbits.URXEN = 1;    // enable UART1 RX
    
}


void init_CNInt(void)
{
    CNEN2bits.CN22IE = 1;
    IFS1bits.CNIF = 0;
    IEC1bits.CNIE = 1;
}


// #####################
// ### I2C functions ###
// #####################

void init_I2C(void)
{
    ANSBbits.ANSB2 = 0;
    ANSBbits.ANSB3 = 0;
    I2C2BRG = 0x27;     // set BAUD rate
    I2C2CONLbits.A10M = 0;
    I2C2CONLbits.I2CEN = 1;
}

void I2Cstart(void)
{
	I2C2CONLbits.SEN = 1;		// start condition
	while(I2C2CONLbits.SEN){}	// wait while start condition in progress
}

void I2Cstop(void)
{
	I2C2CONLbits.PEN = 1;		// stop condition
	while(I2C2CONLbits.PEN){}	// wait while stop condition in progress
}

void I2Crestart(void)
{
	I2C2CONLbits.RSEN = 1;		// restart condition
	while(I2C2CONLbits.RSEN){}	// wait while restart condition in progress
}

unsigned char I2Cwrite(unsigned int c)
{
	I2C2TRN = c;
	while(I2C2STATbits.TBF){}	// wait write
	while(I2C2STATbits.TRSTAT){}	// wait slave ack	
	return(I2C2STATbits.ACKSTAT ? 0 : 1);	// not acknowledge = 0, acknowledge = 1
}

unsigned char I2Cread(void)
{
	//unsigned int c;
	I2C2CONLbits.RCEN = 1;		// master receive enable
	while(I2C2CONLbits.RCEN){}	// wait read
	return(I2C2RCV);
	//c = I2C1RCV;
	//return(c);
}

void I2Cack(void)
{
	I2C2CONLbits.ACKDT = 0;		// ACK bit 0
	I2C2CONLbits.ACKEN = 1;		// send ACK
	while(I2C2CONLbits.ACKEN){}	// wait_ack
}

void I2Cnack(void)
{
	I2C2CONLbits.ACKDT = 1;		// ACK bit 1
	I2C2CONLbits.ACKEN = 1;		// send ACK
	while(I2C2CONLbits.ACKEN){}	// wait_ack
}

// #####################
// ### LCD functions ###
// #####################

void init_LCD(void)
{
    unsigned int i;
    volatile unsigned char LCD_data[2][16] = {"PIC24 Board v2.2", "Timer       2:00"};

    send_1_LCD(0x24);
    for (i = 0 ; i < 16000 ; i++);
    
    send_LCD_CMD(0x28);
    for (i = 0 ; i < 16000 ; i++);
    send_LCD_CMD(0x0E);
    for (i = 0 ; i < 16000 ; i++);
    send_LCD_CMD(0x01);
    for (i = 0 ; i < 16000 ; i++);
    
    for (i = 0 ; i < 16 ; i++)
        send_LCD_DATA(LCD_data[0][i]);
    send_LCD_CMD(0xC0);
    for (i = 0 ; i < 16 ; i++)
        send_LCD_DATA(LCD_data[1][i]);
}

void send_LCD_CMD(unsigned char c)
{
    send_2_LCD(c, 0x0C);
}

void send_LCD_DATA(unsigned char c)
{
    send_2_LCD(c, 0x0D);
}

void send_1_LCD(unsigned char c)
{
    I2Cstart();
    I2Cwrite(0x4E);
    I2Cwrite(c);
    c&=0xFB;    // clear bit #2
    I2Cwrite(c);
    I2Cstop();
}

void send_2_LCD(unsigned char c, unsigned char cd)
{
    unsigned char tmp;
    
    I2Cstart();
    I2Cwrite(0x4E);
    tmp = c;
    tmp &= 0xF0;
    tmp |= cd;
    I2Cwrite(tmp);
    tmp &= 0xFB;
    I2Cwrite(tmp);
    tmp = c<<4;
    tmp |= cd;
    I2Cwrite(tmp);
    tmp &= 0xFB;
    I2Cwrite(tmp);
    I2Cstop();
}


/*
#include "ee308.h"
#include <xc.h>
#include <libpic30.h>

void init_IO(void)
{
    LATCbits.LATC4 = 1; // TTP229 SCLK HIGH
    LATAbits.LATA9 = 0;
    LATAbits.LATA10 = 0;
    TRISCbits.TRISC4 = 0;
    TRISAbits.TRISA9 = 0;
    TRISAbits.TRISA10 = 0;
}

void init_timer(void)
{
	//_T1IP = 4;	// set Timer1 priority, (4 is the default value)
  	T1CONbits.TCKPS0 = 1;
	T1CONbits.TCKPS1 = 1;	// prescaler set to 256 
    T1CONbits.TCS = 0;
    
	TMR1 = 0x0000;
	PR1	 = 3095;		// set timer1 p21548eriod to 14000 -> f = 2e6/256/1548 = 0.5Hz
    IFS0bits.T1IF = 0;
    IEC0bits.T1IE = 1;
    
	T1CONbits.TON = 1;
}

// ########################
// ### Serial functions ###
// ########################

void init_Serial(void) {
    U1BRG = 12;     // 2 MHz/(16*(12+1)) = 9615 (%0.16 error)
    RPINR18 = (RPINR18 & 0xFF00) | 0x0005; // RB5 : UART1 RX
    RPOR3 = (RPOR3 & 0xFF00)| 0x0003; // RB6 : UART1 TX
    
    U1MODEbits.UARTEN = 1;  // enable UART1
    U1STAbits.UTXEN = 1;    // enable UART1 TX
    U1STAbits.URXEN = 1;    // enable UART1 RX
    
}


void init_CNInt(void)
{
    CNEN2bits.CN22IE = 1;
    IFS1bits.CNIF = 0;
    IEC1bits.CNIE = 1;
}

// #####################
// ### I2C functions ###
// #####################

void init_I2C(void)
{
    ANSBbits.ANSB2 = 0;
    ANSBbits.ANSB3 = 0;
    I2C2BRG = 0x27;     // set BAUD rate
    I2C2CONLbits.A10M = 0;
    I2C2CONLbits.I2CEN = 1;
}

void I2Cstart(void)
{
	I2C2CONLbits.SEN = 1;		// start condition
	while(I2C2CONLbits.SEN){}	// wait while start condition in progress
}

void I2Cstop(void)
{
	I2C2CONLbits.PEN = 1;		// stop condition
	while(I2C2CONLbits.PEN){}	// wait while stop condition in progress
}

void I2Crestart(void)
{
	I2C2CONLbits.RSEN = 1;		// restart condition
	while(I2C2CONLbits.RSEN){}	// wait while restart condition in progress
}

unsigned char I2Cwrite(unsigned int c)
{
	I2C2TRN = c;
	while(I2C2STATbits.TBF){}	// wait write
	while(I2C2STATbits.TRSTAT){}	// wait slave ack	
	return(I2C2STATbits.ACKSTAT ? 0 : 1);	// not acknowledge = 0, acknowledge = 1
}

unsigned char I2Cread(void)
{
	//unsigned int c;
	I2C2CONLbits.RCEN = 1;		// master receive enable
	while(I2C2CONLbits.RCEN){}	// wait read
	return(I2C2RCV);
	//c = I2C1RCV;
	//return(c);
}

void I2Cack(void)
{
	I2C2CONLbits.ACKDT = 0;		// ACK bit 0
	I2C2CONLbits.ACKEN = 1;		// send ACK
	while(I2C2CONLbits.ACKEN){}	// wait_ack
}

void I2Cnack(void)
{
	I2C2CONLbits.ACKDT = 1;		// ACK bit 1
	I2C2CONLbits.ACKEN = 1;		// send ACK
	while(I2C2CONLbits.ACKEN){}	// wait_ack
}

// #####################
// ### LCD functions ###
// #####################

void init_LCD(void)
{
    unsigned int i;
    volatile unsigned char LCD_data[2][16] = {">               ", "                "};

    send_1_LCD(0x3C);
    __delay32(16000);
    send_1_LCD(0x3C);
    __delay32(16000);
    send_1_LCD(0x3C);
    __delay32(16000);
    send_1_LCD(0x2C);
    __delay32(16000);
    
    send_LCD_CMD(0x28);
    for (i = 0 ; i < 16000 ; i++);
    send_LCD_CMD(0x0E);
    for (i = 0 ; i < 16000 ; i++);
    send_LCD_CMD(0x01);
    for (i = 0 ; i < 16000 ; i++);
    
    for (i = 0 ; i < 16 ; i++)
        send_LCD_DATA(LCD_data[0][i]);
    send_LCD_CMD(0xC0);
    for (i = 0 ; i < 16 ; i++)
        send_LCD_DATA(LCD_data[1][i]);
    send_LCD_CMD(0x81);
}
printXY(unsigned char x, unsigned char y, unsigned int len, unsigned char* msg)
{
    unsigned char cmd;
    unsigned int  i;
    
    cmd = 0x80 + (x<<6) + y;
    send_LCD_CMD(cmd);
    for (i=0; i<len; i++)
        send_LCD_DATA(msg[i]);
}

void send_LCD_CMD(unsigned char c)
{
    send_2_LCD(c, 0x0C);
}

void send_LCD_DATA(unsigned char c)
{
    send_2_LCD(c, 0x0D);
}

void send_1_LCD(unsigned char c)
{
    I2Cstart();
    I2Cwrite(0x4E);
    I2Cwrite(c);
    c&=0xFB;    // clear bit #2
    I2Cwrite(c);
    I2Cstop();
}

void send_2_LCD(unsigned char c, unsigned char cd)
{
    unsigned char tmp;
    
    I2Cstart();
    I2Cwrite(0x4E);
    tmp = c;
    tmp &= 0xF0;
    tmp |= cd;
    I2Cwrite(tmp);
    tmp &= 0xFB;
    I2Cwrite(tmp);
    tmp = c<<4;
    tmp |= cd;
    I2Cwrite(tmp);
    tmp &= 0xFB;
    I2Cwrite(tmp);
    I2Cstop();
}
 * */