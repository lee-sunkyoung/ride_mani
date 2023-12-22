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
  QImage origin_2_image(img.data, img.cols, img.rows, img.step, QImage::Format_RGB888);  // 세번째 UI
  ui.origin_2->setPixmap(QPixmap::fromImage(origin_2_image));

  /*****************************************************************************
   ** 버드아이뷰 이미지(bird_eye)
   *****************************************************************************/
  cv::Mat bird_eye = Bird_eye_view(img);
  QImage bird_image(bird_eye.data, bird_eye.cols, bird_eye.rows, bird_eye.step, QImage::Format_RGB888);
  ui.bird->setPixmap(QPixmap::fromImage(bird_image));
  QImage bird_2_image(bird_eye.data, bird_eye.cols, bird_eye.rows, bird_eye.step, QImage::Format_RGB888);
  ui.bird_2->setPixmap(QPixmap::fromImage(bird_2_image));

  cv::Mat cloneImage = bird_eye.clone();

  /*****************************************************************************
   ** 이진화 찾는 이미지
   *****************************************************************************/
  Find_Line_Binary_img(cloneImage);
  Find_Sign_Binary_img(img);

  /*****************************************************************************
   ** 라인 인식
   *****************************************************************************/
  //흰색 선 이미지(white_img) :
  int white_HSV_value[6] = { 0, 0, 100, 255, 60, 255 };     // low h, low s, low v, high h, high s, high v
  cv::Mat white_img = Binary(cloneImage, white_HSV_value);  // 흰색 선 이진화
  int white_ROI_value[8] = { 241, 270, 480, 270, 480, 0, 241, 0 };  // 좌측 하단 꼭지점, 우측 하단 꼭지점, 우측 상단
                                                                    // 꼭지점, 좌측 상단 꼭지점
  //cv::Mat wroi = region_of_interest(white_img, white_ROI_value);  // 흰색 관심 영역 설정
  // cv::Mat mw = morphological_transformation(wroi);
  cv::Mat white_edge = canny_edge(white_img);  // 흰색 Canny 엣지 검출 적용
  drawline(white_edge);
  QImage white_QImage(white_edge.data, white_edge.cols, white_edge.rows, white_edge.step, QImage::Format_Grayscale8);
  ui.white->setPixmap(QPixmap::fromImage(white_QImage));

  //노란색 선 이미지(yellow_img) :
  int yellow_HSV_value[6] = { 0, 80, 0, 255, 255, 255 };      // low h, low s, low v, high h, high s, high v
  cv::Mat yellow_img = Binary(cloneImage, yellow_HSV_value);  // 노란색 선 이진화
  int yellow_ROI_value[8] = { 0, 270, 240, 270, 240, 0, 0, 0 };  // 좌측 하단 꼭지점, 우측 하단 꼭지점, 우측 상단
                                                                 // 꼭지점, 좌측 상단 꼭지점
  //cv::Mat yroi = region_of_interest(yellow_img, yellow_ROI_value);  // 노란색 관심 영역 설정
  // cv::Mat my = morphological_transformation(yroi);
  cv::Mat yellow_edge = canny_edge(yellow_img);  // 노란색 Canny 엣지 검출 적용
  drawline(yellow_edge);
  QImage yellow_QImage(yellow_edge.data, yellow_edge.cols, yellow_edge.rows, yellow_edge.step,
                       QImage::Format_Grayscale8);
  ui.yellow->setPixmap(QPixmap::fromImage(yellow_QImage));

  //빨간색 선 이미지(red_img) :
  int red_HSV_value[6] = { 0, 80, 0, 10, 255, 255 };    // low h, low s, low v, high h, high s, high v
  cv::Mat red_img = Binary(cloneImage, red_HSV_value);  // 빨간색 선 이진
  int red_ROI_value[8] = { 0, 100, 480, 100, 480, 0, 0, 0 };  // 좌측 하단 꼭지점, 우측 하단 꼭지점, 우측 상단 꼭지점,
                                                             // 좌측 상단 꼭지점
  //cv::Mat rroi = region_of_interest(red_img, red_ROI_value);  // 빨간색 관심 영역 설정
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
  // int green_HSV_value[6] = { 0, 58, 28, 40, 255, 255 };  // low h, low s, low v, high h, high s, high v
  // cv::Mat green_img = Binary(img, green_HSV_value);     // 초록색 선 이진화
  // int green_ROI_value[8] = { 200, 135, 260, 135, 260, 70, 200, 70 };
  // cv::Mat groi = region_of_interest(green_img, green_ROI_value);
  // cv::Mat green = cutImages(groi);  // 초록색 관심 영역 설정
  // QImage green_QImage(green.data, green.cols, green.rows, green.step, QImage::Format_Grayscale8);
  // ui.green->setPixmap(QPixmap::fromImage(green_QImage));
  // QImage green_2_QImage(green.data, green.cols, green.rows, green.step, QImage::Format_Grayscale8);
  // ui.sign->setPixmap(QPixmap::fromImage(green_2_QImage));
  // findAndDrawContours(green, img);

  XY ans;

  RIDE ride;
  ans= ride.riding(yellow_img,white_img,red_img);
  display_view();
  qnode.lrpm = ans.x;
  qnode.rrpm = ans.y;


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
// 라인 이진화 찾는 함수
void MainWindow::Find_Line_Binary_img(cv::Mat& cloneImage)
{
  // 선 이진화

  cv::Mat LF_Image = Binary(cloneImage, value_line);

  // 이진 이미지를 표시
  QImage line_binaryQImage(LF_Image.data, LF_Image.cols, LF_Image.rows, LF_Image.step, QImage::Format_Grayscale8);
  ui.l_test->setPixmap(QPixmap::fromImage(line_binaryQImage));

  // 슬라이더
  value_line[0] = ui.horizontalSlider_1->value();  // low h
  value_line[1] = ui.horizontalSlider_2->value();  // low s
  value_line[2] = ui.horizontalSlider_3->value();  // low v
  value_line[3] = ui.horizontalSlider_4->value();  // high h
  value_line[4] = ui.horizontalSlider_5->value();  // high s
  value_line[5] = ui.horizontalSlider_6->value();  // high v

  // 슬라이더 값
  ui.l_1->display(value_line[0]);
  ui.l_2->display(value_line[1]);
  ui.l_3->display(value_line[2]);
  ui.l_4->display(value_line[3]);
  ui.l_5->display(value_line[4]);
  ui.l_6->display(value_line[5]);
}

// 표지판 이진화 찾는 함수
void MainWindow::Find_Sign_Binary_img(cv::Mat& img)
{
  // 표지판 이진화

  cv::Mat SF_Image = Binary(img, value_sign);

  // 이진 이미지를 표시
  QImage sign_binaryQImage(SF_Image.data, SF_Image.cols, SF_Image.rows, SF_Image.step, QImage::Format_Grayscale8);
  ui.s_test->setPixmap(QPixmap::fromImage(sign_binaryQImage));

  // 슬라이더
  value_sign[0] = ui.horizontalSlider_7->value();   // low h
  value_sign[1] = ui.horizontalSlider_8->value();   // low s
  value_sign[2] = ui.horizontalSlider_9->value();   // low v
  value_sign[3] = ui.horizontalSlider_10->value();  // high h
  value_sign[4] = ui.horizontalSlider_11->value();  // high s
  value_sign[5] = ui.horizontalSlider_12->value();  // high v

  // 슬라이더 값
  ui.s_1->display(value_sign[0]);
  ui.s_2->display(value_sign[1]);
  ui.s_3->display(value_sign[2]);
  ui.s_4->display(value_sign[3]);
  ui.s_5->display(value_sign[4]);
  ui.s_6->display(value_sign[5]);
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
  cv::Point p2(IMAGE_WIDTH, HALF_HEIGHT); //480 270(135)
  cv::line(Image, p1, p2, cv::Scalar(255, 0, 0), 2);

  cv::Point p3(0, HALF_HEIGHT + 50);
  cv::Point p4(IMAGE_WIDTH, HALF_HEIGHT + 50);  //480 185
  cv::line(Image, p3, p4, cv::Scalar(255, 0, 0), 2);

  cv::Point p5(0, HALF_HEIGHT - 50);
  cv::Point p6(IMAGE_WIDTH, HALF_HEIGHT - 50);  //480 85
  cv::line(Image, p5, p6, cv::Scalar(255, 0, 0), 2);
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

  // 외곽이 강조된 이미지를 반환
  return closed;
}

// 외각선 그리는 함수
void MainWindow::findAndDrawContours(cv::Mat& closed, cv::Mat& image)
{
  // 외곽선을 저장할 변수, // 외곽선 찾기
  std::vector<std::vector<cv::Point>> contours;
  cv::findContours(closed.clone(), contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

  // contours가 비어 있거나 유효하지 않을 때 함수 반환
  if (contours.empty() || !image.data)
  {
    return;
  }

  // 외곽선 그리기용 이미지 생성 // 모든 외곽선을 초록색으로 그림
  cv::Mat contoursImage = image.clone();
  cv::drawContours(contoursImage, contours, -1, cv::Scalar(0, 255, 0), 3);

  QImage sign_QImage(contoursImage.data, contoursImage.cols, contoursImage.rows, contoursImage.step,
                     QImage::Format_RGB888);
  ui.draw->setPixmap(QPixmap::fromImage(sign_QImage.rgbSwapped()));

  // trimAndSaveImage 함수 호출
  trimAndSaveImage(image, contours, 480, 270);

  // 이미지를 사용한 후 메모리 해제
  for (auto& contour : contours)
  {
    contour.clear();
  }
  contours.clear();
}

// 외각선 자르고 저장하는 함수
void MainWindow::trimAndSaveImage(const cv::Mat& image, const std::vector<std::vector<cv::Point>>& contours,
                                  int maxWidth, int maxHeight)
{
  // contours가 비어 있거나 이미지 데이터가 없을 때는 아무 작업도 수행하지 않음
  if (contours.empty() || !image.data)
  {
    return;
  }

  // contours를 numpy array로 변환한 후 x, y의 최소, 최대 값 찾기
  int x_min = INT_MAX, x_max = INT_MIN, y_min = INT_MAX, y_max = INT_MIN;  //초기화

  for (const auto& contour : contours)
  {
    for (const auto& point : contour)
    {
      int x_value = point.x;  // x 좌표
      int y_value = point.y;  // y 좌표

      // x의 최소, 최대값 갱신
      x_min = std::min(x_min, x_value);
      x_max = std::max(x_max, x_value);

      // y의 최소, 최대값 갱신
      y_min = std::min(y_min, y_value);
      y_max = std::max(y_max, y_value);
    }
  }

  // xywh 정의
  int x = x_min;
  int y = y_min;
  int w = x_max - x_min;
  int h = y_max - y_min;

  // 이미지 자르기
  cv::Mat img_trim = image(cv::Rect(x, y, w, h));

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
}

/*****************************************************************************
 ** 자율주행 :
 *****************************************************************************/
void MainWindow::display_view()
{
  ui.Mani_Auto->display(mani_auto_flag);
  ui.Go_Stop->display(go_stop_flag);
  
  ui.m_1->display(angle1);
  ui.m_2->display(angle2);
  ui.m_3->display(angle3);
  ui.m_4->display(angle4);


}
void MainWindow::Mani()
{
  mani_auto_flag = 1;
}
void MainWindow::Auto()
{
  mani_auto_flag = 0;
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
