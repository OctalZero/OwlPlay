#pragma once

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QGLShaderProgram>
#include <mutex>
struct AVFrame;
class OwlVideoWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT

public:
	// ��ʼ��
	void Init(int width, int height);
	// �ػ�ͼ�񣬲��ܳɹ�����ͷ�frame�ռ�
	virtual void Repaint(AVFrame* frame);

	OwlVideoWidget(QWidget* parent);
	~OwlVideoWidget();
protected:
	// ��ʼ��OpenGL
	void initializeGL() override;

	// ˢ����ʾ
	void paintGL() override;

	// ���ڳߴ�仯
	void resizeGL(int width, int height) override;
private:
	std::mutex mutex_;

	//shader����
	QGLShaderProgram program;

	//shader��yuv������ַ
	GLuint unis[3] = { 0 };

	//OpenGL�� texture��ַ
	GLuint texs[3] = { 0 };

	//�����ڴ�ռ�
	unsigned char* datas[3] = { 0 };

	int width_ = 0;
	int height_ = 0;

};
