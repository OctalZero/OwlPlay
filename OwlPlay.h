/*********************************************************************************
  *Date:  2022.04.23
  *Description:  播放器界面类，继承于 QWidget，
  *				 主要处理解播放器中UI事件的响应。
**********************************************************************************/
#pragma once
#include <QtWidgets/QWidget>
#include "ui_OwlPlay.h"

class OwlPlay : public QWidget
{
	Q_OBJECT

public:
	// 定时器 滑动条显示
	void timerEvent(QTimerEvent* e) override;

	// 窗口尺寸变化
	void resizeEvent(QResizeEvent* e) override;

	// 双击全屏，全屏的时候会卡顿
	void mouseDoubleClickEvent(QMouseEvent* e) override;

	// 切换暂停与播放
	void SetPause(bool is_pause);

	OwlPlay(QWidget* parent = Q_NULLPTR);
	~OwlPlay();
public slots:
	// 打开视频文件
	void Open();

	// 填入拉流URL
	void Pull();

	// 切换播放和暂停
	void PlayOrPause();

	// 按住滑动条
	void SliderPress();

	// 松开滑动条，视频进度滑动到相应位置
	void SliderRelease();
private:
	Ui::OwlPlayClass ui;
	bool is_slider_press = false;  // 滑动条默认没被按住
};
