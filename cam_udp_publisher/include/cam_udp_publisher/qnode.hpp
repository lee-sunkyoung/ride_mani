/**
 * @file /include/cam_udp_publisher/qnode.hpp
 *
 * @brief Communications central!
 *
 * @date February 2011
 **/
/*****************************************************************************
** Ifdefs
*****************************************************************************/

#ifndef cam_udp_publisher_QNODE_HPP_
#define cam_udp_publisher_QNODE_HPP_

/*****************************************************************************
** Includes
*****************************************************************************/

// To workaround boost/qt4 problems that won't be bugfixed. Refer to
//    https://bugreports.qt.io/browse/QTBUG-22829
#ifndef Q_MOC_RUN
#include <ros/ros.h>
#endif
#include <string>
#include <QThread>
#include <QStringListModel>
#include <QString>
#include <sensor_msgs/Image.h>
#include <cv_bridge/cv_bridge.h>
#include <ros/topic.h>
#include <ros/master.h>

/*****************************************************************************
** Namespaces
*****************************************************************************/

namespace cam_udp_publisher
{
/*****************************************************************************
** Class
*****************************************************************************/
using namespace cv;

class QNode : public QThread
{
  Q_OBJECT
public:
  QNode(int argc, char** argv);
  virtual ~QNode();
  bool init();
  void run();

  void updateTopics();
  void changeSubTopic();

  QString getStringTopicNew;

  QStringList topicList;

  Mat imgRaw;
  Mat imgView;

Q_SIGNALS:
  void rosShutdown();
  void sigRcvImg();
  void sigReadTopic();

private:
  int init_argc;
  char** init_argv;

  ros::Subscriber subImage;

  void callbackImage(const sensor_msgs::ImageConstPtr& msg);
};

}  // namespace cam_udp_publisher

#endif /* cam_udp_publisher_QNODE_HPP_ */
