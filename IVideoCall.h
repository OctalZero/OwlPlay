#pragma once
// �ӿ��࣬���� OwlVideoWidget�� ֱ�ӿ��Ÿ� OwlVideoThread��
struct AVFrame;
class IVideoCall
{
public:
	// ��ʼ��
	virtual void Init(int width, int height) = 0;
	// �ػ�ͼ�񣬲��ܳɹ�����ͷ�frame�ռ�
	virtual void Repaint(AVFrame* frame) = 0;
};

