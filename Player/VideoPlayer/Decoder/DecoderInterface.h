#ifndef DECODERINTERFACE_H 
#define DECODERINTERFACE_H
#include <QImage>
#include "SyncQueue.h"

class DecoderInterface :public QThread
{
public:
	explicit DecoderInterface(const std::string& strStreamFile);

	virtual ~DecoderInterface();

	virtual void loop() = 0;

	void stop();

	//QImage tack(bool& isValid);
protected:
	void push(const QImage& img);

private:
	void run();

protected:
	SyncQueue<QImage> queue;
	bool mbExit;
	std::string mstrSteamFile;
};
#endif //DECODERINTERFACE_H
