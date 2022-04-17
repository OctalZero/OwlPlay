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
	//初始化gl
	void InitializeGL();

	//刷新显示
	void PaintGL();

	// 窗口尺寸变化
	void ResizeGL(int width, int height);


private:
	//shader程序
	QGLShaderProgram program;

	//shader中yuv变量地址
	GLuint unis[3] = { 0 };
	//openg的 texture地址
	GLuint texs[3] = { 0 };

	//材质内存空间
	unsigned char* datas[3] = { 0 };

	int width = 240;
	int height = 128;

};
