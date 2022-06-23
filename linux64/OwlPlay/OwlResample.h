/*********************************************************************************
  *Date:  2022.04.23
  *Description:  ��Ƶ�ز����࣬
  *				 ��Ҫ������Ƶ���ز�����
**********************************************************************************/
#pragma once
#include <mutex>
struct AVCodecParameters;
struct SwrContext;
struct AVFrame;

class OwlResample
{
public:
	// ����������������һ�£����˲�����ʽ���ΪS16�����ͷ�para
	virtual bool Open(AVCodecParameters* para, bool is_clear_para = false);

	// �ر�	
	virtual void Close();

	// �����ز������С�����ܳɹ�����ͷ�in_data�ռ�
	virtual int Resample(AVFrame* in_data, unsigned char* out_data);

	OwlResample();
	~OwlResample();
public:
    int out_format_ = 0;  // ��Ӧ AVSampleFormat �е� AV_SAMPLE_FMT_S16���ɽ��и���
protected:
	std::mutex mutex_;
	SwrContext* resample_context_ = nullptr;
};
