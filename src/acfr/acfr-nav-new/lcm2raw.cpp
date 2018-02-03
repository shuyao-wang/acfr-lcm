// LCM to RAW log converter

#include "acfr_nav.hpp"
#include "handlers.hpp"

        

void printUsage() {
    // output the program usage
    cout << "lcm2raw [options] [LCM log file] [raw file]\n";
    cout << "Options      -b Broken Iver altitude, only on the OSI message" << endl;
}

int parseArguments(int argc, char **argv, state_c *state, char **lcm_filename, char **raw_filename) {
    if(argc <= 2) {
        cout << "Incorrect number of aurguments\n";
        printUsage();
        return 0;
    }


    
    int c;
    while((c = getopt(argc, argv, "b")) != -1)
        switch(c) {
            case 'b':
                state->broken_iver_alt = true;
                break;
        }


    int n;
    n = strlen((char *)argv[optind]);
    *lcm_filename = (char *)malloc(n);
    strcpy(*lcm_filename, (char *)argv[optind]);

   
    n = strlen((char *)argv[optind+1]);
    *raw_filename = (char *)malloc(n);
    strcpy(*raw_filename, (char *)argv[optind+1]);

    return 1;
}


int main(int argc, char **argv) {

    char *lcm_filename = {0};
    char *raw_filename = {0};

    state_c state;
    state.broken_iver_alt = false;

    parseArguments(argc, argv, &state, &lcm_filename, &raw_filename);
    char url[256];
    sprintf(url, "file://%s?speed=0", lcm_filename);
    cout << url << endl;
        
    state.lcm = new lcm::LCM(url);
    state.mode = RAW;

    state.raw_out.open(raw_filename, ofstream::out);
    if(!state.raw_out.is_open()) {
        cerr << "Could not open input file: " << raw_filename << endl;
        return 1;
    } 
        
        // subscribe to all the channels
        state.lcm->subscribeFunction(".*GPSD_CLIENT", on_gps, &state);
        state.lcm->subscribeFunction(".*TCM", on_tcm_compass, &state);
        state.lcm->subscribeFunction(".*OS_COMPASS", on_os_compass, &state);
        state.lcm->subscribeFunction(".*YSI", on_ysi, &state);
        state.lcm->subscribeFunction(".*PAROSCI", on_parosci, &state);
        state.lcm->subscribeFunction(".*SEABIRD", on_seabird_depth, &state);
        state.lcm->subscribeFunction(".*RDI", on_rdi, &state);
        state.lcm->subscribeFunction(".*IMU", on_imu, &state);
        state.lcm->subscribeFunction(".*LQ_MODEM", on_lq_modem, &state);
        state.lcm->subscribeFunction(".*ACFR_AUV_VIS_RAWLOG", on_vis, &state);
        state.lcm->subscribeFunction(".*USBL_FIX.*", on_evologics, &state);
        state.lcm->subscribeFunction(".*UVC_DVL", on_uvc_dvl, &state);
        state.lcm->subscribeFunction(".*UVC_RPH", on_uvc_rph, &state);
        state.lcm->subscribeFunction(".*UVC_OSI", on_uvc_osi, &state);
        state.lcm->subscribeFunction(".*SEABOTIX_SENSORS", on_seabotix_sensors, &state);
        //state.lcm->subscribeFunction("MICRON_SOUNDER", on_micron_sounder, &state);
        cout << "Done subscribing" << endl;

        
        
        while(!state.lcm->handle());
        delete state.lcm;
   

    return 0;
}
