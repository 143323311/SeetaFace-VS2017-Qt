#pragma once

#include <QtWidgets/QWidget>
#include "ui_OpenVideo.h"
#include <opencv2/opencv.hpp>
#include "MyThread.h" //�Զ����߳�ģ�壨�ࣩ
#include <QThread>  //�߳�ͷ�ļ�

//SeetaFace
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include "cv.h"
#include "highgui.h"
#include "face_detection.h"
#include "face_alignment.h"

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "face_identification.h"
#include "recognizer.h"
#include "face_detection.h"
#include "face_alignment.h"

#include "math_functions.h"

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>


class OpenVideo : public QWidget
{
	Q_OBJECT

public:
	OpenVideo(QWidget *parent = Q_NULLPTR);

protected:
	//��д��ͼ�¼�
	void paintEvent(QPaintEvent *event);
	//��д��갴���¼�
	void mousePressEvent(QMouseEvent *event);
	//��д����ƶ��¼�
	void mouseMoveEvent(QMouseEvent *event);


public slots: //�ۺ���
	void OpenVideoFile(); //����Ƶ�ļ�
	void OpenCamera(); //������ͷ
	void CloseCamera(); //�ر�����ͷ
	void ScreenShot(); //����/��ͼ
	void REC();  // ¼��
	void EndREC(); // ����¼��
	void FaceDetectionAlignment(); //������������
	void InputImage(); //����ͼƬ
	void FaceRecognitionProcess(); //����ʶ��

	void nextFrame(); //��ȡ��һ֡ͼƬ
	void nextSeetaFaceProcessFrame(); //��ȡ��һ֡�����ͼƬ
	void currentDateAndTime();  //��̬��ʾ��ǰ���ں�ʱ��
	void CloseWindow(); //�رմ���

	//��ȡ���߳��źŷ�������Ϣ
	//��ȡ��������������Ϣ
	//void getDetectionAndAlignmentInformation(int faceNum, QImage image);
	//��ȡ����ʶ����Ϣ
	void getRecognitionInformation(int gallery_face_num, int probe_face_num,QImage image,float sim);
	void NoFace();

signals:

	void mainSignal(QString str);
	void VideoSignal(QImage image,bool flag);

private:
	Ui::OpenVideoClass ui;

	cv::Mat frame; //����һ��Mat���������ڴ洢ûһ֡��ͼ��
	cv::Mat temp; //��ʱ����
	cv::VideoCapture capture; //����VideoCapture���󣬻�ȡ����ͷ
	cv::VideoWriter writer;  //����VideoWriter����¼��
	QTimer *timer;  //��ʱ��
	QTimer *timer2;  //����ʶ��ʱ��
	QString ImagePath; //��������ͼƬ��·��

	int x = 0;
	int y = 0;

	bool openREC;
	QPoint p; //����ĵ�

	MyThread *myT; //�Զ����߳�ģ��(��)����
	QThread *thread; //���߳�

	//������������
	int faceNum;
	QImage imageDetectionAndAlignment;

	//����ʶ��
	int GalleryFaceNum;
	int ProbeFaceNum;
	QImage CropFace;
	float Sim;
	bool CameraVideoFlag; //��־λ,true����Camera��false����Video
};
