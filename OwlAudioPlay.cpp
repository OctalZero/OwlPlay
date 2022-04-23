#include "OwlAudioPlay.h"
#include <QAudioFormat>
#include <QAudioOutput>
#include <mutex>
// ����ģʽ
class QtAudioPlay : public OwlAudioPlay
{
public:
	// ����Ƶ
	virtual bool Open() override {
		Close();
		QAudioFormat fmt;
		fmt.setSampleRate(sample_rate_);
		fmt.setSampleSize(sample_size_);
		fmt.setChannelCount(channels_);
		fmt.setCodec("audio/pcm");
		fmt.setByteOrder(QAudioFormat::LittleEndian);
		fmt.setSampleType(QAudioFormat::UnSignedInt);
		mutex_.lock();
		output_ = new QAudioOutput(fmt);
		io_ = output_->start(); //��ʼ����
		mutex_.unlock();

		if (io_) {
			return true;
		}
		return false;
	}

	// �ر���Ƶ
	virtual void Close() override {
		mutex_.lock();
		if (io_) {
			io_->close();  // �ռ���output_������Ӧ��output_�ͷ�
			io_ = nullptr;
		}
		if (output_) {
			output_->stop();
			delete output_;
			output_ = nullptr;
		}
		mutex_.unlock();
	}

	// ������Ƶ����
	virtual void Clear() override {
		mutex_.lock();
		if (io_) {
			io_->reset();
		}
		mutex_.unlock();
	}

	// ���ػ����л�û�в��ŵ�ʱ�䣨����ms��
	virtual long long GetNoPlayMs() {
		mutex_.lock();
		if (!output_) {
			mutex_.unlock();
			return 0;
		}

		long long pts = 0;
		// ��δ���ŵ��ֽ���
		double size = output_->bufferSize() - output_->bytesFree();
		// һ����Ƶ�ֽڴ�С
		double sec_size = sample_rate_ * (sample_size_ / static_cast<double>(8)) * channels_;
		if (sec_size <= 0) {
			pts = 0;
		}
		else {
			pts = (size / sec_size) * 1000;  // ���㻺���л�û�в��ŵ�ʱ��
		}
		mutex_.unlock();

		return pts;
	}


	// ������Ƶ
	virtual bool Write(const unsigned char* data_, int datasize_) override {
		if (!data_ || datasize_ <= 0)  return false;
		mutex_.lock();
		if (!output_ || !io_) {
			mutex_.unlock();
			return false;
		}
		// ����ʵ��д��Ĵ�С
		int size = io_->write((char*)data_, datasize_);
		mutex_.unlock();
		if (size != datasize_) {
			return false;
		}

		return true;
	}

	// �ж���Ƶ�������Ƿ����㹻�ռ���д
	virtual int GetFree() override {
		mutex_.lock();
		if (!output_) {
			mutex_.unlock();
			return 0;
		}
		int free = output_->bytesFree();
		mutex_.unlock();

		return free;
	}

	void SetPause(bool is_pause) override {
		mutex_.lock();
		if (!output_) {
			mutex_.unlock();
			return;
		}
		if (is_pause) {
			output_->suspend();  // ������Ƶ������� 
		}
		else {
			output_->resume();
		}
		mutex_.unlock();
	}

public:
	QAudioOutput* output_ = nullptr;  // ��Ƶ�������
	QIODevice* io_ = nullptr;  // ��Ƶ����豸
	std::mutex mutex_;
};

OwlAudioPlay* OwlAudioPlay::GetAudioPlay()
{
	static QtAudioPlay play;  // ��̬��������η���ֻ�ᴴ��һ��

	return &play;
}

OwlAudioPlay::OwlAudioPlay()
{
}

OwlAudioPlay::~OwlAudioPlay()
{
}
