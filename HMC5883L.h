#ifndef HMC5883L_H
#define HMC5883L_H
#include <stdint.h>

#define HMC_ADDR 	0x3C	// last bit changed to one when reading

// register definitions
#define REG_ConfigA 	0x00
#define REG_ConfigB		0x01
#define REG_Mode			0x02
#define REG_XHI				0x03
#define REG_XLO				0x04
#define REG_ZHI				0x05
#define REG_ZLO				0x06
#define REG_YHI				0x07
#define REG_YLO				0x08
#define REG_Status		0x09
#define REG_IdentA		0x10		//double check before using
#define REG_IdentB		0x11
#define REG_IdentC		0x12

#define _hmc5883_Gauss_LSB_XY  1100
#define _hmc5883_Gauss_LSB_Z   980 
#define SENSORS_GAUSS_TO_MICROTESLA (100)



//#define REG_WHOAMI 0x0D
//#define REG_CTRL1  0x2A
//#define REG_CTRL4  0x2D

//#define WHOAMI 0x1A

//#define COUNTS_PER_G (16384.0)
#define M_PI (3.14159265)

int init_compass(void);
void read_full_compass(void);
float calculate_heading(int16_t x, int16_t y, int16_t z);
void control_antenna(float heading);
//void read_xyz(void);
//void convert_xyz_to_roll_pitch(void);

//extern float roll, pitch;
//extern int16_t acc_X, acc_Y, acc_Z;

#endif
