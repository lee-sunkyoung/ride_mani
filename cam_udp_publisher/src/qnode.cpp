/**
 * @file /src/qnode.cpp
 *
 * @brief Ros communication central!
 *
 * @date February 2011
 **/

/*****************************************************************************
** Includes
*****************************************************************************/

#include <ros/ros.h>
#include <ros/network.h>
#include <string>
#include <std_msgs/String.h>
#include <sstream>
#include "../include/cam_udp_publisher/qnode.hpp"

/*****************************************************************************
** Namespaces
*****************************************************************************/

namespace cam_udp_publisher
{
/*****************************************************************************
** Implementation
*****************************************************************************/

QNode::QNode(int argc, char** argv) : init_argc(argc), init_argv(argv)
{
}

QNode::~QNode()
{
  if (ros::isStarted())
  {
    ros::shutdown();  // explicitly needed since we use ros::start();
    ros::waitForShutdown();
  }
  wait();
}

bool QNode::init()
{
  ros::init(init_argc, init_argv, "cam_udp_publisher");
  if (!ros::master::check())
  {
    return false;
  }
  ros::start();  // explicitly needed since our nodehandle is going out of scope.
  ros::NodeHandle n;
  // Add your ros communications here.
  subImage = n.subscribe("/usb_cam/image_raw", 1, &QNode::callbackImage, this);

  start();
  updateTopics();

  return true;
}

void QNode::run()
{
  ros::Rate loop_rate(500);
  while (ros::ok())
  {
    ros::spinOnce();
    loop_rate.sleep();
  }
  std::cout << "Ros shutdown, proceeding to close the gui." << std::endl;
  Q_EMIT rosShutdown();  // used to signal the gui for a shutdown (useful to roslaunch)
}

void QNode::changeSubTopic()
{
  subImage.shutdown();
  ros::NodeHandle n;
  std::string topicName = getStringTopicNew.toStdString();
  subImage = n.subscribe(topicName, 1, &QNode::callbackImage, this);
}

void QNode::callbackImage(const sensor_msgs::ImageConstPtr& msg)
{
  cv_bridge::CvImagePtr cv_ptr;
  cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
  imgRaw = cv_ptr->image;
  cv::resize(Mat(cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8)->image), imgView, Size(360, 270), 0, 0,
             CV_INTER_LINEAR);
  Q_EMIT sigRcvImg();
}

void QNode::updateTopics()
{
  ros::master::V_TopicInfo topics;
  if (ros::master::getTopics(topics))
  {
    topicList.clear();
    for (const auto& topic : topics)
    {
      topicList << QString::fromStdString(topic.name);
    }
    Q_EMIT sigReadTopic();
    return;
  }
  else
  {
    ROS_ERROR("Failed to retrieve topics.");
    return;
  }
}

}  // namespace cam_udp_publisher
