#include <stdio.h>
#include <string.h>
#include <strlib.h>

#define __NR_get_gps_location 380
struct gps_locater_user {
	int lat_integer;
	int lat_fractional;
	int lng_integer;
	int lng_fractional;
	int accuracy;
};
