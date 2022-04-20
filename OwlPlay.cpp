#include "OwlPlay.h"
#include <QFileDialog>
#include <QMessageBox>
#include "OwlDemuxThread.h"
static OwlDemuxThread demux_thread;

OwlPlay::OwlPlay(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	demux_thread.Start();
}

void OwlPlay::Open()
{
	// ѡ���ļ�
	QString name = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("ѡ����Ƶ�ļ�����������URL"));
	if (name.isEmpty())  return;
	this->setWindowTitle(name);
	demux_thread.Open(name.toLocal8Bit(), ui.video);
	if (!demux_thread.Open(name.toLocal8Bit(), ui.video)) {
		QMessageBox::information(0, "ERROR", "open failed!");
		return;
	}
	//qDebug() << name;
}
