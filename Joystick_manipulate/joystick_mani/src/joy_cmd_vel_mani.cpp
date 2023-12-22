#include <ros/ros.h>
#include<std_msgs/Int64.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/Joy.h>
#include <cmath>
#include <tutorial_msgs/mydmxel.h>


#define ANGULAR1_MAX 10.0  //각속도1의 최댓값 10.0 ~ -10.0
#define ANGULAR2_MAX 10.0  //각속도2의 최댓값 10.0 ~ -10.0
#define SENSITIVITY 0.1    //조이스틱 감도 조절


typedef struct moter_angle{
  int mo1=2300;
  int mo2=2047;
  int mo3=2047;
  int mo4=2047;
  int mo5=2047;
  double theta;
}moter;

int val2dy(double theta){
  int a = theta*(180/3.14)*11.377;
  int dmx=2048+a;

  return dmx;
}

moter calculate_angle(float x, float y, float z,int l1, int l2){
  moter dmxel;

  double theta1,theta2,theta3;
  double cos3, sin3;
  double ex,ey;
  
  ex = sqrt((x*x)+(y*y));
  ey=z;

  //세타 3 구하기
  cos3= ((ex*ex)+(ey*ey))/((l1*l1)+(l2*l2)+(2*l1*l2));  //ex ey인가
  sin3= sqrt(1-(cos3*cos3));

  theta3= atan2(sin3,cos3);

  //세타2 구하기

  theta2=atan2(ex,ey)-atan2(l1+(l2*cos3),(l2*sin3));

  theta1 = atan2(y,x);

  dmxel.mo1=val2dy(theta1);
  dmxel.mo2=val2dy(theta2);
  dmxel.mo3=val2dy(theta3);
  dmxel.mo4=val2dy(theta2+theta3-1.5708);
  dmxel.theta=theta2+theta3;
  return dmxel;
}

class Joy_cmd_vel_mani
{
public:
  Joy_cmd_vel_mani();
  void operate();
  float MAXtoMIN(float angular, float max);

private:
  void joyCallback(const sensor_msgs::Joy::ConstPtr& joy);

  ros::NodeHandle n;
 
  
//조이값 저장변수
  float arm_z=0;   //enf z좌표 업다운
  float arm_x=0;   //enf x좌표 왼오
  float arm_y=0;
  float hand=0;   //조이스틱에서 손목 피치
  //float grip=180;
  float grip_open;
  float grip_close;
  int init_grip=0;
  int init_ride=0;
  int right_angle=0;
  int r4no=0;

//현재값 저장변수
  int crt_init_grip=0;
  int crt_init_ride=0;
  int crt_r4no=0;
  int crt_right_angle=0;
  float crt_arm_x; 
  float crt_arm_z;
  float crt_arm_y;  
  float crt_hand=0;     
  float crt_grip=0;

//계산변수
  int l1=23,l2=18;
 
//모터변수



  ros::Publisher vel_pub_; 
  ros::Subscriber joy_sub_;  // joy 토픽에서 조이스틱 메시지를 받아 joyCallback 콜백 함수를 호출.
};

Joy_cmd_vel_mani::Joy_cmd_vel_mani()
{
  vel_pub_ = n.advertise<tutorial_msgs::mydmxel>("/hello",1000);
  joy_sub_ = n.subscribe<sensor_msgs::Joy>("joy", 10, &Joy_cmd_vel_mani::joyCallback, this);
}

void Joy_cmd_vel_mani::operate()
{
    tutorial_msgs::mydmxel msg;

  //최대길이 체크
  float checkbuf= sqrt((crt_arm_x*crt_arm_x)+(crt_arm_y*crt_arm_y)+(crt_arm_z*crt_arm_z));
  bool check;
  if(checkbuf<48){
    check=true;
  }
  else check=false;

 if(arm_x!=0||arm_y!=0||arm_z!=0||hand!=0||grip_open!=0||grip_close!=0){
  crt_init_ride=0;
  } //고정좌표값 삭제후 이동가능하게 하기

if(check==true){
 
  //현재값 저장
  crt_hand += SENSITIVITY * hand;
  crt_arm_z+= SENSITIVITY * arm_z;
  crt_arm_x+= SENSITIVITY * arm_x;
  crt_arm_y+= SENSITIVITY * arm_y;
  crt_grip=((grip_open-1)/2)*230+((1-grip_close)/2)*230; //조이에서 받는값이 좀 달라서 이 그리퍼움직이는 둘은

  //최대값 제한
  crt_arm_z=MAXtoMIN(crt_arm_z,10);
  crt_arm_x=MAXtoMIN(crt_arm_x,10);
  crt_arm_y=MAXtoMIN(crt_arm_y,10);
  crt_hand =MAXtoMIN(crt_hand,180); 
  crt_grip =MAXtoMIN(crt_grip,230);
}
  moter dmx;
  dmx=calculate_angle(crt_arm_x,crt_arm_y,crt_arm_z,l1,l2);

  //crt_grip=crt_grip*10;
  //crt_hand=crt_hand*10;

  dmx.mo5=crt_grip+230;
  dmx.mo4=crt_hand*11.377+2048;

if(r4no==1||crt_r4no==1){
  crt_r4no==1;
  crt_right_angle=0;
}
if(right_angle==1||crt_right_angle==1){
    crt_right_angle=1;
    crt_r4no=0;
    dmx.mo4=val2dy(dmx.theta);
  }
if(init_grip==1||crt_init_grip==1){
    crt_init_grip=1;
    dmx.mo1=2300;
    dmx.mo2=4000;
    dmx.mo3=3680;
  }
if(init_ride==1||crt_init_ride==1){
      crt_init_ride=1;
      crt_init_grip=0; 
      crt_arm_x=2;
      crt_arm_y=1;
      crt_arm_z=0;
  }

if(dmx.mo2<=2000){dmx.mo2=2000;}

msg.motor1=dmx.mo1;
msg.motor2=dmx.mo2;
msg.motor3=dmx.mo3;
msg.motor4=dmx.mo4;
msg.motor5=dmx.mo5;

vel_pub_.publish(msg);

}


// 조이스틱 메시지에서 각속도 목표값을 읽어와 변수에 저장함.
void Joy_cmd_vel_mani::joyCallback(const sensor_msgs::Joy::ConstPtr& joy)
{

  /*float형식*/

  arm_y= joy->axes[0]; //조이스틱에서 왼쪽 방향키 왼쪽 오른쪽
  arm_x = joy->axes[1]; //조이스틱에서 왼쪽 방향키 
  grip_close =joy->axes[2]; //조이스틱에서 L2
  hand= joy->axes[3];  //조이스틱에서 오른쪽 방향키 왼쪽 오른쪽
  arm_z = joy->axes[4];  //조이스틱에서 오른쪽 방향키 위아래
  grip_open =joy->axes[5];  //조이스틱에서 R2
  //hand= joy->axes[6];  //조이스틱에서 왼쪽위 방향키 왼쪽 오른쪽
  //=joy->axes[7]
 
  /*int 형식*/

  r4no = joy->buttons[0];  //조이스틱에서 오른쪽 X표시 버튼
  right_angle=joy->buttons[1];  //조이스틱에서 오른쪽 O표시 버튼
  //_______ = joy->buttons[2];  //조이스틱에서 오른쪽 삼각형표시 버튼
  //_______ = joy->buttons[3];  //조이스틱에서 오른쪽 사각형표시 버튼
  init_grip = joy->buttons[4];  //조이스틱에서 L1
  init_ride = joy->buttons[5];  //조이스틱에서 R1
  //_______ = joy->buttons[6];  //조이스틱에서 L2
  //_______ = joy->buttons[7];  //조이스틱에서 R2
  //_______ = joy->buttons[8];  //조이스틱에서 share
  //_______ = joy->buttons[9];  //조이스틱에서 options
  //_______ = joy->buttons[10];  //조이스틱에서 가운데 버튼
  //_______ = joy->buttons[11];  //조이스틱에서 왼쪽 스틱 누르기
  //_______ = joy->buttons[12];  //조이스틱에서 오른쪽 스틱 누르기
}

// 해당 각속도의 최대 최소 범위 설정함.
float Joy_cmd_vel_mani::MAXtoMIN(float angular, float max)
{
  if (angular > max)
  {
    angular = max;
  }
  else if (angular < -max)
  {
    angular = -max;
  }
  return angular;
}

  

int main(int argc, char** argv)
{
  
  ros::init(argc, argv, "vel");  // ROS 노드를 초기화하고 vel 이라는 노드를 생성함.
  Joy_cmd_vel_mani vel;          // Joy_cmd_vel_mani 클래스의 객체를 생성함.

  ros::Rate loop_rate(33);  // 노드가 주기적으로 실행되도록 루프를 설정함.
  while (ros::ok())         // ROS가 정상적으로 실행 중인 동안 계속해서 루프를 실행함.
  {
    vel.operate();      // operate 함수를 호출함.
    ros::spinOnce();    // 콜백 함수를 호출하고 메시지 큐를 처리함.
    loop_rate.sleep();  // 설정된 주기에 따라 루프를 대기
  }
  return 0;
}
 