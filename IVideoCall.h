#pragma once
// 接口类，避免 OwlVideoWidget类 直接开放给 OwlVideoThread类
struct AVFrame;
class IVideoCall
{
public:
	// 初始化
	virtual void Init(int width, int height) = 0;
	// 重绘图像，不管成功与否都释放frame空间
	virtual void Repaint(AVFrame* frame) = 0;
};

