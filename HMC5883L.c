#include <MKL25Z4.H>
#include "HMC5883L.h"
#include "I2C.h"
#include "delay.h"
#include "stdio.h"
#include "math.h"
#include "GPS.h"
#include "GPIO_defs.h"
#include "LEDs.h"

extern struct GPS_Message GPS_Message;

int init_compass()
{
	// 1. Write CRA (00) – send 0x3C 0x00 0x70 (8-average, 15 Hz default, normal measurement)
	i2c1_write_byte(HMC_ADDR, REG_ConfigA, 0x70);
	Delay(50);
	// 2. Write CRB (01) – send 0x3C 0x01 0x20 (Gain=1.3, or any other desired gain)
	i2c1_write_byte(HMC_ADDR, REG_ConfigB, 0x20);
	Delay(50);
	// 3. Write Mode (02) – send 0x3C 0x02 0x00 (single-measurement mode
	i2c1_write_byte(HMC_ADDR, REG_Mode, 0x00);
	Delay(50);
	// 4. Wait 6 ms or monitor status register or DRDY hardware interrupt pin
	//Delay(100);
	return 0;
}

void read_full_compass(){
	int i;
	int8_t compass_data[6];
	int16_t raw_x =0, raw_y =0, raw_z =0;
	float adjusted_heading;
	Delay(400);
	// 5. Send 0x3D 0x06 (Read all 6 bytes. If gain is changed then this data set is using previous gain)
	i2c1_start();
	i2c1_read_setup(HMC_ADDR, REG_XHI);
	for(i=0; i<6; i++){
		compass_data[i] = 0;
	}
	compass_data[0] = (int8_t) i2c1_read_byte(HMC_ADDR, REG_XHI);
	Delay(6);
	compass_data[1] = (int8_t) i2c1_read_byte(HMC_ADDR, REG_XLO);
	Delay(6);
	compass_data[2] = (int8_t) i2c1_read_byte(HMC_ADDR, REG_ZHI);
	Delay(6);
	compass_data[3] = (int8_t) i2c1_read_byte(HMC_ADDR, REG_ZLO);
	Delay(6);
	compass_data[4] = (int8_t) i2c1_read_byte(HMC_ADDR, REG_YHI);
	Delay(6);
	compass_data[5] = (int8_t) i2c1_read_byte(HMC_ADDR, REG_YLO);
	Delay(100);
	raw_x = (int16_t) ((compass_data[0]<<8) | compass_data[1]);
	raw_z = (int16_t) ((compass_data[2]<<8) | compass_data[3]);
	raw_y = (int16_t) ((compass_data[4]<<8) | compass_data[5]);
	printf("%i %i %i \r\n", raw_x, raw_y, raw_z);
	//calculations here
	adjusted_heading = calculate_heading(raw_x, raw_y, raw_z);
	control_antenna(adjusted_heading);
}

float calculate_heading(int16_t x, int16_t y, int16_t z){
	float heading =0, headingDegrees, x_mag, y_mag, z_mag;
	float satelliteLat = 0.1599;				// double check signs on these two
	float satelliteLon = 98.5151;
	float x_pos, y_pos;
	float declinationAngle = -0.15998852058;		//radians
	//xraw and y raw settings?
	x_mag = (float) x / _hmc5883_Gauss_LSB_XY * SENSORS_GAUSS_TO_MICROTESLA;
	y_mag = (float) y / _hmc5883_Gauss_LSB_XY * SENSORS_GAUSS_TO_MICROTESLA;
	z_mag = (float) z / _hmc5883_Gauss_LSB_Z 	* SENSORS_GAUSS_TO_MICROTESLA;
	heading = atan2(y_mag, x_mag);
	
	y_pos = (float) sin((satelliteLon*M_PI/180)-(GPS_Message.Longitude * M_PI/180))*cos(satelliteLat*M_PI/180);
	x_pos = (float) (cos(GPS_Message.Latitude*M_PI/180)*sin(satelliteLat*M_PI/180))-(sin(GPS_Message.Latitude*M_PI/180)*cos(satelliteLat*M_PI/180)*cos((satelliteLon*M_PI/180)-(GPS_Message.Longitude*M_PI/180)));
	heading = atan2(y_pos, x_pos) * (180/M_PI);
	declinationAngle += heading*(M_PI/180);
	headingDegrees = heading *180/M_PI;
	printf("Compass Heading: %f\n", headingDegrees);
	return headingDegrees;
}

void control_antenna(float heading) {
	// arguement is in degrees
	// correct to within 0-360
	if(heading < 0){
		heading += 360;
	}
	else if(heading >360){
		heading -= 360;
	}
//	if((heading > 0) && (heading < 45)){
//		// NORTH -> S.O.L.
//	}
	if((heading > 0) && (heading <= 180)){
		// LEFT ANTENNA
		Control_RGB_LEDs(1,0,0);		// RED
		PTB->PSOR	=	MASK(LEFT_ANTENNA_POS);		// LEFT_ANTENNA off
		PTB->PCOR	=	MASK(RIGHT_ANTENNA_POS);	// right antenna on
	}
//	else if((heading > 135) && (heading < 225)){
//		// EAST -> RIGHT ANTENNA
//	}
	else if((heading > 180)){
		// (RIGHT)
		Control_RGB_LEDs(0,1,0);	// GREEN
		PTB->PSOR = MASK(RIGHT_ANTENNA_POS);		// right_antenna off
		PTB->PCOR	=	MASK(LEFT_ANTENNA_POS);			// LEFT_ANTENNA on
	}
//	else printf("ERROR in heading!");
}

