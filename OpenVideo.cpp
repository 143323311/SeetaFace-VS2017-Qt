/**
*Copyright (c) 2018 Young Fan.All Right Reserved.
*Filename:
*Author: Young Fan
*Date: 2018.5.29 - 7.5
*OpenCV version: 3.4.1
*IDE: Visual Studio 2017
*Description: Demo:Qt + Opecv + VS + SeetaFace(via VIPL)
*/

#include "OpenVideo.h"
#include <QFileDialog>
#include <QTimer>
#include <QMovie>
#include <QPainter>
#include <QDebug>
#include <QMessageBox>
#include <QDebug>
#include <QDateTime>
#include <QMouseEvent>
//ʹ��Qt��ý�壬��Ҫ�ڸ������������Qt5Multimedia.lib(debug)/Qt5Multimediad.lib(release)
#include <QtMultimedia/QMediaPlayer> 



OpenVideo::OpenVideo(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	//���ô��ڱ���
	setWindowTitle("OpenVideo");

	//��ͼƬ�Զ���Ӧlabel��С
	ui.labelOpenVideo->setScaledContents(true);
	ui.labelGif->setScaledContents(true);

	//-----����ʾGif������-------
	//��������
	QMovie *myMovie = new QMovie("./UiFile/2.gif");
	//���ö���
	ui.labelGif->setMovie(myMovie);
	//��������
	myMovie->start();

	//�̶������ڴ�С
	setFixedSize(1125, 511);

	//������ʱ��
	timer = new QTimer(this);

	//-----------�������򴰿ڡ�-------------
	//������ȥ���߿򣬴��ϴ��ڵ�flags����ǣ�
	setWindowFlags(Qt::FramelessWindowHint | windowFlags());
	//�Ѵ��ڱ�������Ϊ͸��,���ﲻ����͸��Ҳ���ԣ���ΪͼƬ��͸��
	setAttribute(Qt::WA_TranslucentBackground);

	//���İ�ť��ʽ��
	ui.CloseButton->setFlat(true); //�Ѱ�ť����Ϊ͸����������ʱֻ�а��µ�ʱ����б�����
	ui.CloseButton->setStyleSheet("QPushButton{"
		//"background:rgb(176,0,0);"//��ť����Ϊ͸������������������������Ҳ����ʾ
		//���������"border:2px outset green;"������ڳ�ʼ̬�������̬ͣ��ʾ����ɫ������һ��
		"}"
		"QPushButton:hover{"
		"border:1px;"//����ť������͸���ģ������ӱ߿��ȣ�������hoverЧ��������дҲ��"border:1px outset red;"
		"background:rgb(255,96,96);"//��ť����Ϊ͸������������������������Ҳ����ʾ
									//"border-image:url(E:/C++/Demo/OpenVideo/UiFile/closeWhite.png);"
		"}"
		"QPushButton:pressed{"
		"background:rgb(255,77,77);"//��ʱֻ�а��µ�ʱ����б�����
									//"border-image:url(E:/C++/Demo/OpenVideo/UiFile/closeWhite.png);"
		"}");

	//-----------------------------------------�����̲߳��֡�--------------------------------------------------
	//�Զ���ģ��(��)���󣬷���ռ䣬������ָ����������Ϊ����Ҫ�Ƶ����߳�
	myT = new MyThread;
	//�������߳�,ָ��������
	thread = new QThread(this);
	//���Զ���ģ����ӵ����߳�
	myT->moveToThread(thread);
	//�������̣߳����ǲ�û�������̵߳Ĵ�����,Ҫ���źźͲ������̵߳Ĵ�����
	thread->start();

	//�̴߳�����������ͨ��signal-slot����
	//����ʹ�õ��ǰ�ť���ź���������Ҳ�����ð�ť��ת���ۺ�����������ȥ�����ź�
	//�����ź�ȥ����ģ�����еĴ�����
	connect(ui.buttonFaceRecognition,&QPushButton::pressed,myT,&MyThread::FaceRecognitionProcess);//����ʶ�𲿷�
	
	//һ����ģ�����еĴ������������ͻᷢ��UpdateProcess�źţ��̴߳�����д�ģ�
	connect(myT,&MyThread::UpdateRecognition,this,&OpenVideo::getRecognitionInformation);//����ʶ�𲿷�	
	connect(myT, &MyThread::NoFace, this, &OpenVideo::NoFace);

	//���̸߳����߳�ͨ��
	connect(this, &OpenVideo::mainSignal, myT, &MyThread::getMainSignal);
	connect(this,&OpenVideo::VideoSignal,myT,&MyThread::getVideoSignal);

}
//--------------------------------------------����д��갴���¼���--------------------------------------------
void OpenVideo::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton) //event->button() ���ص���Buttons�¼�
	{
		//�������ֵ���������������ǰ����������������Ļ��
		//����ǰ������� - �������Ͻ�����
		//����ڴ��ڵ�������x��y���������Ļ��globalPos
		//frameGeometry���Ǵ��߿��,�������Ի�ȡ�������Ͻ����꣨�������Ļ�����꣩
		//�ƶ�����ʱ�������Ͻ����ƶ��ģ�Ҫ����ľʹ����ƶ�����Ҫ�������
		p = event->globalPos() - this->frameGeometry().topLeft();
	}
}

void OpenVideo::mouseMoveEvent(QMouseEvent *event)
{
	//������Ϊ�ǰ�ס����ʹ��buttons(����s����button��
	//�����ס�ƶ�����
	if (event->buttons() & Qt::LeftButton) //event->buttons()���ص���intλ�ֶ�
	{
		//�ƶ�����ʱ�������Ͻ����ƶ��ģ���Ҫ��ȥp(��ʱ���൱�ڴӸõ㿪ʼ�ƶ�)
		this->move(event->globalPos() - p);
	}
}

//----------------------------------------------��������ͼ��--------------------------------------------------
void OpenVideo::paintEvent(QPaintEvent *event)
{
	QPainter p(this);
	p.drawPixmap(0, 0, width(), height(), QPixmap("./UiFile/background.png"));
}

//-----------------------------------------------����ʾ��ǰ���ں�ʱ�䡿------------------------------------
void OpenVideo::currentDateAndTime()
{
	//������������ʾʱ�����ڵķ�������C���Եĺ�__DATE__,__TIME__���ٱ���QTime��Ҳ������ʾʱ��������
	//��arg()Ҳ����ʾ��������Ҫ��ʱ��
	ui.labelDateTime->setText(QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm:ss.zzz A"));
}
//----------------------------------------------���رմ��ڡ�--------------------------------------------------
void OpenVideo::CloseWindow()
{
	//д����Ҫ����һ��ϰ�ߣ�Ҫ���ж�
	if (false == thread->isRunning()) //����߳���ֹͣ����ť��������ִ��
	{
		return;
	}
	myT->setFlag(true); //����ѭ��
	thread->quit(); //������˳�
	thread->wait(); //�ȴ���������Դ
					//�����ģ�飨�ࣩ���ڹ����Ļ�(����ѭ��while(1)�����������myT->setFlag(true);)��
					//���̲߳�����ֹͣ���ǻ�������

	this->close(); //�رմ���
	delete myT; //�ͷ���Դ
}

//---------------------------------------------������Ƶ�ļ���------------------------------------------------
void OpenVideo::OpenVideoFile()
{
	//��ʼ����־λ
	CameraVideoFlag = false;
	myT->setFlag(false); //���̴߳�����ѭ��

	//��ʾ��ǩ
	ui.labelOpenVideo->setVisible(true);
	ui.label->setVisible(true);

	//�����ļ��Ի��򲢻�ȡ·��
	QString path = QFileDialog::getOpenFileName(this,
		"open", "../", "Video(*.mp4 *.flv *.mkv *.avi)");

	//������Ƶ
	capture.open(path.toLocal8Bit().data()); //Ҳ����path.toStdString()   

	//�ж϶�ʱ���ļ���״̬
	if (false == timer->isActive())
	{
		//������ʱ��
		timer->start(35);
	}

	//��ʱ������,��ȡ��һ֡ͼƬ
	connect(timer, &QTimer::timeout, this, &OpenVideo::nextFrame);
	connect(timer, &QTimer::timeout,
			[=]()
			{
				ui.label->setText(QStringLiteral("��Ƶ������..."));
				ui.label->move(QPoint(x++, y++));
				if (x >511)
				{
					x = 0;
					y = 0;
				}
			}
			);
}

//----------------------------------------------��������ͷ��---------------------------------------------
void OpenVideo::OpenCamera()
{
	//��ʼ����־λ
	CameraVideoFlag = true;

	//��ʾ��ǩ
	ui.labelOpenVideo->setVisible(true);

	//��������ͷ
	capture.open(0);

	timer->start(35); //��ʱ35ms

	//��ʱ������,��ȡ��һ֡ͼƬ
	connect(timer, &QTimer::timeout, this, &OpenVideo::nextFrame);
}

//-----------------------------------����ȡ����ʾ��һ֡ͼƬ��---------------------------------------
void OpenVideo::nextFrame()
{
	capture >> frame; //��ȡ��ǰ֡
	//ͼ����Qt��ʾǰ�����뽫Mat��ת����QImage��ʽ����OpenCV��Mat��BGR��ʽת����QImage��RGB��ʽ 
	cvtColor(frame, temp, CV_BGR2RGB);
	//ת����QImage��ʽ
	QImage image = QImage((const unsigned char *)(temp.data), temp.cols,
		temp.rows, QImage::Format_RGB888);
	ui.labelOpenVideo->setPixmap(QPixmap::fromImage(image));

	if (!CameraVideoFlag)
	{
		emit VideoSignal(image, CameraVideoFlag);
	}

}

//--------------------------------------������/��ͼ��------------------------------------------------
void OpenVideo::ScreenShot()
{
	//��Mat��tempתΪQt��QImage��
	QImage image = QImage((const unsigned char *)(temp.data), temp.cols,
		temp.rows, QImage::Format::Format_RGB888);
	ui.labelScreenShot->setPixmap(QPixmap::fromImage(image));
	//��ͼƬ�Զ���Ӧlabel��С
	ui.labelScreenShot->setScaledContents(true);

	//��ͼ����
	const QPixmap *img = ui.labelScreenShot->pixmap();
	QString path = QFileDialog::getSaveFileName(this, "save", "../", "Image(*.png)");
	if (false == path.isEmpty()) //�ж�·���Ƿ���Ч
	{
		bool isOk = img->save(path);
		if (true == isOk)
		{

			//#pragma execution_character_set("utf-8") 
			//����ΪUTF-8��ʽ����ʾ���ģ���Ȼ������,����Ӱ����vs�ı����ʽ�����ܴݻ��������ƴ���
			//��ѡ����QStringLiteral("����")�������Ρ������䲻��ȫ������utf-8�������������ݻ��������ƴ���
			QMessageBox::information(this, QStringLiteral("�������"), QStringLiteral("��ͼ�������"));
		}
	}
	else
	{
		//�����ļ�·������
		return;
	}
}

//---------------------------------------��¼����Ƶ��-------------------------------------------
void OpenVideo::REC()
{
	double rate = 12.0;//������Ƶ��֡�ʣ�12���֡�ʺܺ��ʣ����ŵ�ʱ��������
	cv::Size videoSize(frame.cols, frame.rows);
	//¼�� VideoWriter writer; ����ͷ�ļ��ж���
	writer.open("../REC.avi", cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), rate, videoSize, true);
	openREC = true;
	while (openREC)
	{
		capture >> frame;
		writer.write(frame);// ��writer << frame;

							//�Ӹ�������ʾ�����ӵĻ���ֱ������ѭ���Ῠס����
		cv::namedWindow("¼����Ƶ");
		imshow("¼����Ƶ", frame);
		cv::waitKey(30);
	}
}

//-------------------------------------------------������¼�ơ�------------------------------------------
void OpenVideo::EndREC()
{
	cv::destroyWindow("¼����Ƶ");
	openREC = false;
	writer.release();//����¼��
}

//--------------------------------------------�������������롿-----------------------------------------------
void OpenVideo::FaceDetectionAlignment()
{
	//��ʱ������,��ȡ��һ֡ͼƬ
	connect(timer, &QTimer::timeout, this, &OpenVideo::nextSeetaFaceProcessFrame);
	
}

//----------------------------------------------����ȡ��һ֡SeetaFace�����ͼƬ��----------------------------
void OpenVideo::nextSeetaFaceProcessFrame()
{

	std::string MODEL_DIR = "./model/";

	//-----------------------------------------�������������롿--------------------------------------
	////Ҫ���ͼ���ϵ�����������Ӧ����ģ���ļ���·��ʵ����seeta::FaceDetection�Ķ���
	seeta::FaceDetection detector("./model/seeta_fd_frontal_v1.0.bin");
	detector.SetMinFaceSize(40);//����Ҫ������������С�ߴ�(Ĭ��ֵ:20����������),Ҳ���������face_detector.SetMaxFaceSize(size);
	detector.SetScoreThresh(2.f);////���ü�⵽�������ĵ÷���ֵ(Ĭ��ֵ:2.0)
	detector.SetImagePyramidScaleFactor(0.8f);//����ͼ���������������(0 <����< 1��Ĭ��ֵ:0.8)
	detector.SetWindowStep(4, 4);//���û������ڵĲ���(Ĭ��:4),face_detector.SetWindowStep(step_x, step_y);

		capture >> frame;//��������ͷ

		int pts_num = 5;
		cv::Mat img_gray;
		if (frame.channels() != 1)
			//�ǵ�ͨ���ģ�תΪ�Ҷ�ͼ
			cv::cvtColor(frame, img_gray, cv::COLOR_BGR2GRAY);
		else
		{
			img_gray = frame;
		}

		seeta::ImageData image_data;
		image_data.data = img_gray.data;
		image_data.width = img_gray.cols;
		image_data.height = img_gray.rows;
		image_data.num_channels = 1;

		//����Detect()�����������������Ϊseeta::FaceInfo������(����)���ء�
		std::vector<seeta::FaceInfo> faces = detector.Detect(image_data);
		int32_t face_num = static_cast<int32_t>(faces.size()); //��ȡ��������

		seeta::FacialLandmark points[5];//�����沿��5����ǵ�

		//����Ӧ��ʵ����seeta:: faceAlignment�Ķ�����ģ���ļ���·����
		seeta::FaceAlignment point_detector((MODEL_DIR + "seeta_fa_v1.1.bin").c_str());

		//���5���沿���
		for (int f = 0; f < face_num; f++) {
			point_detector.PointDetectLandmarks(image_data, faces[f], points);//�����������
			//�������þ��ο�������εķ�Χ��Detect()���ص�faces������
			cv::rectangle(frame, cv::Point(faces[f].bbox.x, faces[f].bbox.y),//bbox��Rect���͵Ľṹ��
				cv::Point(faces[f].bbox.x + faces[f].bbox.width - 1, faces[f].bbox.y + faces[f].bbox.height - 1), CV_RGB(255, 0, 0));

			// Visualize the results(���󻯽�������Ѽ�����5���ǣ���PointDetectLandmarks�����ڲ�����������СԲ�����)
			for (int i = 0; i < pts_num; i++)
			{
				cv::circle(frame, cv::Point(points[i].x, points[i].y), 2, CV_RGB(0, 255, 0), -CV_FILLED);
				//cv::ellipse(frame, cv::Point(points[i].x, points[i].y),cv::Size(6,2),-45,0,360,cv::Scalar(255,129,0),1,8);
			}
		}

		//ͼ����Qt��ʾǰ������ת����QImage��ʽ����RGBA��ʽת����RGB 
		cvtColor(frame, temp, CV_BGR2RGB);
		//ת����QImage��ʽ
		QImage image = QImage((const unsigned char *)(temp.data), temp.cols,
			temp.rows, QImage::Format_RGB888);

		ui.labelFaceNum->setText(QStringLiteral("��⵽������������%1").arg(face_num));
		ui.labelOpenVideo->setPixmap(QPixmap::fromImage(image));

}


//----------------------------------------------���ر�����ͷ��-------------------------------------------
void OpenVideo::CloseCamera()
{
	capture.release();
	timer->stop();

	//�ر�����ͷ�󣬶Ա�ǩ״̬���п��ƣ������кü��ַ���
	//ui.labelOpenVideo->hide();  //�ؼ�����,Widget��ķ���
	//ui.labelOpenVideo->setVisible(false);//�ؼ��Ƿ�ɼ�,Widget��ķ���
	ui.labelOpenVideo->close();//�ؼ��ر�,Widget��ķ�����������show()����ʹ��;

	//�ر�¼�ƣ���ť���ã�
	cv::destroyWindow("¼����Ƶ");
	openREC = false;

	//ȥ������Ƶ�����С�����
	ui.label->close();

	//��ֹͣ���߳�
	myT->setFlag(true); //����ѭ��
}

//------------------------------------------------������ͼƬ��-------------------------------------------
void OpenVideo::InputImage()
{
	ImagePath = QFileDialog::getOpenFileName(this,
		QStringLiteral("��ͼƬ"), "../", "Image(*.png *.jpg *.bmp)");

	emit mainSignal(ImagePath); //�����źŸ����߳�

	ui.InputImageLabel->setPixmap(QPixmap(ImagePath)); //��ʵֱ�ӷ�QString���͵�Ҳ���ԣ���ֱ�ӷ�ImagePath��
	//��ͼƬ�Զ���Ӧlabel��С
	ui.InputImageLabel->setScaledContents(true);

	//��̬��ʾ��ǰ���ں�ʱ�䣨��ʱ�����ڴ�����ͷʱ���
	connect(timer, &QTimer::timeout, this, &OpenVideo::currentDateAndTime);
}

//----------------------------------------------����ȡ���߳��źŷ�������Ϣ��------------------------------------
//��ȡ����ʶ��ģ�����Ϣ
void OpenVideo::getRecognitionInformation(int gallery_face_num, int probe_face_num,QImage image,float sim)
{
	GalleryFaceNum = gallery_face_num;
	ProbeFaceNum = probe_face_num;
	CropFace = image;
	Sim = sim;
	
	//���²���
	FaceRecognitionProcess(); 
}

//-------------------------------------------------������ʶ��--------------------------------------------
//������ ����Ƶ���ҳ�������ͼƬ��Ӧ������
void OpenVideo::FaceRecognitionProcess()
{

	if (GalleryFaceNum != 0 && ProbeFaceNum != 0)
	{
		//��ʾ���ƶ�
		ui.SimilarityLabel->setText(QStringLiteral("���ƶ�Ϊ��%1").arg(Sim));  //�������arg����ʾfloat��

		if (Sim > 0.7)
		{
			ui.SimilarityLabel->setText(QStringLiteral(" ����Ŀ�� | ���ƶ�Ϊ��%1").arg(Sim));
			//printf("\a"); //ת�����������Qt����Ч�����������Qt�ķ�ʽ

			//������Ƶ
			QMediaPlayer *player = new QMediaPlayer;
			//QUrl::fromLocalFile  �ú���������б�߷ָ�(/)��·���Լ���ƽ̨�ı����ָ�����
			//�˺��������ܾ���˫ǰ��б��(��б��)(\\)��·����ָʾԶ���ļ�
			player->setMedia(QUrl::fromLocalFile("./mp3/Recognition.mp3"));
			player->setVolume(80);
			player->play();

			//----------------------------------�����ü���������ʾ�ڽ����ǩ�ϡ�------------------------------------------
			ui.OutputImageLabel->setPixmap(QPixmap::fromImage(CropFace));
			ui.OutputImageLabel->setScaledContents(true);
		}
	}

	
}

void OpenVideo::NoFace()
{
	ui.SimilarityLabel->setText(QStringLiteral("sorry����ⲻ������"));
}
