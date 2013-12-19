//
//  main.c
//  USB2000Driver
//
//  Created by Daniel Bongiorno on 5/09/13.
//  Copyright (c) 2013 Daniel Bongiorno. All rights reserved.
//

#include <stdio.h>
#include <lcm/lcm.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>
#include <libgen.h>

#include "USB2000Lib.h"
#include "perls-lcmtypes/senlcm_usb2000_spec_t.h"
#include <unistd.h>
#include "perls-common/timestamp.h"
#include <bot_param/param_client.h>
#include "perls-common/serial.h"


enum {io_socket, io_serial};
int program_exit;
void
signal_handler(int sigNum)
{
    // do a safe exit
    program_exit = 1;
}

int main(int argc, const char * argv[])
{
    printf("Spectrometer FOV Calibration Starting\n");
    
    
    // install the signal handler
	program_exit = 0;
    signal(SIGINT, signal_handler);
    
    
    
    int error = 0;
    long INTTIME = 2000;
    unsigned long specData[2048];
    int CHECKRATE = 500;
    int meanThres_autoGain = 2000;
    int meanStartIdx = 172;
    int meanWidth = 891;
    int autoGainOn = 0;
    
    
    //Initalise LCM object - specReading
    lcm_t *lcm = lcm_create(NULL);
    
    
    senlcm_usb2000_spec_t specReading = {
        .timestamp = timestamp_now(),
        .id = "DownwardSpec",
        .numSamples = 2048,
        .startEndIdx = {0,0},
        .sixteenBitData = 1,
    };
    int32_t dd[10] = {0,1,2,3,4};
    specReading.specData = dd;
    
    // Read the LCM config file
    BotParam *param;
	char rootkey[64];
	char key[64];
	
    param = bot_param_new_from_server (lcm, 1);
    
    sprintf(rootkey, "sensors.%s", basename(argv[0]));
    
    // read the config file
    int io;
	sprintf(key, "%s.io", rootkey);
	char *io_str = bot_param_get_str_or_fail(param, key);
    if(!strcmp(io_str, "serial"))
        io = io_serial;
    else if(!strcmp(io_str, "socket"))
        io = io_socket;
    
    char *serial_dev, *parity;
    int baud;
    char *ip, *inet_port;
    
    
    if(io == io_serial)
    {
        sprintf(key, "%s.serial_dev", rootkey);
        serial_dev = bot_param_get_str_or_fail(param, key);
        
    	sprintf(key, "%s.baud", rootkey);
	    baud = bot_param_get_int_or_fail(param, key);
        
	    sprintf(key, "%s.parity", rootkey);
	    parity = bot_param_get_str_or_fail(param, key);
    }
    
    if(io == io_socket)
    {
        sprintf(key, "%s.ip", rootkey);
        ip = bot_param_get_str_or_fail(param, key);
        
        sprintf(key, "%s.port", rootkey);
        inet_port = bot_param_get_str_or_fail(param, key);
    }
    
    
    sprintf(key, "%s.checkRate", rootkey);
    CHECKRATE = bot_param_get_int_or_fail(param, key);
    
    sprintf(key, "%s.meanThres_autoGain", rootkey);
    meanThres_autoGain = bot_param_get_int_or_fail(param, key);
    
    sprintf(key, "%s.meanStartIdx", rootkey);
    meanStartIdx = bot_param_get_int_or_fail(param, key);
    
    sprintf(key, "%s.meanWidth", rootkey);
    meanWidth = bot_param_get_int_or_fail(param, key);
    
    sprintf(key, "%s.autoGainOn", rootkey);
    autoGainOn = bot_param_get_int_or_fail(param, key);
    
    sprintf(key, "%s.intTimeIntial", rootkey);
    INTTIME = (long) bot_param_get_int_or_fail(param, key) * 1000;
    
    // Open either the serial port or the socket
    struct addrinfo hints, *spec_addr;
    int spec_fd;
    if(io == io_serial)
    {
        spec_fd = serial_open(serial_dev, serial_translate_speed(baud), serial_translate_parity(parity), 1);
        if(spec_fd < 0)
        {
            printf("Error opening port %s\n", serial_dev);
            return 0;
        }
    }
    else if(io == io_socket)
    {
        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        getaddrinfo(ip, inet_port, &hints, &spec_addr);
    	spec_fd = socket(spec_addr->ai_family, spec_addr->ai_socktype, spec_addr->ai_protocol);
        if(connect(spec_fd, spec_addr->ai_addr, spec_addr->ai_addrlen) < 0)
        {
	        printf("Could not connect to %s on port %s\n", ip, inet_port);
    		return 1;
        }
        
    }
    
    FILE *fp, *ffp, *logfp;
    
    //Set to Binary Data mode
    error = setDataMode(spec_fd, TRUE);
    printf("Data mode set- error = %u\n",error);
    
    //init int time
    error = setIntTime(spec_fd, INTTIME);
    printf("Int time set - error = %u\n",error);
    
    //Set trigger mode
    error = setTriggerMode(spec_fd, 0);
    printf("Trigger mode set - error = %u\n",error);
    
    //Need to tune these parameters for Auto Gain
    unsigned long thresholds[3] = {25950, 1000, 2000};
    thresholds[2] = meanThres_autoGain;
    thresholds[3] = meanStartIdx;
    thresholds[4] = meanWidth;
    
    //Acquisition Loop
    int i = 0;
    int c = 0,cc = 0;

    time_t current_time;
    struct tm * time_info;
    char timeString[14];

    time(&current_time);
    time_info = localtime(&current_time);
    strftime(timeString, sizeof(timeString), "%H:%M:%S", time_info);

    printf("Start time %s\n",timeString);
    
    //Start a new log file
    logfp = fopen(argv[1], "w");
    fprintf(logfp, "Start time %s\n",timeString);
    for (int i = 0; i < 13; i++) {
        fprintf(logfp, "\n");
    }
    
    

    while (!program_exit) {
        
        fp = fopen("/home/auv/acqNow","r");
        if (fp != 0) {
            c = fgetc(fp);
            fclose(fp);
        }
	
	//printf("fp:%u c:%u\n",fp,c);        

        ffp = fopen("/home/auv/exitFile","r");
        if (fp != 0) {
            cc = fgetc(ffp);
            fclose(ffp);
            if (cc == 121) {
                //time to quit
                fp = fopen("/home/auv/exitFile","w");
                fputc('n',fp);
                fclose(fp);
                
                break;
            }
        }

        
        if (c == 121) { //Start Acquiring a single sample
    	time(&current_time);
    	time_info = localtime(&current_time);
    	strftime(timeString, sizeof(timeString), "%H:%M:%S", time_info);
            printf("Acquiring new spectra - %s - ",timeString);
            
            error = getSpectra(spec_fd, specData);
            
            float mean = findMean(specData, thresholds[4], thresholds[3]);
            unsigned long max[2];
            findMax(max, specData, 2048);
            printf("Mean: %.1f, Max: %lu at Index: %u\n", mean, max[1],max[0]);
            
            specReading.startEndIdx[0] = error;
            specReading.timestamp = timestamp_now();
            specReading.specData = specData;
            
            specReading.numSamples = 2048;
            specReading.intTime = INTTIME / 1000;
            senlcm_usb2000_spec_t_publish(lcm, "SPEC_FOV_CALIB", &specReading);
            
            //reopen file and write a 'n' to the file
            fp = fopen("/home/auv/acqNow","w");
            fputc('n',fp);
            fclose(fp);
            
            //write out to the log file
            char tt[] = "01/01/2001";
            fprintf(logfp, "Spectra\t%s\t%s.000\t%u\t", tt, timeString, INTTIME);
            for (int k = 0; k < 2048; k++) {
                fprintf(logfp, "%lu\t", specData[k]);
            }
            fprintf(logfp, "\n");
        }
        
    }
    
    fclose(logfp);
    
    close(spec_fd);
    lcm_destroy(lcm);
    
    
    
    
    
    
    return 0;
}
