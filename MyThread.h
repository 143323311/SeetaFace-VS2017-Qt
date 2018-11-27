#ifndef MYTHREAD_H  //��ֹͷ�ļ����ظ�����������C���Է�ʽ��C++�ǣ�#pragma once
#define MYTHREAD_H

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

#include <opencv2/opencv.hpp>
#include <QObject>
#include <QTimer> //��ʱ��
#include <QImage> //QImageͷ�ļ�

class MyThread : public QObject
{
	Q_OBJECT
public:
	explicit MyThread(QObject *parent = nullptr);
	
	//�̴߳�����
	void FaceRecognitionProcess(); //����ʶ��

	//��ȡ���߳��źŷ���������
	void getMainSignal(QString str);
	void getVideoSignal(QImage image,bool flag);

	//��־λ����
	void setFlag(bool flag = true); //�����ﶨ����һ��Ĭ�ϲ����������ε�ʱ���ʹ��Ĭ�ϵ�

signals:
	void UpdateRecognition(int GalleryFaceNum,int ProbeFaceNum,QImage image,float sim);
	void NoFace();
	
private:
	cv::Mat frame; //����һ��Mat���������ڴ洢ÿһ֡��ͼ��
	cv::Mat temp; //��ʱ����
	cv::Mat frameTemp;
	cv::VideoCapture capture; //����VideoCapture���󣬻�ȡ����ͷ

	QImage image;

	//��־λ
	bool isStop; 
	bool CameraVideoFlag;//��־λ,true����Camera��false����Video

	QString path;
};

#endif // MYTHREAD_H
