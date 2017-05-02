#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qstringlistmodel.h>
#include <QListWidgetItem>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //<node panel>---------------------------------
    ui->irrlichtWidget->nameText = ui->nameText;
    ui->irrlichtWidget->radiusText = ui->radiusText;

    ui->irrlichtWidget->xText = ui->xText;
    ui->irrlichtWidget->yText = ui->yText;
    ui->irrlichtWidget->zText = ui->zText;

    ui->irrlichtWidget->rText = ui->rText;
    ui->irrlichtWidget->gText = ui->gText;
    ui->irrlichtWidget->bText = ui->bText;
    ui->irrlichtWidget->nodeRGBWeightText = ui->nodeRGBWeightText;

    QObject::connect(ui->xText, SIGNAL(returnPressed()), ui->irrlichtWidget, SLOT(applyX()));
    QObject::connect(ui->yText, SIGNAL(returnPressed()), ui->irrlichtWidget, SLOT(applyY()));
    QObject::connect(ui->zText, SIGNAL(returnPressed()), ui->irrlichtWidget, SLOT(applyZ()));
    QObject::connect(ui->applyXYZ, SIGNAL(clicked()), ui->irrlichtWidget, SLOT(applyX()));
    QObject::connect(ui->applyXYZ, SIGNAL(clicked()), ui->irrlichtWidget, SLOT(applyY()));
    QObject::connect(ui->applyXYZ, SIGNAL(clicked()), ui->irrlichtWidget, SLOT(applyZ()));

    QObject::connect(ui->rText, SIGNAL(returnPressed()), ui->irrlichtWidget, SLOT(applyR()));
    QObject::connect(ui->gText, SIGNAL(returnPressed()), ui->irrlichtWidget, SLOT(applyG()));
    QObject::connect(ui->bText, SIGNAL(returnPressed()), ui->irrlichtWidget, SLOT(applyB()));
    QObject::connect(ui->applyRGB, SIGNAL(clicked()), ui->irrlichtWidget, SLOT(applyR()));
    QObject::connect(ui->applyRGB, SIGNAL(clicked()), ui->irrlichtWidget, SLOT(applyG()));
    QObject::connect(ui->applyRGB, SIGNAL(clicked()), ui->irrlichtWidget, SLOT(applyB()));

    QObject::connect(ui->nameText, SIGNAL(returnPressed()), ui->irrlichtWidget, SLOT(applyName()));
    QObject::connect(ui->radiusText, SIGNAL(returnPressed()), ui->irrlichtWidget, SLOT(applyRadius()));
    QObject::connect(ui->applyRadius, SIGNAL(clicked()), ui->irrlichtWidget, SLOT(applyName()));
    QObject::connect(ui->applyRadius, SIGNAL(clicked()), ui->irrlichtWidget, SLOT(applyRadius()));
    //</node panel>--------------------------------

    //<edge panel>---------------------------------
    ui->irrlichtWidget->edgeNameText = ui->edgeNameText;

    ui->irrlichtWidget->edgeRtext = ui->edgeRtext;
    ui->irrlichtWidget->edgeGtext = ui->edgeGtext;
    ui->irrlichtWidget->edgeBtext = ui->edgeBtext;

    ui->irrlichtWidget->edgeDistText = ui->edgeDistText;
    ui->irrlichtWidget->edgeDistMaxText = ui->edgeDistMaxText;
    ui->irrlichtWidget->edgeDistMinText = ui->edgeDistMinText;
    ui->irrlichtWidget->edgeDistWeightText = ui->edgeDistWeightText;

    ui->irrlichtWidget->edgeRGBdiffText = ui->edgeRGBdiffText;
    ui->irrlichtWidget->edgeRGBdiffMaxText = ui->edgeRGBdiffMaxText;
    ui->irrlichtWidget->edgeRGBdiffMinText = ui->edgeRGBdiffMinText;
    ui->irrlichtWidget->edgeRGBdiffWeightText = ui->edgeRGBdiffWeightText;

    ui->irrlichtWidget->edgeXdiffText = ui->edgeXdiffText;
    ui->irrlichtWidget->edgeXmaxText = ui->edgeXmaxText;
    ui->irrlichtWidget->edgeXminText = ui->edgeXminText;
    ui->irrlichtWidget->edgeXweightText = ui->edgeXweightText;

    ui->irrlichtWidget->edgeYdiffText = ui->edgeYdiffText;
    ui->irrlichtWidget->edgeYmaxText = ui->edgeYmaxText;
    ui->irrlichtWidget->edgeYminText = ui->edgeYminText;
    ui->irrlichtWidget->edgeYweightText = ui->edgeYweightText;

    ui->irrlichtWidget->edgeZdiffText = ui->edgeZdiffText;
    ui->irrlichtWidget->edgeZmaxText = ui->edgeZmaxText;
    ui->irrlichtWidget->edgeZminText = ui->edgeZminText;
    ui->irrlichtWidget->edgeZweightText = ui->edgeZweightText;

    QObject::connect(ui->irrlichtWidget, SIGNAL(setNodePanelVisible(bool)), ui->rightPanelnode, SLOT(setVisible(bool)));
    QObject::connect(ui->irrlichtWidget, SIGNAL(setEdgePanelVisible(bool)), ui->rightPanelEdge, SLOT(setVisible(bool)));

    ui->rightPanelEdge->setVisible(false);
    //</edge panel>--------------------------------

    //<left panel>---------------------------------
    QObject::connect(ui->addPoint, SIGNAL(clicked()), ui->irrlichtWidget, SLOT(createSphere()));
    QObject::connect(ui->addEdge, SIGNAL(clicked()), ui->irrlichtWidget, SLOT(createEdge()));
    QObject::connect(ui->selectTool, SIGNAL(clicked()), ui->irrlichtWidget, SLOT(selectTool()));
    QObject::connect(ui->moveTool, SIGNAL(clicked()), ui->irrlichtWidget, SLOT(moveTool()));

    QStringListModel *model = new QStringListModel();
    QStringList List;
    List << "Clair de Lune" << "Reverie" << "Prelude";
    model->setStringList(List);

    ui->nodeList->setModel(model);
    //</left panel>--------------------------------

    QStringList tableHeader;
    tableHeader<<"from node"<<"edge" << "to node";
    ui->connectionsTable->setHorizontalHeaderLabels(tableHeader);

    QLabel *statusLabel = new QLabel();
    ui->statusBar->addWidget(new QLabel("status: "));
    ui->statusBar->addWidget(statusLabel);
}

MainWindow::~MainWindow()
{
    delete ui;
}
