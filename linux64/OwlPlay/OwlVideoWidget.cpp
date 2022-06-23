#include "OwlVideoWidget.h"
#include <QTimer>
extern "C" {
#include <libavutil/frame.h>
}
// �Զ���˫����
#define GET_STR(x) #x
#define A_VER 3
#define T_VER 4

FILE* fp = NULL;

// ����shader
const char* vString = GET_STR(
	attribute vec4 vertexIn;
attribute vec2 textureIn;
varying vec2 textureOut;
void main(void)
{
	gl_Position = vertexIn;
	textureOut = textureIn;
}
);


// ƬԪshader
const char* tString = GET_STR(
	varying vec2 textureOut;
uniform sampler2D tex_y;
uniform sampler2D tex_u;
uniform sampler2D tex_v;
void main(void)
{
	vec3 yuv;
	vec3 rgb;
	yuv.x = texture2D(tex_y, textureOut).r;
	yuv.y = texture2D(tex_u, textureOut).r - 0.5;
	yuv.z = texture2D(tex_v, textureOut).r - 0.5;
	rgb = mat3(1.0, 1.0, 1.0,
		0.0, -0.39465, 2.03211,
		1.13983, -0.58060, 0.0) * yuv;
	gl_FragColor = vec4(rgb, 1.0);
}
);

void OwlVideoWidget::Init(int width, int height)
{
	mutex_.lock();

	width_ = width;
	height_ = height;
	// �����ռ�
	delete datas[0];
	delete datas[1];
	delete datas[2];
	// ��������ڴ�ռ�
	datas[0] = new unsigned char[width * height];		//Y
	datas[1] = new unsigned char[width * height / 4];	//U
	datas[2] = new unsigned char[width * height / 4];	//V

	if (texs[0]) {
		glDeleteTextures(3, texs);
	}
	// ��������
	glGenTextures(3, texs);

	// Y
	glBindTexture(GL_TEXTURE_2D, texs[0]);
	// �Ŵ���ˣ����Բ�ֵ   GL_NEAREST(Ч�ʸߣ�������������)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// ���������Կ��ռ�
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

	// U
	glBindTexture(GL_TEXTURE_2D, texs[1]);
	// �Ŵ���ˣ����Բ�ֵ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// ���������Կ��ռ�
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width / 2, height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

	// V
	glBindTexture(GL_TEXTURE_2D, texs[2]);
	// �Ŵ���ˣ����Բ�ֵ
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// ���������Կ��ռ�
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width / 2, height / 2, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

	mutex_.unlock();
}

void OwlVideoWidget::Repaint(AVFrame* frame)
{
	if (!frame)  return;

	mutex_.lock();
	// �ݴ�����֤�ߴ���ȷ
	if (!datas[0] || width_ * height_ == 0 || frame->width != width_ || frame->height != height_) {
		av_frame_free(&frame);
		mutex_.unlock();
		return;
	}


	if (width_ == frame->linesize[0]) {  // �������
		memcpy(datas[0], frame->data[0], static_cast<int64_t>(width_) * height_);
		memcpy(datas[1], frame->data[1], static_cast<int64_t>(width_) * height_ / 4);
		memcpy(datas[2], frame->data[2], static_cast<int64_t>(width_) * height_ / 4);
	}
	else {  // �ж�������
		for (int i = 0; i < height_; ++i) {  // Y
			memcpy(datas[0] + width_ * i, frame->data[0] + frame->linesize[0] * i, width_);
		}
		for (int i = 0; i < height_ / 2; ++i) {  // U
			memcpy(datas[1] + width_ / 2 * i, frame->data[1] + frame->linesize[1] * i, width_);
		}
		for (int i = 0; i < height_ / 2; ++i) {  // V
			memcpy(datas[2] + width_ / 2 * i, frame->data[2] + frame->linesize[2] * i, width_);
		}
	}

	mutex_.unlock();
	av_frame_free(&frame);  // ������ռ��ʱ�ͷ�

	// ˢ����ʾ
	update();

}

OwlVideoWidget::OwlVideoWidget(QWidget* parent)
	: QOpenGLWidget(parent)
{
}

OwlVideoWidget::~OwlVideoWidget()
{
}

void OwlVideoWidget::initializeGL()
{
	qDebug() << "initializeGL";
	mutex_.lock();
	// ��ʼ��opengl ��QOpenGLFunctions�̳У����� 
	initializeOpenGLFunctions();

	// program����shader�������ƬԪ���ű�
	// ƬԪ�����أ�
	qDebug() << program.addShaderFromSourceCode(QGLShader::Fragment, tString);
	// ����shader
	qDebug() << program.addShaderFromSourceCode(QGLShader::Vertex, vString);

	// ���ö�������ı���
	program.bindAttributeLocation("vertexIn", A_VER);

	// ���ò�������
	program.bindAttributeLocation("textureIn", T_VER);

	// ����shader
	qDebug() << "program.link() = " << program.link();

	qDebug() << "program.bind() = " << program.bind();

	// ���ݶ���Ͳ�������
	// ����
	static const GLfloat ver[] = {
		-1.0f,-1.0f,
		1.0f,-1.0f,
		-1.0f, 1.0f,
		1.0f,1.0f
	};

	// ����
	static const GLfloat tex[] = {
		0.0f, 1.0f,
		1.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f
	};

	// ����
	glVertexAttribPointer(A_VER, 2, GL_FLOAT, 0, 0, ver);
	glEnableVertexAttribArray(A_VER);

	// ����
	glVertexAttribPointer(T_VER, 2, GL_FLOAT, 0, 0, tex);
	glEnableVertexAttribArray(T_VER);


	// ��shader��ȡ����
	unis[0] = program.uniformLocation("tex_y");
	unis[1] = program.uniformLocation("tex_u");
	unis[2] = program.uniformLocation("tex_v");

	mutex_.unlock();
}

void OwlVideoWidget::paintGL()
{
	/*if (feof(fp))
	{
		fseek(fp, 0, SEEK_SET);
	}
	fread(datas[0], 1, static_cast<int64_t>(width) * height, fp);
	fread(datas[1], 1, static_cast<int64_t>(width) * height / 4, fp);
	fread(datas[2], 1, static_cast<int64_t>(width) * height / 4, fp);*/

	mutex_.lock();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texs[0]); // 0��󶨵�Y����
	// �޸Ĳ�������(�����ڴ�����)
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width_, height_, GL_RED, GL_UNSIGNED_BYTE, datas[0]);
	// ��shader uni��������
	glUniform1i(unis[0], 0);


	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, texs[1]); // 1��󶨵�U����
										   // �޸Ĳ�������(�����ڴ�����)
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width_ / 2, height_ / 2, GL_RED, GL_UNSIGNED_BYTE, datas[1]);
	// ��shader uni��������
	glUniform1i(unis[1], 1);


	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, texs[2]); // 2��󶨵�V����
										   // �޸Ĳ�������(�����ڴ�����)
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width_ / 2, height_ / 2, GL_RED, GL_UNSIGNED_BYTE, datas[2]);
	// ��shader uni��������
	glUniform1i(unis[2], 2);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	mutex_.unlock();
}


// ���ڳߴ�仯
void OwlVideoWidget::resizeGL(int width, int height)
{
	mutex_.lock();
	qDebug() << "resizeGL " << width << ":" << height;
	mutex_.unlock();
}