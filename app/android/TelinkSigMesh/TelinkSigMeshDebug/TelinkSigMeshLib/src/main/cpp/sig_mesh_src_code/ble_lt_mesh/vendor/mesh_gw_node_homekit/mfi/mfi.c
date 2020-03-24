#include "mfi.h"


///////////////  I2C simulation ////////////////////////////////////

#ifndef PIN_I2C_SCL
#define PIN_I2C_SCL				GPIO_PA4
#endif
#ifndef PIN_I2C_SDA
#define PIN_I2C_SDA				GPIO_PA3
#endif

static inline void i2c_sim_wait(void){
}

void i2c_sim_long_wait(void){
	CLOCK_DLY_600NS;
}

// Pulling the line to ground is considered a logical zero while letting the line float is a logical one.   http://en.wikipedia.org/wiki/I%C2%B2C
static inline void i2c_sim_scl_out(int v){
	gpio_set_output_en(PIN_I2C_SCL,(!v));
}

static inline int i2c_sim_scl_in(void){
	return gpio_read(PIN_I2C_SCL);
}

// Pulling the line to ground is considered a logical zero while letting the line float is a logical one.   http://en.wikipedia.org/wiki/I%C2%B2C
static inline void i2c_sim_sda_out(int v){
	gpio_set_output_en(PIN_I2C_SDA,(!v));
}

static inline int i2c_sim_sda_in(void){
	return gpio_read(PIN_I2C_SDA);
}

static inline void i2c_sim_scl_init(void){
	gpio_set_func(PIN_I2C_SCL, AS_GPIO);
}

static inline void i2c_sim_sda_init(void){
	gpio_set_func(PIN_I2C_SDA, AS_GPIO);
	gpio_set_input_en(PIN_I2C_SDA, 1);
}

static inline void i2c_sim_scl_idle(void){
	gpio_set_output_en(PIN_I2C_SCL, 0);
	gpio_write(PIN_I2C_SCL, 0);
}

static inline void i2c_sim_sda_idle(void){
	gpio_set_output_en(PIN_I2C_SDA, 0);
	gpio_write(PIN_I2C_SDA, 0);
}

void i2c_sim_init(){}

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
\\ void i2c_sim_start(void)
\\   Sets clock high, then data high.  This will do a stop if data was low.
\\   Then sets data low, which should be a start condition.
\\   After executing, data is left low, while clock is left high
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/
void i2c_sim_start(void)
{
	i2c_sim_scl_init();
	i2c_sim_sda_init();
	i2c_sim_sda_idle();
	i2c_sim_scl_idle();
	i2c_sim_sda_out(0);		//sda: 0
	i2c_sim_wait();

}

/*
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
\\ void i2c_sim_stop(void)
\\  puts data low, then clock low,
\\  then clock high, then data high.
\\  This should cause a stop, which
\\  should idle the bus, I.E. both clk and data are high.
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/
void i2c_sim_stop(void)
{
	i2c_sim_sda_out(0);
	i2c_sim_wait();
	i2c_sim_scl_out(0);
	i2c_sim_wait();
	i2c_sim_scl_out(1);
	i2c_sim_wait();
	i2c_sim_sda_out(1);
}

static void i2c_sim_wirte_bit(int bit)
{
	i2c_sim_scl_out(0);
	i2c_sim_sda_out(bit);
	i2c_sim_long_wait();
	i2c_sim_scl_out(1);
}

// Read a bit from I2C bus
static int i2c_sim_read_bit(void){
	i2c_sim_wirte_bit(1);
	return i2c_sim_sda_in();
}


int i2c_sim_write_byte(u8 dat){
	int i = 0x80;
	while(i){
		i2c_sim_wirte_bit((dat & i));
		i = i >> 1;
	}
	return i2c_sim_read_bit();
}

u8 i2c_sim_read_byte(int last){
	u8 dat = 0;

	for( int i=0; i<8; i++ ){
		i2c_sim_wirte_bit(1);
		if(i2c_sim_sda_in()){
			dat =(dat << 1) | 0x01;
		}else{
			dat = dat << 1;
		}
	}
	i2c_sim_wirte_bit(last);
	return dat;
}

void i2c_sim_write(u8 id, u8 addr, u8 dat)
{
	i2c_sim_start();
	i2c_sim_write_byte(id);
	i2c_sim_write_byte(addr);
	i2c_sim_write_byte(dat);
	i2c_sim_stop();
}

u8 i2c_sim_read(u8 id, u8 addr)
{
	u8 dat;
	i2c_sim_burst_read(id, addr, &dat, 1);
	return dat;
}

//int		aai2c;
int	i2c_sim_write_byte_ack(u8 id)
{
	int num = 0;
	int nak = 1;
	while (nak && num++ < 2000)
	{
		nak = i2c_sim_write_byte (id);
		if (!nak)
		{
			return 0;
		}
		sleep_us (500);
		//i2c_sim_stop();
		i2c_sim_start();
	}
//	if (num > aai2c)
//		aai2c = num;
	i2c_sim_stop();
	return 1;
}

void i2c_sim_burst_read(u8 id, u8 addr,u8 *p,u8 n)
{
	i2c_sim_start();

	if (i2c_sim_write_byte_ack(id))
	{
		while (1);
		return;
	}
	i2c_sim_write_byte(addr);
	i2c_sim_stop();

	i2c_sim_start();

	if (i2c_sim_write_byte_ack(id | 1))
	{
		while (1);
		return;
	}

	for(int k = 0; k < n; ++k){
		*p++ = i2c_sim_read_byte( k ==(n-1) );
	}
	i2c_sim_stop();

}

void i2c_sim_burst_write(u8 id, u8 addr,u8 *p,u8 n)
{
	i2c_sim_start();

	if (i2c_sim_write_byte_ack(id))
	{
		while (1);
		return;
	}

	i2c_sim_write_byte(addr);

	for(int i=0; i<n; i++ ){
		i2c_sim_write_byte(*p++);
	}
	i2c_sim_stop();
}


bool app_verify_mfi_i2c(u8 * id_data)
{
	bool verify_true = false;

	#ifdef MFI_ENABLE
	#define SLAVE_ADDR		MFI_ENABLE
	#define DEV_ID_ADDR 	0x04
	#if (MFI_AUTH_CHIP_VER == MFI_AUTH_2_0_C)
		#define MFI_I2C_CHIP_ID	0x00000200
	#elif (MFI_AUTH_CHIP_VER == MFI_AUTH_3_0)
		#define MFI_I2C_CHIP_ID	0x00000300
	#endif
	
	i2c_sim_burst_read (SLAVE_ADDR, DEV_ID_ADDR, id_data, 4);

	/*for big-endian refer to Accessory Interface Specification R27.*/
	u32 i2c_id = ((id_data[0] << 24) | (id_data[1] << 16) | (id_data[2] << 8) | (id_data[3] << 0));
	
	if(i2c_id == MFI_I2C_CHIP_ID)
	{
		verify_true = true;
	}
	#endif

	return verify_true;
}


