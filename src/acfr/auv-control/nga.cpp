#include <csignal>
#include <unistd.h>

#include <iostream>

#include "controller.hpp"
#include "pid.h"

#include "acfr-common/timestamp.h"
#include "acfr-common/spektrum-control.h"

#include "perls-lcmtypes++/acfrlcm/auv_nga_motor_command_t.hpp"

// set the delta T to 0.1s, 10Hz loop rate
//#define W_BEARING 0.95 //amount to weight the velocity bearing (slip angle) in the heading controller, to account for water currents
//#define W_HEADING 0.05 //amount to weight the heading in the heading controller


// RC constants
#define RC_OFFSET 1024
#define RC_THROTTLE_OFFSET 1024     // Testing 16092016 JJM
#define RC_HALF_RANGE 685
#define RC_TO_RAD (12*M_PI/180)/RC_HALF_RANGE
#define RC_TO_RPM 8              // Mitch
#define RC_MAX_PROP_RPM 1500
#define RC_DEADZONE 80 // Testing 160902016 JJM
#define RCMULT RC_MAX_PROP_RPM/(RC_HALF_RANGE-RC_DEADZONE)
#define RC_TUNNEL_MULTI 2047/(RC_HALF_RANGE)

class NGAController: public ControllerBase
{
public:
    NGAController(std::string const &process_name, std::string const &vehicle_name);
    virtual ~NGAController() = default;

protected:
    virtual void manual_control(acfrlcm::auv_spektrum_control_command_t sc);
    virtual void automatic_control(acfrlcm::auv_control_t ac, acfrlcm::auv_acfr_nav_t nav);
    virtual void dead_control();

    virtual void init();

    void reset_integrals();

private:
    pid_gains_t gains_vel;
    pid_gains_t gains_roll;
    pid_gains_t gains_depth;
    pid_gains_t gains_altitude;
    pid_gains_t gains_pitch;
    pid_gains_t gains_pitch_r;
    pid_gains_t gains_heading;
    pid_gains_t gains_tunnel_depth;
    pid_gains_t gains_tunnel_descent;
    pid_gains_t gains_tunnel_pitch;
    pid_gains_t gains_tunnel_heading;
};

void
print_help (int exval, char **argv)
{
    printf("Usage:%s [-h] [-n VEHICLE_NAME]\n\n", argv[0]);

    printf("  -h                               print this help and exit\n");
    printf("  -n VEHICLE_NAME                  set the vehicle_name\n");
    exit (exval);
}

std::string
parse_args (int argc, char **argv)
{
    int opt;

    std::string vehicle_name = "DEFAULT";

    while ((opt = getopt (argc, argv, "hn:")) != -1)
    {
        switch(opt)
        {
        case 'h':
            print_help (0, argv);
            break;
        case 'n':
            vehicle_name = std::string((char*)optarg);
            break;
         }
    }

    return vehicle_name;
}

int main(int argc, char **argv)
{
    // super hacky!
    std::string vehicle_name = parse_args(argc, argv);
    NGAController ic("nga-control", vehicle_name);

    ic.run();

    return 0;
}

NGAController::NGAController(std::string const &process_name, std::string const &vehicle_name)
    : ControllerBase(process_name, vehicle_name)
{
}

void NGAController::init()
{
    this->gains_vel = this->get_pid("velocity");
    this->gains_roll = this->get_pid("roll");
    this->gains_pitch = this->get_pid("pitch");
    this->gains_pitch_r = this->get_pid("pitch_r");
    this->gains_depth = this->get_pid("depth");
    this->gains_altitude = this->get_pid("altitude");
    this->gains_heading = this->get_pid("heading");
    this->gains_tunnel_depth = this->get_pid("tunnel_depth");
    this->gains_tunnel_descent = this->get_pid("tunnel_descent");
    this->gains_tunnel_pitch = this->get_pid("tunnel_pitch");
    this->gains_tunnel_heading = this->get_pid("tunnel_heading");
}

void NGAController::automatic_control(acfrlcm::auv_control_t cmd, acfrlcm::auv_acfr_nav_t nav)
{
    std::cout << "Automatic\n";
    acfrlcm::auv_nga_motor_command_t mc;
    memset(&mc, 0, sizeof(mc));
    mc.utime = timestamp_now();

    double prop_rpm = 0.0;
    double pitch = 0.0, plane_angle = 0.0, rudder_angle = 0.0;

    const double dt = this->dt();
    if (cmd.run_mode == acfrlcm::auv_control_t::RUN)// ||
        //state->run_mode == acfrlcm::auv_control_t::DIVE)
    {
        // X Velocity
        prop_rpm = pid(&this->gains_vel, nav.vx, cmd.vx, dt);

        /* For the moment, we will focus on using the tunnel thrusters to control depth and assume that pitch remains fairly stable
        // Pitch to fins
        if (cmd.depth_mode == PITCH_MODE)
        {
            pitch = cmd.pitch;
        }
        // Altitude to pitch
        else if (state->command.depth_mode == ALTITUDE_MODE)
        {
            // Invert sign of pitch reference to reflect pitch
            // orientation
            pitch = -pid(&this->gains_altitude, nav.altitude, cmd.altitude,
                        dt);
        }
        // Depth to pitch mode
        else
        {
            pitch = -pid(&this->gains_depth, nav.depth, cmd.depth,
                        dt);
        }

        if ((nav.vx > -0.05) || (prop_rpm > -100))
            plane_angle = pid(&this->gains_pitch, nav.pitch, pitch,
                            dt);
        else
            plane_angle = pid(&this->gains_pitch_r, nav.pitch, pitch,
                            dt);
        */
        /************************************************************
        * Depth calculation
        * Diving with NGA is a little special. we have tunnel thrusters
        * to reach a target depth
        *************************************************************/
        double target_pitch = 0.0;

        double target_descent = pid(&this->gains_tunnel_depth, 
                nav.depth, cmd.depth, dt);
        double differential_vert = pid(&this->gains_tunnel_pitch,
                nav.pitch, target_pitch, dt);
        double mutual_vert = pid(&this->gains_tunnel_descent,
                nav.vz, target_descent, dt);

	// this IS CORRECT!!!
	differential_vert = -differential_vert;

	//mutual_vert = 0.0;
	//differential_vert = 0.0;


        // Set motor controller values
        mc.vert_fore = mutual_vert + differential_vert;
        mc.vert_aft = mutual_vert - differential_vert;

	std::cout << "Vertical control mutual: " << mutual_vert << " diff: " << differential_vert << std::endl;


        /************************************************************
        * Heading calculation
        * Calculate the diff between desired heading and actual heading.
        * Ensure this diff is between +/-PI
        *************************************************************/

        // to properly do this current should be accounted for
        // so it could be moving towards the target without actually
        // facing it
        while (nav.heading < -M_PI)
            nav.heading += 2 * M_PI;
        while (nav.heading > M_PI)
            nav.heading -= 2 * M_PI;

        while (cmd.heading < -M_PI)
            cmd.heading += 2 * M_PI;
        while (cmd.heading > M_PI)
            cmd.heading -= 2 * M_PI;

        double diff_heading = nav.heading - cmd.heading;
        while( diff_heading < -M_PI )
            diff_heading += 2*M_PI;
        while( diff_heading > M_PI )
            diff_heading -= 2*M_PI;

        // Account for side slip by making the velocity bearing weighted
        // 	on the desired heading
        rudder_angle = pid(&this->gains_heading, diff_heading, 0.0, dt);

        double differential_lat = pid(&this->gains_tunnel_heading,
                diff_heading, 0, dt);

	//differential_lat = -differential_lat;
	std::cout << "Diff lat: " << differential_lat << std::endl;
        mc.lat_fore = -differential_lat;
        mc.lat_aft = +differential_lat;

        // FIXME: Might consider adding some lat tunnel thruster here
        
        //	printf("prop_rpm: %f\n",prop_rpm);
        // Reverse all the fin angles for reverse direction (given rpm is
        // 	negative and so is velocity, so water relative should be
        //	negative, or soon will be). May not be enough due to completely
        //	different dynamics in reverse, hence there are new gains for the
        //	reverse pitch control now.
        if ((nav.vx < -0.05) && (prop_rpm < -100))
        {
            printf("reversing, flipping fin control\n");
            rudder_angle       = -rudder_angle;
            plane_angle      = -plane_angle;
        }

        //printf("hnav:%f, hcmd:%f, rangle:%f r:%.1f p:%.1f \n",
        // state->nav.heading, state->command.heading, rudder_angle, plane_angle);

        // Set motor controller values
        mc.tail_thruster = prop_rpm;
        mc.tail_rudder = rudder_angle;
        mc.tail_elevator = plane_angle;
    }

    // safety hard codes
    mc.tail_elevator = 0.0;
    
    this->lc().publish(this->get_vehicle_name() + ".NEXTGEN_MOTOR", &mc);
}

void NGAController::manual_control(acfrlcm::auv_spektrum_control_command_t sc)
{
    this->reset_integrals();
    acfrlcm::auv_nga_motor_command_t mc;
    memset(&mc, 0, sizeof(mc));

    mc.utime = timestamp_now();

    // Lateral tunnel thrusters
    int fore = 0;
    int aft = 0;
    double rudder;

    fore = (sc.values[RC_RUDDER] - RC_OFFSET) * RC_TUNNEL_MULTI;
    aft = (sc.values[RC_RUDDER] - RC_OFFSET) * RC_TUNNEL_MULTI;
        
    // Check the steering mode switch
    if(sc.values[RC_GEAR] > 1200)
    {
        fore += (sc.values[RC_AILERON] - RC_OFFSET) * RC_TUNNEL_MULTI;
        aft -= (sc.values[RC_AILERON] - RC_OFFSET) * RC_TUNNEL_MULTI;
        rudder = 0;
    }
    else 
    {
        rudder = -(sc.values[RC_AILERON] - RC_OFFSET) * RC_TO_RAD;
    }

    mc.tail_elevator = -(sc.values[RC_ELEVATOR] - RC_OFFSET) * RC_TO_RAD;
    mc.tail_rudder = rudder; 
    mc.lat_aft = aft;
    mc.lat_fore = fore; 
    
    
    int rcval = sc.values[RC_THROTTLE] - RC_THROTTLE_OFFSET;
        
    // if in deadzone at centre
    if (abs(rcval) < RC_DEADZONE)
    {
        mc.tail_thruster = 0;
    }
    else
    {
        mc.tail_thruster = (rcval - RC_DEADZONE) * RCMULT;
        if (mc.tail_thruster > RC_MAX_PROP_RPM)
            mc.tail_thruster = RC_MAX_PROP_RPM;
        else if (mc.tail_thruster < -RC_MAX_PROP_RPM)
            mc.tail_thruster = -RC_MAX_PROP_RPM;
    }        

    this->lc().publish(this->get_vehicle_name() + ".NEXTGEN_MOTOR", &mc);
}

void NGAController::dead_control()
{
    this->reset_integrals();
    acfrlcm::auv_nga_motor_command_t rc;
    memset(&rc, 0, sizeof(rc));
    rc.utime = timestamp_now();

    rc.tail_thruster = 0;
    rc.tail_elevator = 0;
    rc.tail_rudder = 0;

    rc.lat_aft = 0;
    rc.lat_fore = 0;

    rc.vert_fore = 0;
    rc.vert_aft = 0;

    this->lc().publish(this->get_vehicle_name() + ".NEXTGEN_MOTOR", &rc);
}

void NGAController::reset_integrals()
{
    gains_vel.integral = 0;
    gains_roll.integral = 0;
    gains_depth.integral = 0;
    gains_altitude.integral = 0;
    gains_pitch.integral = 0;
    gains_pitch_r.integral = 0;
    gains_heading.integral = 0;
}