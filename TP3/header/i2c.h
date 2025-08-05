#ifndef I2CH
#define I2CH

void i2c_init(void);
void i2c_start(void);
void i2c_write(unsigned char data);
unsigned char i2c_read(unsigned char isLast);
void i2c_stop();




#endif /* I2CH */