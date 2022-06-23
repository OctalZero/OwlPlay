/*********************************************************************************
  *Date:  2022.04.23
  *Description:  ������߳��࣬�̳��� QThread �࣬
  *				 ��Ҫ�����������Ҫ���߼����൱�ڽ����������
**********************************************************************************/
#pragma once
#include <list>
#include <mutex>
#include <QThread>
struct AVPacket;
class OwlDecode;

class OwlDecodeThread : public QThread
{
public:
	// ����packet���������
	virtual void Push(AVPacket* pkt);

	// �������
	virtual void Clear();

	// ������Դ��ֹͣ�߳�
	virtual void Close();

	// ȡ��һ֡���ݣ�����ջ�����û�з��� nullptr 
	virtual AVPacket* Pop();

	OwlDecodeThread();
	virtual ~OwlDecodeThread();
public:
	int max_list_ = 100;  // �����У�����Լ2s
	bool is_exit_ = false;  // �ж��߳��Ƿ��˳�

protected:
	std::mutex mutex_;
	// ������������ģʽ���ɵ���������packet�ӵ������������У������߳�������packet��
	std::list<AVPacket*> packets_;
	OwlDecode* decode_ = nullptr;  // ������
};

