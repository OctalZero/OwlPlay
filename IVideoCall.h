/*********************************************************************************
  *Date:  2022.04.23
  *Description:  ��Ƶ��ʾ�Ľӿ��࣬
				 ���� OwlVideoWidget�� ֱ�ӿ��Ÿ� OwlVideoThread��
**********************************************************************************/
#pragma once
struct AVFrame;

class IVideoCall
{
public:
	// ��ʼ��
	virtual void Init(int width, int height) = 0;
	// �ػ�ͼ�񣬲��ܳɹ�����ͷ�frame�ռ�
	virtual void Repaint(AVFrame* frame) = 0;
};

