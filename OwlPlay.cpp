#include "OwlPlay.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QLineEdit>
#include "OwlDemuxThread.h"
static OwlDemuxThread demux_thread;
// 输入拉流地址
static QString g_pull_url = "http://39.134.65.162/PLTV/88888888/224/3221225611/index.m3u8";

void OwlPlay::timerEvent(QTimerEvent* e)
{
	if (is_slider_press)  return;
	long long total = demux_thread.total_ms_;  // 先保存下来，防止判断>0之后在其他进程被改
	if (total > 0) {
		double pos = static_cast<double>(demux_thread.pts_) / static_cast<double>(total);
		int play_pos = ui.play_pos->maximum() * pos;
		ui.play_pos->setValue(play_pos);
	}
}

void OwlPlay::resizeEvent(QResizeEvent* e)
{
	ui.play_pos->move(50, this->height() - 100);
	ui.play_pos->resize(this->width() - 100, ui.play_pos->height());
	ui.open->move(100, this->height() - 150);
	ui.is_play->move(ui.open->x() + ui.open->width() + 10, ui.open->y());
	ui.video->resize(this->size());
}

void OwlPlay::mouseDoubleClickEvent(QMouseEvent* e)
{
	if (isFullScreen()) {
		this->showNormal();
	}
	else {
		this->showFullScreen();
	}
}

void OwlPlay::SetPause(bool is_pause)
{
	if (is_pause) {
		ui.is_play->setText("Play");
	}
	else {
		ui.is_play->setText("Pause");
	}
}

OwlPlay::OwlPlay(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	demux_thread.Start();

	startTimer(40);
}

OwlPlay::~OwlPlay()
{
	demux_thread.Close();
}

void OwlPlay::Open()
{
	// 选择文件
	QString name = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择视频文件或填入拉流URL"));
	if (name.isEmpty())  return;
	this->setWindowTitle(name);
	if (!demux_thread.Open(name.toLocal8Bit(), ui.video)) {
		QMessageBox::information(0, "ERROR", "open failed!");
		return;
	}

	SetPause(demux_thread.is_pause_);
	return;
}

void OwlPlay::Pull()
{
	// 隐藏组件
	ui.open->setVisible(false);
	ui.is_play->setVisible(false);
	ui.play_pos->setVisible(false);

	qDebug() << g_pull_url.toLocal8Bit() << endl;
	if (!demux_thread.Open(g_pull_url.toLocal8Bit(), ui.video)) {
		QMessageBox::information(0, "ERROR", "open failed!");
		return;
	}

	SetPause(demux_thread.is_pause_);
	return;
}

void OwlPlay::PlayOrPause()
{
	bool is_pause = !demux_thread.is_pause_;
	SetPause(is_pause);
	demux_thread.SetPause(is_pause);
}

void OwlPlay::SliderPress()
{
	is_slider_press = true;
}
#include <iostream>
void OwlPlay::SliderRelease()
{
	is_slider_press = false;
	double pos = 0.0;
	pos = static_cast<double>(ui.play_pos->value()) / ui.play_pos->maximum();
	demux_thread.Seek(pos);
}