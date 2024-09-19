#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QScreen>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // 窗口最大化
//    QScreen * screen = QGuiApplication::primaryScreen();
//    QRect screenGeometry = screen->geometry();
//    int height = screenGeometry.height();
//    int width = screenGeometry.width();
//    this->resize(width, height);
}

MainWindow::~MainWindow()
{
    delete ui;
}
