/*********************************************************************************
  *Date:  2022.04.23
  *Description:  �������࣬�̳��� QSlider��
  *				 ��Ҫ������ QSlider ���һЩ��Ӧ������
**********************************************************************************/
#pragma once
#include <QObject>
#include <QMouseEvent>
#include <QSlider>
class OwlSlider : public QSlider
{
	Q_OBJECT

public:
	void mousePressEvent(QMouseEvent* e) override;
	OwlSlider(QWidget* parent = nullptr);  // parent��Ϊnullptr������ֱ��newʱʧ��
	~OwlSlider();
};
