//���\�]�������T �ĉ����
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(void) {

	VideoCapture camera;
	Mat card[10];//�J�[�h�̉摜���i�[
	cv::Vec3b color;//���̐F��hsv�Ŋi�[����
	double par[10];//�J�[�h�̐F�̈�̖ʐς̊������i�[
	camera.open(1);//�J�������J��
	int value = 40;//�w�i�����p��臒l
	int op = 0;//�I�[�v�j���O�̉�
	int cl = 0;//�N���[�W���O�̉�
	int miss = 3;//��f�l�̒��ߗp
	int a = 0;//�ǂݍ��񂾃J�[�h�̖���
	double sum = 0;//�ʐς̊��������v
	int k = 11;//�ʐς̊����̕��ς���邽�߂̃��[�v�p
	Mat yj;
	Mat rgbImage, hsvImage, grayImage;//�J�����摜�i�����珇�ɁjRGB,HSV,�Z�W�摜
	Mat sorted;//����Đ��p
	Mat  backgroundImage, backgroundImage2;//1.�w�i�摜2.�e�t���[���摜
	Mat diffImage, diffImage2;//1.�w�i����2.�t���[���ԍ���
	Mat maskImage, maskImage2, maskImage3, maskImage4;//�}�X�N�摜�i�����珇�Ɂj1.�w�i����2.�F�̈�3.�w�i�����ƐF�̈��AND4.�t���[���ԍ���
	Mat resultImage;//���ʗp
	Mat chroma = imread("table.jpg");//�N���}�L�[�����p
	//�r�f�I������
	camera >> rgbImage;
	VideoWriter video;
	video.open("movie.avi", CV_FOURCC_MACRO('M', 'J', 'P', 'G'), 1, rgbImage.size(), true);
	//�X���C�h�o�[���쐬
	namedWindow("image_window");
	createTrackbar("value", "image_window", &value, 256);
	createTrackbar("miss", "image_window", &miss, 100);
	createTrackbar("open", "image_window", &op, 10);
	createTrackbar("close", "image_window", &cl, 10);
	
	
	while (1) {
		vector<vector<Point>> contours; //�w�i�����̗̈���i�[
		vector<vector<Point>> contours3;//�t���[���ԍ����̗̈���i�[
		vector<vector<Point>> polygons;//�w�i�����̂������̑傫�������̈���i�[
		vector<double> area;//�w�i�����̗̈�̖ʐς��i�[
		double areasum = 0;//�J�[�h�S�̖̂ʐς��i�[
		double areasum3 = 0;//�t���[���ԍ����̗̈�̖ʐς̍��v
		camera >> rgbImage;
		imshow("Camera", rgbImage);//�J�����摜��\��
		cvtColor(rgbImage, grayImage, CV_BGR2GRAY);//�Z�W�摜�ɕϊ�
		if (backgroundImage.empty() == false) {//�w�i�摜���i�[����Ă���s������
			//�w�i���������
			absdiff(grayImage, backgroundImage, diffImage);
			threshold(diffImage, maskImage, (double)value, 255, THRESH_BINARY);
			findContours(maskImage, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
			for (int i = 0; i < contours.size(); i++) {    
				area.push_back(contourArea(Mat(contours[i])));  
			}
			for (int i = 0; i < contours.size(); i++) {    
				if (area[i] > 1000) {//�ʐς�1000���傫�����݂̂̂𒊏o
					polygons.push_back(contours[i]);
					areasum += area[i];
				}
			}
			drawContours(rgbImage, polygons, -1, Scalar(255, 0, 255), 1, 8);//�̈��`��
			imshow("Card", rgbImage);
			//�t���[���ԍ��������A���̗̈�̖ʐς��v�Z
			absdiff(grayImage, backgroundImage2, diffImage2);
			threshold(diffImage2, maskImage4, (double)value, 255, THRESH_BINARY);
			findContours(maskImage4, contours3, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
			for (int i = 0; i < contours3.size(); i++) {    
				areasum3+=contourArea(Mat(contours3[i]));  
			}
			//�N���}�L�[����
			chroma.copyTo(resultImage);
			rgbImage.copyTo(resultImage, maskImage);
			imshow("Chromakey", resultImage);
			vector<vector<Point>> contours2;//�w��F�̗̈���i�[
			double areasum2 = 0;//�F�̈�̖ʐύ��v�p
			cvtColor(rgbImage, hsvImage, CV_BGR2HSV);//hsv�摜�ɕϊ�
			inRange(hsvImage, Scalar( max(color[0] - miss,0), 0, 0), Scalar(min(color[0] + miss,180), 255, 255), maskImage2);//�摜���̎w��F�iHue�j�Ń}�X�N
			bitwise_and(maskImage, maskImage2, maskImage3);//�J�[�h�̒��̎w��F���}�X�N
			//�N���[�W���O����
			dilate(maskImage3, maskImage3, Mat(), Point(-1, -1), cl);
			erode(maskImage3, maskImage3, Mat(), Point(-1, -1), cl);
			//�I�[�v�j���O����
			erode(maskImage3, maskImage3, Mat(), Point(-1, -1), op);
			dilate(maskImage3, maskImage3, Mat(), Point(-1, -1), op);
			findContours(maskImage3, contours2, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);//�w��F�̗̈�𒊏o
			// �̈�̖ʐς��Z�o���A���v����
			for (int i = 0; i < contours2.size(); i++) {    
				areasum2 += contourArea(Mat(contours2[i]));
			}
			drawContours(resultImage, contours2, -1, Scalar(255, 0, 255), 1, 8);//�̈��`��
			imshow("check", resultImage);//�ǂ̂悤�ȗ̈悪���o����Ă��邩�m�F�p
			//'c'�L�[����������10�񃋁[�v���J��Ԃ��A�f�[�^�𕽋ς��Ċ��������߂�
			if (k < 10 ) {
				if (areasum3 > 100) {//�t���[���ԍ������傫����΃��[�v�𔲂���
					k = 11;//break�̖������ʂ���
					sum = 0;
					printf("�摜�̕ω����傫������̂Ń��[�v�𔲂��܂�\n");
				}
				sum += areasum2 / areasum * 100;//�������v�Z(%)
				k++;
			}
			else if (k == 10) {
				par[a] = sum / 10;//���ς��o��
				//�ʐς̊������摜���ɕ\��
				char str[50];
				sprintf_s(str, 50, "%.2f", par[a]);
				cv::putText(resultImage, str, cv::Point(25, 75), cv::FONT_HERSHEY_SIMPLEX, 2.5, cv::Scalar(255, 255, 0), 3);
				resultImage.copyTo(card[a]);//�z��Ɋi�[
				a++;//�����̃J�E���g�𑝂₷
				k++;//break�̑���
				sum = 0;
				imshow("Result", resultImage);
			}
			 grayImage.copyTo(backgroundImage2);//�t���[���ԍ����p
		}

		int key = waitKey(1);//�L�[�{�[�h���͂�1�~���b�҂�
		if (key == 'q') break;//"q"�L�[�������ƏI��
		else if (key == 'a') {
			grayImage.copyTo(backgroundImage);//�w�i�����p
			grayImage.copyTo(backgroundImage2);//�t���[���ԍ����p
			imshow("background", backgroundImage);
		}
		else if (key == 'b' && polygons.size() > 0) {
			cv::Moments M = cv::moments(polygons[0]);//�d�S���v�Z
			cvtColor(rgbImage, hsvImage, CV_BGR2HSV);//hsv�ɕϊ�
			//�d�S�̉�f�l(hsv)�𒊏o�i���J�[�h�̐F�𒊏o�j
			int x = (int)(M.m10 / M.m00);
			int y = (int)(M.m01 / M.m00);
			color = hsvImage.at<Vec3b>(y, x);//���J�[�h�̐F���i�[
			resultImage.copyTo(yj);//���J�[�h�̉摜��ۑ�
		}
		else if (key == 'c') {//'c'�L�[�������ƃJ�[�h��F�����A�ʐς��v�Z����
			k = 0;//���[�v���J�n����
		}
		else if (key == 'd') {//'d'�L�[�������ƖʐϏ��ɕ��ׂāA����̕ۑ��ƍĐ�
			//�ʐςɊւ��ď��������ɂȂ�悤�Ƀ\�[�g
			double c;
			Mat cd;
			for (int i = 0; i < a; i++) {
				for (int j = 0; j < a - 1 - i; j++) {
					if (par[j] > par[j + 1]) {
						c = par[j];
						par[j] = par[j + 1];
						par[j + 1] = c;
						cd = card[j];
						card[j] = card[j + 1];
						card[j + 1] = cd;
					}
				}
			}
			video << yj;
			//�r�f�I�ɉ摜��ۑ�
			for (int i = 0; i < a; i++) {
				printf("%f,", par[i]);
				video << card[i];
			}
			//�r�f�I��ۑ�
			video.release();
			//�r�f�I���Đ�
			VideoCapture video2("movie.avi");
			while (1) {
				video2 >> sorted; 
				if (sorted.empty()) break; 
				imshow("sorted", sorted);
				int c = waitKey(300);//�t���[�����[�g�����p
			}
		}
		
	}

	return 0;

}
