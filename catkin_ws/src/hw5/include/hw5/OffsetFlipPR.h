#include <hw5/PoseRecipient.h>

class OffsetFlipPR : public PoseRecipient {
public:
  OffsetFlipPR(double x, double y, double z, PoseRecipient &nextInChain);

  void receivePose(geometry_msgs::Pose &pose);

protected:
  double xStuff;
  double yStuff;
  double zStuff;
  PoseRecipient &chainNext;
};