#include <lcm/lcm-cpp.hpp>
#include <signal.h>
#include <string>
#include <libgen.h>
#include <pthread.h>
#include <error.h>
#include <bot_param/param_client.h>
#include "DubinsPath.h"
#include "perls-common/timestamp.h"
#include "perls-lcmtypes++/perllcm/heartbeat_t.hpp"
#include "perls-lcmtypes++/acfrlcm/auv_acfr_nav_t.hpp"
#include "perls-lcmtypes++/acfrlcm/auv_path_command_t.hpp"
#include "perls-lcmtypes++/acfrlcm/auv_path_response_t.hpp"
#include "perls-lcmtypes++/acfrlcm/auv_control_t.hpp"

#ifndef LOCAL_PLANNER_HPP
#define LOCAL_PLANNER_HPP


class LocalPlanner {
    public:
        LocalPlanner();
        ~LocalPlanner();
        int loadConfig(char *programName);
        int onNav(const acfrlcm::auv_acfr_nav_t *nav);
        int onPathCommand(const acfrlcm::auv_path_command_t *pc);
        int calculateWaypoints();
        lcm::LCM lcm;
        int process();
        int sendResponse();
      
        Pose3D getCurrPose( void ) const { return currPose; }
        Pose3D getDestPose( void ) const { return destPose; }
        Pose3D getStartPose( void ) const { return startPose; }
        
        Vector3D getCurrVel(void) const { return currVel; }
        double getDestVel(void) const { return destVel; }
        double getStartVel(void) const { return startVel; }
        
        int getDepthMode(void) const { return depthMode; }
        double getDefaultLegVel(void) const { return defaultLegVel; }
        double getVelChangeDist(void) const { return velChangeDist; }
      
        bool getDestReached( void ) const { return destReached; }
        void setDestReached( bool b ) { destReached = b; }
        
        bool getNewDest( void ) const { return newDest; }
        void setNewDest( bool b ) { newDest = b; }
        
        double getWpDropDist( void ) const { return wpDropDist; }
        
        pthread_mutex_t currPoseLock;
        pthread_mutex_t destPoseLock;
        pthread_mutex_t waypointsLock;
        
        vector<Pose3D> waypoints;
        
    private:
        
        Pose3D currPose;
        Vector3D currVel;
        
        Pose3D startPose;
        double startVel;
        
        Pose3D destPose;
        double destVel;
        
        bool newDest;
        bool destReached;
        
        int depthMode;
        
        int destID;
        
        double wpDropDist;
        double turningRadius;
        double maxPitch;
        double lookaheadVelScale;
        double maxDistFromLine;
        double maxAngleFromLine;
        double velChangeDist;
        double defaultLegVel;

        
        
        
};        
#endif
