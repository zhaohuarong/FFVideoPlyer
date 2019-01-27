#pragma once
#include <QMutex>

extern "C"
{
#include <libavformat/avformat.h>
#include<libswscale/swscale.h>
#include <libswresample/swresample.h>
}

class MyFFmpeg
{
public:
	static MyFFmpeg *GetObj()
	{
		static MyFFmpeg ff;
		return &ff;
	}

	virtual ~MyFFmpeg();

	void OpenVideo(const char *path);
    void DecodeFrame(const AVPacket *pkt);
	AVPacket ReadFrame();
	bool YuvToRGB(char *out, int outweight, int outheight);
	int ToPCM(char *out);

	void CloseForeVideo();

public:
	bool m_isPlay;
    AVFrame *m_yuv = nullptr;
    AVFrame *m_pcm = nullptr;
    SwsContext *m_cCtx = nullptr;

	int m_sampleRate = 48000;
	int m_sampleSize = 16;
	int m_channel = 2;
	
	int m_videoStream = 0;
	int m_audioStream = 1;

protected:
	MyFFmpeg();

    AVFormatContext* m_afc = nullptr;
	
	QMutex mtx;
    SwrContext *m_aCtx = nullptr;
};

