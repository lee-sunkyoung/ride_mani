#include "main_window.hpp"
#include "qnode.hpp"

typedef struct xy{
    int x=0;
    int y=0;
}XY;

typedef struct lr{
XY p1,p2,p3;
}LR;

class RIDE
{
private:

public:
  RIDE();
  ~RIDE();
  LR max_distin(cv::Mat color_img);
  LR min_distin(cv::Mat color_img);
  LR ry360_distin(cv::Mat color_img);
  XY riding(cv::Mat yello, cv::Mat white, cv::Mat red);
  XY judge(LR L, LR R, LR Y, LR STOP);
  XY wheel(int LRPM, int RRPM);
  char grad(LR gradient);
};


XY RIDE::riding(cv::Mat yello, cv::Mat white, cv::Mat red){

LR L,R,Y,STOP;

L=max_distin(yello);
R=min_distin(white);
Y=ry360_distin(yello);
STOP=ry360_distin(red);  //왼쪽오른쪽 123 좌표 저장하기

XY sol;
sol= judge(L,R,Y,STOP);

return sol;
}

XY RIDE::judge(LR L, LR R, LR Y,LR STOP){

XY result; 
static int red_flag=0;
if(STOP.p1.y>150&& STOP.p2.y>150||red_flag==1){red_flag=1; result=wheel(10,10);}
else if(R.p1.x==0&&R.p2.x>240&&L.p1.x>0){result=wheel(12,16);} //right zicgak turn left &&go straight
else if(R.p2.x==0&&L.p1.x>0&&L.p2.x==0&&Y.p1.y>0){result=wheel(15,10);}
else if(R.p2.x==0&&R.p1.x==0&&L.p3.x>0&&Y.p1.y>0&&Y.p2.y>185){result=wheel(16,7);}


else if(R.p2.x==0&&L.p1.x>0&&L.p3.x>0&&Y.p1.y>0&&Y.p1.y<65&&Y.p2.x<180){result=wheel(14,8);}
else if(R.p2.x==0&&L.p1.x>0&&L.p3.x>0&&Y.p1.y>0&&Y.p1.y<65){result=wheel(16,12);}

else if(R.p2.x==0&&L.p1.x>0&&L.p3.x>0&&Y.p1.y>0){result=wheel(16,10);}
else if(R.p2.x==0&&L.p1.x>0&&L.p2.x>0&&Y.p1.y>0){result=wheel(14,14);}

else if(R.p2.x==0&&L.p2.x>0&&Y.p1.y>0){result=wheel(16,12);}

else if(L.p3.x>0&&Y.p1.y>0){result=wheel(17,12);}
else if(L.p2.x==0&&R.p2.x>240){result=wheel(12,15);}
else if(R.p2.x==0&&L.p2.x>0){result=wheel(15,10);}
else {result=wheel(15,15);}//아니면 직진

//곡선우선, 직선은 둘다 직선잡히면 직선임.+
return result;
}

char RIDE::grad(LR gradient){       //기울기 구하려고만든함수...였으나 이제안쓸거같은..고런느낌쓰
    char gradi;
    XY p;

    LR g = gradient;

    p.x=0;p.y=0;

    if(g.p1.x!=0&&g.p2.x!=0){
        if(g.p1.x<g.p2.x){p.x=-1;}
        else if(g.p1.x==g.p2.x){p.x=10;}
        else if(g.p1.x>g.p2.x){p.x=1;}
    }

    if(g.p2.x!=0&&g.p3.x!=0){
        if(g.p2.x<g.p3.x){p.y=-1;}
        else if(g.p2.x==g.p3.x){p.y=10;}
        else if(g.p2.x>g.p3.x){p.y=1;}
    }

    if(g.p1.x!=0&&g.p2.x==0&&g.p3.x!=0){     //p2만 안잡힘
            if(g.p1.x<240&&g.p3.x<240){gradi='d';}
            else if(g.p1.x>240&&g.p3.x>240){gradi='b';}
    }

    if(p.x==-1&&p.y==1){gradi='b';}
    else if(p.x==1&&p.y==-1){gradi='d';}
    else if(p.x==1&&p.y==1){gradi='/';}
    else if(p.x==-1&&p.y==-1){gradi='|';}
    else if(p.x==10||p.y==10){gradi='N';}
    else if(p.x==0&&p.y!=0){if(p.y>0)gradi='b'; else if(p.y<0)gradi='d';} //p1만안잡힘
    else if(p.x!=0&&p.y==0){if(p.x>0)gradi='d'; else if(p.x<0)gradi='b';} //p3만안잡힘

    return gradi;
}

LR RIDE::max_distin(cv::Mat color_img){
     std::vector<cv::Point> Wpoints;
      cv::findNonZero(color_img, Wpoints);
      int targety1 = 75;
      cv::Point maxPoint1;
      int targety2 = 135;
      cv::Point maxPoint2;
      int targety3 = 195;
      cv::Point maxPoint3;
      for (std::vector<cv::Point>::const_iterator it = Wpoints.begin(); it != Wpoints.end(); ++it) {
       const cv::Point& point = *it;
        if (point.y == targety1) {
            if (maxPoint1.x == 0 || point.x > maxPoint1.x) {
                maxPoint1 = point;
            }
        }
        else if(point.y==targety2){
           if (maxPoint2.x == 0 || point.x > maxPoint2.x) {
                maxPoint2 = point;
            }
        }
        else if(point.y==targety3){
            if (maxPoint3.x == 0 || point.x > maxPoint3.x) {
                maxPoint3 = point;
            }
        }
      }
    LR P;
    P.p1.x=maxPoint1.x;
    P.p2.x=maxPoint2.x;
    P.p3.x=maxPoint3.x;

    return P;
    }

LR RIDE::min_distin(cv::Mat color_img){
     std::vector<cv::Point> Wpoints;
      cv::findNonZero(color_img, Wpoints);
      int targety1 = 75;
      cv::Point minPoint1;
      int targety2 = 135;
      cv::Point minPoint2;
      int targety3 = 195;
      cv::Point minPoint3;
      for (std::vector<cv::Point>::const_iterator it = Wpoints.begin(); it != Wpoints.end(); ++it) {
       const cv::Point& point = *it;
        if (point.y == targety1) {
            if (minPoint1.x == 0 || point.x < minPoint1.x) {
                minPoint1 = point;
            }
        }
        else if(point.y==targety2){
           if (minPoint2.x == 0 || point.x < minPoint2.x) {
                minPoint2 = point;
            }
        }
        else if(point.y==targety3){
            if (minPoint3.x == 0 || point.x < minPoint3.x) {
                minPoint3 = point;
            }
        }
      }
    LR P;
    P.p1.x=minPoint1.x;
    P.p2.x=minPoint2.x;
    P.p3.x=minPoint3.x;

    return P;
    }


LR RIDE::ry360_distin(cv::Mat color_img){
     std::vector<cv::Point> Wpoints;
      cv::findNonZero(color_img, Wpoints);
      int targetx1 = 360;
      int targetx2 = 240;
      cv::Point maxPoint1;
      cv::Point maxPoint2;
  
      for (std::vector<cv::Point>::const_iterator it = Wpoints.begin(); it != Wpoints.end(); ++it) {
       const cv::Point& point = *it;
        if (point.x == targetx1) {
            if (maxPoint1.y == 0 || point.y > maxPoint1.y) {
                maxPoint1 = point;
            }
        }
        if (point.x == targetx2) {
            if (maxPoint2.y == 0 || point.y > maxPoint2.y) {
                maxPoint2 = point;
            }
        }
      }
    LR P;
    P.p1.y=maxPoint1.y;
    P.p2.y=maxPoint2.y;

    return P;
    }


XY RIDE::wheel(int LRPM,int RRPM){
        XY a;
        a.x=LRPM;
        a.y=RRPM;

        return a;
   }


RIDE::RIDE(){}

RIDE::~RIDE(){}
