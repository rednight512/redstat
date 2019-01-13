/******************************************************************************
 * redstat
 * Satus bar program for dwm on NetBSD. Uses the Status2d patch to
 * draw battery and volume.
 *
 * Copyright (c) 2019, Michael Jensen All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer
 *     in the documentation and/or other materials provided with the
 *     distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OFUSE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISEDOF THE
 * POSSIBILITY OF SUCH DAMAGE.
 ****************************************************************************/

#define INTERVAL 10

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<err.h>
#include<ctype.h>
#include<X11/Xlib.h>

#include"vol.h"
#include"date_time.h"
#include"redstat.h"
#include"batt.h"


int main(void)
{


	struct batt_info batt0;
	struct batt_info batt1;

	int volume;
	int batt_level=0;
	int dirty=0;

	char date_time_str[128];
	char volume_stat_str[256];
	char batt_bar_str[256];

        char buf[1024];
        int cur_vol;
        int time_out=INTERVAL;
	int batt_time=0;
	int charge_blink=0;
	int charge_time=0;

	sprintf(date_time_str, "");
	sprintf(volume_stat_str, "");
	sprintf(batt_bar_str, "");
	sprintf(buf, "");

        volume=get_volume();
        /* set local var for volume */
	volume=get_volume();
	cur_vol=volume;
	set_vol(volume, volume_stat_str, sizeof(volume_stat_str));

	set_date_time(date_time_str, sizeof(date_time_str));
	batt0=get_batt(0);
	batt1=get_batt(1);

	/* set update flag */
	dirty=1;

        for(;;){
                volume=get_volume();
                if(cur_vol != volume){
                        cur_vol=volume;
			set_vol(volume, volume_stat_str, sizeof(volume_stat_str));
			dirty=1;
                }
		/* set blink mode for charge indicator */
		if(charge_time < 10){
			charge_time++;
		}else{
			if(charge_blink){
				charge_blink=0;
			} else {
				charge_blink=1;
			}
			charge_time=0;
		}

                if(time_out <= 0){
			if(batt_time >= 10){
				set_date_time(date_time_str, sizeof(date_time_str));
				batt0=get_batt(0);
				batt1=get_batt(1);
				batt_time=0;
			} else {
				batt_time++;
			}

                        time_out=INTERVAL;
			dirty=1;
                } else {
                        time_out--;
                }

                if(dirty){
/****************************************************************************
	This is where we create the string for the status bar
****************************************************************************/
			
			sprintf(buf, "");
			snprintf(buf, sizeof(buf), "%s", volume_stat_str);

			/* split bar */
			strncat(buf, "^f4^^d^^r0,0,1,14^^f4^", sizeof(buf));

			batt_level=batt0.percent;
			if(batt_level>0){
				batt_level=(batt_level *2) / 10;
			}

			set_batt_level(batt_level, 0, batt_bar_str, sizeof(batt_bar_str));
			strncat(buf, batt_bar_str, sizeof(buf));

			strncat(buf, "^f4^^d^^r0,0,1,14^^f4^", sizeof(buf));

			if(batt1.is_charging && charge_blink){
				batt_level=0;
			} else {	
				batt_level=batt1.percent;
			}
                        if(batt_level>0){
                                batt_level=(batt_level *2) / 10;
                        }
                        set_batt_level(batt_level, 1, batt_bar_str, sizeof(batt_bar_str));
                        strncat(buf, batt_bar_str, sizeof(buf));


			/* split bar */
			strncat(buf, "^f4^^d^^r0,0,1,14^^f4^", sizeof(buf));
			strncat(buf, date_time_str, sizeof(buf));

/***************************************************************************/

                        set_stat_Bar(buf);

                        dirty=0;
                }

                usleep(100000);
        }
}

void set_stat_Bar(char *buf)
{
        Display *disp;

        /* open X display and set name */

        disp = XOpenDisplay(NULL);
        if(disp == NULL){
                printf("failed to open display\n");
                exit(1);
        }

        if (XStoreName( disp, DefaultRootWindow(disp), buf ) < 0){
                printf("Error setting root windows name\n");
                exit(1);
        }

        if (XCloseDisplay(disp) < 0){
                printf("Error closing display\n");
                exit(1);
        }

}

