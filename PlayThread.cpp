#include "PlayThread.h"
#include "MyFFmpeg.h"
#include <list>
#include "MyAudio.h"

using namespace std;

static list<AVPacket> g_videos; //save video frame

PlayThread::PlayThread()
{
}

PlayThread::~PlayThread()
{
}

void PlayThread::run()
{
	char out[10000] = { 0 };
	while (true)
	{
		if (!(MyFFmpeg::GetObj()->m_isPlay))
		{
            msleep(10);
			continue;
		}

		while (g_videos.size() > 0)
		{
			AVPacket pack = g_videos.front();

			MyFFmpeg::GetObj()->DecodeFrame(&pack);
			av_packet_unref(&pack);
            g_videos.pop_front();
		}

		int free = MyAudio::GetObj()->GetFree();
		if (free < 10000)
		{
			msleep(1);
			continue;
		}

		AVPacket pkt = MyFFmpeg::GetObj()->ReadFrame();

		if (pkt.size <= 0)
		{
			msleep(10);
		}

		if (pkt.stream_index == MyFFmpeg::GetObj()->m_audioStream)
		{
			MyFFmpeg::GetObj()->DecodeFrame(&pkt);
			av_packet_unref(&pkt);
            int len = MyFFmpeg::GetObj()->ToPCM(out); //transform audio
            MyAudio::GetObj()->Write(out, len);       //play audio
			continue;
		}

		if (pkt.stream_index != MyFFmpeg::GetObj()->m_videoStream)
		{
			av_packet_unref(&pkt);
			continue;
		}

		g_videos.push_back(pkt);
	}
}
