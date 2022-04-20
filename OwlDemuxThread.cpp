#include "OwlDemuxThread.h"
#include <iostream>
#include "OwlDemux.h"
#include "OwlVideoThread.h"
#include "OwlAudioThread.h"
using namespace std;

bool OwlDemuxThread::Open(const char* url, IVideoCall* video_call)
{
	// �ݴ�
	if (url == nullptr || url[0] == '0')  return false;
	mutex_.lock();
	if (!demux_)  demux_ = new OwlDemux();
	if (!video_thread_)  video_thread_ = new OwlVideoThread();
	if (!audio_thread_)  audio_thread_ = new OwlAudioThread();

	//�򿪽��װ
	bool re = demux_->Open(url);
	if (!re) {
		mutex_.unlock();
		cout << "demux_->Open(url) failed!" << endl;
		return false;
	}
	// ����Ƶ�������ʹ����߳�
	if (!video_thread_->Open(demux_->CopyVideoPara(),
		video_call, demux_->width_, demux_->height_)) {
		re = false;
		cout << "video_thread_->Open����failed��" << endl;
	}

	// ����Ƶ�������ʹ����߳�
	if (!audio_thread_->Open(demux_->CopyAudioPara(),
		demux_->sample_rate_, demux_->channels_)) {
		re = false;
		cout << "audio_thread_->Open����failed��" << endl;
	}
	mutex_.unlock();
	cout << "OwlDemuxThread::Open��" << re << endl;
	return re;
}

void OwlDemuxThread::Start()
{
	mutex_.lock();
	if (!demux_)  demux_ = new OwlDemux();
	if (!video_thread_)  video_thread_ = new OwlVideoThread();
	if (!audio_thread_)  audio_thread_ = new OwlAudioThread();
	// ������ǰ�߳� 
	QThread::start();
	if (video_thread_)  video_thread_->start();
	if (audio_thread_)	audio_thread_->start();
	mutex_.unlock();
}

void OwlDemuxThread::Close()
{
	is_exit_ = true;
	wait();
	if (video_thread_)  video_thread_->Close();
	if (audio_thread_)  audio_thread_->Close();
	mutex_.lock();
	delete video_thread_;
	delete audio_thread_;
	video_thread_ = nullptr;
	audio_thread_ = nullptr;
	mutex_.unlock();
}

void OwlDemuxThread::run()
{
	while (!is_exit_) {
		mutex_.lock();
		if (!demux_) {
			mutex_.unlock();
			msleep(5);
			continue;
		}

		// ����Ƶͬ����û�п���ֻ����Ƶ��ֻ����Ƶ�����
		if (audio_thread_ && video_thread_) {
			video_thread_->syn_pts_ = audio_thread_->pts_;
		}

		AVPacket* pkt = demux_->Read();
		if (!pkt) {
			mutex_.unlock();
			msleep(5);
			continue;
		}
		// �ж���������Ƶ
		if (demux_->isAudio(pkt)) {
			if (audio_thread_) {
				audio_thread_->Push(pkt);
			}
		}
		else {  // ��Ƶ
			if (video_thread_) {
				video_thread_->Push(pkt);
			}
		}
		mutex_.unlock();
		msleep(1);  // ��ֹ��ȡ̫�죬����Ƶͬ����������
	}
}

OwlDemuxThread::OwlDemuxThread()
{
}

OwlDemuxThread::~OwlDemuxThread()
{
	// ��ֹ�߳�崵�
	is_exit_ = true;
	wait();
}
