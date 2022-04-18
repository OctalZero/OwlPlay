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
	// 初始化
	void Init(int width, int height);
	// 重绘图像，不管成功与否都释放frame空间
	virtual void Repaint(AVFrame* frame);

	OwlVideoWidget(QWidget* parent);
	~OwlVideoWidget();
protected:
	// 初始化OpenGL
	void initializeGL() override;

	// 刷新显示
	void paintGL() override;

	// 窗口尺寸变化
	void resizeGL(int width, int height) override;
private:
	std::mutex mutex_;

	//shader程序
	QGLShaderProgram program;

	//shader中yuv变量地址
	GLuint unis[3] = { 0 };

	//OpenGL的 texture地址
	GLuint texs[3] = { 0 };

	//材质内存空间
	unsigned char* datas[3] = { 0 };

	int width_ = 0;
	int height_ = 0;

};
