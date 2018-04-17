#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>

typedef actionlib::SimpleActionClient<move_base_msg::MoveBaseAction> MoveBaseClient;

//credits to ROS.org Robot Navigation C++ tutorial 
//not exactly functional but good starting point especially when 
//navigation plays a crucial role in our project. 

const int numberOfRooms = 4; 
const std::string rooms[] = {"2.210", "4.304", "5.302", "6.302"};

using namespace std; 
int main(int argc, char** argv) {
    //activating action client
    MoveBaseClient actionClient("move_base", true);
    
    move_base_msgs::MoveBaseGoal goal;
    
    goal.target_pose.header.frame_id = "base_link";
    goal.target_pose.header.stamp = ros::Time::now();
    
    //creating goal to move 1 meter forward
    goal.target_pose.pose.position.x = 1.0;
    goal.target_pose.pose.orientation.w = 1.0; 
    
    ROS_INFO("Sending goal");
    actionClient.sendGoal(goal);
    
    actionClient.waitForResult();
    
    //checking if operation was success
    if(actionClient.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
        ROS_INFO("SUCCESS");
    else
        ROS_INFO("FAILED");
}
//will elaborate more on this code as the main purpose of this navigation 
//is to have to robot lead to the events happening around the GDC
//will have to add code that has the robot navigate through a plan to get to
//the event and robot stopping by each room for the event. 
