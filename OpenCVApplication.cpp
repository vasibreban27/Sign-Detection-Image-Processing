// OpenCVApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"
#include <opencv2/core/utils/logger.hpp>

using namespace std;
using namespace cv;

wchar_t* projectPath;

void testOpenImage()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src;
		src = imread(fname);
		imshow("image", src);
		waitKey();
	}
}

void testOpenImagesFld()
{
	char folderName[MAX_PATH];
	if (openFolderDlg(folderName) == 0)
		return;
	char fname[MAX_PATH];
	FileGetter fg(folderName, "bmp");
	while (fg.getNextAbsFile(fname))
	{
		Mat src;
		src = imread(fname);
		imshow(fg.getFoundFileName(), src);
		if (waitKey() == 27) //ESC pressed
			break;
	}
}

void testImageOpenAndSave()
{
	_wchdir(projectPath);

	Mat src, dst;

	src = imread("Images/Lena_24bits.bmp", IMREAD_COLOR);	// Read the image

	if (!src.data)	// Check for invalid input
	{
		printf("Could not open or find the image\n");
		return;
	}

	// Get the image resolution
	Size src_size = Size(src.cols, src.rows);

	// Display window
	const char* WIN_SRC = "Src"; //window for the source image
	namedWindow(WIN_SRC, WINDOW_AUTOSIZE);
	moveWindow(WIN_SRC, 0, 0);

	const char* WIN_DST = "Dst"; //window for the destination (processed) image
	namedWindow(WIN_DST, WINDOW_AUTOSIZE);
	moveWindow(WIN_DST, src_size.width + 10, 0);

	cvtColor(src, dst, COLOR_BGR2GRAY); //converts the source image to a grayscale one

	imwrite("Images/Lena_24bits_gray.bmp", dst); //writes the destination to file

	imshow(WIN_SRC, src);
	imshow(WIN_DST, dst);

	waitKey(0);
}

void testNegativeImage()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		double t = (double)getTickCount(); // Get the current time [s]

		Mat src = imread(fname, IMREAD_GRAYSCALE);
		int height = src.rows;
		int width = src.cols;
		Mat dst = Mat(height, width, CV_8UC1);
		// Accessing individual pixels in an 8 bits/pixel image
		// Inefficient way -> slow
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				uchar val = src.at<uchar>(i, j);
				uchar neg = 255 - val;
				dst.at<uchar>(i, j) = neg;
			}
		}

		// Get the current time again and compute the time difference [s]
		t = ((double)getTickCount() - t) / getTickFrequency();
		// Print (in the console window) the processing time in [ms] 
		printf("Time = %.3f [ms]\n", t * 1000);

		imshow("input image", src);
		imshow("negative image", dst);
		waitKey();
	}
}

void testNegativeImageFast()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src = imread(fname, IMREAD_GRAYSCALE);
		int height = src.rows;
		int width = src.cols;
		Mat dst = src.clone();

		double t = (double)getTickCount(); // Get the current time [s]

		// The fastest approach of accessing the pixels -> using pointers
		uchar* lpSrc = src.data;
		uchar* lpDst = dst.data;
		int w = (int)src.step; // no dword alignment is done !!!
		for (int i = 0; i < height; i++)
			for (int j = 0; j < width; j++) {
				uchar val = lpSrc[i * w + j];
				lpDst[i * w + j] = 255 - val;
			}

		// Get the current time again and compute the time difference [s]
		t = ((double)getTickCount() - t) / getTickFrequency();
		// Print (in the console window) the processing time in [ms] 
		printf("Time = %.3f [ms]\n", t * 1000);

		imshow("input image", src);
		imshow("negative image", dst);
		waitKey();
	}
}

void testColor2Gray()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src = imread(fname);

		int height = src.rows;
		int width = src.cols;

		Mat dst = Mat(height, width, CV_8UC1);

		// Accessing individual pixels in a RGB 24 bits/pixel image
		// Inefficient way -> slow
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				Vec3b v3 = src.at<Vec3b>(i, j);
				uchar b = v3[0];
				uchar g = v3[1];
				uchar r = v3[2];
				dst.at<uchar>(i, j) = (r + g + b) / 3;
			}
		}

		imshow("input image", src);
		imshow("gray image", dst);
		waitKey();
	}
}

void testBGR2HSV()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src = imread(fname);
		int height = src.rows;
		int width = src.cols;

		// HSV components
		Mat H = Mat(height, width, CV_8UC1);
		Mat S = Mat(height, width, CV_8UC1);
		Mat V = Mat(height, width, CV_8UC1);

		// Defining pointers to each matrix (8 bits/pixels) of the individual components H, S, V 
		uchar* lpH = H.data;
		uchar* lpS = S.data;
		uchar* lpV = V.data;

		Mat hsvImg;
		cvtColor(src, hsvImg, COLOR_BGR2HSV);

		// Defining the pointer to the HSV image matrix (24 bits/pixel)
		uchar* hsvDataPtr = hsvImg.data;

		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				int hi = i * width * 3 + j * 3;
				int gi = i * width + j;

				lpH[gi] = hsvDataPtr[hi] * 510 / 360;	// lpH = 0 .. 255
				lpS[gi] = hsvDataPtr[hi + 1];			// lpS = 0 .. 255
				lpV[gi] = hsvDataPtr[hi + 2];			// lpV = 0 .. 255
			}
		}

		imshow("input image", src);
		imshow("H", H);
		imshow("S", S);
		imshow("V", V);

		waitKey();
	}
}

void testResize()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src;
		src = imread(fname);
		Mat dst1, dst2;
		//without interpolation
		resizeImg(src, dst1, 320, false);
		//with interpolation
		resizeImg(src, dst2, 320, true);
		imshow("input image", src);
		imshow("resized image (without interpolation)", dst1);
		imshow("resized image (with interpolation)", dst2);
		waitKey();
	}
}

void testCanny()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src, dst, gauss;
		src = imread(fname, IMREAD_GRAYSCALE);
		double k = 0.4;
		int pH = 50;
		int pL = (int)k * pH;
		GaussianBlur(src, gauss, Size(5, 5), 0.8, 0.8);
		Canny(gauss, dst, pL, pH, 3);
		imshow("input image", src);
		imshow("canny", dst);
		waitKey();
	}
}

void testVideoSequence()
{
	_wchdir(projectPath);

	VideoCapture cap("Videos/rubic.avi"); // off-line video from file
	//VideoCapture cap(0);	// live video from web cam
	if (!cap.isOpened()) {
		printf("Cannot open video capture device.\n");
		waitKey(0);
		return;
	}

	Mat edges;
	Mat frame;
	char c;

	while (cap.read(frame))
	{
		Mat grayFrame;
		cvtColor(frame, grayFrame, COLOR_BGR2GRAY);
		Canny(grayFrame, edges, 40, 100, 3);
		imshow("source", frame);
		imshow("gray", grayFrame);
		imshow("edges", edges);
		c = waitKey(100);  // waits 100ms and advances to the next frame
		if (c == 27) {
			// press ESC to exit
			printf("ESC pressed - capture finished\n");
			break;  //ESC pressed
		};
	}
}


void testSnap()
{
	_wchdir(projectPath);

	VideoCapture cap(0); // open the deafult camera (i.e. the built in web cam)
	if (!cap.isOpened()) // openenig the video device failed
	{
		printf("Cannot open video capture device.\n");
		return;
	}

	Mat frame;
	char numberStr[256];
	char fileName[256];

	// video resolution
	Size capS = Size((int)cap.get(CAP_PROP_FRAME_WIDTH),
		(int)cap.get(CAP_PROP_FRAME_HEIGHT));

	// Display window
	const char* WIN_SRC = "Src"; //window for the source frame
	namedWindow(WIN_SRC, WINDOW_AUTOSIZE);
	moveWindow(WIN_SRC, 0, 0);

	const char* WIN_DST = "Snapped"; //window for showing the snapped frame
	namedWindow(WIN_DST, WINDOW_AUTOSIZE);
	moveWindow(WIN_DST, capS.width + 10, 0);

	char c;
	int frameNum = -1;
	int frameCount = 0;

	for (;;)
	{
		cap >> frame; // get a new frame from camera
		if (frame.empty())
		{
			printf("End of the video file\n");
			break;
		}

		++frameNum;

		imshow(WIN_SRC, frame);

		c = waitKey(10);  // waits a key press to advance to the next frame
		if (c == 27) {
			// press ESC to exit
			printf("ESC pressed - capture finished");
			break;  //ESC pressed
		}
		if (c == 115) { //'s' pressed - snap the image to a file
			frameCount++;
			fileName[0] = NULL;
			sprintf(numberStr, "%d", frameCount);
			strcat(fileName, "Images/A");
			strcat(fileName, numberStr);
			strcat(fileName, ".bmp");
			bool bSuccess = imwrite(fileName, frame);
			if (!bSuccess)
			{
				printf("Error writing the snapped image\n");
			}
			else
				imshow(WIN_DST, frame);
		}
	}

}

void MyCallBackFunc(int event, int x, int y, int flags, void* param)
{
	//More examples: http://opencvexamples.blogspot.com/2014/01/detect-mouse-clicks-and-moves-on-image.html
	Mat* src = (Mat*)param;
	if (event == EVENT_LBUTTONDOWN)
	{
		printf("Pos(x,y): %d,%d  Color(RGB): %d,%d,%d\n",
			x, y,
			(int)(*src).at<Vec3b>(y, x)[2],
			(int)(*src).at<Vec3b>(y, x)[1],
			(int)(*src).at<Vec3b>(y, x)[0]);
	}
}

void testMouseClick()
{
	Mat src;
	// Read image from file 
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		src = imread(fname);
		//Create a window
		namedWindow("My Window", 1);

		//set the callback function for any mouse event
		setMouseCallback("My Window", MyCallBackFunc, &src);

		//show the image
		imshow("My Window", src);

		// Wait until user press some key
		waitKey(0);
	}
}

/* Histogram display function - display a histogram using bars (simlilar to L3 / Image Processing)
Input:
name - destination (output) window name
hist - pointer to the vector containing the histogram values
hist_cols - no. of bins (elements) in the histogram = histogram image width
hist_height - height of the histogram image
Call example:
showHistogram ("MyHist", hist_dir, 255, 200);
*/
void showHistogram(const std::string& name, int* hist, const int  hist_cols, const int hist_height)
{
	Mat imgHist(hist_height, hist_cols, CV_8UC3, CV_RGB(255, 255, 255)); // constructs a white image

	//computes histogram maximum
	int max_hist = 0;
	for (int i = 0; i < hist_cols; i++)
		if (hist[i] > max_hist)
			max_hist = hist[i];
	double scale = 1.0;
	scale = (double)hist_height / max_hist;
	int baseline = hist_height - 1;

	for (int x = 0; x < hist_cols; x++) {
		Point p1 = Point(x, baseline);
		Point p2 = Point(x, baseline - cvRound(hist[x] * scale));
		line(imgHist, p1, p2, CV_RGB(255, 0, 255)); // histogram bins colored in magenta
	}

	imshow(name, imgHist);
}


//operatii morfologice implementate manual pentru masti binare
//mastile obtinute cu inRange au obiectul alb (255)
// si fundalul negru (0), deci functiile de mai jos lucreaza cu foreground = 255.
bool isInside(const Mat& img, int i, int j)
{
	return i >= 0 && i < img.rows && j >= 0 && j < img.cols;
}

vector<Point> getElementStructural(int vecinatate)
{
	vector<Point> B;

	if (vecinatate == 4)
	{
		B.push_back(Point(0, -1));
		B.push_back(Point(-1, 0));
		B.push_back(Point(0, 0));
		B.push_back(Point(1, 0));
		B.push_back(Point(0, 1));
	}
	else //sau 8
	{
		for (int dy = -1; dy <= 1; dy++)
			for (int dx = -1; dx <= 1; dx++)
				B.push_back(Point(dx, dy));
	}

	return B;
}

vector<Point> getElementStructuralRect(int size)
{
	vector<Point> B;
	int r = size / 2;

	for (int dy = -r; dy <= r; dy++)
		for (int dx = -r; dx <= r; dx++)
			B.push_back(Point(dx, dy));

	return B;
}

vector<Point> getElementStructuralEllipse(int size)
{
	vector<Point> B;
	int r = size / 2;
	double rr = (double)r * (double)r;

	for (int dy = -r; dy <= r; dy++)
		for (int dx = -r; dx <= r; dx++)
			if ((double)(dx * dx + dy * dy) <= rr)
				B.push_back(Point(dx, dy));

	return B;
}

Mat dilatare(const Mat& src, const vector<Point>& B)
{
	Mat dst(src.rows, src.cols, CV_8UC1, Scalar(0));

	for (int i = 0; i < src.rows; i++)
		for (int j = 0; j < src.cols; j++)
			if (src.at<uchar>(i, j) == 255)
				for (int k = 0; k < (int)B.size(); k++)
				{
					int ni = i + B[k].y;
					int nj = j + B[k].x;

					if (isInside(src, ni, nj))
						dst.at<uchar>(ni, nj) = 255;
				}

	return dst;
}

Mat eroziune(const Mat& src, const vector<Point>& B)
{
	Mat dst(src.rows, src.cols, CV_8UC1, Scalar(0));

	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			if (src.at<uchar>(i, j) == 255)
			{
				bool ok = true;

				for (int k = 0; k < (int)B.size(); k++)
				{
					int ni = i + B[k].y;
					int nj = j + B[k].x;

					if (!isInside(src, ni, nj) || src.at<uchar>(ni, nj) != 255)
					{
						ok = false;
						break;
					}
				}

				if (ok)
					dst.at<uchar>(i, j) = 255;
			}
		}
	}

	return dst;
}

Mat deschidere(const Mat& src, const vector<Point>& B)
{
	return dilatare(eroziune(src, B), B);
}

Mat inchidere(const Mat& src, const vector<Point>& B)
{
	return eroziune(dilatare(src, B), B);
}

Mat dilatareRepetata(const Mat& src, const vector<Point>& B, int iterations)
{
	Mat dst = src.clone();

	for (int it = 0; it < iterations; it++)
		dst = dilatare(dst, B);

	return dst;
}


Mat myInRange(const Mat& src, Scalar lower, Scalar upper)
{
	Mat dst(src.rows, src.cols, CV_8UC1);

	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			Vec3b pixel = src.at<Vec3b>(i, j);

			bool ok = true;

			for (int c = 0; c < 3; c++)
			{
				if (pixel[c] < lower[c] || pixel[c] > upper[c])
				{
					ok = false;
					break;
				}
			}

			dst.at<uchar>(i, j) = ok ? 255 : 0;
		}
	}

	return dst;
}

int myCountNonZero(const Mat& src)
{
	int count = 0;

	for (int i = 0; i < src.rows; i++)
	{
		const uchar* row = src.ptr<uchar>(i);

		for (int j = 0; j < src.cols; j++)
		{
			if (row[j] != 0)
				count++;
		}
	}

	return count;
}
// -------------
bool touchesImageBorder(const Rect& box, const Mat& img)
{
	int margin = 5;

	if (box.x <= margin || box.y <= margin)
		return true;

	if (box.x + box.width >= img.cols - margin)
		return true;

	if (box.y + box.height >= img.rows - margin)
		return true;

	return false;
}

bool isValidCandidate(Rect box, double area, const Mat& img)
{
	if (area < 700)
		return false;

	if (box.width < 28 || box.height < 28)
		return false;

	double ratio = (double)box.width / (double)box.height;
	if (ratio < 0.4 || ratio > 2.0)
		return false;

	if (touchesImageBorder(box, img))
		return false;

	return true;
}

std::string getShapeName(int corners, double area, double perimeter)
{
	if (perimeter <= 0)
		return "Forma necunoscuta";

	double circularity = 4 * CV_PI * area / (perimeter * perimeter);

	if (corners == 3)
		return "Triunghi";

	if (corners == 4 || corners == 5)
		return "Patrulater";

	if (corners == 8)
		return "Octogon";

	if (circularity > 0.70 && corners > 8)
		return "Cerc";

	return "Forma necunoscuta";
}

double computeCircularity(double area, double perimeter)
{
	if (perimeter <= 0.0)
		return 0.0;

	return 4.0 * CV_PI * area / (perimeter * perimeter);
}

double getWhiteRatioInBox(const Mat& hsv, const Rect& box)
{
	Rect safeBox = box & Rect(0, 0, hsv.cols, hsv.rows);
	if (safeBox.width <= 0 || safeBox.height <= 0)
		return 0.0;

	Mat roi = hsv(safeBox);

	Mat maskWhite;
	maskWhite = myInRange(roi, Scalar(0, 0, 120), Scalar(180, 160, 255));
	//inRange(roi, Scalar(0, 0, 120), Scalar(180, 160, 255), maskWhite);

	vector<Point> kernel = getElementStructural(8);
	maskWhite = deschidere(maskWhite, kernel);
	maskWhite = inchidere(maskWhite, kernel);

	int whitePixels = myCountNonZero(maskWhite);
	int totalPixels = safeBox.width * safeBox.height;

	if (totalPixels == 0)
		return 0.0;

	return (double)whitePixels / (double)totalPixels;
}

double getRedRatioInBox(const Mat& hsv, const Rect& box)
{
	Rect safeBox = box & Rect(0, 0, hsv.cols, hsv.rows);
	if (safeBox.width <= 0 || safeBox.height <= 0)
		return 0.0;

	Mat roi = hsv(safeBox);

	Mat maskRed1, maskRed2, maskRed;
	maskRed1 = myInRange(roi, Scalar(0, 70, 70), Scalar(10, 255, 255));
	maskRed2 = myInRange(roi, Scalar(170, 70, 70), Scalar(180, 255, 255));
	//inRange(roi, Scalar(0, 70, 70), Scalar(10, 255, 255), maskRed1);
	//inRange(roi, Scalar(170, 70, 70), Scalar(180, 255, 255), maskRed2);
	bitwise_or(maskRed1, maskRed2, maskRed);

	vector<Point> kernel = getElementStructural(8);
	maskRed = deschidere(maskRed, kernel);
	maskRed = inchidere(maskRed, kernel);

	int redPixels = myCountNonZero(maskRed);
	int totalPixels = safeBox.width * safeBox.height;

	if (totalPixels == 0)
		return 0.0;

	return (double)redPixels / (double)totalPixels;
}

double getBlackRatioInBox(const Mat& hsv, const Rect& box)
{
	Rect safeBox = box & Rect(0, 0, hsv.cols, hsv.rows);
	if (safeBox.width <= 0 || safeBox.height <= 0)
		return 0.0;

	Mat roi = hsv(safeBox);

	Mat maskBlack;
	maskBlack = myInRange(roi, Scalar(0, 0, 0), Scalar(180, 255, 70));
	//inRange(roi, Scalar(0, 0, 0), Scalar(180, 255, 70), maskBlack);

	vector<Point> kernel = getElementStructural(8);
	maskBlack = deschidere(maskBlack, kernel);
	maskBlack = inchidere(maskBlack, kernel);

	int blackPixels = myCountNonZero(maskBlack);
	int totalPixels = safeBox.width * safeBox.height;

	if (totalPixels == 0)
		return 0.0;

	return (double)blackPixels / (double)totalPixels;
}



bool hasDirectionalArrowInside(const Mat& hsv, const Rect& box)
{
	Rect safeBox = box & Rect(0, 0, hsv.cols, hsv.rows);
	if (safeBox.width <= 0 || safeBox.height <= 0)
		return false;

	Mat roi = hsv(safeBox);

	Mat maskWhite;
	maskWhite = myInRange(roi, Scalar(0, 0, 150), Scalar(180, 130, 255));
	//inRange(roi, Scalar(0, 0, 150), Scalar(180, 130, 255), maskWhite);

	vector<Point> kernel = getElementStructural(8);
	vector<Point> kernelBig = getElementStructuralEllipse(5);

	maskWhite = deschidere(maskWhite, kernel);
	maskWhite = inchidere(maskWhite, kernelBig);
	maskWhite = inchidere(maskWhite, kernelBig);

	std::vector<std::vector<Point>> contours;
	std::vector<Vec4i> hierarchy;
	findContours(maskWhite, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	int countValid = 0;
	double bestArea = 0.0;
	int bestIdx = -1;

	for (int i = 0; i < (int)contours.size(); i++)
	{
		double a = contourArea(contours[i]);
		if (a < 35)
			continue;

		countValid++;

		if (a > bestArea)
		{
			bestArea = a;
			bestIdx = i;
		}
	}

	if (bestIdx == -1)
		return false;

	Rect wb = boundingRect(contours[bestIdx]);

	double areaRatio = bestArea / (double)(safeBox.width * safeBox.height);
	double wRatio = (double)wb.width / (double)safeBox.width;
	double hRatio = (double)wb.height / (double)safeBox.height;
	double aspectHW = (double)wb.height / (double)wb.width;

	// in imagini reale sageata poate fi sparta in mai multe parti
	if (countValid > 6)
		return false;

	if (areaRatio < 0.06 || areaRatio > 0.50)
		return false;

	if (wRatio < 0.18 || wRatio > 0.60)
		return false;

	if (hRatio < 0.35 || hRatio > 0.90)
		return false;

	if (aspectHW < 1.3)
		return false;

	int cx = wb.x + wb.width / 2;
	if (cx < safeBox.width * 0.20 || cx > safeBox.width * 0.80)
		return false;

	return true;
}

bool hasSingleWhiteArrowBlob(const Mat& hsv, const Rect& box)
{
	Rect safeBox = box & Rect(0, 0, hsv.cols, hsv.rows);
	if (safeBox.width <= 0 || safeBox.height <= 0)
		return false;

	Mat roi = hsv(safeBox);

	Mat maskWhite;
	maskWhite = myInRange(roi, Scalar(0, 0, 160), Scalar(180, 110, 255));
	//inRange(roi, Scalar(0, 0, 160), Scalar(180, 110, 255), maskWhite);

	vector<Point> kernel = getElementStructural(8);
	maskWhite = deschidere(maskWhite, kernel);
	maskWhite = inchidere(maskWhite, kernel);

	std::vector<std::vector<Point>> contours;
	std::vector<Vec4i> hierarchy;
	findContours(maskWhite, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	int countValid = 0;
	double maxArea = 0.0;
	int bestIdx = -1;

	for (int i = 0; i < (int)contours.size(); i++)
	{
		double a = contourArea(contours[i]);
		if (a < 40)
			continue;

		countValid++;

		if (a > maxArea)
		{
			maxArea = a;
			bestIdx = i;
		}
	}

	if (bestIdx == -1)
		return false;

	Rect wb = boundingRect(contours[bestIdx]);

	int margin = 3;

	// daca blobul dominant atinge marginea ROI, cel mai probabil nu este sageata unui semn,
	// ci un obiect mare partial capturat (masina, bord, reflexie etc.)
	if (wb.x <= margin || wb.y <= margin ||
		wb.x + wb.width >= safeBox.width - margin ||
		wb.y + wb.height >= safeBox.height - margin)
	{
		return false;
	}

	double areaRatio = maxArea / (double)(safeBox.width * safeBox.height);
	double wRatio = (double)wb.width / (double)safeBox.width;
	double hRatio = (double)wb.height / (double)safeBox.height;

	double aspect1 = (double)wb.width / (double)wb.height;
	double aspect2 = (double)wb.height / (double)wb.width;
	double elongation = (std::max)(aspect1, aspect2);

	// pentru sens unic acceptam pana la 4 componente valide
	if (countValid > 4)
		return false;

	if (areaRatio < 0.05 || areaRatio > 0.70)
		return false;

	if (elongation < 1.15)
		return false;

	if (wRatio < 0.10 || wRatio > 0.85)
		return false;

	if (hRatio < 0.18 || hRatio > 0.95)
		return false;

	return true;
}

bool hasWhiteHorizontalBar(const Mat& hsv, const Rect& box)
{
	Rect safeBox = box & Rect(0, 0, hsv.cols, hsv.rows);
	if (safeBox.width <= 0 || safeBox.height <= 0)
		return false;

	Mat roi = hsv(safeBox);

	Mat maskWhite;
	maskWhite = myInRange(roi, Scalar(0, 0, 170), Scalar(180, 90, 255));
	//inRange(roi, Scalar(0, 0, 170), Scalar(180, 90, 255), maskWhite);

	vector<Point> kernel = getElementStructural(8);
	maskWhite = deschidere(maskWhite, kernel);
	maskWhite = inchidere(maskWhite, kernel);

	std::vector<std::vector<Point>> contours;
	std::vector<Vec4i> hierarchy;
	findContours(maskWhite, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	for (int i = 0; i < (int)contours.size(); i++)
	{
		double area = contourArea(contours[i]);
		if (area < 80)
			continue;

		Rect wb = boundingRect(contours[i]);

		double wRatio = (double)wb.width / (double)roi.cols;
		double hRatio = (double)wb.height / (double)roi.rows;
		double aspect = (double)wb.width / (double)wb.height;

		int centerY = wb.y + wb.height / 2;
		bool isCentered = (centerY > roi.rows * 0.35 && centerY < roi.rows * 0.65);

		if (wRatio > 0.45 &&
			hRatio > 0.12 && hRatio < 0.45 &&
			aspect > 2.0 &&
			isCentered)
		{
			return true;
		}
	}

	return false;
}

bool hasWhiteTriangleWithBlackDetails(const Mat& hsv, const Rect& box)
{
	Rect safeBox = box & Rect(0, 0, hsv.cols, hsv.rows);
	if (safeBox.width <= 0 || safeBox.height <= 0)
	{
		//printf("FAIL: safeBox invalid\n");
		return false;
	}

	Mat roi = hsv(safeBox);

	//masca albului
	Mat maskWhite;
	maskWhite = myInRange(roi, Scalar(0, 0, 150), Scalar(180, 120, 255));
	//inRange(roi, Scalar(0, 0, 150), Scalar(180, 120, 255), maskWhite);

	//inchidere mai puternica, ca sa umple golurile produse de pieton si zebra
	vector<Point> kernelBig = getElementStructuralEllipse(7);
	vector<Point> kernelSmall = getElementStructural(8);

	maskWhite = inchidere(maskWhite, kernelBig);
	maskWhite = inchidere(maskWhite, kernelBig);
	maskWhite = deschidere(maskWhite, kernelSmall);

	std::vector<std::vector<Point>> contours;
	std::vector<Vec4i> hierarchy;
	findContours(maskWhite, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);


	int bestIdx = -1;
	double bestArea = 0.0;
	std::vector<Point> bestApprox;

	for (int i = 0; i < (int)contours.size(); i++)
	{
		double area = contourArea(contours[i]);
		if (area < 50)
		{
			continue;
		}

		double perimeter = arcLength(contours[i], true);
		if (perimeter <= 0.0)
		{
			continue;
		}

		std::vector<Point> approx;
		approxPolyDP(contours[i], approx, 0.06 * perimeter, true);

		Rect triBox = boundingRect(contours[i]);

		double areaRatio = area / (double)(safeBox.width * safeBox.height);
		double wRatio = (double)triBox.width / (double)safeBox.width;
		double hRatio = (double)triBox.height / (double)safeBox.height;

		// acceptam si 3, si 4 colturi, fiindca dupa closing poate iesi usor deformata
		if ((int)approx.size() < 3 || (int)approx.size() > 6)
		{
			//printf(" -> respins: nu are 3/4 colturi\n");
			continue;
		}

		if (areaRatio < 0.04 || areaRatio > 0.90)
		{
			//printf(" -> respins: areaRatio\n");
			continue;
		}

		if (wRatio < 0.15 || wRatio > 1.00)
		{
			//printf(" -> respins: wRatio\n");
			continue;
		}

		if (hRatio < 0.15 || hRatio > 1.00)
		{
			//printf(" -> respins: hRatio\n");
			continue;
		}

		if (area > bestArea)
		{
			bestArea = area;
			bestIdx = i;
			bestApprox = approx;
		}
	}

	if (bestIdx == -1)
	{
		//printf("FAIL: nu a gasit triunghi alb valid\n");
		return false;
	}

	//masca triunghiului/regiunii albe dominante
	Mat triangleMask = Mat::zeros(roi.size(), CV_8UC1);
	drawContours(triangleMask, contours, bestIdx, Scalar(255), FILLED);

	//cautam negrul in interior
	Mat maskBlack;
	maskBlack = myInRange(roi, Scalar(0, 0, 0), Scalar(180, 255, 90));
	//inRange(roi, Scalar(0, 0, 0), Scalar(180, 255, 90), maskBlack);
	maskBlack = deschidere(maskBlack, kernelSmall);

	Mat blackInsideTriangle;
	bitwise_and(maskBlack, triangleMask, blackInsideTriangle);

	int blackPixels = myCountNonZero(blackInsideTriangle);
	int trianglePixels = myCountNonZero(triangleMask);

	if (trianglePixels == 0)
	{
		//printf("FAIL: trianglePixels = 0\n");
		return false;
	}

	double blackRatioInsideTriangle = (double)blackPixels / (double)trianglePixels;

	// relaxam putin pragul
	if (blackRatioInsideTriangle < 0.005)
	{
		//printf("FAIL: blackRatioInsideTriangle prea mic\n");
		return false;
	}
	return true;
}


bool isLikelyStopSign(const Mat& hsv, const Rect& box, double area)
{
	if (box.width < 35 || box.height < 35)
		return false;

	double ratio = (double)box.width / (double)box.height;
	if (ratio < 0.85 || ratio > 1.15)
		return false;


	double rectArea = (double)box.width * (double)box.height;
	double fillRatio = area / rectArea;
	if (fillRatio < 0.65 || fillRatio > 0.90)
		return false;

	Rect safeBox = box & Rect(0, 0, hsv.cols, hsv.rows);
	if (safeBox.width <= 0 || safeBox.height <= 0)
		return false;

	Mat roi = hsv(safeBox);

	Mat maskWhite;
	maskWhite = myInRange(roi, Scalar(0, 0, 170), Scalar(180, 90, 255));
	//inRange(roi, Scalar(0, 0, 170), Scalar(180, 90, 255), maskWhite);

	int whitePixels = myCountNonZero(maskWhite);
	int totalPixels = safeBox.width * safeBox.height;

	if (totalPixels == 0)
		return false;

	double whiteRatio = (double)whitePixels / (double)totalPixels;

	// STOP are de obicei putin alb din litere, dar nu foarte mult
	if (whiteRatio < 0.01 || whiteRatio > 0.25)
		return false;

	return true;
}

bool isLikelyYieldSign(const Mat& hsv, const Rect& box, double area)
{
	if (box.width < 20 || box.height < 20)
		return false;

	double ratio = (double)box.width / (double)box.height;
	if (ratio < 0.55 || ratio > 1.45)
		return false;

	double rectArea = (double)box.width * (double)box.height;
	if (rectArea <= 0)
		return false;

	double fillRatio = area / rectArea;
	if (fillRatio < 0.20 || fillRatio > 0.85)
		return false;

	double whiteRatio = getWhiteRatioInBox(hsv, box);

	// la Cedeaza ar trebui sa existe destul alb in interior
	if (whiteRatio < 0.28)
		return false;

	return true;
}


double getBlueRatioInBox(const Mat& hsv, const Rect& box)
{
	Rect safeBox = box & Rect(0, 0, hsv.cols, hsv.rows);
	if (safeBox.width <= 0 || safeBox.height <= 0)
		return 0.0;

	Mat roi = hsv(safeBox);

	Mat maskBlue;
	maskBlue = myInRange(roi, Scalar(95, 60, 40), Scalar(135, 255, 255));
	// prag mai larg pentru albastru interior
	//inRange(roi, Scalar(95, 60, 40), Scalar(135, 255, 255), maskBlue);

	int bluePixels = myCountNonZero(maskBlue);
	int totalPixels = safeBox.width * safeBox.height;

	if (totalPixels == 0)
		return 0.0;

	return (double)bluePixels / (double)totalPixels;
}


bool isLikelyNoStoppingSign(const Mat& hsv, const Rect& box, double area, double perimeter)
{
	if (box.width < 25 || box.height < 25)
		return false;

	double ratio = (double)box.width / (double)box.height;
	if (ratio < 0.80 || ratio > 1.20)
		return false;

	double circularity = computeCircularity(area, perimeter);
	if (circularity < 0.60)
		return false;

	double blueRatio = getBlueRatioInBox(hsv, box);
	double redRatio = getRedRatioInBox(hsv, box);
	double whiteRatio = getWhiteRatioInBox(hsv, box);
	if (blueRatio < 0.20)
		return false;

	if (redRatio < 0.10)
		return false;

	//sa nu fie acces interzis
	if (hasWhiteHorizontalBar(hsv, box))
		return false;

	//prea mult alb nu e specific
	if (whiteRatio > 0.25)
		return false;

	return true;
}

bool isLikelyPedestrianCrossingSignRelaxed(const Mat& hsv, const Rect& box, double area, double perimeter)
{
	Rect safeBox = box & Rect(0, 0, hsv.cols, hsv.rows);
	if (safeBox.width <= 0 || safeBox.height <= 0)
		return false;

	if (safeBox.width < 45 || safeBox.height < 45)
		return false;

	if (area < 1800)
		return false;

	double ratio = (double)safeBox.width / (double)safeBox.height;
	if (ratio < 0.80 || ratio > 1.15)
		return false;

	double rectArea = (double)safeBox.width * (double)safeBox.height;
	if (rectArea <= 0.0)
		return false;

	double fillRatio = area / rectArea;
	if (fillRatio < 0.35 || fillRatio > 0.98)
		return false;

	double blueRatio = getBlueRatioInBox(hsv, safeBox);
	double whiteRatio = getWhiteRatioInBox(hsv, safeBox);
	double blackRatio = getBlackRatioInBox(hsv, safeBox);
	bool arrowBlob = hasSingleWhiteArrowBlob(hsv, safeBox);

	if (blueRatio < 0.20)
		return false;

	// la trecere de pietoni vrem destul alb
	if (whiteRatio < 0.22)
		return false;

	// trebuie sa existe macar putin negru in interior
	if (blackRatio < 0.01)
		return false;

	// daca seamana clar cu o sageata, nu e trecere
	if (arrowBlob)
		return false;

	return true;
}

std::string recognizeTrafficSign(
	const std::string& shapeName,
	const std::string& colorName,
	const Mat& hsv,
	const Rect& box,
	double area,
	double perimeter,
	int corners)
{
	double blueRatio = getBlueRatioInBox(hsv, box);
	double ratio = (double)box.width / (double)box.height;
	double rectArea = (double)box.width * (double)box.height;
	double fillRatio = area / rectArea;


	// Cedeaza = rosu + triunghi
	//printf("Corners: %d  ", corners);
	//printf("ShapeName: %s\n\n", shapeName.c_str());
	printf("[recognizeTrafficSign] color=%s area=%.2f perimeter=%.2f box=(%d,%d,%d,%d)\n",
		colorName.c_str(), area, perimeter, box.x, box.y, box.width, box.height);

	//Cedeaza trecerea
	if ((corners == 3 || corners == 4) && colorName == "Rosu" && isLikelyYieldSign(hsv, box, area))
		return "Cedeaza";

	if ((colorName == "Rosu" || colorName == "Albastru") && (shapeName == "Cerc" || corners >= 6))
	{
		//Acces Interzis = cerc + rosu + bara alba orizontala
		if (hasWhiteHorizontalBar(hsv, box))
			return "Acces interzis";

		//Oprire interzisa = cerc + rosu/albastru
		if (isLikelyNoStoppingSign(hsv, box, area, perimeter))
			return "Oprire interzisa";
	}

	// STOP = rosu + 8 colturi + fara bara alba
	if (colorName == "Rosu" && corners == 8 && shapeName == "Octogon" && isLikelyStopSign(hsv, box, area) && !hasWhiteHorizontalBar(hsv, box) && blueRatio < 0.10)
		return "STOP";

	// Drum cu prioritate = patrulater 4/5 colturi + galben
	if (colorName == "Galben" && (shapeName == "Patrulater" || corners == 4 || corners == 5))
	{
		if (ratio > 0.75 && ratio < 1.25)
			return "Drum cu prioritate";
	}

	// Sens unic = semn albastru patrulater + o singura sageata alba dominanta
	if (colorName == "Albastru" &&
		(shapeName == "Patrulater" || corners == 4 || corners == 5))
	{
		bool arrowBlob = hasSingleWhiteArrowBlob(hsv, box);
		bool pedRelaxed = isLikelyPedestrianCrossingSignRelaxed(hsv, box, area, perimeter);
		bool pedStrict = hasWhiteTriangleWithBlackDetails(hsv, box);

		if (arrowBlob && !pedStrict && !pedRelaxed)
			return "Sens unic";
	}


	// Trecere de pietoni = semn albastru patrat + triunghi alb + detalii negre in interior
	if (colorName == "Albastru" &&
		(shapeName == "Patrulater" || corners == 4 || corners == 5))
	{
		bool pedStrict = hasWhiteTriangleWithBlackDetails(hsv, box);
		bool pedRelaxed = isLikelyPedestrianCrossingSignRelaxed(hsv, box, area, perimeter);

		if (pedStrict || pedRelaxed)
			return "Trecere pietoni";
	}

	// Obligatoriu inainte
	if (shapeName == "Cerc" && colorName == "Albastru" && hasDirectionalArrowInside(hsv, box))
		return "Obligatoriu inainte";


	return "Necunoscut";
}

void detectAndRecognizeCandidates(
	const Mat& mask,
	Mat& result,
	const std::string& colorName,
	const Mat& hsv)
{
	std::vector<std::vector<Point>> contours;
	std::vector<Vec4i> hierarchy;

	findContours(mask, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	for (int i = 0; i < (int)contours.size(); i++)
	{
		double area = contourArea(contours[i]);
		Rect box = boundingRect(contours[i]);

		if (!isValidCandidate(box, area, mask))
			continue;

		double perimeter = arcLength(contours[i], true);
		if (perimeter < 1.0)
			continue;

		std::string shapeName = "Forma necunoscuta";
		int bestCorners = 0;

		for (double eps : {0.015, 0.02, 0.025, 0.03})
		{
			std::vector<Point> approx;
			approxPolyDP(contours[i], approx, eps * perimeter, true);
			int corners = (int)approx.size();

			if (corners >= 3)
			{
				shapeName = getShapeName(corners, area, perimeter);
				bestCorners = corners;
				if (shapeName != "Forma necunoscuta")
					break;
			}
		}

		printf("\n--- CANDIDAT NOU ---\n");
		printf("Color: %s\n", colorName.c_str());
		printf("Box: x=%d y=%d w=%d h=%d\n", box.x, box.y, box.width, box.height);
		printf("Area: %.2f\n", area);
		printf("Perimeter: %.2f\n", perimeter);
		printf("Corners: %d\n", bestCorners);
		printf("ShapeName: %s\n", shapeName.c_str());

		double blueRatioDbg = getBlueRatioInBox(hsv, box);
		double whiteRatioDbg = getWhiteRatioInBox(hsv, box);
		double redRatioDbg = getRedRatioInBox(hsv, box);
		double circularityDbg = computeCircularity(area, perimeter);

		if (shapeName == "Forma necunoscuta")
		{
			std::vector<Point> approx;
			approxPolyDP(contours[i], approx, 0.02 * perimeter, true);
			bestCorners = (int)approx.size();
			shapeName = getShapeName(bestCorners, area, perimeter);
		}

		if (shapeName == "Forma necunoscuta")
			continue;

		std::vector<Point> approxFinal;
		approxPolyDP(contours[i], approxFinal, 0.02 * perimeter, true);

		std::string signName = recognizeTrafficSign(
			shapeName,
			colorName,
			hsv,
			box,
			area,
			perimeter,
			bestCorners);

		if (signName == "Necunoscut")
			continue;

		rectangle(result, box, Scalar(0, 255, 0), 2);

		std::vector<std::vector<Point>> drawVec;
		drawVec.push_back(approxFinal);
		drawContours(result, drawVec, 0, Scalar(255, 0, 0), 2);

		int textY = (box.y - 8 > 15) ? (box.y - 8) : 15;
		putText(result, signName,
			Point(box.x, textY),
			FONT_HERSHEY_SIMPLEX, 0.55,
			Scalar(0, 0, 255), 2);

		printf("%s\n", signName.c_str());
	}
}

void testTrafficSignRecognition()
{
	char fname[MAX_PATH];
	while (openFileDlg(fname))
	{
		Mat src = imread(fname);
		if (src.empty())
		{
			printf("Nu s-a putut incarca imaginea.\n");
			return;
		}

		Mat resized;
		resizeImg(src, resized, 700, true);

		// Ignoram doar partea foarte de jos (capota / reflexii)
		resized.rowRange((int)(resized.rows * 0.90), resized.rows)
			.setTo(Scalar(0, 0, 0));

		Mat blurred;
		GaussianBlur(resized, blurred, Size(5, 5), 1.2);

		Mat hsv;
		cvtColor(blurred, hsv, COLOR_BGR2HSV);

		Mat maskRed1, maskRed2, maskRed;
		maskRed1 = myInRange(hsv, Scalar(0, 60, 50), Scalar(12, 255, 255));
		maskRed2 = myInRange(hsv, Scalar(168, 60, 50), Scalar(180, 255, 255));
	/*	inRange(hsv, Scalar(0, 60, 50), Scalar(12, 255, 255), maskRed1);
		inRange(hsv, Scalar(168, 60, 50), Scalar(180, 255, 255), maskRed2);*/
		maskRed = maskRed1 | maskRed2;

		Mat maskBlue;
		maskBlue = myInRange(hsv, Scalar(95, 60, 40), Scalar(135, 255, 255));
		//inRange(hsv, Scalar(95, 60, 40), Scalar(135, 255, 255), maskBlue);

		Mat maskYellow;
		maskYellow = myInRange(hsv, Scalar(15, 60, 60), Scalar(40, 255, 255));
		//inRange(hsv, Scalar(15, 60, 60), Scalar(40, 255, 255), maskYellow);

		vector<Point> kernelSmall = getElementStructural(8);
		vector<Point> kernelBig = getElementStructuralRect(5);

		maskRed = deschidere(maskRed, kernelSmall);
		maskRed = inchidere(maskRed, kernelBig);

		maskBlue = deschidere(maskBlue, kernelSmall);
		maskBlue = inchidere(maskBlue, kernelBig);
		maskBlue = dilatareRepetata(maskBlue, kernelSmall, 2);

		maskYellow = deschidere(maskYellow, kernelSmall);
		maskYellow = inchidere(maskYellow, kernelBig);

		Mat result = resized.clone();

		detectAndRecognizeCandidates(maskRed, result, "Rosu", hsv);
		detectAndRecognizeCandidates(maskBlue, result, "Albastru", hsv);
		detectAndRecognizeCandidates(maskYellow, result, "Galben", hsv);

		// imshow("Imagine originala", resized);
		imshow("Masca rosu", maskRed);
		imshow("Masca albastru", maskBlue);
		imshow("Masca galben", maskYellow);
		imshow("Recunoastere semne", result);

		waitKey(0);
	}
}

int main()
{
	cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_FATAL);
	projectPath = _wgetcwd(0, 0);

	int op;
	do
	{
		system("cls");
		destroyAllWindows();
		printf("Menu:\n");
		printf(" 1 - Open image\n");
		printf(" 2 - Open BMP images from folder\n");
		printf(" 3 - Image negative\n");
		printf(" 4 - Image negative (fast)\n");
		printf(" 5 - BGR->Gray\n");
		printf(" 6 - BGR->Gray (fast, save result to disk) \n");
		printf(" 7 - BGR->HSV\n");
		printf(" 8 - Resize image\n");
		printf(" 9 - Canny edge detection\n");
		printf(" 10 - Edges in a video sequence\n");
		printf(" 11 - Snap frame from live video\n");
		printf(" 12 - Mouse callback demo\n");
		printf(" 13 - Detectare si recunoastere semne\n");
		printf(" 0 - Exit\n\n");
		printf("Option: ");
		scanf("%d", &op);
		switch (op)
		{
		case 1:
			testOpenImage();
			break;
		case 2:
			testOpenImagesFld();
			break;
		case 3:
			testNegativeImage();
			break;
		case 4:
			testNegativeImageFast();
			break;
		case 5:
			testColor2Gray();
			break;
		case 6:
			testImageOpenAndSave();
			break;
		case 7:
			testBGR2HSV();
			break;
		case 8:
			testResize();
			break;
		case 9:
			testCanny();
			break;
		case 10:
			testVideoSequence();
			break;
		case 11:
			testSnap();
			break;
		case 12:
			testMouseClick();
			break;
		case 13:
			testTrafficSignRecognition();
			break;
		}
	} while (op != 0);
	return 0;
}