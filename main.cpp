#include "OwlPlay.h"
#include <QtWidgets/QApplication>
#include <iostream>
using namespace std;
#include "OwlDemux.h"
#include "OwlDecode.h"

int main(int argc, char* argv[])
{
	// ≤‚ ‘Demux
	OwlDemux demux;

	const char* url = "v1080.mp4";
	cout << "demux.Open " << endl;
	demux.Open(url);
	demux.Read();
	//demux.Clear();
	//demux.Close();
	cout << "CopyVideoPara = " << demux.CopyVideoPara() << endl;
	cout << "CopyAudioPara = " << demux.CopyAudioPara() << endl;
	cout << "Seek = " << demux.Seek(0.95) << endl;
	//////////////////////////
	// Ω‚¬Î≤‚ ‘
	OwlDecode video_decode;
	cout << "video_decode.Open() = " << video_decode.Open(demux.CopyVideoPara()) << endl;
	//video_decode.Clear();
	//video_decode.Close();
	OwlDecode audio_decode;
	cout << "audio_decode.Open() = " << audio_decode.Open(demux.CopyAudioPara()) << endl;

	for (;;) {
		AVPacket* pkt = demux.Read();
		if (demux.isAudio(pkt)) {
			audio_decode.Send(pkt);
			AVFrame* frame = audio_decode.Receive();
			//cout << "Audio:" << frame << endl;
		}
		else {
			video_decode.Send(pkt);
			AVFrame* frame = video_decode.Receive();
			//cout << "Video:" << frame << endl;
		}
		if (!pkt) break;
	}

	QApplication a(argc, argv);
	OwlPlay w;
	w.show();
	return a.exec();
}
