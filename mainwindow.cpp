#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->irrlichtWidget->rText = ui->rText;
    ui->irrlichtWidget->gText = ui->gText;
    ui->irrlichtWidget->bText = ui->bText;
    QObject::connect(ui->addPoint, SIGNAL(clicked()), ui->irrlichtWidget, SLOT(createSphere()));
}

MainWindow::~MainWindow()
{
    delete ui;
}
