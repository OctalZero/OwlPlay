#pragma once
#include <mutex>
struct AVCodecParameters;
struct AVCodecContext;
struct AVFrame;
struct AVPacket;
class OwlDecode
{
public:
	// 打开解码器,不管成功与否都释放para空间
	virtual bool Open(AVCodecParameters* para);

	// 发送到解码线程，不管成功与否都释放pkt空间（对象和媒体内容）
	virtual bool Send(AVPacket* pkt);

	// 获取解码数据，一次send可能需要多次Receive，最后获取缓冲中的数据Send NULL再Receive多次
	// 每次复制一份，由调用者释放 av_frame_free
	virtual AVFrame* Receive();



	// 关闭
	virtual void Close();
	// 清理缓存
	virtual void Clear();

	OwlDecode();
	~OwlDecode();
protected:
	AVCodecContext* codec_context_ = 0;
	std::mutex mutex_;
};

