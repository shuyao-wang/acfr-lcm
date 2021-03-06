#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <libgen.h>
//#include <zlib.h>   // for crc32
#include <bot_param/param_client.h>

#include "perls-common/serial.h"
#include "perls-common/units.h"
#include "perls-common/timestamp.h"
#include "perls-lcmtypes/senlcm_kvh1750_t.h"


// set the output rate and output format
int program_imu(int fd, int rate)
{
    // msg_in has to fit output of ?axes command
    char msg_out[64], msg_in[150];

    printf("Programming IMU\n");
    // set the port canonical
    serial_set_canonical(fd, '\r', '\n');

    // put it in config mode
    sprintf(msg_out, "=config,1\r");
    write(fd, msg_out, strlen(msg_out));
    printf("Entering Configuration Mode\n");

    // get response
    int count = 0;
    memset(msg_in, 0, sizeof(msg_in));
    do
    {
    	write(fd, msg_out, strlen(msg_out));
	usleep(100e3);
        read(fd, msg_in, sizeof(msg_in)-1);
	count++;
	//printf("%s\n", msg_in);
	if (count % 1000 == 0)
            printf("%i\n", count);
    }
    while(!strstr(msg_in, "CONFIG,1"));
    // set the data rate
    printf("Configuration Mode Entered\n");

    sprintf(msg_out, "=DR,%d\r\n", rate);
    write(fd, msg_out, strlen(msg_out));
    printf("Setting data rate %d\n", rate);

    // get response
    memset(msg_in, 0, sizeof(msg_in));
    do
    {
        read(fd, msg_in, sizeof(msg_in));
    }
    while(!strstr(msg_in, "DR,"));
    printf("Data rate set\n");

    // check the axes settings
    sprintf(msg_out, "?axes\r\n");
    write(fd, msg_out, strlen(msg_out));
    printf(".");

    memset(msg_in, 0, sizeof(msg_in));
    do
    {
        read(fd, msg_in, sizeof(msg_in));
        printf("\n%s\n", msg_in);
    }
    while(!strstr(msg_in, "AXES,"));
    printf("\n%s\n", msg_in);

    // set the angular output
    sprintf(msg_out, "=rotfmt,RATE\r\n");
    write(fd, msg_out, strlen(msg_out));
    printf("Setting rotation format\n");

    // get response
    memset(msg_in, 0, sizeof(msg_in));
    do
    {
        read(fd, msg_in, sizeof(msg_in));
    }
    while(!strstr(msg_in, "ROTFMT,RATE"));
    printf("Rotation Format Set\n");

    // leave config mode
    sprintf(msg_out, "=CONFIG,0\r\n");
    write(fd, msg_out, strlen(msg_out));
    serial_set_noncanonical(fd, 1, 0);

    printf("IMU programmed and ready to roll\n");
    return 1;
}

float get_double(unsigned char *d)
{
    unsigned char d2[4];
    d2[3] = d[0];
    d2[2] = d[1];
    d2[1] = d[2];
    d2[0] = d[3];
    return *(float *)d2;
}

unsigned int calc_checksum(unsigned char *d, int len)
{
    // lets check the checksum, the standard CRC32 polynomial 0x04C11DB7 is used
    return 0;//crc32(0, d, len);
}

int parse_imu(unsigned char *d, lcm_t *lcm, char *kvh_channel)
{
    // we don't need to check the header as that has already been done
    unsigned int crc = calc_checksum(d, 32);
    if(*(unsigned int *)&d[32] != crc)
    {
        //printf("Checksum mismatch, 0x08%X, 0x08%X\n", *(unsigned int *)&d[32], crc);
        //return 0;
    }

    senlcm_kvh1750_t kvh;
    kvh.utime = timestamp_now();
    kvh.angular[0] = (double)get_double(&d[4]);
    kvh.angular[1] = (double)get_double(&d[8]);
    kvh.angular[2] = (double)get_double(&d[12]);

    kvh.linear[0] = (double)get_double(&d[16]);
    kvh.linear[1] = (double)get_double(&d[20]);
    kvh.linear[2] = (double)get_double(&d[24]);

    kvh.status = d[28];
    kvh.sequence = d[29];

    kvh.temperature = (double)((short)(d[30] >> 8) + d[31]);

    senlcm_kvh1750_t_publish(lcm, kvh_channel, &kvh);

    return 1;
}

int program_exit;
void
signal_handler(int sigNum)
{
    // do a safe exit
    program_exit = 1;
}

void
print_help (int exval, char **argv)
{
    printf("Usage:%s [-h] [-n VEHICLE_NAME]\n\n", argv[0]);

    printf("  -h                               print this help and exit\n");
    printf("  -n VEHICLE_NAME                  set the vehicle_name\n");
    exit (exval);
}

void
parse_args (int argc, char **argv, char **vehicle_name)
{
    int opt;

    const char *default_name = "DEFAULT";
    *vehicle_name = malloc(strlen(default_name));
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


int main(int argc, char **argv)
{
    // install the signal handler
    program_exit = 0;
    signal(SIGINT, signal_handler);

    BotParam *params;
    char rootkey[64];
    char key[64];

    lcm_t *lcm = lcm_create(NULL);

    char *vehicle_name;
    parse_args(argc, argv, &vehicle_name);

    char kvh_channel[100];

    snprintf(kvh_channel, 100, "%s.KVH1750", vehicle_name);

    free(vehicle_name);

    printf("Obtaining parameters\n");
    params = bot_param_new_from_server (lcm, 1);
    sprintf(rootkey, "sensors.%s", basename(argv[0]));

    // read the initial config
    sprintf(key, "%s.device", rootkey);
    char *device = bot_param_get_str_or_fail(params, key);

    sprintf(key, "%s.baud", rootkey);
    int baud = bot_param_get_int_or_fail(params, key);

    sprintf(key, "%s.sample_rate", rootkey);
    int sample_rate = bot_param_get_double_or_fail(params, key);

    // Open the port
    printf("Opening Serial Port\n");
    int serial_fd = serial_open(device, serial_translate_speed(baud), serial_translate_parity("8N1"), 1);
    printf("Serial Port Open (%i)\n", serial_fd);

    // Set the sample rate and output format
    program_imu(serial_fd, sample_rate);
    serial_set_noncanonical(serial_fd, 1, 0);


    unsigned char buf[64];
    int bytes_read;
    // enter the main loop, read the device and publish
    while(!program_exit)
    {
        // read single bytes until we find the header
        do
        {
            read(serial_fd, &buf[0], 1);
        }
        while(buf[0] != 0xFE);

        // read another 3 bytes
        bytes_read = 0;
        while(bytes_read < 3)
            bytes_read += read(serial_fd, &buf[1 + bytes_read], 3 - bytes_read);

        //	printf("%02X, %02X, %02X, %02X, ", buf[0], buf[1], buf[2], buf[3]);

        // check the sequence
        if((buf[1] == 0x81) && (buf[2] == 0xFF) && (buf[3] == 0x55)) //*(int *)&buf[0] == 0x55FF01FE)
        {
            // data packet, read the rest, another 32 bytes
            bytes_read = 0;
            while(bytes_read < 32)
                bytes_read += read(serial_fd, &buf[4 + bytes_read], 32 - bytes_read);


            parse_imu(buf, lcm, kvh_channel);
        }
        else if(*(int *)&buf[0] == 0xAA0081FE)
        {
            // self test packet
            bytes_read = 0;
            while(bytes_read < 7)
                bytes_read += read(serial_fd, &buf[4 + bytes_read], 7 - bytes_read);

            printf("Self check packet received\n");
        }

    }

    close(serial_fd);

    return 0;
}


