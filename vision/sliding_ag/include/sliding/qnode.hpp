/**
 * @file /include/sliding/qnode.hpp
 *
 * @brief Communications central!
 *
 * @date February 2011
 **/
/*****************************************************************************
** Ifdefs
*****************************************************************************/

#ifndef sliding_QNODE_HPP_
#define sliding_QNODE_HPP_

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
#include <geometry_msgs/Twist.h>
//#include <tutorial_msgs/mydmxel.h>
#include <cv_bridge/cv_bridge.h>
#include <image_transport/image_transport.h>

/*****************************************************************************
** Namespaces
*****************************************************************************/

namespace sliding
{
/*****************************************************************************
** Class
*****************************************************************************/

class QNode : public QThread
{
  Q_OBJECT
public:
  QNode(int argc, char** argv);
  virtual ~QNode();
  bool init();
  void run();

  int lrpm=10, rrpm=10;
  cv::Mat *imgRaw = NULL;
  bool isreceived = false;

Q_SIGNALS:
  void rosShutdown();

  void sigRcvImg();

private:
  int init_argc;
  char** init_argv;

  image_transport::Subscriber subImage;
  //ros::Subscriber subJoy;
  
  geometry_msgs::Twist twist;
  ros::Publisher init_pub;
  ros::Publisher pantilt_pub;
  ros::Publisher cmd_vel_pub;
  ros::Publisher wheel_pub;
  
  void callbackImage(const sensor_msgs::ImageConstPtr& msg_img);
  //void callbackJoy(const tutorial_msgs::mydmxelConstPtr& msg);

};

}  // namespace sliding

#endif /* sliding_QNODE_HPP_ */
