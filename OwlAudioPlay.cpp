#include "OwlAudioPlay.h"
#include <QAudioFormat>
#include <QAudioOutput>
#include <mutex>
// 单例模式
class COwlAudioPlay : public OwlAudioPlay
{
public:
	// 打开音频
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
		io_ = output_->start(); //开始播放
		mutex_.unlock();

		if (io_) {
			return true;
		}
		return false;
	}

	// 关闭音频
	virtual void Close() override {
		mutex_.lock();
		if (io_) {
			io_->close();  // 空间由output_产生，应由output_释放
			io_ = nullptr;
		}
		if (output_) {
			output_->stop();
			delete output_;
			output_ = nullptr;
		}
		mutex_.unlock();
	}

	// 返回缓冲中还没有播放的时间（毫秒ms）
	virtual long long GetNoPlayMs() {
		mutex_.lock();
		if (!output_) {
			mutex_.unlock();
			return 0;
		}

		long long pts = 0;
		// 还未播放的字节数
		double size = output_->bufferSize() - output_->bytesFree();
		// 一秒音频字节大小
		double sec_size = sample_rate_ * (sample_size_ / static_cast<double>(8)) * channels_;
		if (sec_size <= 0) {
			pts = 0;
		}
		else {
			pts = (size / sec_size) * 1000;  // 计算缓冲中还没有播放的时间
		}
		mutex_.unlock();

		return pts;
	}

	// 播放音频
	virtual bool Write(const unsigned char* data_, int datasize_) override {
		if (!data_ || datasize_ <= 0)  return false;
		mutex_.lock();
		if (!output_ || !io_) {
			mutex_.unlock();
			return false;
		}
		// 返回实际写入的大小
		int size = io_->write((char*)data_, datasize_);
		mutex_.unlock();
		if (size != datasize_) {
			return false;
		}

		return true;
	}

	// 判断音频缓冲区是否有足够空间来写
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
public:
	QAudioOutput* output_ = nullptr;  // 音频输出数据
	QIODevice* io_ = nullptr;  // 音频输出设备
	std::mutex mutex_;
};

OwlAudioPlay* OwlAudioPlay::GetAudioPlay()
{
	static COwlAudioPlay play;  // 静态变量，多次访问只会创建一次

	return &play;
}

OwlAudioPlay::OwlAudioPlay()
{
}

OwlAudioPlay::~OwlAudioPlay()
{
}
