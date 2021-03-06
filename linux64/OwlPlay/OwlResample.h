/*********************************************************************************
  *Date:  2022.04.23
  *Description:  音频重采样类，
  *				 主要处理音频的重采样。
**********************************************************************************/
#pragma once
#include <mutex>
struct AVCodecParameters;
struct SwrContext;
struct AVFrame;

class OwlResample
{
public:
	// 输出参数和输入参数一致，除了采样格式输出为S16，会释放para
	virtual bool Open(AVCodecParameters* para, bool is_clear_para = false);

	// 关闭	
	virtual void Close();

	// 返回重采样后大小，不管成功与否都释放in_data空间
	virtual int Resample(AVFrame* in_data, unsigned char* out_data);

	OwlResample();
	~OwlResample();
public:
    int out_format_ = 0;  // 对应 AVSampleFormat 中的 AV_SAMPLE_FMT_S16，可进行更换
protected:
	std::mutex mutex_;
	SwrContext* resample_context_ = nullptr;
};

