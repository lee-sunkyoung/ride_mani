/**
 * @file /include/sliding/main_window.hpp
 *
 * @brief Qt based gui for %(package)s.
 *
 * @date November 2010
 **/
#ifndef sliding_MAIN_WINDOW_H
#define sliding_MAIN_WINDOW_H

/*****************************************************************************
** Includes
*****************************************************************************/

#include <QMainWindow>
#include "ui_main_window.h"
#include <opencv2/opencv.hpp>
#include "qnode.hpp"
#include <vector>
#include <cmath>

/*****************************************************************************
** Namespace
*****************************************************************************/

namespace sliding
{
/*****************************************************************************
** Interface [MainWindow]
*****************************************************************************/
/**
 * @brief Qt central, all operations relating to the view part here.
 */

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(int argc, char** argv, QWidget* parent = 0);
  ~MainWindow();

  int abcde = 1234;

  int IMAGE_WIDTH = 480;
  int IMAGE_HEIGHT = 270;
  int HALF_WIDTH = 240;
  int HALF_HEIGHT = 135;

  int value_sign[6];  // sign hsv
  int value_line[6];  // line hsv

  int go_stop_flag = 0;
  int init_v = 0;

  int mani_auto_flag = 1;
  int angle1 = 0;  //초기값 입력
  int angle2 = 0;
  int angle3 = 0;
  int angle4 = 0;

  void Find_Line_Binary_img(cv::Mat& cloneImage);
  void Find_Sign_Binary_img(cv::Mat& cloneImage);

  cv::Mat Bird_eye_view(cv::Mat image);

  cv::Mat Binary(cv::Mat& cloneImage, int val[]);
  cv::Mat region_of_interest(cv::Mat& cloneImage, int val[]);
  cv::Mat morphological_transformation(cv::Mat image);
  cv::Mat canny_edge(cv::Mat image);
  cv::Mat mergeImages(const cv::Mat& whiteImage, const cv::Mat& yellowImage, const cv::Mat& redImage);
  void drawline(cv::Mat& Image);

  cv::Mat cutImages(cv::Mat& cloneImage);
  void labeling(cv::Mat& image, cv::Mat& image2);
  void findAndDrawContours(cv::Mat& closed, cv::Mat& image);
  void trimAndSaveImage(const cv::Mat& image, const std::vector<cv::Point>& contour, int maxWidth, int maxHeight);

  void display_view();
  void pantilt();

public Q_SLOTS:
  void slotUpdateImg();
  void Mani();
  void Auto();
  void autorace_go();
  void autorace_stop();

private:
  Ui::MainWindowDesign ui;
  QNode qnode;
};

}  // namespace sliding

#endif  // sliding_MAIN_WINDOW_H
