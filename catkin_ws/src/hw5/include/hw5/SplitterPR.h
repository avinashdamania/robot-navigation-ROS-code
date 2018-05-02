#ifndef SPLITTER_PR_H
#define SPLITTER_PR_H

#include "PoseRecipient.h"

class SplitterPR : public PoseRecipient {
public:
  SplitterPR(PoseRecipient &a, PoseRecipient &b);

  void receivePose(geometry_msgs::Pose &pose);

protected:
  PoseRecipient &_a;
  PoseRecipient &_b;
};

#endif
