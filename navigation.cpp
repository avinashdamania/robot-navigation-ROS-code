#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include "plan_execution/ExecutePlanAction.h"
#include <actionlib/client/simple_action_client.h>

typedef actionlib::SimpleActionClient<plan_execution::ExecutePlanAction> client; 

//navigation code based on Nick Walker's BWI go_to_location and go_to_door code for BWI API
//not final; subject to change. 

const int numberOfRooms = 4; 
const std::string rooms[] = {"2.210", "4.304", "5.302", "6.302"};

using namespace std; 
int main(int argc, char** argv) {
    ros::init(argc, argv, "navigation");
    ros::NodeHandle n;

    ros::NodeHandle privateNode("~");
    string location;
//  privateNode.param<string>("location", location, "13_414b"); 

    std::vector<string> doors;
    
    doors.push_back("d2_210"); 
    doors.push_back("d4_304"); 
    doors.push_back("d5_302");
    doors.push_back("d6_302"); 
    
    int currentDoor = 0; 

    Client client("action_executor/execute_plan", true);
    client.waitForServer(); 

    bool fromAtoB = true; 
    
    while(ros::ok()) {
	location = doors.at(current_door); 
	current_door++;
	if(current_door >= (int) doors.size())
	    current_door = 0; 

	ROS_INFO_STREAM("going to " << location);
	plan_execution:ExecutePlanGoal goal;
    
    	plan_execution::AspRule rule;
    	plan_execution::AspFluent fluent;
    	fluent.name = "not facing";

    	fluent.variables.push_back(location); 
    
    	rule.body.push_back(fluent);
    	goal.aspGoal.push_back(rule); 
    
    	ROS_INFO("Sending goal");
    	actionClient.sendGoal(goal);

    	ros::Rate wait_rate(10);
    	while(ros::ok() && !client.getState().isDone()) {
	    wait_rate.sleep();
    	}
    
    	if(!client.getState().isDone()) {
	    ROS_INFO("Canceling goal");
	    client.cancelGoal();
	    for(int i = 0; !client.getState.isDone() && i < 50; ++i) 
	    	wait_rate.sleep();
    	}

	//checking if operation was success
    	if(client.getState() == actionlib::SimpleClientGoalState::ABORTED) {
	    ROS_INFO("Aborted");
    	}
    	else if(client.getState() == actionlib::SimpleClientGoalState::PREEMPTED) {
	    ROS_INFO("Preempted");
        }
    	else if(client.getState() == actionlib::SimpleClientGoalState::SUCCEEDED) {
	    ROS_INFO("Succeeded!");
    	}
    	else
	    ROS_INFO("Terminated"); 
    }

    return 0; 
}
