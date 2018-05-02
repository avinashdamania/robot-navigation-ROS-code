#include <tf/transform_broadcaster.h>
#include <hw5/PoseRecipient.h>

class TFBroadcastPR : public PoseRecipient {
public:
  TFBroadcastPR(std::string f1, std::string f2);


void receivePose(geometry_msgs::Pose &pose);

protected:
  std::string fromFrame;
  std::string toFrame;
  tf::TransformBroadcaster br;
};