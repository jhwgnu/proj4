# include <stdio.h>
# include <unistd.h>
# include <ctype.h>
# include <stdlib.h>
# include <errno.h>
# include <string.h>

#define MAX_LNG_FRACTIONAL 999999
#define MIN_LNG_FRACTIONAL 0
#define MAX_LNG_INTEGER 180
#define MIN_LNG_INTEGER -180
#define MAX_LAT_INTEGER 90
#define MIN_LAT_INTEGER -90
#define MAX_LAT_FRACTIONAL 999999
#define MIN_LAT_FRACTIONAL 0
// #define MAX_ACCURACY 999999
#define MIN_ACCURACY 0
#define PRECISION 30
#define UNIT 1000000

long find_distance(int ,int, int, int, int, int, int, int);

struct gps_location {
	int lat_int;
	int lat_frac;
	int lng_int;
	int lng_frac;
	int accuracy;
};

int main(){
	struct gps_location location1 = {30,12345,140,234546,3};
	struct gps_location location2 = {10,25654,100,623455,3};

	long result_long = find_distance(location1.lat_int, location1.lat_frac, location1.lng_int, location1.lng_frac,
	location2.lat_int, location2.lat_frac, location2.lng_int, location2.lng_frac);
	printf("%ld", result_long);
	
//	double result
}

long degree2radian(long degree){
	//degree * pi * 2^(14) / 180
	return (degree*3373259426) / 180;
}

long talyor_square(long x, long y){
	long result = x;
	if (y<=1){
		return x;
	} else if (y==2){
		return (x*x)>>(PRECISION+1);
	} else {
		result = (x*x)>>(PRECISION+1);
		int count = 3;
		while(count<=y){
			result = (result*x/count)>>PRECISION;
			count++;
		}
		return result;
	}
}

long cosine(long x){
	// cos(x) = 1 - x^2/2! + x^4/4! - x^6/6! + x^8/8!
	long r_x = degree2radian(x);
	return (1<<PRECISION)-talyor_square(r_x,2)+talyor_square(r_x,4)-talyor_square(r_x,6)+talyor_square(r_x,8)-talyor_square(r_x,10)+talyor_square(r_x,12)-talyor_square(r_x,14);
}

long sine(long x){
	// sin(x) = x - x^3/3! + x^5/5! - x^7/7! + x^9/9!
	long r_x = degree2radian(x);
	return r_x-talyor_square(r_x,3)+talyor_square(r_x,5)-talyor_square(r_x,7)+talyor_square(r_x,9)-talyor_square(r_x,11)+talyor_square(r_x,13)-talyor_square(r_x,15);
}

long multi_6400000(long x){
	// 6400000 = 2**22 + 2**21 + 2**16 + 2**15 + 2**13 + 2**11
	return (x>>38)+(x>>39)+(x>>44)+(x>>45)+(x>>47)+(x>>49);
}

long multi_6400000_z(long x){
	// 6400000 = 2**22 + 2**21 + 2**16 + 2**15 + 2**13 + 2**11
	return (x>>8)+(x>>9)+(x>>14)+(x>>15)+(x>>17)+(x>>19);
}

void find_xyz(int lat_integer,int lat_fractional,int lng_integer,int lng_fractional,long *x,long *y,long *z){
	// find the coordinate of x,y,z on earth
	long cos_lat_i = cosine(lat_integer);
	long cos_lat_i_1 = cosine(lat_integer+1);
	long cos_lat   = (cos_lat_i_1 * lat_fractional + cos_lat_i * (UNIT-lat_fractional))/UNIT;
	
	long sin_lat_i = sine(lat_integer);
	long sin_lat_i_1 = sine(lat_integer+1);
	long sin_lat   = (sin_lat_i_1 * lat_fractional + sin_lat_i * (UNIT-lat_fractional))/UNIT;

	long cos_lng_i = cosine(lng_integer);
	long cos_lng_i_1 = cosine(lng_integer+1);
	long cos_lng   = (cos_lng_i_1 * lng_fractional + cos_lng_i * (UNIT-lng_fractional))/UNIT;
	
	long sin_lng_i = sine(lng_integer);
	long sin_lng_i_1 = sine(lng_integer+1);
	long sin_lng   = (sin_lng_i_1 * lng_fractional + sin_lng_i * (UNIT-lng_fractional))/UNIT;
	
	*x = multi_6400000(cos_lat*sin_lng);
	*y = multi_6400000(cos_lat*cos_lng);
	*z = multi_6400000_z(sin_lat);
}

long find_distance(int lat_integer,int lat_fractional,int lng_integer,int lng_fractional, int lat_integer_1, int lat_fractional_1,int lng_integer_1, int lng_fractional_1){
	// find the distance between two coordinates on earth.
	long x_d, y_d, z_d;
	long x, x_1, y, y_1, z, z_1;
	int i;
	find_xyz(lat_integer,lat_fractional,lng_integer,lng_fractional,&x,&y,&z);
	find_xyz(lat_integer_1,lat_fractional_1,lng_integer_1,lng_fractional_1,&x_1,&y_1,&z_1);

    long offset = 0;
    long result;
    long msb = 0;
    long sqrt_x;

	if (x>x_1)
		x_d = x-x_1;
	else 
		x_d = x_1-x;
	
	if(y>y_1)
		y_d = y-y_1;
	else
		y_d = y_1-y;
	
	if(z>z_1)
		z_d = z-z_1;
	else
		z_d = z_1-z;
	
	long temp_x = x_d;
	long temp_y = y_d;
	long temp_z = z_d;

	for (i = 1;i<sizeof(long)*8;i++){
		if ((temp_x & 1) | (temp_y & 1) | (temp_z & 1))
			msb = i;
		temp_x >>= 1;
		temp_y >>= 1;
		temp_z >>= 1;
	}
    
	if (msb>31){
		offset = msb-30;
		x_d>>=offset;
		y_d>>=offset;
		z_d>>=offset;
		
		sqrt_x = 1<<15;

	} else{
		sqrt_x = 1<<(msb/2);
	}
	x_d*=x_d;
	y_d*=y_d;
	z_d*=z_d;
	result = x_d+y_d+z_d;
	
	for (i = 1;i<100;i++){
		sqrt_x = (sqrt_x + (result/sqrt_x))>>1;
		if (sqrt_x == 0)
			sqrt_x = 1;
	}
	result = sqrt_x;
	result <<=offset;

	return result;
}
