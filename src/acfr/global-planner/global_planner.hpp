#include <signal.h>

#include <lcm/lcm-cpp.hpp>
#include "perls-lcmtypes++/acfrlcm/auv_path_response_t.hpp"
#include "perls-lcmtypes++/acfrlcm/auv_path_command_t.hpp"
#include "perls-lcmtypes++/acfrlcm/auv_global_planner_t.hpp"
#include "perls-lcmtypes++/acfrlcm/auv_global_planner_usbl_abort_t.hpp"
#include "perls-lcmtypes++/acfrlcm/auv_global_planner_state_t.hpp"
#include "perls-lcmtypes++/acfrlcm/auv_camera_trigger_t.hpp"
#include "perls-lcmtypes++/senlcm/rdi_control_t.hpp"
#include "perls-common/timestamp.h"
#include <bot_param/param_client.h>


#include "acfr-mission/mission.hpp"
#include "acfr-mission/mission_command.hpp"

#ifndef GlobalPlanner_H
#define GlobalPlanner_H

// mission FSM states
typedef enum
{
	globalPlannerFsmIdle,
	globalPlannerFsmRun,
	globalPlannerFsmAbort,
	globalPlannerFsmPause
} GlobalPlannerStateT;

typedef enum
{
	globalPlannerReset = -1,
	globalPlannerRun,
	globalPlannerAbort,
	globalPlannerStop,
	globalPlannerPause,
	globalPlannerResume,
	globalPlannerIdle
} globalPlannerMessageT;

class GlobalPlanner
{
public:
	GlobalPlanner();
	~GlobalPlanner();
	GlobalPlannerStateT getCurrentState();
	string getCurrentStateString();

	int clock();
	int process();

	bool skipWaypoint;
	bool areWeThereYet;
	bool holdOff;
	double distanceToGoal;
	void set_filename(string filename);
	bool loadNewMissionFile(string filename);
	bool loadNewMissionString(string mission_string);
	int loadConfig();
	double turningRadius;

	Mission mis;
	globalPlannerMessageT globalPlannerMessage;
private:

	lcm::LCM lcm;

	GlobalPlannerStateT currentState;
	GlobalPlannerStateT nextState;

	// mission stuff
	list<waypoint>::iterator currPoint;
	int sendLeg();
	int sendCommands(list<MissionCommand> &commands);
	int64_t legStartTime;

	// command settings
	acfrlcm::auv_camera_trigger_t cameraTriggerMsg;
	senlcm::rdi_control_t rdiCommandMsg;
};

#endif
