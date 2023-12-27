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

#include "../include/cam_udp_publisher/main_window.hpp"
#include <QMessageBox>
#include <QtGui>
#include <iostream>

/*****************************************************************************
** Namespaces
*****************************************************************************/

namespace cam_udp_publisher {
using namespace Qt;

/*****************************************************************************
** Implementation [MainWindow]
*****************************************************************************/

MainWindow::MainWindow (int argc, char** argv, QWidget* parent)
: QMainWindow (parent), qnode (argc, argv) {
    ui.setupUi (this); // Calling this incidentally connects all ui's triggers to on_...() callbacks in this class.

    setWindowIcon (QIcon (":/images/camera.png"));
    QPixmap pixmap (":/images/intelligence.png");
    ui.logo->setPixmap (
    pixmap.scaled (ui.logo->size (), Qt::KeepAspectRatio, Qt::SmoothTransformation));

    ui.btnStart->setStyleSheet (styleSheetRed);

    for (int i = 0; i < 6; i++) {
        socket[i] = new QUdpSocket (this);
    }

    initOption ();
    isInit = true;
    qnode.init ();

    QObject::connect (&qnode, SIGNAL (rosShutdown ()), this, SLOT (close ()));
    QObject::connect (&qnode, SIGNAL (sigRcvImg ()), this, SLOT (slotUpdateImg ()));
    QObject::connect (&qnode, SIGNAL (sigReadTopic ()), this, SLOT (slotUpdateTopic ()));
}

MainWindow::~MainWindow () {
}

void MainWindow::initOption () {
    qnode.getStringTopicNew = ui.cbxTopic->currentText ();
    imgResizeWidth          = ui.iptImageWidth->toPlainText ().toInt ();
    imgResizeHeight         = ui.iptImageHeight->toPlainText ().toInt ();

    if (qnode.getStringTopicNew.isEmpty ()) {
        if (isInit) {
            QMessageBox::warning (this, "WARNING", "OPTION IS NOT SETTED PROPERLY");
        }
    } else {
        qnode.changeSubTopic ();
    }
}

void MainWindow::slotUpdateImg () {
    QImage qImage ((const unsigned char*)(qnode.imgView.data),
    qnode.imgView.cols, qnode.imgView.rows, QImage::Format_RGB888);
    ui.labelRawImage->setPixmap (QPixmap::fromImage (qImage));

    if (isStarted) {
        udp::UDP udpInstance;
        if (isConnected[0]) {
            Mat resizeImg;
            cv::resize (qnode.imgRaw, resizeImg, Size (imgResizeWidth, imgResizeHeight));
            udpInstance.MatImgTrs (resizeImg, portList[0], address[0], *socket[0]);
        }
        if (isConnected[1]) {
            Mat resizeImg;
            cv::resize (qnode.imgRaw, resizeImg, Size (imgResizeWidth, imgResizeHeight));
            udpInstance.MatImgTrs (resizeImg, portList[1], address[1], *socket[1]);
        }
        if (isConnected[2]) {
            Mat resizeImg;
            cv::resize (qnode.imgRaw, resizeImg, Size (imgResizeWidth, imgResizeHeight));
            udpInstance.MatImgTrs (resizeImg, portList[2], address[2], *socket[2]);
        }
        if (isConnected[3]) {
            Mat resizeImg;
            cv::resize (qnode.imgRaw, resizeImg, Size (imgResizeWidth, imgResizeHeight));
            udpInstance.MatImgTrs (resizeImg, portList[3], address[3], *socket[3]);
        }
        if (isConnected[4]) {
            Mat resizeImg;
            cv::resize (qnode.imgRaw, resizeImg, Size (imgResizeWidth, imgResizeHeight));
            udpInstance.MatImgTrs (resizeImg, portList[4], address[4], *socket[4]);
        }
        if (isConnected[5]) {
            Mat resizeImg;
            cv::resize (qnode.imgRaw, resizeImg, Size (imgResizeWidth, imgResizeHeight));
            udpInstance.MatImgTrs (resizeImg, portList[5], address[5], *socket[5]);
        }
    }
}

void MainWindow::slotUpdateTopic () {
    ui.cbxTopic->clear ();
    ui.cbxTopic->addItems (qnode.topicList);
}

void MainWindow::on_btnGit_clicked () {
    system ("xdg-open https://github.com/mjlee111");
}

void MainWindow::on_btnApply_clicked () {
    initOption ();
}

void MainWindow::on_btnStart_clicked () {
    if (!isStarted) {
        isStarted = true;
        ui.btnStart->setStyleSheet (styleSheetGreen);
        ui.btnStop->setStyleSheet (styleSheetRed);
    }
}

void MainWindow::on_btnStop_clicked () {
    if (isStarted) {
        isStarted = false;
        ui.btnStart->setStyleSheet (styleSheetRed);
        ui.btnStop->setStyleSheet (styleSheetWhite);
    }
}

void MainWindow::on_btnShutdown_clicked () {
    QMessageBox msgBox;
    msgBox.setText ("Warning : Continue Shutdown?");
    msgBox.setStandardButtons (QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton (QMessageBox::No);
    int choice = msgBox.exec ();
    if (choice == QMessageBox::Yes) {
        system ("sudo init 0");
    }
}

void MainWindow::on_btnListReload_clicked () {
    qnode.updateTopics ();
}

void MainWindow::on_btn1_clicked () {
    int num      = 0;
    QString text = ui.iptIP1->toPlainText ();
    if (text.isEmpty ()) {
        QMessageBox::warning (this, "Warning", "Target IP not set");
        return;
    }
    address[num]  = QHostAddress (ui.iptIP1->toPlainText ());
    portList[num] = ui.iptPort1->toPlainText ().toUShort ();
    if (!isConnected[num]) {
        ui.btn1->setStyleSheet (styleSheetGreen);
        std::cout << "[UDP Publisher] : Starting Stream "
                  << ui.iptIP1->toPlainText ().toStdString ()
                  << ui.iptPort1->toPlainText ().toStdString () << std::endl;
        isConnected[num] = true;
    } else if (isConnected[num]) {
        ui.btn1->setStyleSheet (styleSheetWhite);
        std::cout << "[UDP Publisher] : Ending Stream "
                  << ui.iptIP1->toPlainText ().toStdString ()
                  << ui.iptPort1->toPlainText ().toStdString () << std::endl;
        isConnected[num] = false;
    }
}

void MainWindow::on_btn2_clicked () {
    int num      = 1;
    QString text = ui.iptIP2->toPlainText ();
    if (text.isEmpty ()) {
        QMessageBox::warning (this, "Warning", "Target IP not set");
        return;
    }
    address[num]  = QHostAddress (ui.iptIP2->toPlainText ());
    portList[num] = ui.iptPort2->toPlainText ().toUShort ();
    if (!isConnected[num]) {
        ui.btn2->setStyleSheet (styleSheetGreen);
        std::cout << "[UDP Publisher] : Starting Stream "
                  << ui.iptIP1->toPlainText ().toStdString ()
                  << ui.iptPort1->toPlainText ().toStdString () << std::endl;
        isConnected[num] = true;
    } else if (isConnected[num]) {
        ui.btn2->setStyleSheet (styleSheetWhite);
        std::cout << "[UDP Publisher] : Ending Stream "
                  << ui.iptIP1->toPlainText ().toStdString ()
                  << ui.iptPort1->toPlainText ().toStdString () << std::endl;
        isConnected[num] = false;
    }
}

void MainWindow::on_btn3_clicked () {
    int num      = 2;
    QString text = ui.iptIP3->toPlainText ();
    if (text.isEmpty ()) {
        QMessageBox::warning (this, "Warning", "Target IP not set");
        return;
    }
    address[num]  = QHostAddress (ui.iptIP3->toPlainText ());
    portList[num] = ui.iptPort3->toPlainText ().toUShort ();
    if (!isConnected[num]) {
        ui.btn3->setStyleSheet (styleSheetGreen);
        std::cout << "[UDP Publisher] : Starting Stream "
                  << ui.iptIP1->toPlainText ().toStdString ()
                  << ui.iptPort1->toPlainText ().toStdString () << std::endl;
        isConnected[num] = true;
    } else if (isConnected[num]) {
        ui.btn3->setStyleSheet (styleSheetWhite);
        std::cout << "[UDP Publisher] : Ending Stream "
                  << ui.iptIP1->toPlainText ().toStdString ()
                  << ui.iptPort1->toPlainText ().toStdString () << std::endl;
        isConnected[num] = false;
    }
}

void MainWindow::on_btn4_clicked () {
    int num      = 3;
    QString text = ui.iptIP4->toPlainText ();
    if (text.isEmpty ()) {
        QMessageBox::warning (this, "Warning", "Target IP not set");
        return;
    }
    address[num]  = QHostAddress (ui.iptIP4->toPlainText ());
    portList[num] = ui.iptPort4->toPlainText ().toUShort ();
    if (!isConnected[num]) {
        ui.btn4->setStyleSheet (styleSheetGreen);
        std::cout << "[UDP Publisher] : Starting Stream "
                  << ui.iptIP1->toPlainText ().toStdString ()
                  << ui.iptPort1->toPlainText ().toStdString () << std::endl;
        isConnected[num] = true;
    } else if (isConnected[num]) {
        ui.btn4->setStyleSheet (styleSheetWhite);
        std::cout << "[UDP Publisher] : Ending Stream "
                  << ui.iptIP1->toPlainText ().toStdString ()
                  << ui.iptPort1->toPlainText ().toStdString () << std::endl;
        isConnected[num] = false;
    }
}

void MainWindow::on_btn5_clicked () {
    int num      = 4;
    QString text = ui.iptIP5->toPlainText ();
    if (text.isEmpty ()) {
        QMessageBox::warning (this, "Warning", "Target IP not set");
        return;
    }
    address[num]  = QHostAddress (ui.iptIP5->toPlainText ());
    portList[num] = ui.iptPort5->toPlainText ().toUShort ();
    if (!isConnected[num]) {
        ui.btn5->setStyleSheet (styleSheetGreen);
        std::cout << "[UDP Publisher] : Starting Stream "
                  << ui.iptIP1->toPlainText ().toStdString ()
                  << ui.iptPort1->toPlainText ().toStdString () << std::endl;
        isConnected[num] = true;
    } else if (isConnected[num]) {
        ui.btn5->setStyleSheet (styleSheetWhite);
        std::cout << "[UDP Publisher] : Ending Stream "
                  << ui.iptIP1->toPlainText ().toStdString ()
                  << ui.iptPort1->toPlainText ().toStdString () << std::endl;
        isConnected[num] = false;
    }
}

void MainWindow::on_btn6_clicked () {
    int num      = 5;
    QString text = ui.iptIP6->toPlainText ();
    if (text.isEmpty ()) {
        QMessageBox::warning (this, "Warning", "Target IP not set");
        return;
    }
    address[num]  = QHostAddress (ui.iptIP6->toPlainText ());
    portList[num] = ui.iptPort6->toPlainText ().toUShort ();
    if (!isConnected[num]) {
        ui.btn6->setStyleSheet (styleSheetGreen);
        std::cout << "[UDP Publisher] : Starting Stream "
                  << ui.iptIP1->toPlainText ().toStdString ()
                  << ui.iptPort1->toPlainText ().toStdString () << std::endl;
        isConnected[num] = true;
    } else if (isConnected[num]) {
        ui.btn6->setStyleSheet (styleSheetWhite);
        std::cout << "[UDP Publisher] : Ending Stream "
                  << ui.iptIP1->toPlainText ().toStdString ()
                  << ui.iptPort1->toPlainText ().toStdString () << std::endl;
        isConnected[num] = false;
    }
}
/*****************************************************************************
** Functions
*****************************************************************************/

} // namespace cam_udp_publisher
