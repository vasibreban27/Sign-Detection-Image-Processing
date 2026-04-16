// OpenCVApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"
#include <opencv2/core/utils/logger.hpp>

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
	double circularity = 0.0;
	if (perimeter > 0)
		circularity = 4.0 * CV_PI * area / (perimeter * perimeter);

	if (corners == 3)
		return "Triunghi";

	if (corners == 4)
		return "Patrulater";

	if (corners == 7 || corners == 8)
		return "Octogon";

	// cerc doar daca este foarte circular si nu seamana cu poligoane simple
	if (circularity > 0.88 && corners > 8)
		return "Cerc";

	return "Forma necunoscuta";
}

bool hasWhiteHorizontalBar(const Mat& hsv, const Rect& box)
{
	Rect safeBox = box & Rect(0, 0, hsv.cols, hsv.rows);
	if (safeBox.width <= 0 || safeBox.height <= 0)
		return false;

	Mat roi = hsv(safeBox);

	// masca pentru alb
	Mat maskWhite;
	inRange(roi, Scalar(0, 0, 180), Scalar(180, 70, 255), maskWhite);

	// luam doar banda centrala orizontala
	int y1 = roi.rows / 3;
	int y2 = 2 * roi.rows / 3;
	Rect centerBand(0, y1, roi.cols, y2 - y1);

	Mat whiteBand = maskWhite(centerBand);

	int whitePixels = countNonZero(whiteBand);
	int totalPixels = centerBand.width * centerBand.height;

	if (totalPixels == 0)
		return false;

	double whiteRatio = (double)whitePixels / (double)totalPixels;

	// daca exista suficient alb pe banda centrala, consideram ca are bara alba
	return whiteRatio > 0.25;
}

void testTrafficSignShapeDetection() {
	char fname[MAX_PATH];
	while (openFileDlg(fname)) {
		Mat src = imread(fname);

		Mat resized;
		resizeImg(src, resized, 700, true);

		Mat gray, blurred, edges;
		cvtColor(resized, gray, COLOR_BGR2GRAY); // transform img in gri 

		GaussianBlur(gray, blurred, Size(5, 5), 0); // reduce zgomotul pt a reduce muchiile false

		Canny(blurred, edges, 50, 150); // detectam muchiile
		// Canny ne scoate marginile obiectelor, deci putem merge mai departe pe conturi

		std::vector<std::vector<Point>> contours;
		std::vector<Vec4i> hierarchy;
		findContours(edges, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE); // gasim conturile obiectelor

		Mat result = resized.clone();

		double maxArea = 0.0;
		int bestIdx = -1;

		// alegem cel mai mare contur care ar putea fi semnul
		for (int i = 0; i < (int)contours.size(); i++)
		{
			double area = contourArea(contours[i]);
			if (area > 1000 && area > maxArea)
			{
				maxArea = area;
				bestIdx = i;
			}
		}

		if (bestIdx == -1)
		{
			printf("Nu am gasit un contur suficient de mare.\n");
			imshow("Imagine originala", resized);
			imshow("Muchii", edges);
			waitKey(0);
			continue;
		}

		std::vector<Point> approx;
		double perimeter = arcLength(contours[bestIdx], true);
		approxPolyDP(contours[bestIdx], approx, 0.01 * perimeter, true); // simplifica conturul real intr un poligon cu putine vf

		int corners = (int)approx.size(); // nr de colturi
		double area = contourArea(contours[bestIdx]);

		std::string shapeName = getShapeName(corners, area, perimeter);

		// desenam conturul aproximat
		std::vector<std::vector<Point>> drawVec;
		drawVec.push_back(approx);
		drawContours(result, drawVec, 0, Scalar(0, 255, 0), 3);

		// bounding box
		Rect box = boundingRect(approx);
		rectangle(result, box, Scalar(255, 0, 0), 2);

		// text
		std::string text1 = "Forma: " + shapeName;
		std::string text2 = "Colturi: " + std::to_string(corners);

		putText(result, text1, Point(box.x, box.y - 25), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 0, 255), 2);

		putText(result, text2, Point(box.x, box.y - 5), FONT_HERSHEY_SIMPLEX, 0.7, Scalar(0, 0, 255), 2);

		printf("Contur detectat:\n");
		printf(" - Arie: %.2f\n", area);
		printf(" - Perimetru: %.2f\n", perimeter);
		printf(" - Numar colturi: %d\n", corners);
		printf(" - Forma detectata: %s\n", shapeName.c_str());

		imshow("Imagine originala", resized);
		imshow("Gray", gray);
		imshow("Blurred", blurred);
		imshow("Muchii", edges);
		imshow("Rezultat forma", result);

		waitKey(0);
	}
}

void detectColorCandidates(const Mat& mask, Mat& result, const std::string& colorName)
{
	std::vector<std::vector<Point>> contours;
	std::vector<Vec4i> hierarchy;

	findContours(mask, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	for (int i = 0; i < (int)contours.size(); i++)
	{
		double area = contourArea(contours[i]);

		// ignoram regiunile foarte mici
		if (area < 500)
			continue;

		Rect box = boundingRect(contours[i]);

		// ignoram candidatele prea mici
		if (box.width < 20 || box.height < 20)
			continue;

		rectangle(result, box, Scalar(0, 255, 0), 2);

		std::string text = colorName;
		putText(result, text, Point(box.x, box.y - 5),
			FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 0, 255), 2);

		printf("[%s] Candidat gasit: x=%d y=%d w=%d h=%d area=%.2f\n",
			colorName.c_str(), box.x, box.y, box.width, box.height, area);
	}
}


bool isLikelyStopSign(const Rect& box, double area)
{
	double ratio = (double)box.width / (double)box.height;

	if (ratio < 0.8 || ratio > 1.2)
		return false;

	double rectArea = (double)box.width * (double)box.height;
	double fillRatio = area / rectArea;

	// daca forma ocupa prea putin sau prea mult din dreptunghi, e suspect
	if (fillRatio < 0.55 || fillRatio > 0.95)
		return false;

	return true;
}

void testTrafficSignColorCandidates()
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

		Mat blurred;
		GaussianBlur(resized, blurred, Size(5, 5), 0);

		Mat hsv;
		cvtColor(blurred, hsv, COLOR_BGR2HSV);

		// masti pentru culori
		Mat maskRed1, maskRed2, maskRed;
		Mat maskBlue, maskYellow;

		// ROSU
		inRange(hsv, Scalar(0, 70, 50), Scalar(10, 255, 255), maskRed1);
		inRange(hsv, Scalar(170, 70, 50), Scalar(180, 255, 255), maskRed2);
		maskRed = maskRed1 | maskRed2;

		// ALBASTRU
		inRange(hsv, Scalar(100, 80, 50), Scalar(130, 255, 255), maskBlue);

		// GALBEN
		inRange(hsv, Scalar(15, 80, 80), Scalar(35, 255, 255), maskYellow);

		// curatare masti
		Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));

		morphologyEx(maskRed, maskRed, MORPH_OPEN, kernel);
		morphologyEx(maskRed, maskRed, MORPH_CLOSE, kernel);

		morphologyEx(maskBlue, maskBlue, MORPH_OPEN, kernel);
		morphologyEx(maskBlue, maskBlue, MORPH_CLOSE, kernel);

		morphologyEx(maskYellow, maskYellow, MORPH_OPEN, kernel);
		morphologyEx(maskYellow, maskYellow, MORPH_CLOSE, kernel);

		Mat result = resized.clone();

		detectColorCandidates(maskRed, result, "Rosu");
		detectColorCandidates(maskBlue, result, "Albastru");
		detectColorCandidates(maskYellow, result, "Galben");

		imshow("Imagine originala", resized);
		imshow("HSV", hsv);
		imshow("Masca rosu", maskRed);
		imshow("Masca albastru", maskBlue);
		imshow("Masca galben", maskYellow);
		imshow("Candidate dupa culoare", result);

		waitKey(0);
	}
}


double getBlueRatioInBox(const Mat& hsv, const Rect& box)
{
	Rect safeBox = box & Rect(0, 0, hsv.cols, hsv.rows);
	if (safeBox.width <= 0 || safeBox.height <= 0)
		return 0.0;

	Mat roi = hsv(safeBox);

	Mat maskBlue;
	// prag mai larg pentru albastru interior
	inRange(roi, Scalar(95, 60, 40), Scalar(135, 255, 255), maskBlue);

	int bluePixels = countNonZero(maskBlue);
	int totalPixels = safeBox.width * safeBox.height;

	if (totalPixels == 0)
		return 0.0;

	return (double)bluePixels / (double)totalPixels;
}

std::string recognizeRedCircularSign(const Mat& hsv, const Rect& box)
{
	double blueRatio = getBlueRatioInBox(hsv, box);

	if (blueRatio > 0.08)
		return "Oprire interzisa";

	return "Interzis intrarea";
}

std::string recognizeTrafficSign(
	const std::string& shapeName,
	const std::string& colorName,
	const Mat& hsv,
	const Rect& box,
	double area,
	int corners)
{
	double ratio = (double)box.width / (double)box.height;
	double rectArea = (double)box.width * (double)box.height;
	double fillRatio = area / rectArea;

	// Cedeaza = rosu + triunghi
	if (colorName == "Rosu" && corners == 3)
		return "Cedeaza";

	// Interzis intrarea = semn rosu cu bara alba centrala
	if (colorName == "Rosu" && hasWhiteHorizontalBar(hsv, box))
		return "Interzis intrarea";

	// STOP = rosu + 7/8 colturi + fara bara alba
	if (colorName == "Rosu" && (corners == 7 || corners == 8) && isLikelyStopSign(box, area))
		return "STOP";

	// Semne circulare rosii
	if (shapeName == "Cerc" && colorName == "Rosu")
	{
		if (ratio > 0.75 && ratio < 1.25 && fillRatio > 0.45)
			return recognizeRedCircularSign(hsv, box);

		return "Necunoscut";
	}

	// Drum cu prioritate
	if (colorName == "Galben" && corners == 4)
	{
		if (ratio > 0.6 && ratio < 1.4)
			return "Drum cu prioritate";
	}

	// Obligatoriu inainte
	if (shapeName == "Cerc" && colorName == "Albastru")
		return "Obligatoriu inainte";

	// Sens unic
	if (colorName == "Albastru" && corners == 4)
	{
		if (box.width < 120 && box.height < 120)
			return "Sens unic";

		return "Necunoscut";
	}

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

		for (double eps : {0.02, 0.03, 0.04, 0.05})
		{
			std::vector<Point> approx;
			approxPolyDP(contours[i], approx, eps * perimeter, true);
			int corners = (int)approx.size();

			if (corners == 3 || corners == 4 || corners == 7 || corners == 8)
			{
				shapeName = getShapeName(corners, area, perimeter);
				bestCorners = corners;
				if (shapeName != "Forma necunoscuta")
					break;
			}
		}

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
		inRange(hsv, Scalar(0, 60, 50), Scalar(12, 255, 255), maskRed1);
		inRange(hsv, Scalar(168, 60, 50), Scalar(180, 255, 255), maskRed2);
		maskRed = maskRed1 | maskRed2;

		Mat maskBlue;
		inRange(hsv, Scalar(95, 60, 40), Scalar(135, 255, 255), maskBlue);

		Mat maskYellow;
		inRange(hsv, Scalar(15, 60, 60), Scalar(40, 255, 255), maskYellow);

		Mat kernelSmall = getStructuringElement(MORPH_RECT, Size(3, 3));
		Mat kernelBig = getStructuringElement(MORPH_RECT, Size(5, 5));

		morphologyEx(maskRed, maskRed, MORPH_OPEN, kernelSmall);
		morphologyEx(maskRed, maskRed, MORPH_CLOSE, kernelBig);

		morphologyEx(maskBlue, maskBlue, MORPH_OPEN, kernelSmall);
		morphologyEx(maskBlue, maskBlue, MORPH_CLOSE, kernelBig);
		dilate(maskBlue, maskBlue, kernelSmall, Point(-1, -1), 2);

		morphologyEx(maskYellow, maskYellow, MORPH_OPEN, kernelSmall);
		morphologyEx(maskYellow, maskYellow, MORPH_CLOSE, kernelBig);

		Mat result = resized.clone();

		detectAndRecognizeCandidates(maskRed, result, "Rosu", hsv);
		detectAndRecognizeCandidates(maskBlue, result, "Albastru", hsv);
		detectAndRecognizeCandidates(maskYellow, result, "Galben", hsv);

		imshow("Imagine originala", resized);
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
		printf(" 13 - Detectare semn dupa forma\n");
		printf(" 14 - Detectare candidate dupa culoare\n");
		printf(" 15 - Detectare si recunoastere semne\n");
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
			testTrafficSignShapeDetection();
			break;
		case 14:
			testTrafficSignColorCandidates();
			break;
		case 15:
			testTrafficSignRecognition();
			break;
		}
	} while (op != 0);
	return 0;
}