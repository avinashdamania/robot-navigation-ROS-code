#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <hw5/PoseRecipient.h>
#include <ros/ros.h>
#include <tf/tf.h>
#include <tf/transform_listener.h>
#include <eigen3/Eigen/Eigen>


typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

class NavPR : public PoseRecipient {
public:
  NavPR(tf::TransformListener &tf, MoveBaseClient &ac);


void receivePose(geometry_msgs::Pose &pose);

protected:
  tf::TransformListener &listener;
  MoveBaseClient &client;
};
