void Task_NMEA_Decode(void);
void Task_Report_Drift(void);
void Decode_HeaderGPS(int);
void Decode_HeaderVHW(int);
void Decode_GPS(void);
void Decode_VHW(void);
float Convert_Number_to_Float(int, int);
int checksum(void);

struct GPS_Message{
	float Latitude;
	char Lat_Hemisphere;
	float Longitude;
	char Long_Hemisphere;
  float Speed_over_ground;			
	float Track_angle_true;
	float Mag_variation;
	unsigned char new;			//can't declare uint8_t
};

//ASCII Macros
#define ASCII_C		(0x43)
#define ASCII_E		(0x45)
#define ASCII_G 	(0x47)
#define ASCII_H		(0x48)
#define ASCII_M 	(0x4D)
#define ASCII_P 	(0x50)
#define ASCII_R 	(0x52)
#define ASCII_V		(0x56)
#define ASCII_W		(0x57)

#define ASCII_$			(0x24)
#define ASCII_Comma (0x2C)
#define ASCII_Astrisk	(0x2A)
#define ASCII_to_Decimal	(0x30)

#define VALID_GPS_MESSAGE (1)
#define VALID_VHW_MESSAGE (2)
#define NOT_RMC 	(3)

// precision from convert number to float funciton
#define TYPE_GPS (1)
#define TYPE_VHW_HEADING (1)
#define TYPE_VHW_SPEED	(0)
#define TYPE_LATITUDE 	(2)
#define TYPE_LONGITUDE	(3)
#define TYPE_HEADING_HUNDREDS (4)
#define TYPE_HEADING_TENS			(5)
#define TYPE_HEADING_ONES			(6)
#define GPS_COMMA_MAX		(12)
#define VHW_COMMA_MAX		(9)
#define INDEX_DATA_BEGIN (6)
#define TEN_THOUSANDS (10000)
#define THOUSANDS	(1000)
#define HUNDREDS (100)
#define TENS			(10)

//cases
#define CASE_GPS_SPEED (7)
#define CASE_GPS_HEADING (8)
#define CASE_GPS_MagVar (10)
#define CASE_GPS_Dir		(11)
#define CASE_VHW_HEADING (3)
#define CASE_VHW_Dir		(4)
#define CASE_VHW_SPEED (5)

#define ADVANCE_PAST_DECIMAL (2)
#define CHECKSUM_SIZE (2)
#define CHECKSUM_BEGIN (1)
#define HEX_BASE (16)
