#include "OwlDemuxThread.h"
#include <iostream>
#include "OwlDemux.h"
#include "OwlVideoThread.h"
#include "OwlAudioThread.h"
#include "OwlDecode.h"
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
	total_ms_ = demux_->total_ms_;
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

void OwlDemuxThread::Clear()
{
	mutex_.lock();
	if (demux_)  demux_->Clear();
	if (video_thread_)  video_thread_->Clear();
	if (audio_thread_)  audio_thread_->Clear();
	mutex_.unlock();
}

void OwlDemuxThread::SetPause(bool is_pause)
{
	mutex_.lock();
	is_pause_ = is_pause;
	if (audio_thread_)  audio_thread_->SetPause(is_pause);
	if (video_thread_)  video_thread_->SetPause(is_pause);
	mutex_.unlock();
}

void OwlDemuxThread::Seek(double pos)
{

	mutex_.lock();
	bool status = is_pause_;
	mutex_.unlock();

	// ��ͣ
	SetPause(true);

	// ������
	Clear();

	mutex_.lock();
	if (demux_) {
		demux_->Seek(pos);

		// ʵ��Ҫ��ʾ��λ��pts
		long long seek_pts = pos * demux_->total_ms_;
		// ��ͣ״̬��Ҳ��ˢ�µ�seek����λ�õ�ͼ��
		while (!is_exit_) {
			AVPacket* pkt = demux_->ReadVideo();
			if (!pkt)  break;
			// ������뵽seek_pts
			if (video_thread_->ReaintPts(pkt, seek_pts)) {
				pts_ = seek_pts;
				break;
			}
		}
	}
	mutex_.unlock();

	// Seekʱ���ڷ���ͣ״̬
	if (!status) {
		SetPause(false);
	}
}

void OwlDemuxThread::run()
{
	while (!is_exit_) {
		mutex_.lock();
		//cout << "demux running!" << endl;
		// ������ͣ
		if (is_pause_) {
			mutex_.unlock();
			msleep(5);
			continue;
		}
		if (!demux_) {
			mutex_.unlock();
			msleep(5);
			continue;
			//cout << "û��demux" << endl;
		}

		// ����Ƶͬ����û�п���ֻ����Ƶ��ֻ����Ƶ�����
		if (audio_thread_ && video_thread_) {
			pts_ = audio_thread_->pts_;
			video_thread_->syn_pts_ = audio_thread_->pts_;
		}

		AVPacket* pkt = demux_->Read();
		if (!pkt) {
			//cout << "û��pkt" << endl;
			mutex_.unlock();
			msleep(5);
			continue;
		}
		// �ж���������Ƶ
		if (demux_->isAudio(pkt)) {
			if (audio_thread_) {
				//cout << "push audio��" << endl;
				audio_thread_->Push(pkt);
			}
		}
		else {  // ��Ƶ
			if (video_thread_) {
				//cout << "push video��" << endl;
				video_thread_->Push(pkt);  // ������Push�п����������
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
