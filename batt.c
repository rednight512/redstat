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
#include<prop/proplib.h>
#include<sys/envsys.h>

#include"batt.h"

struct batt_info get_batt(int batt_no)
{

	int fd;
	struct batt_info batt;

	prop_dictionary_t dict;
	prop_array_t array;
	prop_object_iterator_t dict_iter;
	prop_object_iterator_t array_iter;
	prop_object_t dict_obj;
	prop_object_t array_obj;
	prop_object_t batt_obj;

	char *device_tag;

	batt.full_design=-1;
	batt.remaining=-1;
	batt.discharge_rate=-1;
	batt.voltage=-1;
	batt.percent=-1;
	batt.last_full_cap=-1;
	batt.is_charging=0;
	batt.watt_as_unit=0;

	asprintf(&device_tag, "acpibat%d", batt_no);


	/* open sysmon device */

	if((fd=open("/dev/sysmon", O_RDONLY)) == -1){
		perror("/dev/sysmon: ");
		exit(1);
	}

	if(prop_dictionary_recv_ioctl(fd, ENVSYS_GETDICTIONARY, &dict)==-1){
		perror("Get dictionary: ");
		close(fd);
		exit(1);
	}

	if(prop_dictionary_count(dict)==0){
		prop_object_release(dict);
		close(fd);
		fprintf(stderr, "Dictionary Empty\n");
		exit(1);
	}

	
	if((dict_iter=prop_dictionary_iterator(dict))==NULL){
		prop_object_release(dict);
		close(fd);
		fprintf(stderr, "Failed to get iterator\n");
		exit(1);
	}
	
	/* loping though the dictionaries */

	while((dict_obj=prop_object_iterator_next(dict_iter))!=NULL){
		/* skip irrelevant dicts */
		if((strlen(prop_dictionary_keysym_cstring_nocopy(dict_obj)) ==
                   strlen(device_tag)) &&
		   (strncmp(device_tag, prop_dictionary_keysym_cstring_nocopy(dict_obj),
                   strlen(device_tag)) != 0)) continue;

		array=prop_dictionary_get_keysym(dict, dict_obj);
		if(prop_object_type(array)!=PROP_TYPE_ARRAY){
			prop_object_iterator_release(dict_iter);
			prop_object_release(dict);
			close(fd);
			fprintf(stderr, "Returned object not an array\n");
			exit(1);
		}

		if(!(array_iter=prop_array_iterator(array))){
			prop_object_iterator_release(dict_iter);
			prop_object_release(dict);
			close(fd);
			fprintf(stderr, "Failed to get array iterator\n");
			exit(1);
		}

		/* loop through array for battery properties */

		while((array_obj=prop_object_iterator_next(array_iter)) != NULL){
			batt_obj=prop_dictionary_get(array_obj, "description");

			if(batt_obj &&
			   strlen(prop_string_cstring_nocopy(batt_obj))==8 &&
			   strncmp("charging",
                           prop_string_cstring_nocopy(batt_obj), 8)==0){

				batt_obj=prop_dictionary_get(array_obj, "cur-value");
				if(prop_number_integer_value(batt_obj)){
					batt.is_charging=1;
				}else{
					batt.is_charging=0;
				}
				continue;
			}

			if(batt_obj &&
			   strlen(prop_string_cstring_nocopy(batt_obj))==6 &&
			   strncmp("charge", prop_string_cstring_nocopy(batt_obj), 6)==0){

				batt_obj=prop_dictionary_get(array_obj, "cur-value");
				batt.remaining=prop_number_integer_value(batt_obj);

				batt_obj=prop_dictionary_get(array_obj, "max-value");
				batt.full_design=prop_number_integer_value(batt_obj);

				batt_obj=prop_dictionary_get(array_obj, "type");
				if(strncmp("Ampere hour", 
                                  prop_string_cstring_nocopy(batt_obj), 11) == 0){
					batt.watt_as_unit=0;
				}else{
					batt.watt_as_unit=1;
				}

				continue;
			}

			if(batt_obj &&
			   strlen(prop_string_cstring_nocopy(batt_obj))==14 &&
			   strncmp("discharge rate", prop_string_cstring_nocopy(batt_obj),
			            14) == 0){
				batt_obj=prop_dictionary_get(array_obj, "cur-value");
				batt.discharge_rate=prop_number_integer_value(batt_obj);
				continue;
			}


			if(batt_obj &&
			   strlen(prop_string_cstring_nocopy(batt_obj))==13 &&
			   strncmp("last full cap", prop_string_cstring_nocopy(batt_obj),
			            13) == 0){
				batt_obj=prop_dictionary_get(array_obj, "cur-value");
				batt.last_full_cap=prop_number_integer_value(batt_obj);
				continue;
			}

			if(batt_obj &&
			   strlen(prop_string_cstring_nocopy(batt_obj))==7 &&
			   strncmp("voltage", prop_string_cstring_nocopy(batt_obj),
			            7) == 0){
				batt_obj=prop_dictionary_get(array_obj, "cur-value");
				batt.voltage=prop_number_integer_value(batt_obj);
				continue;
			}

		}
		prop_object_iterator_release(array_iter);
	}

	free(device_tag);
	prop_object_iterator_release(dict_iter);
	prop_object_release(dict);
	close(fd);

	if(batt.last_full_cap > 0){
		batt.percent=((float)batt.remaining / (float)batt.last_full_cap) * 100;
	}else{
		batt.percent=0;
	}

	return batt;

}

int set_batt_level(int i, int batt_no, char * batt_str, size_t batt_str_size)
{

	if(i <= 20 && i >= 0){
			snprintf(batt_str, batt_str_size,"Battery %d ^c#FFD700^^r0,4,22,8^^c#444444^^r1,5,20,6^^c#FFD700^^r1,5,%d,6^^f22^^r0,6,1,4^",batt_no, i);
		return 0;
	} else {
		return -1;

	}
}

