#include "OwlSlider.h"

void OwlSlider::mousePressEvent(QMouseEvent* e)
{
	QSlider::mousePressEvent(e);
	double pos = static_cast<double>(e->pos().x()) / width();
	setValue(pos * this->maximum());
	// ���غ󣬱�֤ԭ���¼�Ҳ�õ�����
	//QSlider::mousePressEvent(e);  // �������⣺�����϶�����βλ�ã��϶������ʱ�ɿ��¼�û����Ӧ
	QSlider::sliderReleased();
}

OwlSlider::OwlSlider(QWidget* parent)
	: QSlider(parent)
{
}

OwlSlider::~OwlSlider()
{
}
