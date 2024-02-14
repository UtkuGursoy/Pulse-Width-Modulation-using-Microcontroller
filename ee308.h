#ifndef EE308
#define EE308

#define FOSC    (8000000ULL)
#define FCY     (FOSC/2)

void init_IO(void);
void init_timer(void);
void init_CNInt(void);

// Serial Functions
void init_Serial(void);

// I2C functions
void init_I2C(void);
void I2Cstart();
void I2Cstop(void);
void I2Crestart(void);
unsigned char I2Cwrite(unsigned int c);
unsigned char I2Cread(void);
void I2Cack(void);
void I2Cnack(void);

// LCD functions
void init_LCD(void);
void send_LCD_CMD(unsigned char);
void send_LCD_DATA(unsigned char);
void send_1_LCD(unsigned char);
void send_2_LCD(unsigned char, unsigned char);
void printXY(unsigned char, unsigned char, unsigned int, unsigned char*);

#endif