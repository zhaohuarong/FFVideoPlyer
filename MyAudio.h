#pragma once
#include <QtMultimedia/QAudioOutput>
#include <QMutex>

class MyAudio
{
public:
	static MyAudio *GetObj()
	{
		static MyAudio mau;
		return &mau;
	}

	~MyAudio();

	void Stop();
	bool Start();
	void Play(bool isPlay);
	bool Write(const char *data, int datasize);
	int GetFree();

	int sampleRate = 48000;
	int sampleSize = 16;
	int channel = 2;

public:
	QAudioOutput *output = NULL;
	QIODevice *io = NULL;
	QMutex mutex;

protected:
	MyAudio();
};

