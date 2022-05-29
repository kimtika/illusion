//性能評価実験Ⅰ 米岡主税
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(void) {

	VideoCapture camera;
	Mat card[10];//カードの画像を格納
	cv::Vec3b color;//矢印の色をhsvで格納する
	double par[10];//カードの色領域の面積の割合を格納
	camera.open(1);//カメラを開く
	int value = 40;//背景差分用の閾値
	int op = 0;//オープニングの回数
	int cl = 0;//クロージングの回数
	int miss = 3;//画素値の調節用
	int a = 0;//読み込んだカードの枚数
	double sum = 0;//面積の割合を合計
	int k = 11;//面積の割合の平均を取るためのループ用
	Mat yj;
	Mat rgbImage, hsvImage, grayImage;//カメラ画像（左から順に）RGB,HSV,濃淡画像
	Mat sorted;//動画再生用
	Mat  backgroundImage, backgroundImage2;//1.背景画像2.各フレーム画像
	Mat diffImage, diffImage2;//1.背景差分2.フレーム間差分
	Mat maskImage, maskImage2, maskImage3, maskImage4;//マスク画像（左から順に）1.背景差分2.色領域3.背景差分と色領域のAND4.フレーム間差分
	Mat resultImage;//結果用
	Mat chroma = imread("table.jpg");//クロマキー合成用
	//ビデオを準備
	camera >> rgbImage;
	VideoWriter video;
	video.open("movie.avi", CV_FOURCC_MACRO('M', 'J', 'P', 'G'), 1, rgbImage.size(), true);
	//スライドバーを作成
	namedWindow("image_window");
	createTrackbar("value", "image_window", &value, 256);
	createTrackbar("miss", "image_window", &miss, 100);
	createTrackbar("open", "image_window", &op, 10);
	createTrackbar("close", "image_window", &cl, 10);
	
	
	while (1) {
		vector<vector<Point>> contours; //背景差分の領域を格納
		vector<vector<Point>> contours3;//フレーム間差分の領域を格納
		vector<vector<Point>> polygons;//背景差分のうち一定の大きさを持つ領域を格納
		vector<double> area;//背景差分の領域の面積を格納
		double areasum = 0;//カード全体の面積を格納
		double areasum3 = 0;//フレーム間差分の領域の面積の合計
		camera >> rgbImage;
		imshow("Camera", rgbImage);//カメラ画像を表示
		cvtColor(rgbImage, grayImage, CV_BGR2GRAY);//濃淡画像に変換
		if (backgroundImage.empty() == false) {//背景画像が格納されてから行う処理
			//背景差分を取る
			absdiff(grayImage, backgroundImage, diffImage);
			threshold(diffImage, maskImage, (double)value, 255, THRESH_BINARY);
			findContours(maskImage, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
			for (int i = 0; i < contours.size(); i++) {    
				area.push_back(contourArea(Mat(contours[i])));  
			}
			for (int i = 0; i < contours.size(); i++) {    
				if (area[i] > 1000) {//面積が1000より大きいもののみを抽出
					polygons.push_back(contours[i]);
					areasum += area[i];
				}
			}
			drawContours(rgbImage, polygons, -1, Scalar(255, 0, 255), 1, 8);//領域を描画
			imshow("Card", rgbImage);
			//フレーム間差分を取り、その領域の面積を計算
			absdiff(grayImage, backgroundImage2, diffImage2);
			threshold(diffImage2, maskImage4, (double)value, 255, THRESH_BINARY);
			findContours(maskImage4, contours3, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
			for (int i = 0; i < contours3.size(); i++) {    
				areasum3+=contourArea(Mat(contours3[i]));  
			}
			//クロマキー合成
			chroma.copyTo(resultImage);
			rgbImage.copyTo(resultImage, maskImage);
			imshow("Chromakey", resultImage);
			vector<vector<Point>> contours2;//指定色の領域を格納
			double areasum2 = 0;//色領域の面積合計用
			cvtColor(rgbImage, hsvImage, CV_BGR2HSV);//hsv画像に変換
			inRange(hsvImage, Scalar( max(color[0] - miss,0), 0, 0), Scalar(min(color[0] + miss,180), 255, 255), maskImage2);//画像内の指定色（Hue）でマスク
			bitwise_and(maskImage, maskImage2, maskImage3);//カードの中の指定色をマスク
			//クロージング処理
			dilate(maskImage3, maskImage3, Mat(), Point(-1, -1), cl);
			erode(maskImage3, maskImage3, Mat(), Point(-1, -1), cl);
			//オープニング処理
			erode(maskImage3, maskImage3, Mat(), Point(-1, -1), op);
			dilate(maskImage3, maskImage3, Mat(), Point(-1, -1), op);
			findContours(maskImage3, contours2, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);//指定色の領域を抽出
			// 領域の面積を算出し、合計する
			for (int i = 0; i < contours2.size(); i++) {    
				areasum2 += contourArea(Mat(contours2[i]));
			}
			drawContours(resultImage, contours2, -1, Scalar(255, 0, 255), 1, 8);//領域を描画
			imshow("check", resultImage);//どのような領域が抽出されているか確認用
			//'c'キーが押されると10回ループを繰り返し、データを平均して割合を求める
			if (k < 10 ) {
				if (areasum3 > 100) {//フレーム間差分が大きければループを抜ける
					k = 11;//breakの役割を果たす
					sum = 0;
					printf("画像の変化が大きすぎるのでループを抜けます\n");
				}
				sum += areasum2 / areasum * 100;//割合を計算(%)
				k++;
			}
			else if (k == 10) {
				par[a] = sum / 10;//平均を出す
				//面積の割合を画像中に表示
				char str[50];
				sprintf_s(str, 50, "%.2f", par[a]);
				cv::putText(resultImage, str, cv::Point(25, 75), cv::FONT_HERSHEY_SIMPLEX, 2.5, cv::Scalar(255, 255, 0), 3);
				resultImage.copyTo(card[a]);//配列に格納
				a++;//枚数のカウントを増やす
				k++;//breakの代わり
				sum = 0;
				imshow("Result", resultImage);
			}
			 grayImage.copyTo(backgroundImage2);//フレーム間差分用
		}

		int key = waitKey(1);//キーボード入力を1ミリ秒待つ
		if (key == 'q') break;//"q"キーを押すと終了
		else if (key == 'a') {
			grayImage.copyTo(backgroundImage);//背景差分用
			grayImage.copyTo(backgroundImage2);//フレーム間差分用
			imshow("background", backgroundImage);
		}
		else if (key == 'b' && polygons.size() > 0) {
			cv::Moments M = cv::moments(polygons[0]);//重心を計算
			cvtColor(rgbImage, hsvImage, CV_BGR2HSV);//hsvに変換
			//重心の画素値(hsv)を抽出（矢印カードの色を抽出）
			int x = (int)(M.m10 / M.m00);
			int y = (int)(M.m01 / M.m00);
			color = hsvImage.at<Vec3b>(y, x);//矢印カードの色を格納
			resultImage.copyTo(yj);//矢印カードの画像を保存
		}
		else if (key == 'c') {//'c'キーを押すとカードを認識し、面積を計算する
			k = 0;//ループを開始する
		}
		else if (key == 'd') {//'d'キーを押すと面積順に並べて、動画の保存と再生
			//面積に関して小さい順になるようにソート
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
			//ビデオに画像を保存
			for (int i = 0; i < a; i++) {
				printf("%f,", par[i]);
				video << card[i];
			}
			//ビデオを保存
			video.release();
			//ビデオを再生
			VideoCapture video2("movie.avi");
			while (1) {
				video2 >> sorted; 
				if (sorted.empty()) break; 
				imshow("sorted", sorted);
				int c = waitKey(300);//フレームレート調整用
			}
		}
		
	}

	return 0;

}
