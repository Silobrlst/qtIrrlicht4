#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qstringlistmodel.h>
#include <QListWidgetItem>
#include <qstandarditemmodel.h>
#include "attributetreedelegate.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QObject::connect(ui->actionSave_data, SIGNAL(triggered()), ui->irrlichtWidget, SLOT(saveData()));

    QObject::connect(ui->selectTool, SIGNAL(clicked()), this, SLOT(setSelectTool()));
    QObject::connect(ui->moveTool, SIGNAL(clicked()), this, SLOT(setMoveTool()));
    QObject::connect(ui->rotateTool, SIGNAL(clicked()), this, SLOT(setRotateTool()));
    QObject::connect(ui->scaleTool, SIGNAL(clicked()), this, SLOT(setScaleTool()));

    ui->selectTool->click();

    //<left panel>---------------------------------
    ui->irrlichtWidget->moveFromCameraInverse = ui->moveFromCameraInverse;
    ui->irrlichtWidget->moveFromCameraDist = ui->moveFromCameraDist;

    QObject::connect(ui->selectTool, SIGNAL(clicked()), ui->irrlichtWidget, SLOT(selectTool()));
    QObject::connect(ui->moveTool, SIGNAL(clicked()), ui->irrlichtWidget, SLOT(moveTool()));

    QObject::connect(ui->moveFromCameraDist, SIGNAL(returnPressed()), ui->irrlichtWidget, SLOT(applyFromCameraDist()));
    //</left panel>--------------------------------

    QStandardItemModel *model = new QStandardItemModel();
    model->setItem(0, 0, new QStandardItem("qweqweqwe ryytrey"));

    QStandardItem *item = new QStandardItem();
    item->setData("zxczxc", Qt::DisplayRole);
    model->setItem(0, 1, item);

    ui->treeView->setModel(model);
    ui->treeView->setItemDelegate(new AttributeTreeDelegate());

    QLabel *statusLabel = new QLabel();
    ui->statusBar->addWidget(new QLabel("status: "));
    ui->statusBar->addWidget(statusLabel);
}

void MainWindow::resetToolButtons(){
    ui->selectTool->setChecked(false);
    ui->moveTool->setChecked(false);
    ui->rotateTool->setChecked(false);
    ui->scaleTool->setChecked(false);
}

void MainWindow::setSelectTool(){
    resetToolButtons();
    ui->selectTool->setChecked(true);
}

void MainWindow::setMoveTool(){
    resetToolButtons();
    ui->moveTool->setChecked(true);
}

void MainWindow::setRotateTool(){
    resetToolButtons();
    ui->rotateTool->setChecked(true);
}

void MainWindow::setScaleTool(){
    resetToolButtons();
    ui->scaleTool->setChecked(true);
}

void MainWindow::setXYZvisible(bool visibleIn){
}


MainWindow::~MainWindow()
{
    delete ui;
}
