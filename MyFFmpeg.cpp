#include <QDebug>
#include "MyFFmpeg.h"



MyFFmpeg::MyFFmpeg()
{
	av_register_all();

	m_isPlay = false;
}


MyFFmpeg::~MyFFmpeg()
{
}

void MyFFmpeg::OpenVideo(const char *path)
{
	CloseForeVideo();
	mtx.lock();
	int nRet = avformat_open_input(&m_afc, path, 0, 0);
	if (nRet != 0)
	{
		mtx.unlock();
		char buf[1024] = { 0 };
		av_strerror(nRet, buf, sizeof(buf));
        qDebug() << "error:" << buf;
		return;
	}

    //av_seek_frame(m_afc,-1,10*AV_TIME_BASE,AVSEEK_FLAG_ANY);

    for (int i = 0; i < m_afc->nb_streams; i++)
	{
        AVCodecContext *acc = m_afc->streams[i]->codec;

        if (acc->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			m_videoStream = i;
            AVCodec *codec = avcodec_find_decoder(acc->codec_id);

			if (!codec)
			{
				mtx.unlock();
				return;
            }

            int err = avcodec_open2(acc, codec, NULL);

			if (err != 0)
			{
				mtx.unlock();
				char buf[1024] = { 0 };
				av_strerror(err, buf, sizeof(buf));
				return;
			}
		}
		else if (acc->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			m_audioStream = i;
			AVCodec *codec = avcodec_find_decoder(acc->codec_id);
			int aRet = avcodec_open2(acc, codec, NULL);
			if (aRet < 0)
			{
				mtx.unlock();
				return;
			}

            this->m_sampleRate = acc->sample_rate;
            this->m_channel = acc->channels;
			switch (acc->sample_fmt)
			{
			case AV_SAMPLE_FMT_S16:
				this->m_sampleSize = 16;
				break;
			case AV_SAMPLE_FMT_S32:
				this->m_sampleSize = 32;
				break;
			default:
				break;
			}
		}
	}
	mtx.unlock();
}

void MyFFmpeg::CloseForeVideo()
{
	mtx.lock();
	if (m_afc)
	{
		avformat_close_input(&m_afc);
	}

	if (m_yuv)
	{
		av_frame_free(&m_yuv);
	}

	if (m_cCtx)
	{
		sws_freeContext(m_cCtx);
        m_cCtx = nullptr;
	}

	if (m_aCtx)
	{
		swr_free(&m_aCtx);
	}

	mtx.unlock();
}

void MyFFmpeg::DecodeFrame(const AVPacket *pkt)
{
	mtx.lock();
	if (!m_afc)
	{
		mtx.unlock();
		return;
	}

    if (m_yuv == nullptr)
	{
		m_yuv = av_frame_alloc();
	}

    if (m_pcm == nullptr)
	{
		m_pcm = av_frame_alloc();
	}

    AVFrame *frame = m_yuv;
	if (pkt->stream_index == m_audioStream)
	{
		frame = m_pcm;
	}
	
	int re = avcodec_send_packet(m_afc->streams[pkt->stream_index]->codec, pkt);
	if (re != 0)
	{
		mtx.unlock();
		return;
	}

	re = avcodec_receive_frame(m_afc->streams[pkt->stream_index]->codec, frame);
	if (re != 0)
	{
		mtx.unlock();
		return;
	}

	mtx.unlock();
}

AVPacket MyFFmpeg::ReadFrame()
{
	AVPacket pkt;
	memset(&pkt, 0, sizeof(AVPacket));

	mtx.lock();
	if (!m_afc)
	{
		mtx.unlock();
		return pkt;
	}

	int err = av_read_frame(m_afc, &pkt);
	if (err != 0)
    {
	}
	mtx.unlock();

	return  pkt;
}

bool MyFFmpeg::YuvToRGB(char *out, int outweight, int outheight)
{
	mtx.lock();
    if (!m_afc || !m_yuv)
	{
		mtx.unlock();
		return false;
	}

	AVCodecContext *videoCtx = m_afc->streams[this->m_videoStream]->codec;
	m_cCtx = sws_getCachedContext(m_cCtx, videoCtx->width, videoCtx->height,
        videoCtx->pix_fmt,
        outweight, outheight,
        AV_PIX_FMT_BGRA,
        SWS_BICUBIC,
        nullptr, nullptr, nullptr);

//    qDebug() << "w:" << videoCtx->width << "h:" << videoCtx->height;

	if (m_cCtx)
	{
	}
	else
	{
	}

	uint8_t *data[AV_NUM_DATA_POINTERS] = { 0 };

    data[0] = (uint8_t *)out;
	int linesize[AV_NUM_DATA_POINTERS] = { 0 };
    linesize[0] = outweight * 4;

	int h = sws_scale(m_cCtx, m_yuv->data, m_yuv->linesize, 0, videoCtx->height,
		data,
		linesize
		);

    mtx.unlock();
    return true;
}

int MyFFmpeg::ToPCM(char *out)
{
	mtx.lock();
	if (!m_afc || !m_pcm || !out)
	{
		mtx.unlock();
		return 0;
	}

	AVCodecContext *ctx = m_afc->streams[m_audioStream]->codec;
	if (m_aCtx == NULL)
	{
		m_aCtx = swr_alloc();
		swr_alloc_set_opts(m_aCtx, ctx->channel_layout,
			AV_SAMPLE_FMT_S16,
			ctx->sample_rate,
			ctx->channels,
			ctx->sample_fmt,
			ctx->sample_rate,
			0, 0);

		swr_init(m_aCtx);
	}

	uint8_t *data[1];
	data[0] = (uint8_t *)out;
	int len = swr_convert(m_aCtx, data, 10000, (const uint8_t **)m_pcm->data, m_pcm->nb_samples);
	if (len <= 0)
	{
		mtx.unlock();
		return 0;
	}

    int outsize = av_samples_get_buffer_size(nullptr, ctx->channels, m_pcm->nb_samples, AV_SAMPLE_FMT_S16, 0);
	mtx.unlock();

	return outsize;
}

