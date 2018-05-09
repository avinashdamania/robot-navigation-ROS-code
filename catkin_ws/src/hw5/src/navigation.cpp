
#include <ros/ros.h>
#include "plan_execution/ExecutePlanAction.h"
#include <actionlib/client/simple_action_client.h>
#include "std_msgs/String.h"


// SimpleActionClient used to send doors/rooms as navigation goals
typedef actionlib::SimpleActionClient<plan_execution::ExecutePlanAction> Client;

using namespace std;

ros::Publisher chatter_pub;
string location; // the destination
int stopBool; // used to implement the stop button functionality




void navCallback(const std_msgs::String::ConstPtr& loc) {

	// retrieve the string representing the navigation destination
    location = loc->data;

    // create our SimpleActionClient as typedef'd above
    Client client("/action_executor/execute_plan", true);
    client.waitForServer();
    // we send a bad location if the stop button was clicked
    if (location.compare("eeeee") == 0) {
        stopBool = 1;
    }

    ROS_INFO_STREAM("going to " << location);
    
    // create the navigation goal. taken from visit_door_list.cpp by Piyush Khandelwal
    // and Yuqian Jiang in the utexas-bwi repo
    plan_execution::ExecutePlanGoal goal;
    plan_execution::AspRule rule;
    plan_execution::AspFluent fluent;
    fluent.name = "not facing";
    fluent.variables.push_back(location);
    rule.body.push_back(fluent);
    goal.aspGoal.push_back(rule);
    
    // send our goal to the SimpleActionClient
    ROS_INFO("sending goal");
    client.sendGoal(goal);
    
    
    ros::Rate wait_rate(10);
    // during navigation, check if stopBool was set
    while (ros::ok() && !client.getState().isDone()) {
        if (stopBool) {
	        ROS_INFO("canceling goal");
	        client.sendGoal(goal);
	        client.cancelGoal();
	        stopBool = 0;
    	}  
        wait_rate.sleep();
    }      

    if (client.getState() == actionlib::SimpleClientGoalState::ABORTED) {
        ROS_INFO("Aborted");
    } else if (client.getState() == actionlib::SimpleClientGoalState::PREEMPTED) {
        ROS_INFO("Preempted");
        return;
    } else if (client.getState() == actionlib::SimpleClientGoalState::SUCCEEDED) {
        ROS_INFO("Succeeded!");
        // send a message back to the frontend web interface so it can display that
        // the robot successfully arrived
        std_msgs::String msg;
        std::stringstream ss;
        ss << "arrived";
        msg.data = ss.str();
        ROS_INFO("%s", msg.data.c_str());
        chatter_pub.publish(msg);
    } else {
        ROS_INFO("Terminated");
    }
}

// used to change the stopBool when the stop button is clicked
void stopNavCallback(const std_msgs::String::ConstPtr& loc) {
    ROS_INFO("boolean changed we good ");
    stopBool = 1;
}


int main(int argc, char **argv) {
    ros::init(argc, argv, "door_navigation");
    ros::NodeHandle n;
    stopBool = 0;

    chatter_pub = n.advertise<std_msgs::String>("chatter_pub", 1000); // for sending info back to the web interface
    ros::Subscriber sub = n.subscribe("roomNumber", 100, navCallback); // for retrieving navigation goals from the web interface
    ros::Subscriber sub2 = n.subscribe("stopNav", 100, stopNavCallback); // for retrieving stop requests from the web interface stop button

    ros::AsyncSpinner spinner(4);
    spinner.start();
    ros::waitForShutdown();
    
    return 0;
}

