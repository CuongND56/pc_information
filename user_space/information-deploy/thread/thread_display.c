#include "thread_manager.h"
#include <fcntl.h>

#define DEV_FILE    "/dev/ili9341_device"
#define STR_MAX     100

#define RING_METER  "ring_meter"

static long map(long x, long in_min, long in_max, long out_min, long out_max);
static int ringMeter1(int fd, int value, int vmin, int vmax, int x, int y, int r, int w, uint16_t bcolor, uint16_t scheme);
static int open_device_file();
static uint16_t rainbow(uint16_t value);

static long map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

static int ringMeter1(int fd, int value, int vmin, int vmax, int x, int y, int r, int w, uint16_t bcolor, uint16_t scheme) {
	
	// Minimum value of r is about 52 before value text intrudes on ring
	// drawing the text first is an option
										   
												   //  int w = r / 8;    // Width of outer ring is 1/4 of radius
	int angle;							   // Half the sweep angle of meter (300 degrees)
												   //  int text_colour = 0; // To hold the text colour
	int v;											 // Map the value to an angle v
	uint16_t seg = 5;							   // Segments are 5 degrees wide = 60 segments for 300 degrees
	uint16_t inc = 5;							   // Draw segments every 5 degrees, increase to 10 for segmented ring
	int colour = 0;
	int i;
	float sx, sy, sx2, sy2;
	uint16_t x0, y0, x1, y1;
	int x2, y2, x3, y3;
    char str[STR_MAX] = {0};

	angle = 150;
	v = map(value, vmin, vmax, -angle, angle);
	x += r;											
	y += r;	

	// Draw colour blocks every inc degrees
	for (i = -angle; i < angle; i += inc) {

        colour = 0;
		switch (scheme) {
			case 0:
				colour = RED;
				break; // Fixed colour
			case 1:
				colour = GREEN;
				break; // Fixed colour
			case 2:
				colour = BLUE;
				break; // Fixed colour
			case 3:
				colour = rainbow(map(i, -angle, angle, 0, 127));
				break; // Full spectrum blue to red
			case 4:
				colour = rainbow(map(i, -angle, angle, 63, 127));
				break; // Green to red (high temperature etc)
			case 5:
				colour = rainbow(map(i, -angle, angle, 127, 63));
				break; // Red to green (low battery etc)
			case 6:
				colour = BLACK;
				break;
			case 7:
				colour = PINK;
				break;
			default:
				colour = BLUE;
				break; // Fixed colour
		}

		// Calculate pair of coordinates for segment start
		sx = cos((i - 90) * 0.0174532925f);
		sy = sin((i - 90) * 0.0174532925f);
		x0 = sx * (r - w) + x;
		y0 = sy * (r - w) + y;
		x1 = sx * r + x;
		y1 = sy * r + y;

		// Calculate pair of coordinates for segment end
		sx2 = cos((i + seg - 90) * 0.0174532925f);
		sy2 = sin((i + seg - 90) * 0.0174532925f);
		x2 = sx2 * (r - w) + x;
		y2 = sy2 * (r - w) + y;
		x3 = sx2 * r + x;
		y3 = sy2 * r + y;

		if (i < v) {	

            memset(str, 0, STR_MAX);
            sprintf(str, "%s %d %d %d %d %d %d %d", RING_METER, x0, y0, x1, y1, x2, y2, colour);
            write(fd, str, strlen(str));

            memset(str, 0, STR_MAX);
            sprintf(str, "%s %d %d %d %d %d %d %d", RING_METER, x1, y1, x2, y2, x3, y3, colour);
            write(fd, str, strlen(str));

		} else {
			
            memset(str, 0, STR_MAX);
            sprintf(str, "%s %d %d %d %d %d %d %d", RING_METER, x0, y0, x1, y1, x2, y2, bcolor);
            write(fd, str, strlen(str));

            memset(str, 0, STR_MAX);
            sprintf(str, "%s %d %d %d %d %d %d %d", RING_METER, x1, y1, x2, y2, x3, y3, bcolor);
            write(fd, str, strlen(str));
		}
	}
	return x + r;
}

static uint16_t rainbow(uint16_t value) {

	// Value is expected to be in range 0-127
	// The value is converted to a spectrum colour from 0 = blue through to 127 = red

	uint16_t red = 0; // Red is the top 5 bits of a 16 bit colour value
	uint16_t green = 0;// Green is the middle 6 bits
	uint16_t blue = 0; // Blue is the bottom 5 bits

	uint16_t quadrant = value / 32;

	switch (quadrant) {

		case 0:
			blue = 31;
			green = 2 * (value % 32);
			red = 0;
			break;

		case 1:
			blue = 31 - (value % 32);
			green = 63;
			red = 0;
			break;
		
		case 2:
			blue = 0;
			green = 63;
			red = value % 32;
			break;
		
		case 3:
			blue = 0;
			green = 63 - 2 * (value % 32);
			red = 31;
			break;
		
		default:
			break;

	}

	return (red << 11) + (green << 5) + blue;
}

static int open_device_file() {
    int fd = open(DEV_FILE, O_WRONLY);
    if (fd == -1) {
        printf("Open device file failed. Please check the dev folder\n");
        exit(EXIT_FAILURE);
    }
    return fd;
}


void *thread_display(void *args) {

    int status, fd;
    struct Data mData = {0};
    struct Thread_worker *mWorker = (struct Thread_worker *)args;

    fd = open_device_file();
    ringMeter1(fd, 500, 0, 1020, 30, 20, 50, 10, 0xFFE0, 0);
    ringMeter1(fd, 750, 0, 1020, 30, 130, 50, 10, YELLOW, 2);

    while (1) {
        
        pthread_mutex_lock(&(mWorker->mManager->lock));
        while (mWorker->mManager->event != READ) {
            pthread_cond_wait(&(mWorker->mManager->cond), &(mWorker->mManager->lock));
        }

        mWorker->mManager->event = DRAWING;

        if (mData.cpu_stat.cpu_num == 0 && mWorker->mManager->inform.cpu_stat.cpu_num != 0) {
            mData.cpu_stat.cpu = (struct cpu_struct_t *)malloc(mWorker->mManager->inform.cpu_stat.cpu_num * sizeof(struct cpu_struct_t));
        }

        mData = mWorker->mManager->inform;
        memcpy(mData.cpu_stat.cpu, mWorker->mManager->inform.cpu_stat.cpu, mData.cpu_stat.cpu_num * sizeof(struct cpu_struct_t));

        _print_proc_stat(&mData.cpu_stat);

        pthread_mutex_unlock(&(mWorker->mManager->lock));



        // display 
    }
}