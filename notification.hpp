#ifndef KEYMOLEN_NOTIFICATION_H
#define KEYMOLEN_NOTIFICATION_H

#include "common.hpp"
#include "opencv2/core/core.hpp"
#include <vector>

namespace keymolen
{
  class Notification
  {
  public:
    class NotificationInfo
    {
      cv::Mat frame_original_;
      cv::Mat frame_boxes_;
      std::vector<std::pair<std::string, double>> classes_;
    };
  public:
    Notification();
    virtual ~Notification();
  public:
    bool Notify(NotificationInfo& info);
  };

} //ns

#endif
