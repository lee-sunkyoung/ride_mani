#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/Joy.h>

#define ANGULAR1_MAX 1.0  //각속도1의 최댓값 1.0 ~ -1.0
#define ANGULAR2_MAX 1.0  //각속도2의 최댓값 1.0 ~ -1.0
#define ACC_LINEAR 0.1
#define ACC_ANGULAR 0.2

class Joy_cmd_vel
{
public:
  Joy_cmd_vel();
  void operate();

private:
  void joyCallback(const sensor_msgs::Joy::ConstPtr& joy);

  ros::NodeHandle n;

  float target_angular1;   //조이스틱에서 받은 목표 각속도1
  float target_angular2;   //조이스틱에서 바은 목표 각속도2
  float current_angular1;  //현재 각속도1 -> 값들이 목표 각속도1에 수렴하도록 함.
  float current_angular2;  //현재 각속도2 -> 값들이 목표 각속도2에 수렴하도록 함.

  ros::Publisher vel_pub_;   // cmd_vel 토픽으로 로봇의 속도 명령을 발행.
  ros::Subscriber joy_sub_;  // joy 토픽에서 조이스틱 메시지를 받아 joyCallback 콜백 함수를 호출.
};

Joy_cmd_vel::Joy_cmd_vel() : target_angular1(0.0), target_angular2(0.0), current_angular1(0.0), current_angular2(0.0)
{
  vel_pub_ = n.advertise<geometry_msgs::Twist>("/cmd_vel", 2);
  joy_sub_ = n.subscribe<sensor_msgs::Joy>("joy", 10, &Joy_cmd_vel::joyCallback, this);
}

void Joy_cmd_vel::operate()
{
  geometry_msgs::Twist twist;

  // 현재 각속도1의 값이 1.0 ~ -1.0 사이에 있도록 함.
  if (current_angular1 > ANGULAR1_MAX)
  {
    current_angular1 = ANGULAR1_MAX;
  }
  else if (current_angular1 < -ANGULAR1_MAX)
  {
    current_angular1 = -ANGULAR1_MAX;
  }

  // 현재 각속도2의 값이 1.0 ~ -1.0 사이에 있도록 함.
  if (current_angular2 > ANGULAR2_MAX)
  {
    current_angular2 = ANGULAR2_MAX;
  }
  else if (current_angular2 < -ANGULAR2_MAX)
  {
    current_angular2 = -ANGULAR2_MAX;
  }

  twist.linear.x = current_angular1;
  twist.angular.z = current_angular2;

  vel_pub_.publish(twist);  //설정된 Twist 메시지를 발행함.
}

// 조이스틱 메시지에서 각속도 목표값을 읽어와 변수에 저장함.
void Joy_cmd_vel::joyCallback(const sensor_msgs::Joy::ConstPtr& joy)
{
  target_angular1 = joy->axes[3];  //조이스틱에서 오른쪽 방향키 위아래
  target_angular2 = joy->axes[1];  //조이스틱에서 왼쪽 방향키 위아래
}

int main(int argc, char** argv)
{
  ros::init(argc, argv, "vel");  // ROS 노드를 초기화하고 vel 이라는 노드를 생성함.
  Joy_cmd_vel vel;               // Joy_cmd_vel 클래스의 객체를 생성함.

  ros::Rate loop_rate(33);  // 노드가 주기적으로 실행되도록 루프를 설정함.
  while (ros::ok())         // ROS가 정상적으로 실행 중인 동안 계속해서 루프를 실행함.
  {
    vel.operate();      // operate 함수를 호출함.
    ros::spinOnce();    // 콜백 함수를 호출하고 메시지 큐를 처리함.
    loop_rate.sleep();  // 설정된 주기에 따라 루프를 대기
  }
  return 0;
}