#include "OwlAudioThread.h"
#include <iostream>
#include "OwlDecode.h"
#include "OwlAudioPlay.h"
#include "OwlResample.h"
using namespace std;

bool OwlAudioThread::Open(AVCodecParameters* para, int sample_rate, int channels)
{
	if (!para)  return false;
	mutex_.lock();
	if (!decode_)  decode_ = new OwlDecode();
	if (!resample_)  resample_ = new OwlResample();
	if (!audio_play_) audio_play_ = OwlAudioPlay::GetAudioPlay();

	bool re = true;  // �����쳣�����ֱ�ӷ��أ���ֹparaû���ͷ�
	if (!resample_->Open(para, false)) {
		cout << "OwlResample open failed!" << endl;
		re = false;
		//mutex_.unlock();
		//return false;
	}

	audio_play_->sample_rate_ = sample_rate;
	audio_play_->channels_ = channels;

	if (!audio_play_->Open()) {
		re = false;
		cout << "OwlAudioPlay open failed!" << endl;
		//mutex_.unlock();
		//return false;
	}
	// decode_��Open�������ͷ�para������������ͷ�
	if (!decode_->Open(para)) {
		cout << "audio OwlDecode open failed!" << endl;
		re = false;
	}
	mutex_.unlock();
	cout << "OwlAudioThread::Open��" << re << endl;

	return re;
}

void OwlAudioThread::Push(AVPacket* pkt)
{
	if (!pkt)  return;

	// ���������ݲ��ܶ�
	while (!is_exit_) {
		mutex_.lock();
		if (packets_.size() < maxList_) {
			packets_.push_back(pkt);
			mutex_.unlock();
			break;
		}
		mutex_.unlock();
		msleep(1);
	}

}

void OwlAudioThread::run()
{
	// �ز�������Ƶ������ݣ�����10M���
	unsigned char* pcm = new unsigned char[1024 * 1024 * 10];

	while (!is_exit_) {
		mutex_.lock();
		// û�����ݣ�����decode_��resample_��audio_play_û�г�ʼ����
		if (packets_.empty() || !decode_ || !resample_ || !audio_play_) {
			mutex_.unlock();
			msleep(1);
			continue;
		}

		AVPacket* packet = packets_.front();
		packets_.pop_front();

		bool re = decode_->Send(packet);
		if (!re) {
			mutex_.unlock();
			msleep(1);
			continue;
		}
		// һ��Send�����Receive
		while (!is_exit_) {
			AVFrame* frame = decode_->Receive();
			if (!frame)  break;
			// �ز���
			int size = resample_->Resample(frame, pcm);
			// ��ʼ������Ƶ
			while (!is_exit_) {
				if (size <= 0)  break;
				// ����δ���꣬�ռ䲻��
				if (audio_play_->GetFree() < size) {
					msleep(1);
					continue;
				}
				audio_play_->Write(pcm, size);
				break;
			}
		}
		mutex_.unlock();
	}
	delete[] pcm;
}

OwlAudioThread::OwlAudioThread()
{
}

OwlAudioThread::~OwlAudioThread()
{
	// �ȴ��߳��˳�����Ȼ�̻߳������У��������ռ��Ѿ��ͷ���
	is_exit_ = true;
	wait();
}
