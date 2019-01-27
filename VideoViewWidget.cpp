#include "VideoViewWidget.h"
#include "MyFFmpeg.h"
#include "PlayThread.h"
#include <QPainter>
#include <iostream>
#include <list>

using namespace std;

VideoViewWidget::VideoViewWidget(QWidget *parent)
	: QOpenGLWidget(parent)
{
    startTimer(20); //reflush video
	PlayThread::GetObj()->start();
}

VideoViewWidget::~VideoViewWidget()
{

}

void VideoViewWidget::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    static QImage *image = nullptr;

    static int w = 0; //save window size
	static int h = 0;
	if (w != width() || h != height())
	{
        //if window's size changed, free memery
		if (image)
		{
            delete image->bits();
			delete image;
            image = nullptr;
		}
	}

    if (image == nullptr)
	{
		uchar *buf = new uchar[width() * height() * 4];
		image = new QImage(buf, width(), height(), QImage::Format_ARGB32);
	}

    MyFFmpeg::GetObj()->YuvToRGB((char *)(image->bits()), width(), height());

	QPainter painter;
	painter.begin(this);
	painter.drawImage(QPoint(0, 0), *image);
	painter.end();
}

void VideoViewWidget::timerEvent(QTimerEvent *e)
{
    Q_UNUSED(e);
	this->update();
}
