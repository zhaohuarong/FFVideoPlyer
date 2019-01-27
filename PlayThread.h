#pragma once

#include <QThread>

class PlayThread : public QThread
{
	Q_OBJECT

public:
	static PlayThread* GetObj()
	{
		static PlayThread pt;
		return &pt;
	}

	~PlayThread();

	void run();

protected:
	PlayThread();
};
