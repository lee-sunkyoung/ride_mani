/**
 * @file /include/cam_udp_publisher/main_window.hpp
 *
 * @brief Qt based gui for %(package)s.
 *
 * @date November 2010
 **/
#ifndef cam_udp_publisher_MAIN_WINDOW_H
#define cam_udp_publisher_MAIN_WINDOW_H

/*****************************************************************************
** Includes
*****************************************************************************/

#include <QMainWindow>
#include "ui_main_window.h"
#include "qnode.hpp"
#include "udp.h"
#include <QComboBox>
#include <opencv2/opencv.hpp>
#include <QImage>
#include <QMessageBox>
#include <QtNetwork/QUdpSocket>
#include <QHostAddress>

/*****************************************************************************
** Namespace
*****************************************************************************/

namespace cam_udp_publisher
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

  void initOption();

  bool isInit = false;
  bool isStarted = false;
  bool isConnected[6] = { false, false, false, false, false, false };

  int imgResizeWidth = 320;
  int imgResizeHeight = 180;

  QString styleSheetRed = "color: red;";
  QString styleSheetGreen = "color: green;";
  QString styleSheetWhite = "color: white;";

  QHostAddress address[6];
  QUdpSocket* socket[6];
  uint16_t portList[6];

public Q_SLOTS:
  void slotUpdateImg();
  void slotUpdateTopic();

  void on_btnGit_clicked();
  void on_btnApply_clicked();
  void on_btnStart_clicked();
  void on_btnStop_clicked();
  void on_btnShutdown_clicked();
  void on_btnListReload_clicked();
  void on_btn1_clicked();
  void on_btn2_clicked();
  void on_btn3_clicked();
  void on_btn4_clicked();
  void on_btn5_clicked();
  void on_btn6_clicked();

private:
  Ui::MainWindowDesign ui;
  QNode qnode;
};

}  // namespace cam_udp_publisher

#endif  // cam_udp_publisher_MAIN_WINDOW_H
