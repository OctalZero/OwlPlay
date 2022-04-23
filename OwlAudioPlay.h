/*********************************************************************************
  *Date:  2022.04.23
  *Description:  ������Ƶ�Ľӿ��࣬
  *				 ʹ�ù���ģʽ���ɾ��廻�ɲ�ͬ����Ƶ�ӿڣ�����ʹ�õ���Qt��Audio�ӿڡ�
**********************************************************************************/
#pragma once

class OwlAudioPlay
{
public:
	// ����Ƶ���ţ�����Ϊ���麯������������ʵ��
	virtual bool Open() = 0;

	// �ر���Ƶ
	virtual void Close() = 0;

	// ������Ƶ����
	virtual void Clear() = 0;

	// ���ػ����л�û�в��ŵ�ʱ�䣨����ms�� 
	virtual long long GetNoPlayMs() = 0;

	// ������Ƶ
	virtual bool Write(const unsigned char* data_, int datasize_) = 0;

	// �ж���Ƶ�������Ƿ����㹻�ռ���д
	virtual int GetFree() = 0;

	// ������ͣ
	virtual void SetPause(bool is_pause) = 0;

	// ��������ȡ�������Ƶ���������ɻ��ɲ�ͬ�Ľӿ�
	static OwlAudioPlay* GetAudioPlay();

	OwlAudioPlay();
	virtual ~OwlAudioPlay();

public:
	int sample_rate_ = 44100;
	int sample_size_ = 16;  // λ���������ֽ������ǹ̶��ģ��ز�����ͳһΪ16
	int channels_ = 2;
};

