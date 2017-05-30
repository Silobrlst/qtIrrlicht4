#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    QApplication *app;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setApp(QApplication *appIn){
        app = appIn;
    }

public slots:
    void setSelectTool();
    void setMoveTool();
    void setRotateTool();
    void setScaleTool();

    void setXYZvisible(bool visibleIn);

private:
    Ui::MainWindow *ui;

    void resetToolButtons();
};

#endif // MAINWINDOW_H
