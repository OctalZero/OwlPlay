#pragma once

#include <QtWidgets/QWidget>
#include "ui_OwlPlay.h"

// 播放器类
class OwlPlay : public QWidget
{
	Q_OBJECT

public:
	OwlPlay(QWidget* parent = Q_NULLPTR);
public slots:
	// 打开文件或拉流URL
	void Open();

private:
	Ui::OwlPlayClass ui;
};
