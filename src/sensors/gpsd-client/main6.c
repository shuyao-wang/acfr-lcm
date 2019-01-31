#include <stdio.h>
#include <stdlib.h>
//#include <glib.h>
#include <unistd.h>

#include <gps.h>


//#include "acfr-common/sensor.h"
#include "acfr-common/units.h"
#include "acfr-common/timestamp.h"
#include "acfr-common/nmea.h"

/*
#include "perls-common/bot_util.h"
#include "perls-common/error.h"
#include "perls-common/generic_sensor_driver.h"
#include "perls-common/getopt.h"
#include "perls-common/nmea.h"
#include "perls-common/timestamp.h"
#include "perls-common/units.h"
*/

#include "perls-lcmtypes/senlcm_gpsd3_t.h"
#include "perls-lcmtypes/senlcm_nmea_gphdt_t.h"
#include "perls-lcmtypes/senlcm_ppsboard_t.h"
#include "perls-lcmtypes/senlcm_raw_t.h"

#if (GPSD_API_MAJOR_VERSION < 6)
#error "gspd API version < 6"
#endif

//#define DTOR (UNITS_DEGREE_TO_RADIAN)
//#define RTOD (UNITS_RADIAN_TO_DEGREE)

static const unsigned int FLAGS = WATCH_ENABLE|WATCH_JSON|WATCH_NMEA;
static const int TIMEOUT_MICROSEC = 5000000;

typedef struct gps_data_t gpsdata_t;

typedef struct _state_t state_t;
struct _state_t
{
    gpsdata_t *gpsdata;
    char *gpsddev;
    char *gps_server;
    char *gps_port;
    char *vehicle_name;
    
    lcm_t *lcm;

    //generic_sensor_driver_t *gsd;

    char ppsboard_channel[128];
    char gphdt_channel[128];
    char gps_channel[128];
    char raw_channel[128];
};

/*
static int
gpsd_opts (generic_sensor_driver_t *gsd)
{
    getopt_add_description (gsd->gopt, "gpsd client driver.");
    getopt_add_string (gsd->gopt, 's', "server", "localhost", "gpsd server");
    getopt_add_string (gsd->gopt, 'p', "port", "2947",        "gpsd port");
    getopt_add_string (gsd->gopt, '\0', "gpsddev", "",        "gpsd device");
    return 0;
}
*/

state_t*
state_init (int argc, char *argv[])
{
    state_t* state = calloc (1, sizeof (*state));
    //state->gsd = gsd_create (argc, argv, NULL, &gpsd_opts);
	state->lcm = lcm_create(NULL);
	
    // gps device---not clear if this really does anything within libgps
    /*state->gpsddev = NULL;
    if (getopt_has_flag (state->gsd->gopt, "gpsddev"))
        state->gpsddev = g_strdup (getopt_get_string (state->gsd->gopt, "gpsddev"));
    else
    {
        char key[256];
        snprintf (key, sizeof key, "%s.gpsddev", state->gsd->rootkey);
        if (bot_param_get_str (state->gsd->params, key, &state->gpsddev))
        {
            ERROR ("gpsddev not set");
            exit (EXIT_FAILURE);
        }
    }
*/
/*
    // non-default lcm channels
    state->ppsboard_channel = g_strconcat (state->gsd->channel, ".PPSBOARD", NULL);
    state->gphdt_channel = g_strconcat (state->gsd->channel, ".GPHDT", NULL);

    // gpsd client
    const char *server = getopt_get_string (state->gsd->gopt, "server");
    const char *port = getopt_get_string (state->gsd->gopt, "port");
    state->gpsdata = calloc (1, sizeof (gpsdata_t));
    if (gps_open (server, port, state->gpsdata) != 0)
    {
        ERROR ("gps_open () failed");
        exit (EXIT_FAILURE);
    }
*/
    return state;
}

void
state_destroy (state_t *state)
{
    gps_stream (state->gpsdata, WATCH_DISABLE, NULL);
    gps_close (state->gpsdata);

    free (state->gpsddev);
    //gsd_destroy (state->gsd);
    free (state->ppsboard_channel);
    free (state->gphdt_channel);
    free (state->raw_channel);
    free (state->gps_channel);
}

static int
parse_ppsda (const char *buf, senlcm_ppsboard_t *ppsboard)
{
    ppsboard->utime = timestamp_now ();

    char tmp_ntp_time[32];
    if (!nmea_arg (buf, 1, tmp_ntp_time))
        return 0;
    else
        free (ppsboard->ntp_time);
    ppsboard->ntp_time = strdup (tmp_ntp_time);

    char ntp_status;
    if (!nmea_argc (buf, 2, &ntp_status))
        return 0;
    else
    {
        if (ntp_status == 'A')
            ppsboard->ntp_status = 1;
        else
            ppsboard->ntp_status = 0;
    }

    char src_type;
    if (!nmea_argc (buf, 4, &src_type))
        return 0;
    else
    {
        free (ppsboard->src_type);
        if (src_type == 'I')
            ppsboard->src_type = strdup ("IVER");
        else if (src_type == 'G')
            ppsboard->src_type = strdup ("GARMIN");
        else
            ppsboard->src_type = strdup ("UNKNOWN");
    }

    int src_pps = 0;
    if (!nmea_argi (buf, 5, &src_pps))
        return 0;
    else
        ppsboard->src_pps = src_pps;

    int sync_mode = 0;
    if (!nmea_argi (buf, 7, &sync_mode))
        return 0;
    else
        ppsboard->sync_mode = sync_mode;

    int sync_num = 0;
    if (!nmea_argi (buf, 8, &sync_num))
        return 0;
    else
        ppsboard->sync_num = sync_num;

    char tmp_sync_date[32];
    if (!nmea_arg  (buf, 9, tmp_sync_date))
    {
        free (ppsboard->sync_date);
        ppsboard->sync_date = strdup ("");
    }
    else
    {
        free (ppsboard->sync_date);
        ppsboard->sync_date = strdup (tmp_sync_date);
    }

    char tmp_sync_time[32];
    if (!nmea_arg  (buf, 10, tmp_sync_time))
    {
        free (ppsboard->sync_time);
        ppsboard->sync_time = strdup ("");
    }
    else
    {
        free (ppsboard->sync_time);
        ppsboard->sync_time = strdup (tmp_sync_time);
    }

    int offset_counts = 0;
    if (!nmea_argi (buf, 12, &offset_counts))
        ppsboard->offset_counts = atoi ("NaN");
    else
        ppsboard->offset_counts = offset_counts;

    if (!nmea_argf (buf, 13, &ppsboard->offset_usecs))
        ppsboard->offset_usecs = atof ("NaN");

    if (!nmea_argf (buf, 15, &ppsboard->temperature))
        return 0;

    return 1;
}

static int
parse_gphdt (const char *buf, senlcm_nmea_gphdt_t *gphdt)
{
    // magnetic heading
    double h_raw = 0.0;
    if (!nmea_argf (buf, 1, &h_raw))
        return 0;

    // magnetic deviation
    double d = 0.0;
    if (!nmea_argf (buf, 2, &d))
        return 0;

    // easterly or westernly?
    char D = '\0';
    if (!nmea_argc (buf, 3, &D))
        return 0;
    switch (D)
    {
    case 'E':
        break;
    case 'W':
        d *= -1.0;
        break;
    default:
        return 0;
    }

    // magnetic variation
    double v = 0.0;
    if (!nmea_argf (buf, 4, &v))
        return 0;

    // easterly or westernly?
    char V = '\0';
    if (!nmea_argc (buf, 5, &V))
        return 0;
    switch (V)
    {
    case 'E':
        break;
    case 'W':
        v *= -1.0;
        break;
    default:
        return 0;
    }

    *gphdt = (senlcm_nmea_gphdt_t)
    {
        .utime     = timestamp_now (),
         .h_true    = (h_raw + d + v)*DTOR,
          .h_raw     = h_raw*DTOR,
           .deviation = d*DTOR,
            .variation = v*DTOR,
    };

    return 1;
}

static const int MAXSTRINGLEN = 32;

static void
handle_nmea (state_t *state, const char *buf)
{
    senlcm_raw_t raw =
    {
        .utime = timestamp_now (),
        .length = strlen (buf) + 1, // +1 for '\0' char
        .data = (uint8_t *) buf,
    };
    senlcm_raw_t_publish (state->lcm, state->raw_channel, &raw);

    if (strncmp (buf, "$PPSDA", 6) == 0)
    {
        senlcm_ppsboard_t *ppsboard = calloc (1, sizeof (*ppsboard));
        ppsboard->ntp_time = malloc (MAXSTRINGLEN * sizeof (*ppsboard->ntp_time));
        ppsboard->src_type = malloc (MAXSTRINGLEN * sizeof (*ppsboard->src_type));
        ppsboard->sync_date = malloc (MAXSTRINGLEN * sizeof (*ppsboard->sync_date));
        ppsboard->sync_time = malloc (MAXSTRINGLEN * sizeof (*ppsboard->sync_time));
        if (parse_ppsda (buf, ppsboard))
        {
            senlcm_ppsboard_t_publish (state->lcm, state->ppsboard_channel, ppsboard);
            //gsd_update_stats (state->gsd, 1);
        }
        //else
        //    gsd_update_stats (state->gsd, -1);
        senlcm_ppsboard_t_destroy (ppsboard);
    }
    //else if (nmea_validate_checksum (buf))
    //    gsd_update_stats (state->gsd, 1);
    //else
    //{
    //    gsd_update_stats (state->gsd, -1);
    //    return;
   // }

    if (0==strncmp (buf, "$GPHDG", 6))
    {
        senlcm_nmea_gphdt_t gphdt = {0};
        if (parse_gphdt (buf, &gphdt))
            senlcm_nmea_gphdt_t_publish (state->lcm, state->gphdt_channel, &gphdt);
    }
}

static int
pack_gpsd (gpsdata_t *ud, senlcm_gpsd3_t *gd)
{
    gd->utime = timestamp_now ();
    gd->online    = (int64_t) (ud->online * 1.0E6);

    /* accumulated PVT data */
    gd->fix.utime = (int64_t) (ud->fix.time * 1.0E6);
    gd->fix.mode = ud->fix.mode;
    gd->fix.ept = ud->fix.ept;
    gd->fix.latitude = ud->fix.latitude * DTOR;
    gd->fix.epy = ud->fix.epy;
    gd->fix.longitude = ud->fix.longitude * DTOR;
    gd->fix.epx = ud->fix.epx;
    gd->fix.altitude = ud->fix.altitude;
    gd->fix.epv = ud->fix.epv;
    gd->fix.track = ud->fix.track;
    gd->fix.epd = ud->fix.epd;
    gd->fix.speed = ud->fix.speed;
    gd->fix.eps = ud->fix.eps;
    gd->fix.climb = ud->fix.climb;
    gd->fix.epc = ud->fix.epc;

    gd->geoidal_separation = ud->separation;

    /* GPS status */
    gd->status = ud->status;

    /* precision of fix */
    gd->satellites_used = ud->satellites_used;
    for (int i=0; i<ud->satellites_used; i++)
        gd->used[i] = ud->skyview[i].used;
    gd->dop.pdop = ud->dop.pdop;
    gd->dop.hdop = ud->dop.hdop;
    gd->dop.tdop = ud->dop.tdop;
    gd->dop.gdop = ud->dop.gdop;
    gd->dop.xdop = ud->dop.xdop;
    gd->dop.ydop = ud->dop.ydop;

    /* spherical position error, 95% confidence */
    gd->epe    = ud->epe;

    /* satellite status */
    gd->skyview_utime = (int64_t) (ud->skyview_time * 1.0E6);
    gd->satellites_visible = ud->satellites_visible;
    for (size_t i=0; i<gd->satellites_visible; i++)
    {
        gd->PRN[i] = ud->skyview[i].PRN;
        gd->elevation[i] = ud->skyview[i].elevation;
        gd->azimuth[i] = ud->skyview[i].azimuth;
        gd->ss[i] = ud->skyview[i].ss;
    }

    /* devconfig_t data */
    if (ud->set & DEVICE_SET)
    {
        gd->dev.path = strdup (ud->dev.path);
        gd->dev.flags = ud->dev.flags;
        gd->dev.driver = strdup (ud->dev.driver);
        gd->dev.subtype = strdup (ud->dev.subtype);
        gd->dev.activated = ud->dev.activated;
        gd->dev.baudrate = ud->dev.baudrate;
        gd->dev.stopbits = ud->dev.stopbits;
        gd->dev.cycle = ud->dev.cycle;
        gd->dev.mincycle = ud->dev.mincycle;
        gd->dev.driver_mode = ud->dev.driver_mode;
    }
    else
    {
        gd->dev.path = strdup ("");
        gd->dev.driver = strdup ("");
        gd->dev.subtype = strdup ("");
    }

    /* policy_t data ignored */

    gd->tag = ""; // no longer available strdup (ud->tag);

    return 0;
}

static void
handle_gpsd (state_t *state, const char *buf)
{
    senlcm_gpsd3_t *gpsd = calloc (1, sizeof (*gpsd));

    // MAXCHANNELS defined in gps.h
    gpsd->used = malloc (MAXCHANNELS * sizeof (*gpsd->used));
    gpsd->PRN = malloc (MAXCHANNELS * sizeof (*gpsd->PRN));
    gpsd->elevation = malloc (MAXCHANNELS * sizeof (*gpsd->elevation));
    gpsd->azimuth = malloc (MAXCHANNELS * sizeof (*gpsd->azimuth));
    gpsd->ss = malloc (MAXCHANNELS * sizeof (*gpsd->ss));

    if (pack_gpsd (state->gpsdata, gpsd) == 0)
    {
        senlcm_gpsd3_t_publish (state->lcm, state->gps_channel, gpsd);
    }

    free(gpsd->used);
    free(gpsd->PRN);
    free(gpsd->elevation);
    free(gpsd->azimuth);
    free(gpsd->ss);

    free(gpsd);
}

static void
parse_gpsdata (state_t *state)
{
    const char* buf = gps_data (state->gpsdata);
    if (buf[0] == '$') handle_nmea (state, buf);
    else handle_gpsd (state, buf);
}





void
print_help (int exval, char **argv)
{
    printf("Usage:%s [-h] [-n VEHICLE_NAME]\n\n", argv[0]);

    printf("  -h                               print this help and exit\n");
    printf("  -n VEHICLE_NAME                  set the vehicle_name\n");
    printf("  -s GPS_SERVER                    set the gpsd server, defualt localhost\n");
    printf("  -p GPS_PORT                  	   set the gpsd port, defautl 2947\n");
    exit (exval);
}

void
parse_args (int argc, char **argv, state_t *state)
{
    int opt;

    const char *default_name = "DEFAULT";
    state->vehicle_name = malloc(strlen(default_name)+1);
    strcpy(state->vehicle_name, default_name);
    
    const char *default_server = "localhost";
    state->gps_server = malloc(strlen(default_server)+1);
    strcpy(state->gps_server, default_server);
    
    const char *default_port = "2947";
    state->gps_port = malloc(strlen(default_port)+1);
    strcpy(state->gps_port, default_port);
    
    
    int n;
    while ((opt = getopt (argc, argv, "hn:s:p:")) != -1)
    {
        switch(opt)
        {
        case 'h':
            print_help (0, argv);
            break;
        case 'n':
            n = strlen((char *)optarg);
            free(state->vehicle_name);
            state->vehicle_name = malloc(n);
            strcpy(state->vehicle_name, (char *)optarg);
            break;
        case 's':
            n = strlen((char *)optarg);
            free(state->gps_server);
            state->gps_server = malloc(n);
            strcpy(state->gps_server, (char *)optarg);
            break;
        case 'p':
            n = strlen((char *)optarg);
            free(state->gps_port);
            state->gps_port = malloc(n);
            strcpy(state->gps_port, (char *)optarg);
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
    setvbuf (stdout, (char *) NULL, _IONBF, 0);

    state_t state;
    memset(&state, 0, sizeof(state_t));
    parse_args(argc, argv, &state);
    state.lcm = lcm_create(NULL);
    
    // install the signal handler
    program_exit = 0;
    signal(SIGINT, signal_handler);

	
	snprintf(state.ppsboard_channel, 128, "%s.GPSD_CLIENT.PPSBOARD", state.vehicle_name);
	snprintf(state.gps_channel, 128, "%s.GPSD_CLIENT", state.vehicle_name);
	snprintf(state.gphdt_channel, 128, "%s.GPSD_CLIENT.GPHDT", state.vehicle_name);
	snprintf(state.raw_channel, 128, "%s.GPSD_CLIENT.RAW", state.vehicle_name);
	
	
    state.gpsdata = calloc (1, sizeof (gpsdata_t));
    if (gps_open (state.gps_server, state.gps_port, state.gpsdata) != 0)
    {
        fprintf(stderr, "gps_open () failed");
        exit (EXIT_FAILURE);
    }


    // main gps read loop
    gps_stream (state.gpsdata, FLAGS, NULL);
    while (!program_exit)
    {	
        if (gps_waiting (state.gpsdata, TIMEOUT_MICROSEC))
        {

            if (gps_read (state.gpsdata) == -1)
            {
                fprintf(stderr, "gps_read error");

            }
            else
            {
                parse_gpsdata (&state);
            }
        }



        /*
        if (!gps_waiting (state->gpsdata, TIMEOUT_MICROSEC)) {
            ERROR ("gpsd-client timed out waiting for data");
            exit (EXIT_FAILURE);
        }
        if (gps_read (state->gpsdata) == -1) {
            ERROR ("gps_read error");
            exit (EXIT_SUCCESS);
        }
        else {
            parse_gpsdata (state);
        }
        */
    }

    state_destroy (&state);
    exit (EXIT_SUCCESS);
}