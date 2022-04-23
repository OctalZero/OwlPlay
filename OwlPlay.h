/*********************************************************************************
  *Date:  2022.04.23
  *Description:  �����������࣬�̳��� QWidget��
  *				 ��Ҫ����ⲥ������UI�¼�����Ӧ��
**********************************************************************************/
#pragma once
#include <QtWidgets/QWidget>
#include "ui_OwlPlay.h"

class OwlPlay : public QWidget
{
	Q_OBJECT

public:
	// ��ʱ�� ��������ʾ
	void timerEvent(QTimerEvent* e) override;

	// ���ڳߴ�仯
	void resizeEvent(QResizeEvent* e) override;

	// ˫��ȫ����ȫ����ʱ��Ῠ��
	void mouseDoubleClickEvent(QMouseEvent* e) override;

	// �л���ͣ�벥��
	void SetPause(bool is_pause);

	OwlPlay(QWidget* parent = Q_NULLPTR);
	~OwlPlay();
public slots:
	// ����Ƶ�ļ�
	void Open();

	// ��������URL
	void Pull();

	// �л����ź���ͣ
	void PlayOrPause();

	// ��ס������
	void SliderPress();

	// �ɿ�����������Ƶ���Ȼ�������Ӧλ��
	void SliderRelease();
private:
	Ui::OwlPlayClass ui;
	bool is_slider_press = false;  // ������Ĭ��û����ס
};
