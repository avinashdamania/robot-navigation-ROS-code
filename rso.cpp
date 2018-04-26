#include <ros/ros.h>
#include "plan_execution/ExecutePlanAction.h"
#include <actionlib/client/simple_action_client.h>

typedef actionlib::SimpleActionClient<plan_execution::ExecutePlanAction> Client;


using namespace std;

int main(int argc, char**argv) {
  ros::init(argc, argv, "door_navigation");
  ros::NodeHandle n;

  ros::NodeHandle privateNode("~");
  
  std::vector<string> doors;
  
  doors.push_back("d3_424");

  Client client("/action_executor/execute_plan", true);
  client.waitForServer();

  bool fromAtoB = true;
  int count;
  count = 0;

  while (ros::ok() && count < doors.size()) {

    string location = doors.at(count);
	count++;

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
    } else
      ROS_INFO("Terminated");

  }

  return 0;
}
