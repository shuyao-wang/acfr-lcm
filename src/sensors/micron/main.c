#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <libgen.h>
#include <bot_param/param_client.h>

#include "acfr-common/sensor.h"
#include "acfr-common/units.h"
#include "acfr-common/timestamp.h"
#include "perls-lcmtypes/senlcm_micron_ping_t.h"

typedef enum
{
    angle_low,
    angle_medium,
    angle_high,
    angle_ultimate
} angle_size_t;

enum {SEANET_VERSION = 1, SEANET_HEAD, SEANET_SCANLINE, SEANET_ALIVE};

typedef struct
{
    lcm_t *lcm;
    acfr_sensor_t *sensor;
    int run;
    double minimum_range;
    angle_size_t angle_resolution;
    double margin;
    double initial_gain;
    senlcm_micron_ping_t mp;
} state_t;

#define MAX_BUF_LEN 512

// Fill in the header structure of the commands for the sonar head
int compose_command_header(unsigned char *d, unsigned char tx, unsigned char rx, short length, short message_length, unsigned char message_type)
{
    char cb[12];

    memset(cb, 0, sizeof(cb));
    cb[0] = '@';                            // Header
    snprintf(&cb[1], 5, "%04X", length);    // ASCII hex length
    memcpy(&cb[5], &length, 2);             // Binary length
    cb[7] = tx;                             // TX node
    cb[8] = rx;                             // RX node
    cb[9] = message_length;                 // Message byte count
    cb[10] = message_type;                  // Message type
    cb[11] = 0x80;                          // Sequence end
    memcpy(d, cb, 12);

    // return the number of bytes
    return 12;
}

// Sonar head command
int compose_head_command(unsigned char *d, state_t *state)
{
    unsigned char c[66];
    memset(c, 0, 66);

    // Fill in the header
    compose_command_header(c, 0xff, 0x02, 60, 56, 19);
    c[12] = 2;
    c[13] = 1;


    // Head control bits
    c[15] = 0x23;
    c[14] = 0x01;

    // Head type = DST
    c[16] = 11;

    // range, not actually used by the sonar head, just there for a reference in the
    // returned data
    unsigned short r = (unsigned short)(state->mp.range * 10);
    memcpy(&c[35], &r, 2);

    // add 10 degrees to the limits to allow margin for spurious returns at the end points - wont use extra angle returns
    // then convert the limit angles from degrees to 1/16 gradians
    // 0 is astern, 3200 is ahead, 1600 is left 90 degrees, 4800 is right 90 degrees
    short left_limit_grad16 = (short)((state->mp.left_limit - state->margin) / 360.0 * 400.0) * 16;
    short right_limit_grad16 = (short)((state->mp.right_limit + state->margin) / 360.0 * 400.0) * 16;
    // Chop at 0..360 degrees if beyond that with extra 10 degrees
    if (left_limit_grad16 < 0)
    {
        left_limit_grad16 = 0;
    }
    if (right_limit_grad16 > 6400)
    {
         right_limit_grad16 = 6400; // 16x400 
    }
    memcpy(&c[37], &left_limit_grad16, 2);
    memcpy(&c[39], &right_limit_grad16, 2);

    printf("angles %u, %u\n", left_limit_grad16, right_limit_grad16);

    // AD span and lower limit, input values are in dB
    // we are using 8 bit mode only so these are not used
    c[41] = 50; // (unsigned char)(state->ad_span / 80.0 * 255.0);
    c[42] = 40; // (unsigned char)(state->ad_low / 80.0 * 255.0);


    // the initial gain is 0 - 80dB, translates to 0 - 210
    c[43] = (unsigned char)(state->initial_gain / 80.0 * 210.0);
    c[44] = c[43];
    printf("Initial gain value = %u\n", c[43]);

    // high speed limit of the scanning motor, 10 micosecond units
    c[49] = 25;

    // step angle size, 0.225, 0.45, 0.9, 1.8 degrees
    switch(state->angle_resolution)
    {
    case angle_low:
        c[50] = 32;
        break;
    case angle_medium:
        c[50] = 16;
        break;
    case angle_high:
        c[50] = 8;
        break;
    case angle_ultimate:
        c[50] = 4;
        break;
    }

    printf("Step size = %u\n", c[50]);

    // Calculate the AD interval and the number of bins given the desired range and resolution
    // Using a VOS of 1500 m/s

    double sample_time = 2.0 * state->mp.range / 1500.0;      // two times the time of flight
    state->mp.num_bins = (int)(state->mp.range / state->mp.range_resolution);
    if(state->mp.num_bins > 800)                         // upper limit for micron sonar
        state->mp.num_bins = 800;

    // the AD interval is in units of 640ns
    state->mp.AD_interval = sample_time / (double)state->mp.num_bins;
    unsigned short AD_interval = (unsigned short)(state->mp.AD_interval / 640e-9);

    memcpy(&c[51], &AD_interval, 2);
    memcpy(&c[53], &state->mp.num_bins, 2);

    printf("Setting AD interval to %f with %d bins, Range %f, Range res %f\n", state->mp.AD_interval, state->mp.num_bins,
           state->mp.range, state->mp.range_resolution
          );

    // set some default parameters
    unsigned short t;
    t = 1000;
    memcpy(&c[55], &t, 2);                   // Max AD buffer
    t = (unsigned short)((state->minimum_range / 1500.0) * 1e6);
    memcpy(&c[57], &t, 2);                   // Lockout
    t = 1600;
    memcpy(&c[59], &t, 2);                   // Minor axis direction
    c[61] = 1;                              // Major axis direction

    c[65] = 0x0A;

    memcpy(d, c, 66);

    return 66;
}

int compose_send_data_command(unsigned char *d)
{
    unsigned char c[18];
    memset(c, 0, 18);
    int len;
    len = compose_command_header(c, 0xff, 0x02, 12, 7, 25);
    c[len - 1] = 0x02;

    // for now we will leave the time flag set to zero as it is only 32 bits
    // we can do something tricky later

    c[17] = 0x0A;

    memcpy(d, c, 18);

    return 18;
}

int compose_reboot_command(unsigned char *d)
{
    unsigned char c[14];
    memset(c, 0, 14);
    int len;
    len = compose_command_header(c, 0xff, 0x02, 12, 3, 16);
    c[len - 1] = 0x02;

    // for now we will leave the time flag set to zero as it is only 32 bits
    // we can do something tricky later

    c[13] = 0x0A;

    memcpy(d, c, 18);

    return 14;
}

// decode a seanet packet
int decode_seanet(unsigned char *d, int len, state_t *p)
{
    int message_type = d[10];
    int ret = 0;

    switch(message_type)
    {
    case 1:
    {
        // Version
        unsigned int cpu_id = *(unsigned int *)&d[13];
        unsigned int program_length = *(unsigned int *)&d[17];
        unsigned short checksum = *(unsigned short *)&d[21];

        printf("CPU ID = 0x%08X, program length = %u, checksum = 0x%04X\n", cpu_id, program_length, checksum);
        ret = SEANET_VERSION;
        break;
    }
    case 19:        // Head command response
        ret = SEANET_HEAD;
        break;

    case 2:         // Scanline data reply
    {
        p->mp.range = (double)(*(unsigned short *)&d[20]) / 10.0;
        p->mp.gain = (double)d[26] / 210.0;
        p->mp.slope = (double)(*(unsigned short *)&d[27]) / 255.0;
        p->mp.AD_interval = (double)(*(unsigned short *)&d[33]);
        p->mp.left_limit = ((double)(*(unsigned short *)&d[35]) / 16.0) / 400.0 * 2 * M_PI;
        p->mp.right_limit = ((double)(*(unsigned short *)&d[37]) / 16.0) / 400.0 * 2 * M_PI;
        p->mp.angle = ((double)(*(unsigned short *)&d[40]) / 16.0) / 400.0 * 2 * M_PI;
        p->mp.num_bins = (int)(*(unsigned short *)&d[42]);
	p->mp.bins = &d[44];
	//realloc(p->mp.bins, p->mp.num_bins);
        //memcpy(p->mp.bins, &d[44], p->mp.num_bins);
        ret = SEANET_SCANLINE;
        break;
    }
    case 4:         // Alive message
    {
        ret = SEANET_ALIVE;
        break;
    }
    default:        // Not decoded
        printf("Got a unknown message type %d\n", message_type);
        break;
    }

    return ret;
}


int program_micron(state_t *state)
{
    // As the sonar doesn't always end up in a state where the software works after a
    // restart we are going to reboot the sonar
    unsigned char micron_cmd[128];
    int bytes;
    bytes = compose_reboot_command(micron_cmd);
    acfr_sensor_write(state->sensor, (char *)micron_cmd, bytes);
    // We delay here for the reboot to happen
    usleep(1000000);

    // Send a head command to the micron
    bytes = compose_head_command(micron_cmd, state);

    // flush serial since reboot
    tcflush(state->sensor->fd, TCIOFLUSH);
    acfr_sensor_write(state->sensor, (char *)micron_cmd, bytes);

    // Send inital data command
    usleep(1000);
    bytes = compose_send_data_command(micron_cmd);
    acfr_sensor_write(state->sensor, (char *)micron_cmd, bytes);

    return 1;
}

void
print_help (int exval, char **argv)
{
    printf("Usage:%s [-h] [-n VEHICLE_NAME]\n\n", argv[0]);

    printf("  -h                               print this help and exit\n");
    printf("  -n VEHICLE_NAME                  set the vehicle_name\n");
    exit (exval);
}

void parse_args (int argc, char **argv, char **vehicle_name)
{
    int opt;

    const char *default_name = "DEFAULT";
    *vehicle_name = malloc(strlen(default_name)+1);
    strcpy(*vehicle_name, default_name);
    
    int n;
    while ((opt = getopt (argc, argv, "hn:")) != -1)
    {
        switch(opt)
        {
        case 'h':
            print_help (0, argv);
            break;
        case 'n':
            n = strlen((char *)optarg);
            free(*vehicle_name);
            *vehicle_name = malloc(n);
            strcpy(*vehicle_name, (char *)optarg);
            break;
         }
    }
}

int program_exit;
void
signal_handler(int sigNum)
{
    // do a safe exit
    program_exit = 1;
}


int main(int argc, char **argv)
{
    state_t state;
    state.mp.bins = NULL;

    // install the signal handler
    program_exit = 0;
    signal(SIGINT, signal_handler);

    char *vehicle_name;
    parse_args(argc, argv, &vehicle_name);
    char sensor_channel[100];
    snprintf(sensor_channel, 100, "%s.MICRON", vehicle_name);
    
    BotParam *params;
    char rootkey[64];
    char key[64];

    state.lcm = lcm_create(NULL);

    params = bot_param_new_from_server (state.lcm, 1);
    sprintf(rootkey, "sensors.%s", basename(argv[0]));

    // Read the serial config to setup serial connection to motor
    state.sensor = acfr_sensor_create(state.lcm, rootkey);
    if(state.sensor == NULL)
    {
        fprintf(stderr, "Could not open micron OAS serial port\n");
        return 0;
    }
    acfr_sensor_noncanonical(state.sensor, 1, 0);

    // Read the rest of the config
    sprintf(key, "%s.range", rootkey);
    state.mp.range = bot_param_get_double_or_fail(params, key);

    sprintf(key, "%s.minimum_range", rootkey);
    state.minimum_range = bot_param_get_double_or_fail(params, key);
    
    sprintf(key, "%s.range_resolution", rootkey);
    state.mp.range_resolution = bot_param_get_double_or_fail(params, key);

    sprintf(key, "%s.angle_resolution", rootkey);
    char *angle_resolution = bot_param_get_str_or_fail(params, key);
    if(strstr(angle_resolution, "low") != NULL)
        state.angle_resolution = angle_low;
    else if(strstr(angle_resolution, "medium") != NULL)
        state.angle_resolution = angle_medium;
    else if(strstr(angle_resolution, "high") != NULL)
        state.angle_resolution = angle_high;
    else if(strstr(angle_resolution, "ultimate") != NULL)
        state.angle_resolution = angle_ultimate;
    else
    {
        printf("angle resolution must be one of the following: low, medium, high, ultimate\n");
    return 1;
    }

    sprintf(key, "%s.left_limit", rootkey);
    state.mp.left_limit = bot_param_get_double_or_fail(params, key);

    sprintf(key, "%s.right_limit", rootkey);
    state.mp.right_limit = bot_param_get_double_or_fail(params, key);

    sprintf(key, "%s.margin", rootkey);
    state.margin = bot_param_get_double_or_fail(params, key);

    sprintf(key, "%s.initial_gain", rootkey);
    state.initial_gain = bot_param_get_double_or_fail(params, key);

    program_micron(&state);

    int return_count = 0;

    fd_set rfds;
    unsigned char buf[MAX_BUF_LEN];
    int64_t last_timestamp;
    int lcm_fd = lcm_get_fileno(state.lcm);
    int bytes;
    unsigned char micron_cmd[128];

    last_timestamp = timestamp_now();

    // now enter the main loop
    while(!program_exit)
    {
        FD_ZERO(&rfds);
        FD_SET(lcm_fd, &rfds);
        FD_SET(state.sensor->fd, &rfds);

        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        int ret = select (FD_SETSIZE, &rfds, NULL, NULL, &tv);
        if(ret == -1)
            perror("Select failure: ");
        else if(ret != 0)
        {
            if(FD_ISSET(lcm_fd, &rfds)) // no LCM subscriptions, so n/a but would fall through anyway
            {
                lcm_handle(state.lcm); 
            }
            if(FD_ISSET(state.sensor->fd, &rfds)) // serial readable
            {
                state.mp.utime = timestamp_now();
                bytes = 0;
                do
                {
                    bytes += acfr_sensor_read(state.sensor, (char *)&buf[bytes], 7 - bytes);
                    //bytes += read(state->sensor.fd, &buf[bytes], 7 - bytes);
                }
                while(bytes < 7);

                if(bytes == 7)
                {
                    unsigned short data_len = *(unsigned short *)&buf[5];
                    // read the rest
                    do
                    {
                        bytes += acfr_sensor_read(state.sensor, (char *)&buf[bytes], (data_len + 7 - 1) - bytes);
                    }
                    while(bytes < (data_len + 7 - 1));
                }

                ret = decode_seanet(buf, bytes, &state);

                // we need to send the data command for every second return data packet
                if(ret == SEANET_SCANLINE)
                {
                    last_timestamp = state.mp.utime;
                    senlcm_micron_ping_t_publish(state.lcm, sensor_channel, &state.mp);

                    if(return_count++ == 1)
                    {
                        bytes = compose_send_data_command(micron_cmd);
                        acfr_sensor_write(state.sensor, (char *)micron_cmd, bytes);
                        return_count = 0;
                    }
                }

                if((timestamp_now() - last_timestamp) > 2e6)
                    program_micron(&state);

            }
        }
    }
    acfr_sensor_destroy(state.sensor);

    return 0;
}

