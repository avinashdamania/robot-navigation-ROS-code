
#include <ros/ros.h>
#include "plan_execution/ExecutePlanAction.h"
#include <actionlib/client/simple_action_client.h>
#include "std_msgs/String.h"

typedef actionlib::SimpleActionClient<plan_execution::ExecutePlanAction> Client;


using namespace std;

ros::Publisher chatter_pub;
string location;
int stopBool;




void navCallback(const std_msgs::String::ConstPtr& loc) {

    //TLDR: By creating a bad location destination, the robot stops.
    location = loc->data;

    Client client("/action_executor/execute_plan", true);
    client.waitForServer();
    if (location.compare("eeeee") == 0) {
        stopBool = 1;
    }


    ROS_INFO_STREAM("going to " << location);
    
    plan_execution::ExecutePlanGoal goal;
    
    plan_execution::AspRule rule;
    plan_execution::AspFluent fluent;
    fluent.name = "not facing";
    
    fluent.variables.push_back(location);
    
    rule.body.push_back(fluent);
    goal.aspGoal.push_back(rule);
    
    ROS_INFO("sending goal");
    //maybe use clientgoalhandle?
    client.sendGoal(goal);
    
    
    ros::Rate wait_rate(10);
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

void stopNavCallback(const std_msgs::String::ConstPtr& loc) {
    ROS_INFO("boolean changed we good ");
    stopBool = 1;
}


int main(int argc, char**argv) {
    ros::init(argc, argv, "door_navigation");
    ros::NodeHandle n;
    stopBool = 0;
        
    
    //doors.push("d3_424");
    
    chatter_pub = n.advertise<std_msgs::String>("chatter_pub", 1000);
    ros::Subscriber sub = n.subscribe("roomNumber", 100, navCallback);
    ros::Subscriber sub2 = n.subscribe("stopNav", 100, stopNavCallback);
    
    // ros::spin();
    //ASYNC SPINNER IS GOD
    ros::AsyncSpinner spinner(4);
    spinner.start();
    ros::waitForShutdown();
    
    return 0;
}

