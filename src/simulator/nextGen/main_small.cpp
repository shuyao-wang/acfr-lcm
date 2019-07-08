// nga AUV simulator


#include <iostream>
#include <fstream>
#include <lcm/lcm-cpp.hpp>
#include <signal.h>
#include <boost/numeric/odeint.hpp>
#include <small/Pose3D.hh>
#include <bot_param/param_client.h>
#include "acfr-common/auv_map_projection.hpp"
#include "perls-lcmtypes++/perllcm/heartbeat_t.hpp"
#include "perls-lcmtypes++/acfrlcm/auv_acfr_nav_t.hpp"
#include "perls-lcmtypes++/acfrlcm/auv_nga_motor_command_t.hpp"
#include "perls-common/timestamp.h"
#include "perls-lcmtypes++/senlcm/tcm_t.hpp"
#include "perls-lcmtypes++/senlcm/ysi_t.hpp"
#include "perls-lcmtypes++/senlcm/gpsd3_t.hpp"
#include "perls-lcmtypes++/senlcm/rdi_pd5_t.hpp"
#include "perls-lcmtypes++/senlcm/IMU_t.hpp"
#include "perls-lcmtypes++/senlcm/os_power_system_t.hpp"
#include "perls-lcmtypes++/senlcm/os_power_cont_t.hpp"


using namespace std;
using namespace boost::numeric::odeint;
using namespace acfrlcm;
using namespace perllcm;
using namespace senlcm;
using namespace libplankton;

// generated by matlab
#include "vehicle_params_small.h"

#define Kdp 0.01
//#define CURRENT 0.1
#define CURRENT 0.0
#define WATER_DEPTH 30

#define GPS_CHANNELS 4
#define MAG_VAR 12.67*M_PI/180
#define TAU_A 500
#define TAU_G 500
#define STD_A 0.0372
#define STD_G 0.00010732
#define BIAS_A 0.0196
#define BIAS_G 9.6963e-06

typedef boost::numeric::ublas::vector< double > state_type;

// Inertial Matrix
SMALL::Matrix33 longM, latM;

// state vectors
state_type state(12);
runge_kutta_cash_karp54<state_type> stepper;

SMALL::Vector3D accel;

ofstream fp, fp_nav;
Local_WGS84_TM_Projection *map_projection_sim;

int64_t last_dvl_time = 4.611686e+18, last_gps_time = 4.611686e+18, last_ysi_time = 4.611686e+18, last_tcm_time = 4.611686e+18, last_print_time = 4.611686e+18; // 2^62
int64_t last_obs_time = 4.611686e+18;

// earth rotation in the navigation frame
SMALL::Vector3D earth_rot;

//gravity
SMALL::Vector3D grav;

// the state vector is X Y Z r p h u v w p q r
#define XNDX 0
#define YNDX 1
#define ZNDX 2
#define ROLLNDX 3
#define PITCHNDX 4
#define HDGNDX 5
#define XDOTNDX 6
#define YDOTNDX 7
#define ZDOTNDX 8
#define ROLLDOTNDX 9
#define PITCHDOTNDX 10
#define HDGDOTNDX 11

// the control vector is RPM prop_torque rudder, plane
//SMALL::Vector6D in;
auv_nga_motor_command_t in;

double ba_x,ba_y,ba_z,bg_x,bg_y,bg_z;

string vehicle_name = "DEFAULT";

void auv( const state_type &x , state_type &dxdt , const double /* t */ )
{
    double Xa, Ya, Za;
    double u, v, w, p, q, r;
    double phi, theta, psi;
    Xa = x[XNDX];
    Ya = x[YNDX];
    Za = x[ZNDX];
    phi = x[ROLLNDX];
    theta = x[PITCHNDX];
    psi = x[HDGNDX];
    u = x[XDOTNDX];
    v = x[YDOTNDX];
    w = x[ZDOTNDX];
    p = x[ROLLDOTNDX];
    q = x[PITCHDOTNDX];
    r = x[HDGDOTNDX];
    
    SMALL::Vector4D nuLinear, nuRotational;
    nuLinear = u, v, w, 0.0;
    nuRotational = p, q, r, 0.0;

    // adjustments for a constant water current (or wind)

    SMALL::Rotation3D Cbn;
    Cbn.setRollPitchYawRad(phi, theta, psi);

    // current in the navigation frame
    SMALL::Vector3D vc_n;
    vc_n = CURRENT,0,0;
    // current in the body frame
    SMALL::Vector3D vc_b;
    vc_b = (Cbn.i() * vc_n);

    // vehicle velocity in the current
    nuLinear(1) = nuLinear(1) - vc_b(1);
    nuLinear(2) = nuLinear(2) - vc_b(2);
    nuLinear(3) = nuLinear(3) - vc_b(3);

    u = u - vc_b(1);
    v = v - vc_b(2);
    w = w - vc_b(3);


    // make heading 0 to 2pi
    while(psi < 0)
        psi += 2 * M_PI;
    while(psi > (2 * M_PI))
        psi -= 2 * M_PI;

    // Where we are at, ie the auv pose
    SMALL::Pose3D auvPose;
    auvPose.setPosition(Xa, Ya, Za);
    auvPose.setRollPitchYawRad(phi, theta, psi);

    // Prop force calculations
    double prop_alpha = 0.02290; //0.01; // advance ratio
    double prop_diameter = 0.25;
    double rho = 1030;              // Water density
    double J0 = 0; // open water advance coefficient
    double Kt = 0; // propeller thrust coefficient
    double Kq = 0; // properler torque coefficient
    
    // Sanity check on input
    /*if(in(0) != in(0))
    {
        in(0) = 0;
        cerr << "In 0 NaN" << endl;
    }*/
    
    // propeller revolutions per second (rps)
    // converting desired rpm to rps
    double n = in.tail_thruster/60;
    
    // limit the max rpm to 1500 = 25 rps
    if(fabs(n) > 25)
        n = n / fabs(n) * 25;
    
    // advance velocity as per Fossen eq 4.6
    double omega = 0.1;
    double Va = (1 - omega) * u;
    
    if(fabs(n) > 1e-3)
        J0 = Va / (n * prop_diameter);      // as per Fossen eq 6.107

    double alpha1 = 0.5;
    double alpha2 = -4.0/11.0;
    double alpha3 = (0.45 - alpha1)/(-0.2);
    double alpha4 = 0.45 -(-0.2* (0.95-0.45) / (-0.5-(-0.2)) );
    double alpha5 = (0.95-0.45) / (-0.5-(-0.2));

    // these need to be determined
    // but relate to the torque from the rear thruster
    // not sure but it may result in a torque not around
    // the x-axis of the vehicle
    double beta1 = 0.0;
    double beta2 = -0.0;

    if (J0 > 0)
    {
        Kt = alpha1 + alpha2 * J0;   // Fossen eq 6.113
        Kq = beta1 + beta2 * J0;
    }
    else if (J0 > -0.2)
    {
        Kt = alpha1 + alpha3 * J0;			// Fossen eq 6.113
        Kq = beta1 + beta2 * J0;
    }
    else
    {
        Kt = alpha4 + alpha5 * J0; // Fossen eq 6.113
        Kq = beta1 + beta2 * J0;
    }

    double prop_force, prop_torque;
    prop_force = rho * pow(prop_diameter,4) * Kt * fabs(n) * n;     // As per Fossen eq 4.2
    prop_torque = rho * pow(prop_diameter,5) * Kq * fabs(n) * n; // Fossen 6.113
    //    cout << "n = " << n << " Va = " << Va << " J0 = " << J0 << " Kt = " << Kt << " F = " << prop_force << endl;
    if(prop_force !=  prop_force)
    {
        cerr << "Prop force error" << endl;
        prop_force = 0;
    }
    if(fabs(prop_force) > 10.0)
        prop_force = prop_force / fabs(prop_force) * 10;
    

    // the tunnel thruster forces needs to be determined as well
    // the lateral components affect yaw - psi and motion in y
    // the vertical components affect pitch - theta and motion in z
    // there is no effect on surge motion or roll
    //
    double tunnel_diameter = 0.1;

    // there is ZERO basis for this number (we use +-1500, full range is +-2048
    // from the DAC
    double to_rps = 0.03;

    n = in.vert_fore * to_rps;
    double vert_fore_force = rho * pow(tunnel_diameter,4) * Kt * fabs(n) * n;
    n = in.vert_aft * to_rps;
    double vert_aft_force = rho * pow(tunnel_diameter,4) * Kt * fabs(n) * n;
    n = in.lat_fore * to_rps;
    double lat_fore_force = rho * pow(tunnel_diameter,4) * Kt * fabs(n) * n;
    n = in.lat_aft * to_rps;
    double lat_aft_force = rho * pow(tunnel_diameter,4) * Kt * fabs(n) * n;

    double mutual_vert = vert_fore_force + vert_aft_force;
    double differential_vert = vert_fore_force - vert_aft_force;

    double mutual_lat = lat_fore_force + lat_aft_force;
    double differential_lat = lat_fore_force - lat_aft_force;

    double tail_x = prop_force * cos(in.tail_rudder) * cos(in.tail_elevator);
    double tail_y = prop_force * sin(in.tail_rudder) * cos(in.tail_elevator);
    double tail_z = prop_force * sin(in.tail_elevator);

    // external applied forces
    double X, Y, Z, K, M, N;
    X = tail_x;
    Y = tail_y + mutual_lat;
    Z = tail_z + mutual_vert;

    // it may also not be strictly just roll if the thrust is vectored
    K = prop_torque;  // this is approximate and still needs coefficients defined.
    // FIXME: This scaling factor shouldn't be necessary seems required to get the vehicle model to turn adequately
    M = tail_z * 1.25 + 0.75 * differential_vert;
    N = tail_y * 1.25 + 0.75 * differential_lat; 

    // Force vectors, props and control surfaces
    SMALL::Vector3D longF, latF;
    longF = X, Z, M;
    latF = Y, K, N;

    
    // Hydrodynamic Forces and Moments
    /*
    X = Xuu * u * fabs(u) + (Xwq - m) * w * q
        + (Xqq + m * xG) * pow(q, 2) + (Xvr + m) * v * r
        + (Xrr + m * xG) * pow(r, 2) - m * yG * p * q
        - m * zG * p * r + prop_force;
        
    Y = Yvv * v * fabs(v) + Yrr * r * fabs(r)
        + m * yG * pow(r, 2) + (Yur - m) * u * r
        + (Ywp + m) * w * p + (Ypq - m * xG) * p * q
        + Yuv * u * v + m * yG * pow(p, 2) - m * zG * q * r
        + Ydr * pow(u, 2) * rudder;
        
    Z = Zww * w * fabs(w) + Zqq * q * fabs(q)
        + (Zuq + m) * u * q + (Zvp - m) * v * p
        + (Zrp - m * xG) * r * p + Zuw * u * w
        + m * zG * (pow(p, 2) + pow(q, 2)) - m * yG * r * q
        + Zdp * pow(u, 2) * plane;
        
    K = Kpp * p * fabs(p) - (Izz - Iyy) * q * r
        + m * yG * (u * q - v * p) - m * zG * (w * p - u * r)
        + in(1);
        
    M = Mww * w * fabs(w) + Mqq * q * fabs(q)
        + (Muq - m * xG) * u * q + (Mvp + m * xG) * v * p
        + (Mrp - (Ixx - Izz)) * r * p
        + m * zG * (v * r - w * q) + Muw * u * w
        + Mdp * pow(u, 2) * plane;
        
    N = Nvv * v * fabs(v) + Nrr * r * fabs(r)
        + (Nur - m * xG) * u * r + (Nwp + m * xG) * w * p
        + (Npq - (Iyy - Ixx)) * p * q
        - m * yG * (v * r - w * q) + Nuv * u * v
        + Ndr * pow(u, 2) * rudder;
*/

    // For decoupled lateral and longitudinal forces as per Fossen 7.5.6

    // Velocity vectors, current state
    SMALL::Vector3D longV, latV;
    longV = u, w, q;
    latV = v, p, r;
    
    // Damping matrices modified to match the above equations
    SMALL::Matrix33 longDv, latDv;
    longDv = -Xuu * fabs(u), 						-Xwq * q, 					-Xqq * fabs(q),
            -Zuq * q - Zuw * w, 					-Zww * fabs(w), 			-Zqq * fabs(q),
            - Muq * q - Muw * w, 					-Mww * fabs(w), 			-Mqq * fabs(q);
    
    latDv = -Yvv * fabs(v) - Yuv * u,	-Ywp * w - Ypq * q, 					-Yrr * fabs(r) - Yur * u,
            0, 							-Kpp * fabs(p), 						0, // new addition based on above equations
//                        -Mvp * p, 					-Mpp * fabs(p), 						-Mrp * p,
            -Nvv * fabs(v) - Nuv * u, 	- Nwp * w - Npq * q, 					-Nrr * fabs(r) - Nur * u;
    
    // Damping matrices
    /*
    SMALL::Matrix33 longDv, latDv;
    longDv = -Xuu * fabs(u), 						-Xwq * q, 					-Xqq * fabs(q) - Xwq * w,
             -Zuq * q - Zuw * w, 					-Zww * fabs(w) - Zuw * u, 	-Zqq * fabs(q),
             -Muu * fabs(u) - Muq * q - Muw * w, 	-Mww * fabs(w), 			-Mqq * fabs(q);

    latDv = -Yvv * fabs(v), 			-Ywp * w - Ypq * q, 					-Yrr * fabs(r) - Yur * r,
            -Mvp * p, 					-Mpp * fabs(p), 						-Mrp * p,
            -Nvv * fabs(v) - Nuv * u, 	-Npp * fabs(p) - Nwp * w - Npq * q, 	-Nrr * fabs(r) - Nur * u;
    */

    // Coriolis matrices
    SMALL::Matrix33 longCv, latCv;
    longCv = 	0.0, 		0.0, 					0.0,
            0.0, 		0.0, 					-(m - Xudot) * u,
            0.0, 		(Zwdot - Xudot) * u, 	m * xG * u;
    
    latCv = 	0.0, 		0.0, 					(m- Xudot) * u,
            0.0, 		0.0, 					0.0,
            (Xudot - Yvdot) * u, 	0.0, 		m * xG * u;
    
    // Gravity and buoyancy
    SMALL::Vector3D longGn, latGn;
    longGn = 0.0, 0.0, W * zG  * sin(theta);
    latGn = 0.0, W * zG * sin(phi), 0.0;
    
    /*    cout << "X = ";
        for(int j=0; j<12; j++)
                cout << x[j] << " ";
        cout << endl;
*/    
    SMALL::Vector3D t1, t2;
    //    t1 = (longF - longGn - longCv * longV  - longDv * longV);
    t1 = longDv * longV;
    t2 = latDv * latV;
    //    t2 = (latF - latGn - latCv * latV - latDv * latV);
    //    cout << "F = " << t1(1) << " " << t2(1) << " " << t1(2) << " " << t2(2) << " " << t1(3) << " " << t2(3) << " "<< endl;
    static int count = 0;
    if (count++ > 999)
    { 
        cout << count << endl;
        cout << "propF = " << prop_force << endl;
        cout << "X = " << X << " Y = " << Y << " Z = " << Z << endl;
        cout << "K = " << K << " M = " << M << " N = " << N << endl;
        cout << "longF = " << longF.toString() << endl;
        cout << "longGn = " << longGn.toString() << endl;
        cout << "longDv = " << t1.toString() << endl;
        cout << "longCv = " << (longCv * latV).toString() << endl;
        cout << "latF = " << latF.toString() << endl;
        cout << "latGn = " << latGn.toString() << endl;
        cout << "latDv = " << t2.toString() << endl;
        cout << "latCv = " << (latCv * latV).toString() << endl;
        cout << endl;
        count = 0;
    }

    SMALL::Vector3D longDot, latDot;
    longDot = (longF - longGn - longCv * longV - longDv * longV) / longM;
    latDot = (latF - latGn - latCv * latV - latDv * latV) / latM;

    accel = longDot[0], latDot[0], longDot[1];

    // Rotate the velocites to the world frame
    SMALL::Vector3D etaDotLin;//, etaDotRot;
    etaDotLin = (auvPose.get3x4TransformationMatrix() * nuLinear);
    //etaDotRot = (auvPose.get3x4TransformationMatrix() * nuRotational); // will be unused, as the Euler rates transform is used below
    
    SMALL::Vector6D nu_dot;
    nu_dot = longDot[0], latDot[0], longDot[1], latDot[1], longDot[2], latDot[2];
    
    dxdt[XNDX] = etaDotLin[0] + CURRENT;
    dxdt[YNDX] = etaDotLin[1];
    dxdt[ZNDX] = etaDotLin[2];
    dxdt[ROLLNDX] = p + q*cos(phi)*tan(theta) + r*sin(phi)*tan(theta); //etaDotRot[0];
    dxdt[PITCHNDX] = q*cos(phi) - r*sin(phi); //etaDotRot[1];

    if (fabs(theta) > M_PI/2 - 0.1)
    {
        cout << "Close to gimbal lock" << endl;
    }
    dxdt[HDGNDX] = q*sin(phi)/cos(theta) + r*cos(phi)/cos(theta); //etaDotRot[2];
    dxdt[XDOTNDX] = nu_dot[0];
    dxdt[YDOTNDX] = nu_dot[1];
    dxdt[ZDOTNDX] = nu_dot[2];
    dxdt[ROLLDOTNDX] = nu_dot[3];
    dxdt[PITCHDOTNDX] = nu_dot[4];
    dxdt[HDGDOTNDX] = nu_dot[5];

}

// motor command callback
void on_motor_command(const lcm::ReceiveBuffer* rbuf, const std::string& channel, const auv_nga_motor_command_t *mc, lcm::LCM *lcm) 
{
    in = *mc;
}


double rand_n(void) // generate normally distributed variable given uniformly distributed variable using the Box-Muller method
{
    double u,v;
    u = (double)rand()/(double)RAND_MAX;
    v = (double)rand()/(double)RAND_MAX;

    return pow(-2*log(u),0.5)*sin(2*M_PI*v);
}

// Calculate the path, this is where the magic happens
void calculate(const lcm::ReceiveBuffer* rbuf, const std::string& channel, const heartbeat_t *heartbeat, lcm::LCM *lcm) 
{
    // call the solver
    //    integrate_n_steps(stepper, auv, state , 0.0 , 0.001 , 100 );
    integrate_const(stepper, auv, state , 0.0 , 0.01 , 0.001 ); // 1x speed simulation

    int64_t timeStamp = timestamp_now();

    //construct the nav message
    auv_acfr_nav_t nav;
    nav.utime = timeStamp;
    nav.x = state(0);
    nav.y = state(1);
    nav.depth = state(2);
    nav.roll = state(3);
    nav.pitch = state(4);
    nav.heading = state(5);
    nav.vx = state(6);
    nav.vy = state(7);
    nav.vz = state(8);
    nav.rollRate = state(9);
    nav.pitchRate = state(10);
    nav.headingRate = state(11);

    nav.altitude = WATER_DEPTH - nav.depth;

    double latitude_fix, longitude_fix;
    map_projection_sim->calc_geo_coords( nav.x + rand_n()*0.1, nav.y + rand_n()*0.1, latitude_fix, longitude_fix );
    nav.latitude = latitude_fix * M_PI/180;
    nav.longitude = longitude_fix * M_PI/180;

    if (timeStamp - last_print_time > 1*1e6) // 10 Hz
    {
        last_print_time = timeStamp;

        cout << "Truth: ";

        if(! fp.is_open() )
            fp.open("/tmp/log.txt",
        			ios::out|ios::app);
        for(int i=0; i<12; i++) {
            printf("%2.3f ", state(i));
            fp << state(i) << " ";
        }
        printf("%2.3f ", in.vert_fore);
        fp << in.vert_fore << " ";
        printf("%2.3f ", in.vert_aft);
        fp << in.vert_aft << " ";
        printf("%2.3f ", in.lat_fore);
        fp << in.lat_fore << " ";
        printf("%2.3f ", in.tail_thruster);
        fp << in.tail_thruster << " ";
        printf("%2.3f ", in.tail_rudder);
        fp << in.tail_rudder << " ";
        printf("%2.3f ", in.tail_elevator);
        fp << in.tail_elevator << " ";
        fp << "\n";
        fp.close();
        printf("\n");
        fflush(NULL);



        // publish the nav message
        lcm->publish(vehicle_name+".ACFR_NAV", &nav);

    }
    //	lcm->publish(vehicle_name+".ACFR_NAV", &nav);
    // for simulating the sensors, acfr_nav_new should be publishing the ACFR_NAV

    //IMU
    //rotate gravity into local frame
    SMALL::Vector3D grav_b;

    SMALL::Rotation3D Cbn;
    Cbn.setRollPitchYawRad(state(3), state(4), state(5));

    grav_b = (Cbn.i() * grav); // look to local_planner.cpp implementation if this doesn't work

    //rotate earth rotation into the local frame
    SMALL::Vector3D earth_rot_b;
    earth_rot_b = (Cbn.i() * earth_rot);

    //compute acceleration at current time step
//    SMALL::Vector3D accel;
//    auv_accel(accel);

    //construct IMU message (biases to come later)
    senlcm::IMU_t imu;
    imu.utime = timeStamp;

    double dt = (timeStamp - last_obs_time)/1e6;

    if (dt > 0)
    {
        ba_x = ba_x + ( -1/TAU_A * ba_x + pow( (2*pow(BIAS_A,2)) / (TAU_A*dt) , 0.5 )*rand_n())*dt;
        ba_y = ba_y + ( -1/TAU_A * ba_y + pow( (2*pow(BIAS_A,2)) / (TAU_A*dt) , 0.5 )*rand_n())*dt;
        ba_z = ba_z + ( -1/TAU_A * ba_z + pow( (2*pow(BIAS_A,2)) / (TAU_A*dt) , 0.5 )*rand_n())*dt;
        bg_x = bg_x + ( -1/TAU_G * bg_x + pow( (2*pow(BIAS_G,2)) / (TAU_G*dt) , 0.5 )*rand_n())*dt;
        bg_y = bg_y + ( -1/TAU_G * bg_y + pow( (2*pow(BIAS_G,2)) / (TAU_G*dt) , 0.5 )*rand_n())*dt;
        bg_z = bg_z + ( -1/TAU_G * bg_z + pow( (2*pow(BIAS_G,2)) / (TAU_G*dt) , 0.5 )*rand_n())*dt;
    }

    imu.angRate[0] = state(9) + earth_rot_b[0] + bg_x + STD_G*rand_n();
    imu.angRate[1] = state(10) + earth_rot_b[1] + bg_y + STD_G*rand_n();
    imu.angRate[2] = state(11) + earth_rot_b[2] + bg_z + STD_G*rand_n();
    imu.accel[0] = accel[0] + grav_b[0] + ba_x + STD_A*rand_n();
    imu.accel[1] = accel[1] + grav_b[1] + ba_y + STD_A*rand_n();
    imu.accel[2] = accel[2] + grav_b[2] + ba_z + STD_A*rand_n();
    lcm->publish(vehicle_name+".IMU", &imu);

    //    if (timeStamp - last_print_time > 0.1*1e6) // 10 Hz
    //    {
    //        last_print_time = timeStamp;
    //        cout << "IMU: " << imu.angRate[0] << " " << imu.angRate[1] << " "<< imu.angRate[2] << " "<< imu.accel[0] << " "<< imu.accel[1] << " "<< imu.accel[2] << endl;
    //        cout << "accel: " << accel[0] << " " << accel[1] << " "<< accel[2] << endl;

    //    }


    //TCM compass
    if (timeStamp - last_tcm_time > 0.1*1e6) // 10 Hz
    {
        last_tcm_time = timeStamp;
        senlcm::tcm_t tcm;
        tcm.utime = timeStamp;
        tcm.heading = state(5) - MAG_VAR + rand_n()*0.5*M_PI/180;
        tcm.roll= state(3) + rand_n()*0.25*M_PI/180;
        tcm.pitch= state(4) + rand_n()*0.25*M_PI/180;
        tcm.temperature = 20;
        lcm->publish(vehicle_name+".TCM", &tcm);
    }

    // YSI depth
    if (timeStamp - last_ysi_time > 0.1*1e6) // 10 Hz
        //if(0)
    {
        last_ysi_time = timeStamp;
        senlcm::ysi_t ysi;
        ysi.utime = timeStamp;
        ysi.salinity = 35; // ppt
        ysi.temperature = 20;
        ysi.depth = state(2) + rand_n()*0.1;
        ysi.turbidity = 0;
        ysi.chlorophyl = 0;
        ysi.conductivity = 0;
        ysi.oxygen = 0;
        ysi.battery = 0;
        lcm->publish(vehicle_name+".YSI", &ysi);
    }

    // GPS
    if (timeStamp - last_gps_time > 1*1e6) // 1 Hz
    {
        last_gps_time = timeStamp;
        if (nav.depth < 2) // 1m depth is where GPS cuts out, but simulation starts at 1m depth, so make it 2
        {
            SMALL::Vector4D nuLinear;
            nuLinear = nav.vx, nav.vy, nav.vz, 0.0;
            SMALL::Pose3D auvPose;
            auvPose.setPosition(nav.x, nav.y, nav.depth);
            auvPose.setRollPitchYawRad(nav.roll, nav.pitch, nav.heading);
            SMALL::Vector3D etaDotLin;
            etaDotLin = (auvPose.get3x4TransformationMatrix() * nuLinear);

            senlcm::gpsd3_t * gpsd3;
            gpsd3 = new senlcm::gpsd3_t;
            std::vector< int16_t > gused;
            std::vector< int16_t > gPRN;
            std::vector< int16_t > gelevation;
            std::vector< int16_t > gazimuth;
            std::vector< int16_t > gss;

            gpsd3->online = 0;
            gpsd3->fix.ept = 0;
            gpsd3->fix.epy = 0;
            gpsd3->fix.epx = 0;
            gpsd3->fix.epv = 0;
            gpsd3->fix.epd = 0;
            gpsd3->fix.eps = 0;
            gpsd3->fix.climb = 0;
            gpsd3->fix.epc = 0;
            gpsd3->geoidal_separation = 0;
            gpsd3->satellites_used = GPS_CHANNELS;
            for (int i=0; i<gpsd3->satellites_used; i++)
                gused.push_back(0);

            gpsd3->used = gused;
            gpsd3->dop.pdop = 0;
            gpsd3->dop.hdop = 0;
            gpsd3->dop.tdop = 0;
            gpsd3->dop.gdop = 0;
            gpsd3->dop.xdop = 0;
            gpsd3->dop.ydop = 0;
            gpsd3->epe    = 0;
            gpsd3->skyview_utime = 0;
            gpsd3->satellites_visible = GPS_CHANNELS;
            for (int i=0; i<gpsd3->satellites_visible; i++) {
                gPRN.push_back(0);
                gelevation.push_back(0);
                gazimuth.push_back(0);
                gss.push_back(0);
            }
            gpsd3->PRN = gPRN;
            gpsd3->elevation = gelevation;
            gpsd3->azimuth = gazimuth;
            gpsd3->ss = gss;
            gpsd3->dev.path = strdup("");
            gpsd3->dev.flags = 0;
            gpsd3->dev.driver = strdup("");
            gpsd3->dev.subtype = strdup("");
            gpsd3->dev.activated = 0;
            gpsd3->dev.baudrate = 0;
            gpsd3->dev.stopbits = 0;
            gpsd3->dev.cycle = 0;
            gpsd3->dev.mincycle = 0;
            gpsd3->dev.driver_mode = 0;
            gpsd3->fix.track = atan2( etaDotLin[1], etaDotLin[0] ) * 180/M_PI;
            gpsd3->fix.speed = pow(pow(etaDotLin[0],2) + pow(etaDotLin[1],2),0.5);
            double latitude_fix, longitude_fix;
            map_projection_sim->calc_geo_coords( nav.x + rand_n()*2, nav.y + rand_n()*2, latitude_fix, longitude_fix );
            gpsd3->fix.latitude = latitude_fix * M_PI/180;
            gpsd3->fix.longitude = longitude_fix * M_PI/180;
            gpsd3->fix.altitude = -nav.depth;
            gpsd3->fix.utime = timeStamp;
            gpsd3->utime = timeStamp;
            gpsd3->fix.mode = 3;
            gpsd3->status = 1;
            gpsd3->tag = strdup("");
            lcm->publish(vehicle_name+".GPSD_CLIENT", gpsd3);
            delete gpsd3;

                              // publish battery data
              senlcm::os_power_system_t battery_pack;
              battery_pack.utime = timestamp_now();
              battery_pack.avg_charge_p = 100;
              // need to have number of controllers set or you get a seg fault when publishing sometimes.
              battery_pack.num_controllers = 0;
              lcm->publish("NGA.BATTERY", &battery_pack);
        }
    }

    // DVL
    if (timeStamp - last_dvl_time > 0.3*1e6) // once every 0.3 seconds
    {
        last_dvl_time = timeStamp;
        senlcm::rdi_pd5_t rdi;

        rdi.utime = timeStamp;
        rdi.pd4.xducer_head_temp = 20;
        rdi.pd4.altitude = nav.altitude;
        rdi.pd4.range[0] = nav.altitude;
        rdi.pd4.range[1] = nav.altitude;
        rdi.pd4.range[2] = nav.altitude;
        rdi.pd4.range[3] = nav.altitude;
        rdi.pd4.btv[0] = nav.vx + rand_n()*0.003;
        rdi.pd4.btv[1] = nav.vy + rand_n()*0.003;
        rdi.pd4.btv[2] = nav.vz + rand_n()*0.003;
        rdi.pitch = nav.pitch + rand_n()*0.25*M_PI/180;
        rdi.roll = nav.roll + rand_n()*0.25*M_PI/180;
        rdi.heading = nav.heading  - MAG_VAR + rand_n()*2*M_PI/180;

        if (nav.altitude < 50)
            rdi.pd4.btv_status = 0;
        else
            rdi.pd4.btv_status = 1; // beyond dvl bl range

        rdi.pd4.speed_of_sound = 1521.495;
        lcm->publish(vehicle_name+".RDI", &rdi);
    }

    last_obs_time = timeStamp;

}

void on_nav_store(const lcm::ReceiveBuffer* rbuf, const std::string& channel, const auv_acfr_nav_t *nav, lcm::LCM *lcm) {
    cout << "Nav: ";

    double nav_state[13];

    nav_state[0] = nav->x;
    nav_state[1] = nav->y;
    nav_state[2] = nav->depth;
    nav_state[3] = nav->roll;
    nav_state[4] = nav->pitch;
    nav_state[5] = nav->heading;
    nav_state[6] = nav->vx;
    nav_state[7] = nav->vy;
    nav_state[8] = nav->vz;
    nav_state[9] = nav->rollRate;
    nav_state[10] = nav->pitchRate;
    nav_state[11] = nav->headingRate;

    nav_state[12] = nav->altitude;

    if( !fp_nav.is_open() )
        fp_nav.open( "/tmp/log_nav.txt",
    			ios::out | ios::app);
    for(int i=0; i<13; i++) {
        printf("%2.3f ", nav_state[i]);
        fp_nav << nav_state[i] << " ";
    }
    fp_nav << "\n";
    fp_nav.close();
    printf("\n");
    fflush(NULL);

}

void print_help (int exval, char **argv)
{                                                                               
    printf("Usage:%s [-h] [-n VEHICLE_NAME]\n\n", argv[0]);                     
    
    printf("  -h                               print this help and exit\n");    
    printf("  -n VEHICLE_NAME                  set the vehicle_name\n");        
    exit (exval);                                                               
}                                                                               
                                                                                
void                                                                            
parse_args (int argc, char **argv)                                              
{                                                                            
    int opt;                                                                    
                                                                                
    while ((opt = getopt (argc, argv, "hn:")) != -1)                            
    {                                                                           
        switch(opt)                                                             
        {                                                                       
            case 'h':                                                               
                print_help (0, argv);                                               
                break;                                                              
            case 'n':                                                               
                vehicle_name = (char*)optarg;                                       
                break;                                                              
         }                                                                      
    }                                                                           
}
        
int main_exit;
void signal_handler(int sig)
{
    main_exit = 1;
}

int main(int argc, char **argv)
{
    cout << "Starting sim..." << endl;
    parse_args(argc, argv);

    // install the signal handler
    main_exit = 0;
    signal(SIGINT, signal_handler);

    lcm::LCM lcm;

    //srand(time(NULL));
    srand(1234); // seeding the same way every time to allow filter comparisons

    BotParam *param = NULL;
    param = bot_param_new_from_server (lcm.getUnderlyingLCM(), 1);
    if(param == NULL)
        return 0;

    char rootkey[64];
    char key[128];
    sprintf (rootkey, "nav.acfr-nav-new");

    double latitude_sim, longitude_sim;
    sprintf(key, "%s.latitude", rootkey);
    latitude_sim = bot_param_get_double_or_fail(param, key);
    sprintf(key, "%s.longitude", rootkey);
    longitude_sim = bot_param_get_double_or_fail(param, key);

    map_projection_sim = new Local_WGS84_TM_Projection(latitude_sim, longitude_sim);

    //Earth rotation vector in the navigation frame
    earth_rot = cos(latitude_sim*M_PI/180)*7.292115e-5,0,-sin(latitude_sim*M_PI/180)*7.292115e-5;

    //Set gravity
    grav = 0.0, 0.0, -9.81;

    int64_t timeStamp = timestamp_now();
    last_dvl_time = timeStamp;
    last_gps_time = timeStamp;
    last_ysi_time = timeStamp;
    last_tcm_time = timeStamp;
    last_print_time = timeStamp;
    last_obs_time = timeStamp;

    //initialise drifting biases
    ba_x = BIAS_A*rand_n();
    ba_y = BIAS_A*rand_n();
    ba_z = BIAS_A*rand_n();
    bg_x = BIAS_G*rand_n();
    bg_y = BIAS_G*rand_n();
    bg_z = BIAS_G*rand_n();


    lcm.subscribeFunction(vehicle_name+".NEXTGEN_MOTOR", on_motor_command, &lcm);
    lcm.subscribeFunction("HEARTBEAT_10HZ", calculate, &lcm);
    //lcm.subscribeFunction("HEARTBEAT_100HZ", calculate, &lcm); // needs to happen at 100 Hz due to IMU
    //lcm.subscribeFunction(vehicle_name+".ACFR_NAV", on_nav_store, &lcm);

    //populate_inv_inertia();

    // Interial matrices
    longM = m - Xudot, 			-Xwdot, 			m * zG - Xqdot,
            -Xwdot, 			m - Zwdot, 			-m * xG - Zqdot,
            m * zG - Xqdot, 	-m * xG - Zqdot, 	Iy - Mqdot;

    latM = 	m - Yvdot,			-m * zG - Ypdot,	m * xG - Yrdot,
            -m * zG - Ypdot,	Ix - Kpdot,			-Izx - Krdot,
            m * xG - Yrdot,		-Izx - Krdot,		Iz - Nrdot;

    // initial conditions
    state(0) = -10;
    state(1) = -10;
    state(2) = 1.9;  //Z = 1;
    state(3) = 0;
    state(4) = 0;
    state(5) = 0;
    state(6) = 0;
    state(7) = 0;
    state(8) = 0;
    state(9) = 0;
    state(10) = 0;
    state(11) = 0;

    in.vert_fore = 0;
    in.vert_aft = 0;
    in.lat_fore = 0;
    in.lat_aft = 0;
    in.tail_thruster = 0;
    in.tail_rudder = 0;
    in.tail_elevator = 0;

    fp_nav.open( "/tmp/log_nav.txt", ios::out);
    fp.open("/tmp/log.txt", ios::out);

    int fd = lcm.getFileno();
    fd_set rfds;
    while(!main_exit)
    {
        FD_ZERO (&rfds);
        FD_SET (fd, &rfds);
        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        int ret = select (fd + 1, &rfds, NULL, NULL, &timeout);
        if(ret > 0)
            lcm.handle();
    }
    delete map_projection_sim;
    if( fp.is_open())
    	fp.close();
    if( fp_nav.is_open() )
    	fp_nav.close();
    return 0;
}



