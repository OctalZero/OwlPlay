#include "OwlPlay.h"
#include <QtWidgets/QApplication>
#include <iostream>
#include <QThread>
using namespace std;
#include "OwlDemux.h"
#include "OwlDecode.h"
#include "OwlVideoWidget.h"
#include "OwlResample.h"
#include "OwlAudioPlay.h"
#include "OwlAudioThread.h"

class TestThread : public QThread
{
public:
	void Init() {
		// ����Demux
		const char* url = "v1080.mp4";
		cout << "demux.Open " << endl;
		// ����������͹ر�
		demux.Open(url);
		demux.Read();
		demux.Clear();
		demux.Close();
		// ���´�
		demux.Open(url);
		cout << "CopyVideoPara = " << demux.CopyVideoPara() << endl;
		cout << "CopyAudioPara = " << demux.CopyAudioPara() << endl;
		//cout << "seek = " << demux.Seek(0.95) << endl;
		//////////////////////////
		// �������
		cout << "video_decode.Open() = " << video_decode.Open(demux.CopyVideoPara()) << endl;
		//video_decode.Clear();
		//video_decode.Close();
		//cout << "audio_decode.Open() = " << audio_decode.Open(demux.CopyAudioPara()) << endl;
		//cout << "resample.Open() = " << resample.Open(demux.CopyAudioPara()) << endl;
		// ������Ƶ
		//OwlAudioPlay::GetAudioPlay()->sample_rate_ = demux.sample_rate_;
		//OwlAudioPlay::GetAudioPlay()->channels_ = demux.channels_;
		//cout << "OwlAudioPlay::GetAudioPlay()->Open() = " <<
		//	OwlAudioPlay::GetAudioPlay()->Open() << endl;
		cout << "audio_thread.Open����="
			<< audio_thread.Open(demux.CopyAudioPara(), demux.sample_rate_, demux.channels_);
		audio_thread.start();
	}
	// �������
	unsigned char* pcm = new unsigned char[1024 * 1024];
	void run() {

		for (;;) {
			AVPacket* pkt = demux.Read();
			if (demux.isAudio(pkt)) {
				audio_thread.Push(pkt);
				/*audio_decode.Send(pkt);
				AVFrame* frame = audio_decode.Receive();
				int len = resample.Resample(frame, pcm);
				cout << "Resample:" << len << " ";
				while (len > 0) {
					if (OwlAudioPlay::GetAudioPlay()->GetFree() >= len) {
						OwlAudioPlay::GetAudioPlay()->Write(pcm, len);
						break;
					}
					msleep(1);
				}*/

				//cout << "Audio:" << frame << endl;
			}
			else {
				video_decode.Send(pkt);
				AVFrame* frame = video_decode.Receive();
				video->Repaint(frame);
				//msleep(40);
				//cout << "Video:" << frame << endl;
			}
			if (!pkt) break;
		}
	}

public:
	OwlAudioThread audio_thread;
	OwlDemux demux;
	OwlDecode video_decode;
	OwlDecode audio_decode;
	OwlResample resample;
	OwlVideoWidget* video = nullptr;
};
int main(int argc, char* argv[])
{

	TestThread tt;
	tt.Init();

	QApplication a(argc, argv);
	OwlPlay w;
	w.show();

	// ��ʼ��GL����
	w.ui.video->Init(tt.demux.width_, tt.demux.height_);
	tt.video = w.ui.video;
	tt.start();

	return a.exec();
}
