#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *e);

private slots:
    void slotOpenFile();
    //void slotPlay();
    void slotAbout();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
