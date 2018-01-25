#include "local_planner_nga.hpp"
#include <string>
#include <libgen.h>
#include <signal.h>
#include <pthread.h>
#include <error.h>
#include <unistd.h>
#include <bot_param/param_client.h>
#include <cstdio>
#include <iomanip>
//#include <atomic>

#include "perls-lcmtypes++/acfrlcm/auv_path_response_t.hpp"
#include "perls-lcmtypes++/acfrlcm/auv_control_t.hpp"
#include "perls-lcmtypes++/acfrlcm/auv_local_path_t.hpp"

using namespace std;


/** *************************************************************************
 *
 * LocalPlanner
 *
 */
LocalPlannerTunnel::LocalPlannerTunnel() :
    LocalPlanner()
{
	gpState.state = acfrlcm::auv_global_planner_state_t::IDLE;

	fp.open("/tmp/log_waypoint.txt", ios::out);
	fp_wp.open("/tmp/log_waypoint_now.txt", ios::out);

	cout << endl << endl << endl << "LocalPlannerNGA started" << endl;
}

// this is so the parent class destructor is called
// do NOT delete
LocalPlannerTunnel::~LocalPlannerTunnel()
{
}



/**
 * Call Dubins path planner to generate a path from current location to the
 * destination waypoint
 */
int LocalPlannerTunnel::calculateWaypoints()
{

	// Get a copy of curr and dest pose
	Pose3D currPose = this->currPose;
	Pose3D destPose = this->destPose;
	double currVel = this->currVel[0];

	cout << timestamp_now() << " Calculating new waypoints..." << endl;
	cout << setprecision(2) << "CurrPose=" << currPose.getX() << ","
			<< currPose.getY() << "," << currPose.getZ() << " < "
			<< currPose.getYawRad() / M_PI * 180 << endl;
	cout << setprecision(2) << "DestPose=" << destPose.getX() << ","
			<< destPose.getY() << "," << destPose.getZ() << " < "
			<< destPose.getYawRad() / M_PI * 180 << endl;

	Pose3D destPoseRel = getRelativePose(destPose);
	double relAngle = atan2( destPoseRel.getY(), destPoseRel.getX() );
	cout << "Dest rel: X=" << destPoseRel.getX()
			<< ", angle=" << relAngle/M_PI*180 << endl;

	bool success = false;
	vector<Pose3D> wps;

	// If the waypoint is just ahead of us no need to use Dubins. We will rely
	//	on the controller to get us there.
	if (destPoseRel.getX() < 0 ||
	    destPoseRel.getX() > 2*turningRadius ||
		fabs(relAngle) > 45./180*M_PI )
	{
		DubinsPath dp;
		dp.setCircleRadius(turningRadius);
		dp.setMaxPitch(maxPitch);
		dp.setWaypointDropDist(wpDropDist);
		dp.setWaypointDropAngleFromDropDist();

		// Don't use current pose but a pose looking a bit ahead. We adjust this
		//	by employing the current velocity
		double lookAheadTime = 0.2; // [s]
		Pose3D lookAheadPose;
		// Using fabs of vel to ensure that even if we are going backwards the
		// lookaheadpose is in front of us
		lookAheadPose.setX(fabs(currVel) * lookAheadTime);
		Pose3D startPose = currPose.compose(lookAheadPose);

		// Try to calculate a feasible Dubins path. If we fail we try
		//  a second time with twice the circle radius
		wps = dp.calcPath(startPose, destPose);

		// TODO: should we be more intelligent here?
		// maybe increase the radius and try again (we had this behavior before)
		if ((wps.size() == 0))
		{
			cerr << "Failed to calculate a feasible path using Dubins path"
					<< endl;

				wps.push_back(destPose);
				success = false;
		}
		else {
			cout << "New waypoints calculated using Dubins" << endl;
			success = true;
		}
	}
	else
	{
		cout << "We are close to the new waypoint "
				<< "and use the controller to get us there."
				<< endl;
		wps.push_back(destPose);
		success = true;
	}

	// Managed to calculate a path to destination
	// TODO: do we need mutex around this?
	waypoints.clear();
	waypoints = wps;

	// Save the start pose and start velocity
	startPose = currPose;

	//setDestReached( false );

	printWaypoints();
	publishWaypoints();

	return success;
}


int LocalPlannerTunnel::loadConfig(char *program_name)
{
	BotParam *param = NULL;
	param = bot_param_new_from_server(lcm.getUnderlyingLCM(), 1);
	if (param == NULL
	)
		return 0;

	char rootkey[64];
	char key[128];
	sprintf(rootkey, "acfr.%s", program_name);

	sprintf(key, "%s.turning_radius", rootkey);
	turningRadius = bot_param_get_double_or_fail(param, key);

	sprintf(key, "%s.minimum_altitude", rootkey);
	minAltitude = bot_param_get_double_or_fail(param, key);

	sprintf(key, "%s.maximum_pitch", rootkey);
	maxPitch = bot_param_get_double_or_fail(param, key);

	sprintf(key, "%s.velocity_change_distance", rootkey);
	velChangeDist = bot_param_get_double_or_fail(param, key);

	sprintf(key, "%s.default_leg_velocity", rootkey);
	defaultLegVel = bot_param_get_double_or_fail(param, key);

	sprintf(key, "%s.look_ahead_velocity_scale", rootkey);
	lookaheadVelScale = bot_param_get_double_or_fail(param, key);

	sprintf(key, "%s.max_dist_from_line", rootkey);
	maxDistFromLine = bot_param_get_double_or_fail(param, key);

	sprintf(key, "%s.max_angle_from_line", rootkey);
	maxAngleFromLine = bot_param_get_double_or_fail(param, key);

	sprintf(key, "%s.waypoint_timeout", rootkey);
	waypointTimeout = bot_param_get_double_or_fail(param, key);

	sprintf(key, "%s.forward_bound", rootkey);
	forwardBound = bot_param_get_double_or_fail(param, key);

	sprintf(key, "%s.side_bound", rootkey);
	sideBound = bot_param_get_double_or_fail(param, key);

	sprintf(key, "%s.drop_dist", rootkey);
	wpDropDist = bot_param_get_double_or_fail(param, key);

	sprintf(key, "%s.dist_to_dest_bound", rootkey);
	distToDestBound = bot_param_get_double_or_fail(param, key);

	sprintf(key, "%s.max_angle_waypoint_change", rootkey);
	maxAngleWaypointChange = bot_param_get_double_or_fail(param, key);

	sprintf(key, "%s.radius_increase", rootkey);
	radiusIncrease = bot_param_get_double_or_fail(param, key);

	sprintf(key, "%s.max_angle", rootkey);
	maxAngle = bot_param_get_double_or_fail(param, key);

	sprintf(key, "%s.drop_angle", rootkey);
	wpDropAngle = bot_param_get_double_or_fail(param, key);

	sprintf(key, "%s.replan_interval", rootkey);
	replanInterval = bot_param_get_double_or_fail(param, key);

	return 1;
}


/**
 * Look at the next waypoint and
 * 1) remove it from the list if we have reached it or
 * 2) calculate commands for the controller
 */
int LocalPlannerTunnel::processWaypoints()
{

	// Nothing to do
	if (waypoints.size() == 0)
	{
		return 0;
	}

	// Get the next waypoint
	Pose3D wp = waypoints.at(0);

	// We have reached the next waypoint
	if (pointWithinBound(wp))
	{

		printf( "[%3.2f, %3.2f, %3.2f] reached.\n",
				wp.getX(),
				wp.getY(),
				wp.getZ() );
		waypoints.erase(waypoints.begin());
		resetWaypointTime(timestamp_now());

		printWaypoints();

		// No more waypoints to process
		if (waypoints.size() == 0)
		{
			cout << timestamp_now() << " No more waypoints!" << endl;

			if (pointWithinBound(destPose))
			{
				setDestReached(true);
				cout << "We have reached our destination :)" << endl;
			}

//			// form a STOP message to send
//			acfrlcm::auv_control_t cc;
//			cc.utime = timestamp_now();
//			cc.run_mode = acfrlcm::auv_control_t::STOP;
//			lcm.publish("AUV_CONTROL", &cc);

			return getDestReached();
		}

	}

	Pose3D currPose = getCurrPose();

	// Calculate desired heading to way point
	//  This is not our bearing to the point but a global angle
	double desHeading = atan2(wp.getY() - currPose.getY(),
			wp.getX() - currPose.getX());

	// Calculate desired velocity. This is set to dest velocity by default
	double desVel = destVel;
	// Ramp down the velocity when close to the destination
	//double distToDest = getDistToDest();
	//if( distToDest < velChangeDist ) {
	//	desVel = destVel * (distToDest / velChangeDist);
	//}

	// form a message to send
	acfrlcm::auv_control_t cc;
	cc.utime = timestamp_now();
	cc.run_mode = acfrlcm::auv_control_t::RUN;
	cc.heading = desHeading;
	cc.vx = desVel;
    static double depth_ref = 0.0;
    double curr_depth_ref;
	if (getDepthMode() == acfrlcm::auv_path_command_t::DEPTH)
	{
		//cc.depth = wp.getZ();
        curr_depth_ref = wp.getZ();

        // check we don't get closer to the bottom than our minimum
		double curr_alt_ref = currPose.getZ() + (currAltitude - minAltitude);
        if (curr_alt_ref < curr_depth_ref)
            curr_depth_ref = curr_alt_ref;

		cc.depth_mode = acfrlcm::auv_control_t::DEPTH_MODE;
	}
	else
	{
		// set the depth goal using the filtered desired altitude.
		//cc.depth = currPose.getZ() + (currAltitude - wp.getZ());
		curr_depth_ref = currPose.getZ() + (currAltitude - wp.getZ());
		cc.depth_mode = acfrlcm::auv_control_t::DEPTH_MODE;
	}
    // FIXME: limit the depth rate change to yield an achievable 
    // trajectory. This is modelled on a forward speed of 0.75m/s 
    // with a max pitch of 0.3rad.  This should be configurable or
    // calculated automatically.
    double NAV_DT = 0.1;
    double max_depth_ref_change = 0.2*NAV_DT;
    double depth_ref_error = curr_depth_ref - depth_ref;
    if (depth_ref_error > max_depth_ref_change)
        depth_ref += max_depth_ref_change;
    else if (depth_ref_error < -max_depth_ref_change)
        depth_ref -= max_depth_ref_change;
    else
        depth_ref = curr_depth_ref;

    cc.depth = depth_ref;

	lcm.publish("AUV_CONTROL."+vehicle_name, &cc);
	return 1;
}
