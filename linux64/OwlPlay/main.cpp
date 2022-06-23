#include "OwlPlay.h"
#include <QtWidgets/QApplication>
#include <QThread>
#include "OwlDemux.h"
#include "OwlDecode.h"
#include "OwlVideoWidget.h"
#include "OwlAudioThread.h"
#include "OwlVideoThread.h"
#include "OwlDemuxThread.h"
using namespace std;

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	OwlPlay w;
	w.show();

	return a.exec();
}
