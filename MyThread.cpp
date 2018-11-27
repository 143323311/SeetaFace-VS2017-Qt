#include "MyThread.h"

MyThread::MyThread(QObject *parent) : QObject(parent)
{
	//���ó�ʼ��־λ
	isStop = false; 
	CameraVideoFlag = true;
}

void MyThread::setFlag(bool flag)
{
	isStop = flag;
}

//----------------------------------------------------������ʶ��------------------------------------------------
void MyThread::FaceRecognitionProcess()
{
	using namespace seeta;

	std::string MODEL_DIR = "./model/";

	//��ģ���ļ���·��ʵ����seeta::FaceDetection�Ķ���
	seeta::FaceDetection detector("./model/seeta_fd_frontal_v1.0.bin");
	detector.SetMinFaceSize(40);//����Ҫ������������С�ߴ�(Ĭ��ֵ:20����������),Ҳ���������face_detector.SetMaxFaceSize(size);
	detector.SetScoreThresh(2.f); //���ü�⵽�������ĵ÷���ֵ(Ĭ��ֵ:2.0)
	detector.SetImagePyramidScaleFactor(0.8f);//����ͼ���������������(0 <����< 1��Ĭ��ֵ:0.8)
	detector.SetWindowStep(4, 4);//���û������ڵĲ���(Ĭ��:4),face_detector.SetWindowStep(step_x, step_y);

	//��ʼ������������ģ��
	seeta::FaceAlignment point_detector("./model/seeta_fa_v1.1.bin");

	//��ʼ��������ʶ��ģ��
	FaceIdentification face_recognizer((MODEL_DIR + "seeta_fr_v1.0.bin").c_str());

	//����һ��ͼƬ
	cv::Mat gallery_img_color = cv::imread(path.toStdString(), 1); 
	cv::Mat gallery_img_gray;
	cv::cvtColor(gallery_img_color, gallery_img_gray, CV_BGR2GRAY);

	cv::Mat probe_img_color;

	if (CameraVideoFlag)
	{
		capture.open(0);
	}

	while (!isStop)
	{
		//��ȡ����ͷ/��Ƶ��ÿһ֡����
		if (CameraVideoFlag)
		{
			capture >> probe_img_color;
		}
		else
		{
			//����QImage�ĸ�ʽ��RGB��Ҫת��OpenCV��Mat��BGR��ʽ
			cvtColor(frameTemp, frame, CV_RGB2BGR);
			probe_img_color = frame;
		}
		cv::Mat probe_img_gray;
		cv::cvtColor(probe_img_color, probe_img_gray, CV_BGR2GRAY);

		ImageData gallery_img_data_color(gallery_img_color.cols, gallery_img_color.rows, gallery_img_color.channels());
		gallery_img_data_color.data = gallery_img_color.data;//data  uchar�͵�ָ�롣Mat���Ϊ����������:����ͷ��ָ��������ݲ���(���о���ֵ)��ָ�룬data����ָ��������ݵ�ָ�롣

		ImageData gallery_img_data_gray(gallery_img_gray.cols, gallery_img_gray.rows, gallery_img_gray.channels());
		gallery_img_data_gray.data = gallery_img_gray.data;

		ImageData probe_img_data_color(probe_img_color.cols, probe_img_color.rows, probe_img_color.channels());
		probe_img_data_color.data = probe_img_color.data;

		ImageData probe_img_data_gray(probe_img_gray.cols, probe_img_gray.rows, probe_img_gray.channels());
		probe_img_data_gray.data = probe_img_gray.data;

		// Detect faces
		std::vector<seeta::FaceInfo> gallery_faces = detector.Detect(gallery_img_data_gray);
		int32_t gallery_face_num = static_cast<int32_t>(gallery_faces.size());

		std::vector<seeta::FaceInfo> probe_faces = detector.Detect(probe_img_data_gray);
		int32_t probe_face_num = static_cast<int32_t>(probe_faces.size());

		if (gallery_face_num != 0 && probe_face_num != 0)
		{
			//���5���沿���
			seeta::FacialLandmark gallery_points[5];
			point_detector.PointDetectLandmarks(gallery_img_data_gray, gallery_faces[0], gallery_points);

			seeta::FacialLandmark probe_points[5];
			point_detector.PointDetectLandmarks(probe_img_data_gray, probe_faces[0], probe_points);

			for (int i = 0; i < 5; i++)
			{
				cv::circle(gallery_img_color, cv::Point(gallery_points[i].x, gallery_points[i].y), 2,
					CV_RGB(0, 255, 0));
				cv::circle(probe_img_color, cv::Point(probe_points[i].x, probe_points[i].y), 2,
					CV_RGB(0, 255, 0));
			}
			cv::imwrite("gallery_point_result.jpg", gallery_img_color);
			cv::imwrite("probe_point_result.jpg", probe_img_color);

			//��ȡ�沿��������
			float gallery_fea[2048];
			float probe_fea[2048];
			face_recognizer.ExtractFeatureWithCrop(gallery_img_data_color, gallery_points, gallery_fea);
			face_recognizer.ExtractFeatureWithCrop(probe_img_data_color, probe_points, probe_fea);

			//-------------------------------------------���ü�������----------------------------------------------------
			//����һ��ͼ�����洢�ü�������
			cv::Mat dst_img(face_recognizer.crop_height(),
				face_recognizer.crop_width(),
				CV_8UC(face_recognizer.crop_channels()));
			ImageData dst_img_data(dst_img.cols, dst_img.rows, dst_img.channels());
			dst_img_data.data = dst_img.data;

			//�ü�
			face_recognizer.CropFace(probe_img_data_color, probe_points, dst_img_data);
			//����ü�������
			cv::imwrite("dst_img.jpg", dst_img);

			//ͼ����Qt��ʾǰ�����뽫Mat��ת����QImage��ʽ����RGBA��ʽת����RGB 
			cvtColor(dst_img, temp, CV_BGR2RGB);
			//ת����QImage��ʽ
			QImage CropFace = QImage((const unsigned char *)(temp.data), temp.cols,
				temp.rows, QImage::Format_RGB888);

			//�����������������ƶ�
			float sim = face_recognizer.CalcSimilarity(gallery_fea, probe_fea);

			//Qt���̵߳��źŴ���ֻ�ܴ�������������
			//Mat ���߿������int_8 / int_32(�Զ������������) ʲô�Ĳ��ܴ�
			//����ת��
			int GalleryFaceNum = gallery_face_num;
			int ProbeFaceNum = probe_face_num;
			//�����ź�
			emit UpdateRecognition(GalleryFaceNum, ProbeFaceNum, CropFace, sim);
		}

		else
		{
			emit NoFace();
		}
	}
	
}

void MyThread::getMainSignal(QString str)
{
	//��ȡ����ͼƬ��·��
	path = str;
}

void MyThread::getVideoSignal(QImage image,bool flag)
{
	this->image = image;
	CameraVideoFlag = flag;
	frameTemp = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
	//���ܷ�������ת��ʽ������ɲ��ܼ�ʱ����
	//�ʰѸ�ʽת�����ڴ���������
}

