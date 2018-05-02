#include <ros/ros.h>
#include "plan_execution/ExecutePlanAction.h"
#include <actionlib/client/simple_action_client.h>
#include "std_msgs/String.h"

typedef actionlib::SimpleActionClient<plan_execution::ExecutePlanAction> Client;


using namespace std;

//std::queue<string> doors;
ros::Publisher chatter_pub;
string location;


void navCallback(const std_msgs::String::ConstPtr& loc) {
    
    Client client("/action_executor/execute_plan", true);
    client.waitForServer();
    location = loc->data;
    //string location = doors.front();
    //doors.pop();
    
    ROS_INFO_STREAM("going to " << location);
    
    plan_execution::ExecutePlanGoal goal;
    
    plan_execution::AspRule rule;
    plan_execution::AspFluent fluent;
    fluent.name = "not facing";
    
    fluent.variables.push_back(location);
    
    rule.body.push_back(fluent);
    goal.aspGoal.push_back(rule);
    
    ROS_INFO("sending goal");
    client.sendGoalAndWait(goal);
    
    
    ros::Rate wait_rate(10);
    while (ros::ok() && !client.getState().isDone()) {
        
        
        
        
        wait_rate.sleep();
    }
    
    
    if(!client.getState().isDone()) {
        ROS_INFO("canceling goal");
        client.cancelGoal();
        
        
        for (int i = 0; i < 50 && !client.getState().isDone();i++) {
            wait_rate.sleep();
        }
    }
    
    
    
    if (client.getState() == actionlib::SimpleClientGoalState::ABORTED) {
        ROS_INFO("Aborted");
    } else if (client.getState() == actionlib::SimpleClientGoalState::PREEMPTED) {
        ROS_INFO("Preempted");
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




int main(int argc, char**argv) {
    ros::init(argc, argv, "door_navigation");
    ros::NodeHandle n;
    
    
    
    
    //doors.push("d3_424");
    
    chatter_pub = n.advertise<std_msgs::String>("chatter_pub", 1000);
    
    ros::Subscriber sub = n.subscribe("roomNumber", 100, navCallback);
    
    
    
    ros::spin();
    
    return 0;
}

