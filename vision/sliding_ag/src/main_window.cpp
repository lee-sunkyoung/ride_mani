/**
 * @file /src/main_window.cpp
 *
 * @brief Implementation for the qt gui.
 *
 * @date February 2011
 **/
/*****************************************************************************
** Includes
*****************************************************************************/

#include <QtGui>
#include <QMessageBox>
#include <iostream>
#include "../include/sliding/main_window.hpp"
#include "../include/sliding/ride.hpp"
/*****************************************************************************
** Namespaces
*****************************************************************************/

namespace sliding
{
using namespace Qt;

/*****************************************************************************
** Implementation [MainWindow]
*****************************************************************************/

MainWindow::MainWindow(int argc, char** argv, QWidget* parent) : QMainWindow(parent), qnode(argc, argv)
{
  ui.setupUi(this);  // Calling this incidentally connects all ui's triggers to on_...() callbacks in this class.

  setWindowIcon(QIcon(":/images/icon.png"));

  qnode.init();

  QObject::connect(&qnode, SIGNAL(rosShutdown()), this, SLOT(close()));
  // subscribe한 이미지의 callback에서 시그널을 발생시켜 위 함수를 부른다.
  QObject::connect(&qnode, SIGNAL(sigRcvImg()), this, SLOT(slotUpdateImg()));

  QObject::connect(ui.manipulation, SIGNAL(clicked()), this, SLOT(Mani()));
  QObject::connect(ui.autorace, SIGNAL(clicked()), this, SLOT(Auto()));
  QObject::connect(ui.GO_button, SIGNAL(clicked()), this, SLOT(autorace_go()));
  QObject::connect(ui.STOP_button, SIGNAL(clicked()), this, SLOT(autorace_stop()));
}

MainWindow::~MainWindow()
{
}

/*****************************************************************************
** Functions
*****************************************************************************/
// delete로 고치기

// 이미지 처리 및 UI 업데이트를 수행
void MainWindow::slotUpdateImg()
{
  /*****************************************************************************
   ** 원본 이미지(img) : imgRaw 크기를 480x270으로 리사이즈 : Mat -> QImage 변환
   *****************************************************************************/
  cv::Mat img = qnode.imgRaw->clone();                               // 원본 이미지
  cv::resize(img, img, cv::Size(480, 270), 0, 0, cv::INTER_LINEAR);  // 리사이즈

  QImage origin_image(img.data, img.cols, img.rows, img.step, QImage::Format_RGB888);  // 첫번째 UI
  ui.origin->setPixmap(QPixmap::fromImage(origin_image));

  QImage origin_2_image(img.data, img.cols, img.rows, img.step, QImage::Format_RGB888); 
  ui.origin_2->setPixmap(QPixmap::fromImage(origin_2_image));

  QImage origin_3_image(img.data, img.cols, img.rows, img.step, QImage::Format_RGB888);
  ui.origin_3->setPixmap(QPixmap::fromImage(origin_3_image));
  
  QImage origin_4_image(img.data, img.cols, img.rows, img.step, QImage::Format_RGB888);
  ui.origin_4->setPixmap(QPixmap::fromImage(origin_4_image));

  /*****************************************************************************
   ** 버드아이뷰 이미지(bird_eye)
   *****************************************************************************/


  /*****************************************************************************
   ** 이진화 찾는 이미지
   *****************************************************************************/
  Find_Binary_img(img);

  /*****************************************************************************
   ** 라인 인식
   *****************************************************************************/
  //흰색 선 이미지(white_img) :
  int white_HSV_value[6] = { 89, 0, 75, 179, 43, 158};  // low h, low s, low v, high h, high s, high v
  cv::Mat white_img = Binary(img, white_HSV_value);      // 흰색 선 이진화
  int white_ROI_value[8] = { 241, 270, 480, 270, 480, 0, 241, 0 };  // 좌측 하단 꼭지점, 우측 하단 꼭지점, 우측 상단
                                                                     // 꼭지점, 좌측 상단 꼭지점
                                                                     
  cv::Mat mask = cv::getStructuringElement(cv::MORPH_RECT,cv::Size(3,3),cv::Point(1,1)); 
  cv::erode(white_img,white_img,mask,cv::Point(-1,-1),1);
  cv::dilate(white_img,white_img,mask,cv::Point(-1,-1),3);
  // cv::Mat wroi = region_of_interest(white_img, white_ROI_value);  // 흰색 관심 영역 설정
  // cv::Mat mw = morphological_transformation(wroi);
  cv::Mat white_edge = canny_edge(white_img);  // 흰색 Canny 엣지 검출 적용
  drawline(white_edge);
  QImage white_QImage(white_edge.data, white_edge.cols, white_edge.rows, white_edge.step, QImage::Format_Grayscale8);
  ui.white->setPixmap(QPixmap::fromImage(white_QImage));

  //노란색 선 이미지(yellow_img) :
  int yellow_HSV_value[6] = { 81, 119, 67, 120, 191, 145};  // low h, low s, low v, high h, high s, high v
  cv::Mat yellow_img = Binary(img, yellow_HSV_value);     // 노란색 선 이진화
  int yellow_ROI_value[8] = { 0, 270, 240, 270, 240, 0, 0, 0 };  // 좌측 하단 꼭지점, 우측 하단 꼭지점, 우측 상단
                                                                 // 꼭점, 좌측 상단 꼭지점
  //cv::erode(yellow_img,yellow_img,mask,cv::Point(-1,-1),1);
  cv::dilate(yellow_img,yellow_img,mask,cv::Point(-1,-1),2);
  // cv::Mat yroi = region_of_interest(yellow_img, yellow_ROI_value);  // 노란색 관심 영역 설정
  // cv::Mat my = morphological_transformation(yroi);
  cv::Mat yellow_edge = canny_edge(yellow_img);  // 노란색 Canny 엣지 검출 적용
  drawline(yellow_edge);
  QImage yellow_QImage(yellow_edge.data, yellow_edge.cols, yellow_edge.rows, yellow_edge.step,
                       QImage::Format_Grayscale8);
  ui.yellow->setPixmap(QPixmap::fromImage(yellow_QImage));

  //빨간색 선 이미지(red_img) :
  int red_HSV_value[6] = { 0, 80, 0, 10, 255, 255 };   // low h, low s, low v, high h, high s, high v
  cv::Mat red_img = Binary(img, red_HSV_value);  // 빨간색 선 이진
  int red_ROI_value[8] = { 0, 100, 480, 100, 480, 0, 0, 0 };  // 좌측 하단 꼭지점, 우측 하단 꼭지점, 우측 상단 꼭지점,
                                                              // 좌측 상단 꼭지점
  // cv::Mat rroi = region_of_interest(red_img, red_ROI_value);  // 빨간색 관심 영역 설정
  // cv::Mat mr = morphological_transformation(rroi);
  cv::Mat red_edge = canny_edge(red_img);  // 빨간색 Canny 엣지 검출 적용
  drawline(red_edge);
  QImage red_QImage(red_edge.data, red_edge.cols, red_edge.rows, red_edge.step, QImage::Format_Grayscale8);
  ui.red->setPixmap(QPixmap::fromImage(red_QImage));

  // 흰색+노란색+빨간색 합쳐진 이미지(mergedImage), 외각 강조 이미지(binaryImage)
  cv::Mat addImage = mergeImages(white_img, yellow_img, red_img);
  QImage binaryQImage(addImage.data, addImage.cols, addImage.rows, addImage.step, QImage::Format_Grayscale8);
  ui.result->setPixmap(QPixmap::fromImage(binaryQImage));
  QImage binary_2_QImage(addImage.data, addImage.cols, addImage.rows, addImage.step, QImage::Format_Grayscale8);
  ui.line->setPixmap(QPixmap::fromImage(binary_2_QImage));

  //초록색 선 이미지(red_img) :
  int green_HSV_value[6] = { 0, 100, 0, 50, 255, 255 };  // low h, low s, low v, high h, high s, high v
  cv::Mat green_img = Binary(img, green_HSV_value);     // 초록색 선 이진화
  int green_ROI_value[8] = { 200, 135, 260, 135, 260, 70, 200, 70 };
  // cv::Mat mg = morphological_transformation(green_img);
  // cv::Mat groi = region_of_interest(green_img, green_ROI_value);
  cv::Mat green = cutImages(green_img);  // 초록색 관심 영역 설정
  QImage green_QImage(green.data, green.cols, green.rows, green.step, QImage::Format_Grayscale8);
  ui.green->setPixmap(QPixmap::fromImage(green_QImage));
  QImage green_2_QImage(green.data, green.cols, green.rows, green.step, QImage::Format_Grayscale8);
  ui.sign->setPixmap(QPixmap::fromImage(green_2_QImage));

  //sign
  findAndDrawContours(green, img);

  //ride
  XY ans;
  RIDE ride;
  ans = ride.riding(yellow_img, white_img, red_img);
  qnode.lrpm = ans.x;
  qnode.rrpm = ans.y;

  //pantilt
 
  if (pantilt_flag == 1)
  {
    qnode.lrpm=10; 
    qnode.rrpm=10;
  } 
  
  display_view();
  if(cnt_flag ==1)
  {
    cnt++;
  }
  /*****************************************************************************
   ** 초기화
   *****************************************************************************/
  if (qnode.imgRaw != nullptr)
  {
    delete qnode.imgRaw;     // 동적으로 할당된 이미지 데이터 해제
    qnode.imgRaw = nullptr;  // 포인터를 NULL로 설정하여 dangling pointer 문제 방지
  }
  qnode.isreceived = false;
}

/*****************************************************************************
 ** 라인 & 표지판 TEST
 *****************************************************************************/
// 이진화 찾는 함수
void MainWindow::Find_Binary_img(cv::Mat& img)
{
  // 표지판 이진화
  cv::Mat F_Image = Binary(img, value_hsv);

  // 이진 이미지를 표시
  QImage sign_binaryQImage(F_Image.data, F_Image.cols, F_Image.rows, F_Image.step, QImage::Format_Grayscale8);
  ui.test->setPixmap(QPixmap::fromImage(sign_binaryQImage));

  // 슬라이더
  value_hsv[0] = ui.horizontalSlider_7->value();   // low h
  value_hsv[1] = ui.horizontalSlider_8->value();   // low s
  value_hsv[2] = ui.horizontalSlider_9->value();   // low v
  value_hsv[3] = ui.horizontalSlider_10->value();  // high h
  value_hsv[4] = ui.horizontalSlider_11->value();  // high s
  value_hsv[5] = ui.horizontalSlider_12->value();  // high v

  // 슬라이더 값
  ui.s_1->display(value_hsv[0]);
  ui.s_2->display(value_hsv[1]);
  ui.s_3->display(value_hsv[2]);
  ui.s_4->display(value_hsv[3]);
  ui.s_5->display(value_hsv[4]);
  ui.s_6->display(value_hsv[5]);
}

/*****************************************************************************
 ** bird_eye_view : 원근 변환
 *****************************************************************************/
cv::Mat MainWindow::Bird_eye_view(cv::Mat image)
{
  // 원근 변환 포인트 설정
  cv::Point src_pts1 = cv::Point2f(HALF_WIDTH - 150, HALF_HEIGHT - 10);  //좌상
  cv::Point src_pts2 = cv::Point2f(0, IMAGE_HEIGHT - 80);                //좌하
  cv::Point src_pts3 = cv::Point2f(HALF_WIDTH + 150, HALF_HEIGHT - 10);  //우상
  cv::Point src_pts4 = cv::Point2f(IMAGE_WIDTH, IMAGE_HEIGHT - 80);      //우하

  // 변환될 이미지의 좌표
  cv::Point dist_pts1 = cv::Point2f(0, 0);                       //좌상
  cv::Point dist_pts2 = cv::Point2f(0, IMAGE_HEIGHT);            //좌하
  cv::Point dist_pts3 = cv::Point2f(IMAGE_WIDTH, 0);             //우상
  cv::Point dist_pts4 = cv::Point2f(IMAGE_WIDTH, IMAGE_HEIGHT);  //우하

  // 변환 행렬 계산
  std::vector<cv::Point2f> warp_src_mtx = { src_pts1, src_pts2, src_pts3, src_pts4 };
  std::vector<cv::Point2f> warp_dist_mtx = { dist_pts1, dist_pts2, dist_pts3, dist_pts4 };
  cv::Mat src_to_dist_mtx = getPerspectiveTransform(warp_src_mtx, warp_dist_mtx);

  // 원근 변환 이미지
  cv::Mat bird_eye_view;
  warpPerspective(image, bird_eye_view, src_to_dist_mtx, cv::Size(IMAGE_WIDTH, IMAGE_HEIGHT), cv::INTER_LINEAR);

  return bird_eye_view;
}

/*****************************************************************************
 ** (선)이진화 하는데 사용되는 함수 :
 *****************************************************************************/
// 이진화
cv::Mat MainWindow::Binary(cv::Mat& cloneImage, int val[])
{
  // 복사 이미지 HSV로 변환
  cv::Mat hsvImg;
  cv::cvtColor(cloneImage, hsvImg, cv::COLOR_BGR2HSV);

  // Gaussian blur로 반사율 쥴이기
  cv::Mat blurredImage;
  cv::GaussianBlur(hsvImg, blurredImage, cv::Size(5, 5), 0);

  // HSV 이미지를 사용하여 범위 내의 색상을 임계값으로 설정
  cv::Scalar lower(val[0], val[1], val[2]);
  cv::Scalar upper(val[3], val[4], val[5]);
  cv::Mat binaryImage;
  cv::inRange(blurredImage, lower, upper, binaryImage);

  return binaryImage;
}

// 선 관심 영역 설정 함수
cv::Mat MainWindow::region_of_interest(cv::Mat& cloneImage, int val[])
{
  // 입력 이미지의 높이와 너비 가져오기
  int imageHeight = cloneImage.rows;
  int imageWidth = cloneImage.cols;

  // 이미지 크기와 같은 빈 이미지 생성
  cv::Mat imageMask = cv::Mat::zeros(cloneImage.size(), cloneImage.type());

  std::vector<cv::Point> roi_pts(4);       // 관심 영역의 꼭지점 좌표 설정
  roi_pts[0] = cv::Point(val[0], val[1]);  // 좌측 하단 꼭지점
  roi_pts[1] = cv::Point(val[2], val[3]);  // 우측 하단 꼭지점
  roi_pts[2] = cv::Point(val[4], val[5]);  // 우측 상단 꼭지점
  roi_pts[3] = cv::Point(val[6], val[7]);  // 좌측 상단 꼭지점

  const cv::Point* ppt[1] = { roi_pts.data() };  // 꼭지점 배열을 포인터
  int npt[] = { 4 };                             // 꼭지점 수

  // 다각형이 채워질 대상 이미지, 꼭지점을 지정하는 포인터 배열, 꼭지점 수, 다각형의 수, 색상
  cv::fillPoly(imageMask, ppt, npt, 1, cv::Scalar(255, 255, 255));
  cv::Mat maskingImage;                                  // 결과이미지
  cv::bitwise_and(cloneImage, imageMask, maskingImage);  // 이미지와 마스크를 AND 연산하여 관심 영역만 추출

  return maskingImage;
}

// 노이즈 제거
cv::Mat MainWindow::morphological_transformation(cv::Mat image)
{
  cv::Mat morphological_transformation_image;
  cv::Mat kernel = cv::Mat::ones(6, 6, CV_32F);
  cv::morphologyEx(image, morphological_transformation_image, cv::MORPH_OPEN, kernel);

  return morphological_transformation_image;
}

// Canny 엣지 검출
cv::Mat MainWindow::canny_edge(cv::Mat image)
{
  // 가우시안 블러된 이미지에 Canny 엣지 검출 적용
  cv::Mat canny_conversion;
  cv::Canny(image, canny_conversion, 50, 200, 3);

  return canny_conversion;
}

// 흰색 이미지와 노란색 이미지와 빨강이미지 합치기
cv::Mat MainWindow::mergeImages(const cv::Mat& whiteImage, const cv::Mat& yellowImage, const cv::Mat& redImage)
{
  cv::Mat merge, add;
  cv::bitwise_or(whiteImage, yellowImage, merge);
  cv::bitwise_or(merge, redImage, add);

  return add;
}

// 3줄 긋기
void MainWindow::drawline(cv::Mat& Image)
{
  cv::Point p1(0, HALF_HEIGHT);
  cv::Point p2(IMAGE_WIDTH, HALF_HEIGHT);
  cv::line(Image, p1, p2, cv::Scalar(255, 0, 0), 1);

  cv::Point p3(0, HALF_HEIGHT + 60);
  cv::Point p4(IMAGE_WIDTH, HALF_HEIGHT + 60);
  cv::line(Image, p3, p4, cv::Scalar(255, 0, 0), 1);

  cv::Point p5(0, HALF_HEIGHT - 60);
  cv::Point p6(IMAGE_WIDTH, HALF_HEIGHT - 60);
  cv::line(Image, p5, p6, cv::Scalar(255, 0, 0), 1);

  cv::Point p7(HALF_WIDTH, 0);
  cv::Point p8(HALF_WIDTH, IMAGE_HEIGHT);
  cv::line(Image, p7, p8, cv::Scalar(255, 0, 0), 1);

    cv::Point p9(HALF_WIDTH+120, 0);
  cv::Point p10(HALF_WIDTH+120, IMAGE_HEIGHT);
  cv::line(Image, p9, p10, cv::Scalar(255, 0, 0), 1);

    cv::Point p11(HALF_WIDTH-120, 0);
  cv::Point p12(HALF_WIDTH-120, IMAGE_HEIGHT);
  cv::line(Image, p11, p12, cv::Scalar(255, 0, 0), 1);

}

/*****************************************************************************
 ** 표지판 인식 :
 *****************************************************************************/
// 외각선 따는 함수
cv::Mat MainWindow::cutImages(cv::Mat& cloneImage)
{
  // 입력 이미지에 큰 크기의 사각형 커널을 사용하여 closing 모폴로지 연산을 적용
  cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(15, 15));
  cv::Mat closed;
  int iterations = 3;  // 반복 횟수를 늘림
  cv::morphologyEx(cloneImage, closed, cv::MORPH_CLOSE, kernel, cv::Point(-1, -1), iterations);

  // 외곽이 강조된 이미지를 반환dddss
  return closed;
}

//라벨링 하는 함수
void MainWindow::labeling(cv::Mat& image, cv::Mat& image2)
{
  cv::Mat result, stats, centroids;
  int num = cv::connectedComponentsWithStats(image, result, stats, centroids, 8, CV_32S);

  for (int i = 1; i < num; i++)
  {
    int left = static_cast<int>(stats.at<int>(i, cv::CC_STAT_LEFT));
    int top = static_cast<int>(stats.at<int>(i, cv::CC_STAT_TOP));
    int width = static_cast<int>(stats.at<int>(i, cv::CC_STAT_WIDTH));
    int height = static_cast<int>(stats.at<int>(i, cv::CC_STAT_HEIGHT));

    cv::rectangle(image2, cv::Rect(left, top, width, height), cv::Scalar(0, 255, 255), 4);
  }
}

// 외각선 그리는 함수
void MainWindow::findAndDrawContours(cv::Mat& closed, cv::Mat& image)
{
  
  if (closed.empty() || image.empty())
  {
    // qDebug() << "Error: Input image or mask is empty!";
    return;
  }

  // 외곽선을 저장할 변수, // 외곽선 찾기
  std::vector<std::vector<cv::Point>> contours;
  cv::findContours(closed.clone(), contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

  if (contours.empty())
  {
    // qDebug() << "No contours found!";
    return;
  }

  // 외각선을 크기에 따라 내림차순으로 정렬
  std::sort(contours.begin(), contours.end(), [](const std::vector<cv::Point>& a, const std::vector<cv::Point>& b) {
    return cv::contourArea(a) > cv::contourArea(b);
  });

  // 외곽선 그리기용 이미지 생성 // 가장 큰 외각선만 초록색으로 그림
  cv::Mat contoursImage = image.clone();
  cv::drawContours(contoursImage, std::vector<std::vector<cv::Point>>(1, contours[0]), -1, cv::Scalar(0, 255, 0), 3);

  QImage sign_QImage(contoursImage.data, contoursImage.cols, contoursImage.rows, contoursImage.step,
                     QImage::Format_RGB888);
  ui.draw->setPixmap(QPixmap::fromImage(sign_QImage));

  trimAndSaveImage(image, contours[0], 480, 270);  // 가장 큰 외각선만 전달
}

/// 외각선 자르고 저장하는 함수
void MainWindow::trimAndSaveImage(const cv::Mat& image, const std::vector<cv::Point>& contour, int maxWidth,
                                  int maxHeight)
{
  if (image.empty())
  {
    // qDebug() << "Error: Input image is empty!";
    return;
  }

  if (contour.empty()) 
  {
    // qDebug() << "No contour provided for trimming!";
    return;
  }

  // contour를 사용하여 x, y의 최소, 최대 값 찾기
  int x_min = INT_MAX, x_max = INT_MIN, y_min = INT_MAX, y_max = INT_MIN;

  for (const auto& point : contour)
  {
    int x_value = point.x;
    int y_value = point.y;

    x_min = std::min(x_min, x_value);
    x_max = std::max(x_max, x_value);
    y_min = std::min(y_min, y_value);
    y_max = std::max(y_max, y_value);
  }

  int x = x_min;
  int y = y_min;
  int w = x_max - x_min;
  int h = y_max - y_min;

  if (w <= 0 || h <= 0)
  {
    // qDebug() << "Invalid dimensions for trimming!";
    return;
  }

  // 이미지 자르기
  cv::Mat img_trim = image(cv::Rect(x, y, w, h));

  // 자른 이미지의 크기가 120x120 보다 클 경우만 이미지 조정
  if ((w > 50 && h > 50) && sign_end_flag == 0)
  {
    cnt_flag =1;
    //pantilt
    pantilt_flag = 1;
    qDebug() << "???????????????";
    qnode.pointval.x = x+w/2;
    qnode.pointval.y = y+h/2;
    qnode.pantilt_pub.publish(qnode.pointval);

    // 원본 비율 유지하면서 최대로 키우기
    double aspectRatio = static_cast<double>(w) / h;
    int targetWidth, targetHeight;

    if (aspectRatio > static_cast<double>(maxWidth) / maxHeight)
    {
      targetWidth = maxWidth;
      targetHeight = static_cast<int>(maxWidth / aspectRatio);
    }
    else
    {
      targetHeight = maxHeight;
      targetWidth = static_cast<int>(maxHeight * aspectRatio);
    }

    // 이미지 크기 조정
    cv::Mat resized_img;
    cv::resize(img_trim, resized_img, cv::Size(targetWidth, targetHeight));

    QImage signre_QImage(resized_img.data, resized_img.cols, resized_img.rows, resized_img.step, QImage::Format_RGB888);
    ui.result_2->setPixmap(QPixmap::fromImage(signre_QImage));
    QImage signre_2_QImage(resized_img.data, resized_img.cols, resized_img.rows, resized_img.step, QImage::Format_RGB888);
    ui.sign_result->setPixmap(QPixmap::fromImage(signre_2_QImage));
    
    //pantilt end moment
    if(((x >= 235 && x<=245) || (y >= 133 && y<= 137))|| cnt >=200)
    {
      
      pantilt_flag = 0;
      sign_end_flag = 1;
      qnode.boolval.data = true;
      qnode.init_pub.publish(qnode.boolval);
      return;
    }

  }
  
}
/*****************************************************************************
 ** 자율주행 :
 *****************************************************************************/
void MainWindow::display_view()
{
  ui.Mani_Auto->display(mani_auto_flag);
  ui.Go_Stop->display(go_stop_flag);

  ui.m_1->display(pantilt_flag);
  ui.m_2->display(cnt);
  ui.m_3->display(angle3);
  ui.m_4->display(angle4);

}

void MainWindow::pantilt()
{
  //(오토레이스)차선 인식 시작 -> 계속 고정해야 하는 각도값을 전달
  if ((mani_auto_flag == 0||mani_auto_flag ==1) && pantilt_flag == 0)  //차선
  {
    qnode.boolval.data = true;
  }
  else if(pantilt_flag == 1) //차선, 표지판 둘다 값 없음
  {
    qnode.boolval.data = false;
  }
  qnode.init_pub.publish(qnode.boolval);
}

void MainWindow::Mani()
{
  mani_auto_flag = 1;
   pantilt();
}
void MainWindow::Auto()
{
  mani_auto_flag = 0;
   pantilt();
}

void MainWindow::autorace_go()
{
  go_stop_flag = 1;
  init_v = 0;
}
void MainWindow::autorace_stop()
{
  go_stop_flag = 0;
}

}  // namespace sliding