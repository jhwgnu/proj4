#include <stdio.h>
#include <math.h>

#define PI 3.14159265358979323846264338327950288419

double CALC_DISTANCE(double, double, double, double);
double GET_LAT_DIFF(double, double);
double GET_LON_DIFF(double, double);

int main(){
    double lon1, lat1, lon2, lat2;
    double result;    
    result = CALC_DISTANCE(30, 120, 25, -117);
//    printf(result);

    return 0;
}

double CALC_DISTANCE(double lat1, double lon1, double lat2, double lon2){
	int r = 6371;
	double lat_diff, lon_diff;
	double lat_diff_rad, lon_diff_rad;
	lat_diff = fabs(lat1-lat2);
	lon_diff = (fabs(lon1-lon2) >= 360-fabs(lon1-lon2)) ? 360-fabs(lon1-lon2) : fabs(lon1-lon2);
	
	printf("lat_diff = %f\n",lat_diff);
	printf("lon_diff = %f\n",lon_diff);	
	lat_diff = lat_diff*PI/180;
	lon_diff = lon_diff*PI/180;	
	printf("lat_diff = %f\n",lat_diff);
	printf("lon_diff = %f\n",lon_diff);
	
	printf("cos(lat_diff) = %f\n",cos(lat_diff));
	printf("cos(lon_diff) = %f\n",cos(lon_diff));	
	
	printf("test: %f\n",cos(lat_diff)*cos(lon_diff));
	double degree = acos(cos(lat_diff)*cos(lon_diff))*180/PI;
	// 그림 통해서 도출한 공식 degree = arccos(cos(lat_diff)*cos(long_diff)) 
	printf("degree: %f\n",degree);
	
	double result = 2*PI*r*degree/360;
	
	printf("%f", result);
		
	return 0;
}

