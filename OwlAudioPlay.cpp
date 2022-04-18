#include "OwlAudioPlay.h"
#include <QAudioFormat>
#include <QAudioOutput>
#include <mutex>
class COwlAudioPlay : public OwlAudioPlay
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
	virtual int GetFree() {
		mutex_.lock();
		if (!output_) {
			mutex_.unlock();
			return 0;
		}
		int free = output_->bytesFree();
		mutex_.unlock();

		return free;
	}
public:
	QAudioOutput* output_ = nullptr;  // ��Ƶ�������
	QIODevice* io_ = nullptr;  // ��Ƶ����豸
	std::mutex mutex_;
};

OwlAudioPlay* OwlAudioPlay::GetAudioPlay()
{
	static COwlAudioPlay play;  // ��̬��������η���ֻ�ᴴ��һ��

	return &play;
}

OwlAudioPlay::OwlAudioPlay()
{
}

OwlAudioPlay::~OwlAudioPlay()
{
}
