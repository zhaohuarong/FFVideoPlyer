#include "MyAudio.h"


MyAudio::MyAudio()
{
}


MyAudio::~MyAudio()
{
}

void MyAudio::Stop()
{
	mutex.lock();
	if (output)
	{
		output->stop();
		delete output;
        output = nullptr;
        io = nullptr;
	}

	mutex.unlock();
}

bool MyAudio::Start()
{
	Stop();
	mutex.lock();
    QAudioFormat fmt;
    fmt.setSampleRate(this->sampleRate);
    fmt.setSampleSize(this->sampleSize);
    fmt.setChannelCount(this->channel);
    fmt.setCodec("audio/pcm");
    fmt.setByteOrder(QAudioFormat::LittleEndian);
    fmt.setSampleType(QAudioFormat::UnSignedInt);

	output = new QAudioOutput(fmt);
	io = output->start();
	mutex.unlock();
	return true;
}

void MyAudio::Play(bool isPlay)
{
	mutex.lock();
	if (!output)
	{
		mutex.unlock();
		return;
	}

	if (isPlay)
	{
		output->resume();
	}
	else
	{
		output->suspend();
	}

	mutex.unlock();
}

bool MyAudio::Write(const char *data, int datasize)
{
	if (!data || datasize <= 0)
		return false;
	mutex.lock();
	if (io)
	{
		mutex.unlock();
        io->write(data, datasize);
		return true;
	}

	mutex.unlock();
    return true;
}

int MyAudio::GetFree()
{
	mutex.lock();
	if (!output)
	{
		mutex.unlock();
		return 0;
	}

	int free = output->bytesFree();
	mutex.unlock();

	return free;
}
