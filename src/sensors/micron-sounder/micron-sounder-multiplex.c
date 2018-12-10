#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <libgen.h>

#include "perls-lcmtypes/senlcm_micron_sounder_t.h"
#include "perls-lcmtypes/perllcm_heartbeat_t.h"
#include "acfr-common/timestamp.h"
#include "acfr-common/sensor.h"

#define MAX_OAS 4

typedef struct 
{
    lcm_t *lcm;
    acfr_sensor_t *sensor[MAX_OAS];
    int num_oas;
    int oas_ports[MAX_OAS];
    int altitude;
    char *channel_name;
} state_t;

void heartbeat_handler(const lcm_recv_buf_t *rbuf, const char *ch, const perllcm_heartbeat_t *hb, void *u)
{
    state_t *state = (state_t *)u;
    char msg[32];
    char resp[75];
    int ret;
    
    for(int i=0; i<state->num_oas; i++)
    {
        char value[16];
        senlcm_micron_sounder_t micron;
        for(int i=0; i < state->num_oas ; i++)
        {
            memset(msg, 0, sizeof(msg));
            sprintf(msg, "z\r");
            acfr_sensor_write(state->sensor[i], msg, strlen(msg));
                usleep(100);    
            // Wait for a response with a timeout
            memset(resp, 0, sizeof(resp));
            ret = acfr_sensor_read_timeoutms(state->sensor[i], resp, sizeof(resp), 50);
            if(ret > 0){
                // parse out the range value leaving the m off then end
                micron.utime = timestamp_now();
                memset(value, 0, sizeof(value));
                strncpy(value, resp, strlen(resp) - 1);
                micron.altitude = atof(value);
                memset(msg, 0, sizeof(msg));
                sprintf(msg, "_%d", state->oas_ports[i]);
                strcat(state->channel_name, msg);
                senlcm_micron_sounder_t_publish(state->lcm, state->channel_name, &micron);            
            }
            usleep(100);
        }
    }    
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
parse_args (int argc, char **argv, char **channel_name)
{
    int opt;

    const char *default_name = "DEFAULT";
    *channel_name = malloc(strlen(default_name)+1);
    strcpy(*channel_name, default_name);
    
    while ((opt = getopt (argc, argv, "hn:")) != -1)
    {
        switch(opt)
        {
        case 'h':
            print_help (0, argv);
            break;
        case 'n':
            free(*channel_name);
            *channel_name = malloc(200);
            snprintf(*channel_name, 200, "%s.MICRON_SOUNDER", (char *)optarg);
            break;
         }
    }
}

int program_exit;
void signal_handler(int sig_num)
{
    // do a safe exit
    program_exit = 1;
}

int
main (int argc, char *argv[])
{
    // install the signal handler
    program_exit = 0;
    signal(SIGINT, signal_handler);

    state_t state;

    parse_args(argc, argv, &state.channel_name);

    //Initalise LCM object - specReading
    state.lcm = lcm_create(NULL);

    char rootkey[64];
    sprintf(rootkey, "sensors.%s", basename(argv[0]));

    state.sensor[0] = acfr_sensor_create(state.lcm, rootkey);
    if(state.sensor[0] == NULL)
        return 0;

    // Get the serial_port names
    char key[64];
    char msg[16];
    sprintf(key, "%s.serial_ports", rootkey);
    state.num_oas = bot_param_get_int_array(state.sensor[0]->param, key, state.oas_ports, MAX_OAS);
    if (state.num_oas > 1)
    {
        for(int i=1; i < state.num_oas ; i++)
        {
            memset(msg, 0, sizeof(msg));
            sprintf(msg, "/dev/ttyCTI%d", state.oas_ports[i]);
            state.sensor[i] = state.sensor[0];
            state.sensor[i]->serial_dev = msg;
            state.sensor[i]->port_open = 0;
            acfr_sensor_open(state.sensor[i]);      
        }
    }

    for(int i=0; i < state.num_oas ; i++)
    {
    acfr_sensor_canonical(state.sensor[i], '\r', '\n');
    }

    perllcm_heartbeat_t_subscribe(state.lcm, "HEARTBEAT_1HZ", &heartbeat_handler, &state);

    while (!program_exit)
    {
        lcm_handle_timeout(state.lcm, 1000);
    }

    for(int i=0; i < state.num_oas ; i++)
    {
    acfr_sensor_destroy(state.sensor[i]);   
    }
    return 1;
}





