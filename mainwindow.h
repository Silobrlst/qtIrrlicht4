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


private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
