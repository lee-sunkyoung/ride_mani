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
#include "../include/sliding/qnode.hpp"
#include <tutorial_msgs/mydmxel.h>
/*****************************************************************************
** Namespaces
*****************************************************************************/

namespace sliding
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
  ros::init(init_argc, init_argv, "sliding");
  if (!ros::master::check())
  {
    return false;
  }
  ros::start();  // explicitly needed since our nodehandle is going out of scope.
  ros::NodeHandle n;
  // Add your ros communications here.

  image_transport::ImageTransport image(n);
  subImage = image.subscribe("/usb_cam/image_raw", 1, &QNode::callbackImage, this);
  //subJoy = n.subscribe("/hello", 100, &QNode::callbackJoy, this);

  // autorace->로봇 제어 명령을 보낼 퍼블리셔 생성
  init_pub = n.advertise<std_msgs::Bool>("init_pub", 10);
  pantilt_pub = n.advertise<geometry_msgs::Point>("target", 10);
  cmd_vel_pub = n.advertise<geometry_msgs::Twist>("/cmd_vel", 10);

  wheel_pub = n.advertise<tutorial_msgs::mydmxel>("/bye",1000);

  start();
  return true;
}

void QNode::run()
{
  ros::Rate loop_rate(33);
  while (ros::ok())
  {
    ros::spinOnce();

    tutorial_msgs::mydmxel msg;
    msg.L_wheel=lrpm;
    msg.R_wheel=rrpm;
    std::cout<<msg.L_wheel<<std::endl;
    std::cout<<msg.R_wheel<<std::endl;
    wheel_pub.publish(msg);
    loop_rate.sleep();
  }
  std::cout << "Ros shutdown, proceeding to close the gui." << std::endl;
  Q_EMIT rosShutdown();  // used to signal the gui for a shutdown (useful to roslaunch)
}

//이미지 메시지를 처리하는 함수
void QNode::callbackImage(const sensor_msgs::ImageConstPtr& msg_img)
{
  if (imgRaw == NULL && !isreceived)  // imgRaw -> NULL, isreceived -> false
  {
    // ROS 이미지 메시지를 OpenCV Mat 형식으로 변환, 이미지 객체에 할당
    imgRaw = new cv::Mat(cv_bridge::toCvCopy(msg_img, sensor_msgs::image_encodings::RGB8)->image);

    if (imgRaw != NULL)  // imgRaw 변환 성공
    {
      Q_EMIT sigRcvImg();  // 이미지 수신을 알리는 시그널 발생
      isreceived = true;
    }
  }
}
//void QNode::callbackJoy(const tutorial_msgs::mydmxelConstPtr& msg){}
}  // namespace sliding
