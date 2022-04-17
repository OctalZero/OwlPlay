#pragma once
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QGLShaderProgram>
class OwlVideoWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT

public:
	OwlVideoWidget(QWidget* parent);
	~OwlVideoWidget();

protected:
	//��ʼ��gl
	void InitializeGL();

	//ˢ����ʾ
	void PaintGL();

	// ���ڳߴ�仯
	void ResizeGL(int width, int height);


private:
	//shader����
	QGLShaderProgram program;

	//shader��yuv������ַ
	GLuint unis[3] = { 0 };
	//openg�� texture��ַ
	GLuint texs[3] = { 0 };

	//�����ڴ�ռ�
	unsigned char* datas[3] = { 0 };

	int width = 240;
	int height = 128;

};
