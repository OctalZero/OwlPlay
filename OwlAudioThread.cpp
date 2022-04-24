#include "OwlAudioThread.h"
#include <iostream>
#include "OwlDecode.h"
#include "OwlAudioPlay.h"
#include "OwlResample.h"
using namespace std;

bool OwlAudioThread::Open(AVCodecParameters* para, int sample_rate, int channels)
{
	if (!para)  return false;
	Clear();

	audio_mutex_.lock();
	pts_ = 0;
	bool re = true;  // �����쳣�����ֱ��return����ֹparaû���ͷ�
	if (!resample_->Open(para, false)) {
		cout << "OwlResample open failed!" << endl;
		re = false;
	}

	audio_play_->sample_rate_ = sample_rate;
	audio_play_->channels_ = channels;

	if (!audio_play_->Open()) {
		re = false;
		cout << "OwlAudioPlay open failed!" << endl;
	}
	// decode_��Open�������ͷ�para������������ͷ�
	if (!decode_->Open(para)) {
		cout << "audio OwlDecode open failed!" << endl;
		re = false;
	}
	audio_mutex_.unlock();
	cout << "OwlAudioThread::Open��" << re << endl;

	return re;
}

void OwlAudioThread::Close()
{
	OwlDecodeThread::Close();
	if (resample_) {
		resample_->Close();
		audio_mutex_.lock();
		delete resample_;
		resample_ = nullptr;
		audio_mutex_.unlock();
	}
	if (audio_play_) {
		audio_play_->Close();
		audio_mutex_.lock();
		// audio_paly_ ���˵���ģʽ����һ����̬�ĳ�Ա������Ҫ����
		audio_play_ = nullptr;
		audio_mutex_.unlock();
	}
}

void OwlAudioThread::Clear()
{
	OwlDecodeThread::Clear();
	mutex_.lock();
	if (audio_play_)  audio_play_->Clear();
	mutex_.unlock();
}

void OwlAudioThread::SetPause(bool is_pause)
{
	is_pause_ = is_pause;
	if (audio_play_) {
		audio_play_->SetPause(is_pause);
	}
}


void OwlAudioThread::run()
{
	// �ز�������Ƶ������ݣ�����10MB���
	unsigned char* pcm = new unsigned char[1024 * 1024 * 10];

	while (!is_exit_) {
		audio_mutex_.lock();

		// ������ͣ
		if (is_pause_) {
			audio_mutex_.unlock();
			msleep(5);
			continue;
		}

		//// û�����ݣ�����decode_��resample_��audio_play_û�г�ʼ����
		//if (packets_.empty() || !decode_ || !resample_ || !audio_play_) {
		//	audio_mutex_.unlock();
		//	msleep(1);
		//	continue;
		//}

		//AVPacket* packet = packets_.front();
		//packets_.pop_front();

		AVPacket* packet = Pop();
		bool re = decode_->Send(packet);
		if (!re) {
			audio_mutex_.unlock();
			msleep(1);
			continue;
		}
		// һ��Send�����Receive
		while (!is_exit_) {
			AVFrame* frame = decode_->Receive();
			if (!frame)  break;

			// ��ȥ������δ���ŵ�ʱ�� 
			pts_ = decode_->pts_ - audio_play_->GetNoPlayMs();

			//cout << "audio pts = " << pts_ << endl;
			// �ز���
			int size = resample_->Resample(frame, pcm);
			// ��ʼ������Ƶ
			while (!is_exit_) {
				if (size <= 0)  break;
				// ����δ���꣬�ռ䲻������ͣʱ��Ƶ  ������ҲӦ����ͣ
				if (audio_play_->GetFree() < size || is_pause_) {
					//cout << "��Ƶ������" << endl;
					msleep(1);
					continue;
				}
				audio_play_->Write(pcm, size);
				break;
			}
		}
		audio_mutex_.unlock();
	}
	delete[] pcm;
}

OwlAudioThread::OwlAudioThread()
{
	// ȷ�����������ڣ������ٽ����ж�
	if (!resample_)  resample_ = new OwlResample();
	if (!audio_play_) audio_play_ = OwlAudioPlay::GetAudioPlay();
}

OwlAudioThread::~OwlAudioThread()
{
}
