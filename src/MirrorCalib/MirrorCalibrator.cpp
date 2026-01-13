#include "MirrorCalibrator.h"
#include "Utils.h"
#include <filesystem>
#include <map>
#include <opencv2/core.hpp>
#include <vector>
#include <fstream>
#include <string>

using namespace cv;

std::vector<Point3f> MirrorCalibrator::from2dToCamSpace(std::vector<Point2f> points2d, std::vector<int>& ids)
{
	if (points2d.size() < 16)
	{
		std::cerr << "[MirrorCalibrator] Not enough points (16 required)" << std::endl;
		ids.clear();
		return std::vector<Point3f>();
	}

	std::vector<Point3f> objpoints;
	for (auto id: ids)
	{
		objpoints.push_back(objp[id]);
	}

	// std::vector<Point3f> objpoints; std::vector<Point2f> imgpoints;
	// detector.getMatchingPoints(points2d, ids, objpoints, imgpoints);

	Matx31d rvec, tvec;
	bool ret = solvePnP(objpoints, imgpoints, camCalib.getIntrinsicsMatrix(), camCalib.getDistortionParameters(), rvec, tvec);

	if (!ret)
	{
		std::cerr << "[MirrorCalibrator] Failed to find rvec and tvec" << std::endl;
	}

	Matx33d R;
	Rodrigues(rvec, R);
	cv::Matx44d b2cam = Utils::extrinsicFromRt(R, tvec);

	std::vector<cv::Point3f> cObjp;
	perspectiveTransform(objpoints, cObjp, b2cam);
	return cObjp;
}

std::vector<Point3f> MirrorCalibrator::getPlanePointsFull(int debugDelay)
{
	std::vector<std::string> imgsPaths = Utils::loadImages(imgsFolder);
	if (imgsPaths.size() != 1)
	{
		std::cerr << "[MirrorCalibrator] Full view mirror calibration only allows one image for calibration" << std::endl;
		exit(1);
	}

	Mat img = imread(imgsPaths[0]);

	//GaussianBlur(img, img, Size(3, 3), 1);

	std::vector<Point3f> planePoints;
	detectFull(img, planePoints, debugDelay);

	return planePoints;
}

std::vector<cv::Point3f> MirrorCalibrator::getPlanePointsRV(int debugDelay)
{
	std::vector<Point3f> planePoints3d;
	int i = 0;
	for (const auto& entry : Utils::loadImages(imgsFolder))
	{
		Mat img = imread(entry);
		//GaussianBlur(img, img, Size(3, 3), 1);

		detectRV(img, planePoints3d, debugDelay);

		++i;

	}

	if (planePoints3d.size() < 3)
	{
		std::cerr << "[MirrorCalibrator] Not enough points found." << std::endl;
	}

	return planePoints3d;
}

std::vector<cv::Point3f> MirrorCalibrator::getPlanePointsFull(std::shared_ptr<DeviceFactory::Device> cam, int debugDelay)
{
	int imgId = 0;
	std::vector<Point3f> planePoints;

	while (imgId < 1)
	{
		Mat img; double timestamp;
		cam->captureImages(img, timestamp);

		Mat saveImg = img.clone();

		std::cout << "Trying new image..\n";

		bool detected = detectFull(img, planePoints, debugDelay);
		if (detected)
		{
			std::stringstream ss;
			ss << std::setfill('0') << std::setw(2) << imgId << ".png";
			Utils::verifyDirectories(imgsFolder + "/" + ss.str());
			imwrite(imgsFolder + "/" + ss.str(), saveImg);
			++imgId;
		}
	}

	return planePoints;
}

std::vector<cv::Point3f> MirrorCalibrator::getPlanePointsRV(std::shared_ptr<DeviceFactory::Device> cam, int patterns, int debugDelay)
{
	int imgId = 0;
	std::vector<Point3f> planePoints3d;

	while (imgId < patterns)
	{
		Mat img; double timestamp;
		cam->captureImages(img, timestamp);

		Mat saveImg = img.clone();

		std::cout << "Trying new image..\n";

		bool detected = detectRV(img, planePoints3d, debugDelay);
		if (detected)
		{
			std::stringstream ss;
			ss << std::setfill('0') << std::setw(2) << imgId << ".png";
			Utils::verifyDirectories(imgsFolder + "/" + ss.str());
			imwrite(imgsFolder + "/" + ss.str(), saveImg);
			++imgId;
			waitKey(2000);
		}
	}

	return planePoints3d;
}

bool MirrorCalibrator::detectFull(cv::Mat img, std::vector<cv::Point3f>& planePoints, int debugDelay)
{
	Mat gray;
	cvtColor(img, gray, COLOR_BGR2GRAY);

	std::vector<Point2f> corners;
	std::vector<int> ids;
	detector.detectCharucoCorners(gray, corners, ids);

	if (debugDelay >= 0)
	{
		cv::aruco::drawDetectedCornersCharuco(img, corners, ids, cv::Scalar(0, 255, 0));
		imshow("Img", img);
		if (waitKey(debugDelay) == 'q')
			exit(1);
	}

	planePoints = from2dToCamSpace(corners, ids);

	return not planePoints.empty();
}

bool MirrorCalibrator::detectRV(cv::Mat img, std::vector<cv::Point3f>& planePoints, int debugDelay)
{
	Mat gray;
	cvtColor(img, gray, COLOR_BGR2GRAY);

	std::vector<Point2f> realPoints2d, virtualPoints2d;
	std::vector<int> realIds, virtualIds;

	detector.detectCharucoCorners(gray, realPoints2d, realIds);
	flip(gray, gray, 1);
	detector.detectCharucoCorners(gray, virtualPoints2d, virtualIds);
	Utils::flip2dPoints(virtualPoints2d, gray.size().width);

	if (debugDelay >= 0)
	{
		cv::aruco::drawDetectedCornersCharuco(img, realPoints2d, realIds, cv::Scalar(0, 255, 0));
		cv::aruco::drawDetectedCornersCharuco(img, virtualPoints2d, virtualIds, cv::Scalar(0, 255, 0));
		imshow("Img", img);
		if (waitKey(debugDelay) == 'q')
			exit(1);
	}

	std::vector<Point3f> realPoints3d = from2dToCamSpace(realPoints2d, realIds);
	std::vector<Point3f> virtualPoints3d = from2dToCamSpace(virtualPoints2d, virtualIds);

	std::vector<Point3f> midPoints3d = computeMidPoints(realPoints3d, realIds, virtualPoints3d, virtualIds);
	
	if (midPoints3d.size() > 8)
	{
		planePoints.insert(planePoints.end(), midPoints3d.begin(), midPoints3d.end());
		return true;
	}
	else
	{
		return false;
	}
}

std::vector<cv::Point3f> MirrorCalibrator::computeMidPoints(const std::vector<cv::Point3f>& realPoints3d, const std::vector<int>& realIds, 
		const std::vector<cv::Point3f>& virtualPoints3d, const std::vector<int>& virtualIds)
{
	std::vector<Point3f> midPoints3d;
	std::map<int, Point3f> virtualPointsMap;
	for (size_t i = 0; i < virtualIds.size(); ++i)
	{
		virtualPointsMap[virtualIds[i]] = virtualPoints3d[i];
	}

	int matches = 0;
	for (size_t i = 0; i < realIds.size(); ++i)
	{
		int id = realIds[i];
		if (virtualPointsMap.find(id) != virtualPointsMap.end())
		{
			Point3f realP = realPoints3d[i];
			Point3f virtualP = virtualPointsMap[id];
			Point3f midP{
				(realP.x + virtualP.x) / 2.0f,
				(realP.y + virtualP.y) / 2.0f,
				(realP.z + virtualP.z) / 2.0f
			};
			midPoints3d.push_back(midP);
			++matches;
		}
	}

	std::cout << "[MirrorCalibrator] Found " << matches << " matching points." << std::endl;

	return midPoints3d;
}


MirrorCalibrator::MirrorCalibrator()
{
}

void MirrorCalibrator::init(const std::string& recording, const std::string& camCalibName)
{
	imgsFolder = recording;
	this->camCalibName = camCalibName;

	for (int i = 0; i < detector.getBoardSize().height; i++)
	{
		for (int j = 0; j < detector.getBoardSize().width; j++)
		{
			objp.push_back(Point3f{ (float)j * 1.65f, (float)i * 1.65f, 0.0f }); // real 1.65f fake 2.22f
		}
	}
	
	Utils::readJSONFileToCameraCalibration(camCalibName, camCalib);

	std::cout << camCalib;
}



void MirrorCalibrator::calibrate(bool debug)
{

	std::string lastFolder = std::filesystem::path(imgsFolder).filename().string();
	std::vector<Point3f> planePoints;
	if (lastFolder[0] == 'F')
	{
		std::cout << "[MirrorCalibrator]: Running full view mirror calibration" << std::endl;
		if (debug)
			planePoints = getPlanePointsFull(0);
		else
			planePoints = getPlanePointsFull();
	}
	else if (lastFolder[0] == 'M')
	{
		std::cout << "[MirrorCalibrator]: Running reflection mirror calibration" << std::endl;
		if (debug)
			planePoints = getPlanePointsRV(0);
		else
			planePoints = getPlanePointsRV();
	}

	destroyAllWindows();
	mp.fromPoints(planePoints);
}

void MirrorCalibrator::calibrate(std::shared_ptr<DeviceFactory::Device> cam, int patterns)
{
	std::vector<Point3f> planePoints;
	if (imgsFolder[0] == 'F')
	{
		planePoints = getPlanePointsFull(cam, 150);
	}
	else if (imgsFolder[0] == 'M')
	{
		planePoints = getPlanePointsRV(cam, patterns, 150);
	}
	destroyAllWindows();

	mp.fromPoints(planePoints);
}

void MirrorCalibrator::saveToJSON()
{
	std::string lastFolder = std::filesystem::path(imgsFolder).filename().string();
	std::string seqName = std::filesystem::path(camCalibName).filename().string();
	mp.saveToJSON(Config::mirrorCalibrationFolder + seqName);
}
