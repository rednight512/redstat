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

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#include<unistd.h>
#include<err.h>
#include<ctype.h>

#include"date_time.h"

int set_date_time(char * date_time_str, size_t date_time_str_size)
{
        time_t raw_time;
        struct tm *info;
        char cur_time[8];
        char cur_date[32];

        time(&raw_time);
        info=localtime(&raw_time);

        strftime(cur_time,sizeof(cur_time),"%H:%M",info);
        strftime(cur_date,sizeof(cur_date),"%b %d",info);

        snprintf(date_time_str, date_time_str_size, "Date ^c#FFD700^%s ^d^Time ^c#FFD700^%s ",cur_date, cur_time);

}


