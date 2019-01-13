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
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 ****************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<string.h>
#include<sys/ioctl.h>
#include<unistd.h>
#include<err.h>
#include<ctype.h>
#include<soundcard.h>

#include"vol.h"

int get_volume(void)
{
        size_t i;
        int afd, devmask, r, vol, len;
        char *vnames[] = SOUND_DEVICE_NAMES;

        if ((afd = open("/dev/mixer" , O_RDONLY | O_NONBLOCK)) < 0) {
                warn("open '%s':", "/dev/mixer");
                return -1;
        }

        if (ioctl(afd, (int)SOUND_MIXER_READ_DEVMASK, &devmask) < 0) {
                warn("ioctl 'SOUND_MIXER_READ_DEVMASK':");
                close(afd);
                return -1;
        }

	len=sizeof(vnames)/sizeof*(vnames);

        for (i = 0; i < len; i++) {
                if (devmask & (1 << i) && !strcmp("vol", vnames[i])) {
                        if (ioctl(afd, MIXER_READ(i), &vol) < 0) {
                                warn("ioctl 'MIXER_READ(%ld)':", i);
                                close(afd);
                                return -1;
                        }
                }
        }

        close(afd);

        vol=(vol & 0xff);
        if(vol != 0){
                r=vol%10;
                vol=vol/10;
                if(r >= 5)vol++;
        }

	return vol;

}



int set_vol(int vol, char * vol_str, size_t vol_str_size)
{
        int i;

        snprintf(vol_str, vol_str_size, "Volume ^c#FF0000^");
        for(i=0;i<10;i++){
                if(i==vol){
                        strncat(vol_str, "^c#444444^", vol_str_size);
                }
                strncat(vol_str, "^r0,2,2,10^^f3^", vol_str_size);
        }


}

