#include "OwlResample.h"
#include <iostream>
using namespace std;
extern "C" {
#include <libswresample/swresample.h>
#include <libavcodec/avcodec.h>
}
#pragma comment(lib, "swresample.lib")

bool OwlResample::Open(AVCodecParameters* para, bool is_clear_para)
{
	if (!para)  return false;
	mutex_.lock();

	//��Ƶ�ز��� ���� SwrContext �ṹ��
	if (!resample_context_) {
		resample_context_ = swr_alloc();
	}
	// ���resampleΪNULL���Զ�����ռ�
	resample_context_ = swr_alloc_set_opts(resample_context_,
		av_get_default_channel_layout(out_channel_),	//�����ʽ
		(AVSampleFormat)out_format_,		//���������ʽ 1 AV_SAMPLE_FMT_S16
		out_sample_rate_,					//���������
		av_get_default_channel_layout(para->channels),//�����ʽ
		(AVSampleFormat)para->format,
		para->sample_rate,
		0, 0
	);
	// �����ͷŲ���ָ��,Ĭ�ϲ��ͷţ���ֹ����ͷ�
	if (is_clear_para) {
		avcodec_parameters_free(&para);
	}

	// ��ʼ�� SwrContext
	int re = swr_init(resample_context_);
	mutex_.unlock();

	if (re != 0)
	{
		char buf[1024] = { 0 };
		av_strerror(re, buf, sizeof(buf) - 1);
		cout << "swr_init  failed! :" << buf << endl;
		return false;
	}
	//unsigned char* pcm = nullptr;

	return true;
}


void OwlResample::Close()
{
	mutex_.lock();
	if (resample_context_) {
		swr_free(&resample_context_);
	}
	mutex_.unlock();
}

int OwlResample::Resample(AVFrame* in_data, unsigned char* out_data)
{
	if (!in_data)  return 0;
	if (!out_data) {
		av_frame_free(&in_data);
		return 0;
	}

	uint8_t* data[2] = { 0 };
	data[0] = out_data;
	// ��ʼ�ز���ת������ȡÿ��ͨ�������������
	int re = swr_convert(resample_context_,
		data, in_data->nb_samples,		//���
		(const uint8_t**)in_data->data, in_data->nb_samples	//����
	);
	if (re <= 0)  return re;
	out_sample_size_ = av_get_bytes_per_sample((AVSampleFormat)out_format_);
	int out_size = re * out_channel_ * out_sample_size_;
	av_frame_free(&in_data);

	return out_size;
}

OwlResample::OwlResample()
{
}

OwlResample::~OwlResample()
{
}
