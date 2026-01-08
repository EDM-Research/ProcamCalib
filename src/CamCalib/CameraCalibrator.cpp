#include "CameraCalibrator.h"
#include "CameraCalibrator.h"
#include "Utils.h"
#include "Config.h"
#include <filesystem>

using namespace cv;

void CameraCalibrator::init()
{
	for (int i = 0; i < detector.getBoardSize().height; i++)
	{
		for (int j = 0; j < detector.getBoardSize().width; j++)
		{
			objp.push_back(Point3f{ (float)j * 4.43f, (float)i * 4.43f, 0.0f });
		}
	}
}

void CameraCalibrator::calibrateInternal(Size camSize)
{
	Matx33d camInt;
	std::vector<double> camDist;
	Mat _rvecs, _tvecs;
	float camRMS = calibrateCamera(objPoints, imgPoints, camSize, camInt, camDist, _rvecs, _tvecs);

	std::cout << std::endl << "Camera RMS: " << camRMS << std::endl << "Intrinsics:" << std::endl << camInt << std::endl;

	camCalib.setIntrinsicsMatrix(camInt);
	camCalib.setDistortionParameters(camDist);
	camCalib.setWidth(camSize.width);
	camCalib.setHeight(camSize.height);
}

bool CameraCalibrator::detectAll(Mat img, bool mirrored, int debugDelay)
{
	if (debugDelay >= 0)
	{
		imshow("Camera", img);
		if (waitKey(1) == 'q')
		{
			destroyAllWindows();
			exit(1);
		}
	}

	Mat gray;
	cvtColor(img, gray, COLOR_BGR2GRAY);

	if (mirrored)
		flip(gray, gray, 1);

	std::vector<Point2f> corners;
	std::vector<int> ids;
	detector.detectCharucoCorners(gray, corners, ids);

	if (corners.size() >= 35)
	{
		std::cout << "-- Charuco detected" << std::endl;

		if (mirrored)
		{
			Utils::flip2dPoints(corners, gray.size().width);
		}

		objPoints.push_back(objp);
		imgPoints.push_back(corners);

		if (debugDelay >= 0)
		{
			drawChessboardCorners(img, detector.getBoardSize(), corners, true);

			imshow("Camera", img);
			if (waitKey(debugDelay) == 'q')
			{
				destroyAllWindows();
				exit(1);
			}
		}

		return true;
	}
	else
	{
		std::cout << "!!!! Failed to find charuco board !!!!" << std::endl;
		return false;
	}
}

CameraCalibrator::CameraCalibrator()
{
}

void CameraCalibrator::init(const std::string& imgsFolder)
{
	this->imgsFolder = imgsFolder;
	init();
}

void CameraCalibrator::calibrate(bool debug)
{
	bool mirrored = false;
	if (std::filesystem::path(imgsFolder).filename().string()[0] == 'S')
	{
		mirrored = true;
	}

	int debugDelay = -1;
	if (debug)
		debugDelay = 0;

	Mat refImg;
	int imgId = -1;

	int detections = 0;

	for (const auto& entry : Utils::loadImages(imgsFolder))
	{
		++imgId;

		Mat img = imread(entry);
		if (imgId == 0)
			refImg = img;

		//GaussianBlur(img, img, Size(3, 3), 1);

		std::cout << "Loaded image " << imgId << std::endl;

		if (detectAll(img, mirrored, debugDelay))
			++detections;
	}

	destroyAllWindows();

	std::cout << "==== Number detections: " << detections << std::endl;

	calibrateInternal(refImg.size());
}

void CameraCalibrator::calibrate(std::shared_ptr<DeviceFactory::Device> cam, int patterns)
{
	bool mirrored = false;
	if (std::filesystem::path(imgsFolder).filename().string()[0] == 'S')
	{
		mirrored = true;
	}

	int imgId = 0;
	Mat refImg;

	while (imgId < patterns)
	{
		Mat cap; double timestamp;
		cam->captureImages(cap, timestamp);

		Mat img = cap.clone();
		Mat sImg = img.clone();

		if (imgId == 0)
			refImg = img;

		std::cout << "Trying new image..\n";

		bool detected = detectAll(img, mirrored, 2000);
		if (detected)
		{
			std::stringstream ss;
			ss << std::setfill('0') << std::setw(2) << imgId << ".png";
			Utils::verifyDirectories(imgsFolder + "/" + ss.str());
			imwrite(imgsFolder + "/" + ss.str(), sImg);
			++imgId;
		}
	}

	destroyAllWindows();

	calibrateInternal(refImg.size()); 	
}

void CameraCalibrator::saveToJSON()
{
	std::string seqName = std::filesystem::path(imgsFolder).filename().string();
	std::string filePath = Config::cameraCalibrationFolder + "/" + seqName + ".json";
	Utils::verifyDirectories(filePath);
    cv::FileStorage fs{ filePath, cv::FileStorage::WRITE + cv::FileStorage::FORMAT_JSON };
    if (!fs.isOpened())
    {
        std::cerr << "[CameraCalibration] Error: Could not open the input file " << filePath << std::endl;
        exit(1);
    }

    fs << "cam_width" << camCalib.getWidth();
    fs << "cam_height" << camCalib.getHeight();
    fs << "cam_int" << camCalib.getIntrinsicsMatrix();
    fs << "cam_dist" << camCalib.getDistortionParameters();
    fs << "cam_fisheye" << camCalib.isFishEye();
    fs.release();
}
