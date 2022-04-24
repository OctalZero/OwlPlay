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
	bool re = true;  // 不在异常情况下直接return，防止para没有释放
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
	// decode_的Open用完后会释放para，放在最后面释放
	if (!decode_->Open(para)) {
		cout << "audio OwlDecode open failed!" << endl;
		re = false;
	}
	audio_mutex_.unlock();
	cout << "OwlAudioThread::Open：" << re << endl;

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
		// audio_paly_ 做了单例模式，是一个静态的成员，不需要清理
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
	// 重采样后音频输出数据，分配10MB存放
	unsigned char* pcm = new unsigned char[1024 * 1024 * 10];

	while (!is_exit_) {
		audio_mutex_.lock();

		// 处理暂停
		if (is_pause_) {
			audio_mutex_.unlock();
			msleep(5);
			continue;
		}

		//// 没有数据，或者decode_、resample_、audio_play_没有初始化好
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
		// 一次Send，多次Receive
		while (!is_exit_) {
			AVFrame* frame = decode_->Receive();
			if (!frame)  break;

			// 减去缓冲中未播放的时间 
			pts_ = decode_->pts_ - audio_play_->GetNoPlayMs();

			//cout << "audio pts = " << pts_ << endl;
			// 重采样
			int size = resample_->Resample(frame, pcm);
			// 开始播放音频
			while (!is_exit_) {
				if (size <= 0)  break;
				// 缓冲未播完，空间不够，暂停时音频  缓冲区也应该暂停
				if (audio_play_->GetFree() < size || is_pause_) {
					//cout << "音频缓冲满" << endl;
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
	// 确保变量都存在，避免再进行判断
	if (!resample_)  resample_ = new OwlResample();
	if (!audio_play_) audio_play_ = OwlAudioPlay::GetAudioPlay();
}

OwlAudioThread::~OwlAudioThread()
{
}
