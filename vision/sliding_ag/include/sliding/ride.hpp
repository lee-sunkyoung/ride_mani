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
  LR distin(cv::Mat color_img);
  XY riding(cv::Mat yello, cv::Mat white, cv::Mat red);
  XY judge(LR a, LR b);
  XY wheel(int LRPM, int RRPM);
  char grad(LR gradient);
};


XY RIDE::riding(cv::Mat yello, cv::Mat white, cv::Mat red){

LR L,R,STOP;

L=distin(yello);
R=distin(white);
STOP=distin(red);  //왼쪽오른쪽 123 좌표 저장하기

XY sol;

sol= judge(L,R);

return sol;
}

XY RIDE::judge(LR a, LR b){

XY yello,white,result;

char yello_grad= grad(a);
char white_grad= grad(b);

//곡선우선, 직선은 둘다 직선잡히면 직선임.

if(yello_grad=='b'){result= wheel(12,14);}
else if(yello_grad=='d'){result= wheel(14,12);}

return result;
}

char RIDE::grad(LR gradient){
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

LR RIDE::distin(cv::Mat color_img){
     std::vector<cv::Point> Wpoints;
      cv::findNonZero(color_img, Wpoints);
      int targety1 = 85;
      cv::Point minPoint1;  
      int targety2 = 135;
      cv::Point minPoint2;  
      int targety3 = 185;
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

XY RIDE::wheel(int LRPM,int RRPM){
        XY a;
        a.x=LRPM;
        a.y=RRPM;

        return a;
   }


RIDE::RIDE(){}

RIDE::~RIDE(){}
