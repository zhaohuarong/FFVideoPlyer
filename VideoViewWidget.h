#pragma once

#include <QOpenGLWidget>

class VideoViewWidget : public QOpenGLWidget
{
	Q_OBJECT

public:
	VideoViewWidget(QWidget *parent);
	~VideoViewWidget();

	void paintEvent(QPaintEvent *e);
	void timerEvent(QTimerEvent *e);
};
