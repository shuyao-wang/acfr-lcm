

#include <libxml++/libxml++.h>
#include <string>
#include <fstream>
#include <iostream>
#include "MissionPrimitive.hpp"
#include "ZamboniePath.h"
#include "SpiralInwardPath.h"
#include "SpiralPath.h"
#include "GotoPath.h"
#include "LegPath.h"    

#ifndef MISSION_HPP
#define MISSION_HPP

class raw_command
{
    public:
        Glib::ustring command;
        Glib::ustring value;
};
    

class Mission
{
    public:
        Mission();
        Mission(string filename);
        ~Mission();
        
        int load(string filename);
        
        int dump();
        void dumpMatlab(string filename="matlab_plot.m");
        // the list of goal points
        list<waypoint> waypoints;
        
    private:
        string filename;
        int load();
        
        // helper functions
        int grid_to_points();
        int getXYZ(const xmlpp::Element* element, double &x, double &y, double &z);
        int getVelocity(const xmlpp::Element* element, double &vx, double &vy, double &vz);
        int getSingleValue(const xmlpp::Element* element, string tag, double &val);
        int getDepthMode(const xmlpp::Element* element);
        int getSingleValue(const xmlpp::Element* element, string tag, Glib::ustring &val);
        int getCommand(const xmlpp::Element* element);
        
        // parsers
        int parsePrimitive(xmlpp::Node *node);
        int parseGlobals(xmlpp::Node::NodeList &globals);
        
        
        // common variables
        list<MissionCommand> commands;
        double timout;
        depthModeT depthMode;
        
        // Mission globals
        double dropDistance;
        double dropAngleRad;
        double missionTimeout;
        
};


#endif
