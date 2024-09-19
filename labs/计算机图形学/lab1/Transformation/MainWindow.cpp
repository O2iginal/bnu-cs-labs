#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QLabel>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    QLabel * label = new QLabel(this);
    ui->statusBar->addWidget(label);
    connect(ui->openGLWidget, &OpenGLWidget::cursorPosition, label, &QLabel::setText);
}

MainWindow::~MainWindow()
{
    delete ui;
}
