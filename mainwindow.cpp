#include <QKeyEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <iostream>
#include <list>
#include <QDebug>
#include "MyFFmpeg.h"
#include "MyAudio.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->action_Open, SIGNAL(triggered()), this, SLOT(slotOpenFile()));
    connect(ui->action_About, SIGNAL(triggered()), this, SLOT(slotAbout()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    switch(e->key())
    {
    case Qt::Key_Space:
        MyFFmpeg::GetObj()->m_isPlay = !MyFFmpeg::GetObj()->m_isPlay;
        break;
    }
}

void MainWindow::slotOpenFile()
{
    QString fname = QFileDialog::getOpenFileName(this, tr("Select file"));
    if (fname.isEmpty())
    {
        return;
    }

    setWindowTitle(fname);

    MyFFmpeg::GetObj()->OpenVideo(fname.toLocal8Bit());

    MyAudio::GetObj()->sampleRate = MyFFmpeg::GetObj()->m_sampleRate;
    MyAudio::GetObj()->channel = MyFFmpeg::GetObj()->m_channel;
    MyAudio::GetObj()->sampleSize = 16;

    MyAudio::GetObj()->Start();

    MyFFmpeg::GetObj()->m_isPlay = true;
}

void MainWindow::slotAbout()
{
    QMessageBox box(this);
    box.setWindowTitle(tr("About"));
    box.setText("00000000000000000");
    box.exec();
}
