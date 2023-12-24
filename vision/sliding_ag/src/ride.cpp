// /**
//  * @file /src/main_window.cpp
//  *
//  * @brief Implementation for the qt gui.
//  *
//  * @date February 2011
//  **/
// /*****************************************************************************
// ** Includes
// *****************************************************************************/

// #include <QtGui>
// #include <QMessageBox>
// #include <iostream>
// #include "../include/udp_project_receive/main_window.hpp"
// #include <opencv2/opencv.hpp>
// #include <opencv2/highgui.hpp>
// #include <opencv2/core/mat.hpp>
// #include <opencv2/imgcodecs.hpp>

// #include <QPixmap>
// #include <QImage>
// //my IP_"192.168.0.79"

// /*****************************************************************************
// ** Namespaces
// *****************************************************************************/
//       int H_min = 0; 
//       int S_min = 0; 
//       int V_min = 0;
//       int H_max = 0; 
//       int S_max = 0; 
//       int V_max = 0;  
//       cv::Scalar blue = cv::Scalar(255, 0, 0); // BGR 형식에서 파란색

// namespace udp_project_receive
// {
//   using namespace Qt;


//   /*****************************************************************************
//   ** Implementation [MainWindow]
//   *****************************************************************************/

//   MainWindow::MainWindow(int argc, char** argv, QWidget* parent) : QMainWindow(parent), qnode(argc, argv)
//   {
//     ui.setupUi(this);  // Calling this incidentally connects all ui's triggers to on_...() callbacks in this class.

//     setWindowIcon(QIcon(":/images/icon.png"));

//     qnode.init();
//     MY_IP = QHostAddress("192.168.0.79");
//     NOOK_IP = QHostAddress("192.168.0.51");

//     PORT=8888;
// img_socket = new QUdpSocket(this);

//      if(img_socket->bind(MY_IP,PORT))
//     { std::cout<<"3"<<std::endl;
//         QObject::connect(img_socket, SIGNAL(readyRead()),this,SLOT(Load()));
//     }




//     QObject::connect(&qnode, SIGNAL(rosShutdown()), this, SLOT(close()));

//   }

//   MainWindow::~MainWindow()
//   {
 
//   }

  






// // void MainWindow::on_horizontalSlider_valueChanged(int value)
// // {

// //     H_min = value; 
// // }
// // void MainWindow::on_horizontalSlider_2_valueChanged(int value2)
// // {
// //     S_min = value2; 
// // }

// // void MainWindow::on_horizontalSlider_valueChanged_3(int value3)
// // {
// //     V_min = value3; 
// // }

// // void MainWindow::on_horizontalSlider_valueChanged_4(int value4)
// // {
// //     H_max = value4; 
// // }

// // void MainWindow::on_horizontalSlider_valueChanged_5(int value5)
// // {
// //     S_max=value5; 
// // }

// // void MainWindow::on_horizontalSlider_valueChanged_6(int value6)
// // {
// //     V_max = value6; 
// // }



// void MainWindow::Load(){
// cv::Mat mask = cv::getStructuringElement(cv::MORPH_RECT,cv::Size(3,3),cv::Point(1,1)); 
//     clone_mat = udp_.MatImgRcv(img, PORT, NOOK_IP, *img_socket);

// cv:: Mat img_HSV; 
// cv:: Mat img_HSV2; 
// cv:: Mat img_HSV3; 
// cv:: Mat img_HSV4; 

//         cv::Mat clone_mat2 = clone_mat;
//          cv::Mat clone_mat3 = clone_mat;
//          cv::resize(clone_mat3, clone_mat3, cv::Size(320, 240));
//       cv::resize(clone_mat2, clone_mat2, cv::Size(320, 240));
        
//       cv::GaussianBlur(clone_mat2,clone_mat2,cv::Size(),1); 
//       cv::cvtColor(clone_mat2,img_HSV,cv::COLOR_BGR2HSV);



// cv:: Mat line_img;
//       cv::inRange(clone_mat2,cv::Scalar(ui.horizontalSlider->value(),ui.horizontalSlider_2->value(),ui.horizontalSlider_3->value()),cv::Scalar(ui.horizontalSlider_4->value(),ui.horizontalSlider_5->value(),ui.horizontalSlider_6->value()),img_HSV); 
//        cv::inRange(clone_mat2,cv::Scalar(202,200,192),cv::Scalar(243,233,242),img_HSV2);   
//         cv::inRange(clone_mat2,cv::Scalar(0,100,153),cv::Scalar(111,184,255),img_HSV3); 
//              cv::inRange(clone_mat2,cv::Scalar(57,137,92),cv::Scalar(139,225,191),img_HSV4);     
//           cv::erode(img_HSV,img_HSV,mask,cv::Point(-1,-1),1);
//        cv::dilate(img_HSV,img_HSV,mask,cv::Point(-1,-1),4);
//         //  cv::dilate(img_HSV2,img_HSV2,mask,cv::Point(-1,-1),5);
//               cv::erode(img_HSV2,img_HSV2,mask,cv::Point(-1,-1),1);
//               cv::erode(img_HSV3,img_HSV3,mask,cv::Point(-1,-1),4);
//        cv::dilate(img_HSV3,img_HSV3,mask,cv::Point(-1,-1),4);
//               cv::erode(img_HSV4,img_HSV4,mask,cv::Point(-1,-1),4);
//       //  cv::dilate(img_HSV4,img_HSV4,mask,cv::Point(-1,-1),1);
        
 
//        //관심영역 지정해주기 
//        cv::Canny(img_HSV2,line_img,10,300,5);
//        int wedth = 0; 
//        int height = 0;
//        cv::Point points[4]; 

//       points[0] = cv::Point(0,100);   
//       points[1] = cv::Point(0,150);   
//       points[2] = cv::Point(270,150);   
//       points[3] = cv::Point(270,125);   
//      line_img = region_of_interest(line_img,points); 
//      std::vector<cv::Vec4i> white_line;  
//      cv::HoughLinesP(line_img,white_line,1,CV_PI/180,50,30,60); 
//      for(size_t i = 0; i<white_line.size();i++){
//       cv::Vec4i L = white_line[i];
//       cv::line(clone_mat3,cv::Point(L[0],L[1]),cv::Point(L[2],L[3]), cv::Scalar(255, 0, 0),3,8);  
//      }
//   // std::cout<<"1"<<ui.horizontalSlider->value()<<std::endl;
//   //   std::cout<<"2"<<ui.horizontalSlider_2->value()<<std::endl;
//   //     std::cout<<"3"<<ui.horizontalSlider_3->value()<<std::endl;
//   //       std::cout<<"4"<<ui.horizontalSlider_4->value()<<std::endl;
//   //         std::cout<<"5"<<ui.horizontalSlider_5->value()<<std::endl;
//   //           std::cout<<"6"<<ui.horizontalSlider_6->value()<<std::endl;



//      QImage img3((uchar*)img_HSV3.data,img_HSV3.cols,img_HSV3.rows,QImage::Format_Indexed8);
//     QPixmap pixmap3 = QPixmap::fromImage(img3);
//     ui.label_5->setPixmap(pixmap3); 


//      QImage img4((uchar*)img_HSV4.data,img_HSV4.cols,img_HSV4.rows,QImage::Format_Indexed8);
//     QPixmap pixmap4 = QPixmap::fromImage(img4);
//     ui.label_6->setPixmap(pixmap4); 



//      QImage img5((uchar*)img_mask.data,img_mask.cols,img_mask.rows,QImage::Format_Indexed8);
//     QPixmap pixmap5 = QPixmap::fromImage(img5);
//     ui.label_3->setPixmap(pixmap5); 




// cv:: Mat stats; 
// cv:: Mat centroids; 

// cv:: Mat stats2; 
// cv:: Mat centroids2; 

// int num_red = cv::connectedComponentsWithStats(img_HSV3, img_HSV3, stats, centroids, 8, CV_32S);

//  for (int i = 1; i < num_red; ++i) {
//         x = static_cast<int>(stats.at<int>(i, cv::CC_STAT_LEFT));
//         y = static_cast<int>(stats.at<int>(i, cv::CC_STAT_TOP));
//         width = static_cast<int>(stats.at<int>(i, cv::CC_STAT_WIDTH));
//         height = static_cast<int>(stats.at<int>(i, cv::CC_STAT_HEIGHT));
//   cv::rectangle(clone_mat3, cv::Rect(x, y, width, height), cv::Scalar(0, 0, 255),4);

// //   std::cout<<"x = "<<x2<<std::endl;
// // std::cout<<"y = "<<y2<<std::endl;
// // std::cout<<"width = "<<width2<<std::endl;
// // std::cout<<"height = "<<height2<<std::endl;


//  }

//  if(y<60){
//   std::cout<<"1"<<std::endl;
//     ui.textEdit_2->setText("Top"); 
// }
// else if(y>85){

//    std::cout<<"2"<<std::endl;
//      ui.textEdit_2->setText("down"); 
// }
// else{

//    std::cout<<"3"<<std::endl;
//      ui.textEdit_2->setText("line"); 
// }


//  int num_green = cv::connectedComponentsWithStats(img_HSV4, img_HSV4, stats2, centroids2, 8, CV_32S);

//  for (int i = 1; i < num_green; ++i) {
//          x2 = static_cast<int>(stats2.at<int>(i, cv::CC_STAT_LEFT));
//          y2 = static_cast<int>(stats2.at<int>(i, cv::CC_STAT_TOP));
//          width2 = static_cast<int>(stats2.at<int>(i, cv::CC_STAT_WIDTH));
//          height2 = static_cast<int>(stats2.at<int>(i, cv::CC_STAT_HEIGHT));
//   cv::rectangle(clone_mat3, cv::Rect(x2, y2, width2, height2), cv::Scalar(0, 255, 0),4);




//  }
//  if(y2<90){
//   std::cout<<"1"<<std::endl;
//   ui.textEdit->setText("Top"); 
// }
// else if(y2>113){

//    std::cout<<"2"<<std::endl;
//     ui.textEdit->setText("down"); 
// }
// else{

//    std::cout<<"3"<<std::endl;
//      ui.textEdit->setText("line"); 
// }

//     QImage img0((uchar*)clone_mat2.data,clone_mat2.cols,clone_mat2.rows,QImage::Format_RGB888);
//     img0 = img0.rgbSwapped();
//     QPixmap pixmap0 = QPixmap::fromImage(img0);
//     ui.label->setPixmap(pixmap0); 




//     QImage img((uchar*)clone_mat3.data,clone_mat3.cols,clone_mat3.rows,QImage::Format_RGB888);
//     img = img.rgbSwapped();
//     QPixmap pixmap = QPixmap::fromImage(img);
//     ui.label_2->setPixmap(pixmap); 
 

//      QImage img2((uchar*)line_img.data,line_img.cols,line_img.rows,QImage::Format_Indexed8);
//     QPixmap pixmap2 = QPixmap::fromImage(img2);
//     ui.label_4->setPixmap(pixmap2); 

// value = ui.horizontalSlider->value();
// value2 = ui.horizontalSlider_2->value();
// value3 = ui.horizontalSlider_3->value();
// value4 = ui.horizontalSlider_4->value();
// value5 = ui.horizontalSlider_5->value();
// value6 = ui.horizontalSlider_6->value();
// ui.lab_1->setText(QString::number(value));
// ui.lab_2->setText(QString::number(value2));
// ui.lab_3->setText(QString::number(value3));
// ui.lab_4->setText(QString::number(value4));
// ui.lab_5->setText(QString::number(value5));
// ui.lab_6->setText(QString::number(value6));

 

 
// }

// cv::Mat MainWindow::region_of_interest(cv::Mat img_edges,cv::Point *points){


//  img_mask = cv::Mat::zeros(img_edges.rows,img_edges.cols,CV_8UC1);
// const cv::Point*ppt[1] = {points}; 
// int npt[]={4}; 
// cv::fillPoly(img_mask,ppt,npt,1,cv::Scalar(255,255,255),cv::LINE_8); 
// cv::bitwise_and(img_edges,img_mask,img_masked);
// return img_masked; 


// }
//   /*****************************************************************************
//   ** Functionss
//   *****************************************************************************/

// }  // namespace s