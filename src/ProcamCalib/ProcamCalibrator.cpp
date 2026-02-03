#include "ProcamCalibrator.h"
#include <filesystem>
#include <opencv2/core.hpp>
#include "Config.h"
#include "Utils.h"
#include <sstream>

using namespace cv;

std::vector<Point3f> ProcamCalibrator::pointsToBoardSpace(std::vector<Point2f> points2d, std::vector<Point2f> refPoints2d, std::vector<Point3f> objp, Matx33d cameraIntrinsics, std::vector<double> distortionCoeffs)
{
	std::vector<Point2f> undistortedPoints2d;
	undistortPoints(points2d, undistortedPoints2d, cameraIntrinsics, distortionCoeffs, noArray(), cameraIntrinsics);

	std::vector<Point2f> undistortedRefPoints2d;
	undistortPoints(refPoints2d, undistortedRefPoints2d, cameraIntrinsics, distortionCoeffs, noArray(), cameraIntrinsics);

	std::vector<Point2f> objpPlanar;
	for (auto p : objp)
	{
		objpPlanar.push_back(Point2f{ p.x, p.y });
	}

	Mat H = findHomography(undistortedRefPoints2d, objpPlanar);
	
	std::vector<Point2f> transformedPoints2d;
	perspectiveTransform(undistortedPoints2d, transformedPoints2d, H);

	std::vector<Point3f> points3d;
	for (auto p : transformedPoints2d)
	{
		points3d.push_back(Point3f{ p.x, p.y, 0 });
	}

	return points3d;
}

bool ProcamCalibrator::detectAll(Mat pattern, Mat img, bool mirrored, int debugDelay)
{
	std::vector<Point2f> circlesPattern;
	bool retPattern = findCirclesGrid(pattern, circlesGridSize, circlesPattern, (CALIB_CB_ASYMMETRIC_GRID + CALIB_CB_CLUSTERING), circlesDetector);

	if (mirrored)
	{
		Utils::flip2dPoints(circlesPattern, pattern.cols);
	}

	Mat gray;
	cvtColor(img, gray, COLOR_BGR2GRAY);
	if (mirrored)
	{
		flip(gray, gray, 1);
	}

	Mat grayB;
	GaussianBlur(gray, grayB, Size(3, 3), 1);


	std::vector<Point2f> circlesFrame;
	bool retFrame = findCirclesGrid(grayB, circlesGridSize, circlesFrame, (CALIB_CB_ASYMMETRIC_GRID + CALIB_CB_CLUSTERING), circlesDetector);
	if (retFrame == false || circlesFrame.size() <= 0)
	{
		std::cout << "!!!! Failed to find the circle grid !!!!" << std::endl;
		return false;
	}

	if (mirrored)
	{
		Utils::flip2dPoints(circlesFrame, gray.cols);
	}

	std::cout << "-- Grid detected" << std::endl;

	std::vector<Point2f> corners;
	std::vector<int> ids;
	detector.detectCharucoCorners(gray, corners, ids);
	if (corners.size() >= 35)
	{
		std::cout << "-- Charuco detected" << std::endl;

		if (mirrored)
		{
			Utils::flip2dPoints(corners, gray.cols);
		}

		std::vector<Point3f> circles3d = pointsToBoardSpace(circlesFrame, corners, objp, camCalib.getIntrinsicsMatrix(), camCalib.getDistortionParameters());

		if (debugDelay >= 0)
		{

			drawChessboardCorners(img, circlesGridSize, circlesFrame, true);
			drawChessboardCorners(img, detector.getBoardSize(), corners, true);

			imshow("Camera", img);
			auto c = waitKey(debugDelay);
			if (c == 'q')
			{
				destroyAllWindows();
				exit(1);
			}
			else if (c == 's')
			{
				return false;
			}
		}

		objPointsVirtual.push_back(circles3d);
		imgPointsVirtualProj.push_back(circlesPattern);
		imgPointsCamera.push_back(circlesFrame);

		return true;
	}
	else
	{
		std::cout << "!!!! Failed to find charuco board !!!!" << std::endl;
		return false;
	}
}

void ProcamCalibrator::calibrateInternal(bool mirrored, const Size& projSize, const Size& camSize)
{
	Mat _rvecs, _tvecs;
	projRMS = cv::calibrateCamera(objPointsVirtual, imgPointsVirtualProj, projSize, projInt, projDist, _rvecs, _tvecs, 0, TermCriteria(TermCriteria::COUNT + TermCriteria::EPS, 1e6, DBL_EPSILON));

	std::cout << camCalib;
	std::cout << std::endl << "Projector\n----------------\nRMS: " << projRMS << std::endl << "Intrinsics:" << std::endl << projInt << std::endl;
	std::cout << "Distortion coeffs:\n" << "[ " << projDist[0] << ", " << projDist[1] << ", " << projDist[2] << ", " << projDist[3] << ", " << projDist[4] << " ] " << std::endl;

	Matx33d R;
	Matx31d T;
	Mat E, F, rvecs, tvecs, perViewErrors;
	stereoRMS = cv::stereoCalibrate(objPointsVirtual, imgPointsVirtualProj, imgPointsCamera, projInt, projDist, camCalib.getIntrinsicsMatrix(), camCalib.getDistortionParameters(), camSize, R, T, E, F, perViewErrors, 256, TermCriteria(TermCriteria::COUNT + TermCriteria::EPS, 1e2, DBL_EPSILON));

	Matx44d projCalib = Utils::extrinsicFromRt(R, T);
	
	if (mirrored)
	{
		virtualProj2Cam = projCalib;
		Matx33d scalingMatrix = Matx33d{ -1,0,0,
										  0,1,0,
										  0,0,1 };

		Rect roi{ 0,0,3,3 };

		Mat virtualProj2CamMat = Mat(virtualProj2Cam).clone();
		virtualProj2CamMat(roi) = virtualProj2CamMat(roi) * scalingMatrix;

		virtualProj2Cam = Matx44d(virtualProj2CamMat);

		Matx44d realProj2Cam = mp.reflectPose(virtualProj2Cam);
		cam2Proj = realProj2Cam.inv();
	}
	else
	{
		cam2Proj = projCalib.inv();
	}

	std::cout << std::endl << "Stereo\n----------------\nRMS: " << stereoRMS << std::endl << "Cam2Proj:" << std::endl << cam2Proj << std::endl;
}

ProcamCalibrator::ProcamCalibrator(): detections{0}
{
}

void ProcamCalibrator::init(const std::string& imgsFolder, const std::string& mirrorCalibName, Projector* proj, const std::string& camCalibName)
{
	this->mirrorCalibName = mirrorCalibName;
	mp = MirrorPlane(mirrorCalibName);
	init(imgsFolder, proj, camCalibName);
}

void ProcamCalibrator::init(const std::string& imgsFolder, Projector* proj, const std::string& camCalibName)
{
	this->imgsFolder = imgsFolder;
	this->proj = proj;
	circlesGridSize = proj->getPatternSize();
	this->camCalibName = camCalibName;
	Utils::readJSONFileToCameraCalibration(camCalibName, camCalib);
	std::cout << camCalib;
	init();
}

void ProcamCalibrator::init()
{
	//objp = detector.getObjectPoints();

	for (int i = 0; i < detector.getBoardSize().height; i++)
	{
		for (int j = 0; j < detector.getBoardSize().width; j++)
		{
			objp.push_back(Point3f{ (float)j * 4.43f, (float)i * 4.43f, 0.0f });
		}
	}

	SimpleBlobDetector::Params paramsFrame;
	paramsFrame.blobColor = 255;
	paramsFrame.filterByColor = true;
	paramsFrame.filterByArea = true;
	paramsFrame.minArea = 20;
	paramsFrame.filterByConvexity = 0;
	paramsFrame.filterByInertia = 0;
	paramsFrame.filterByCircularity = 1;
	paramsFrame.minDistBetweenBlobs = 5;

	// This parameter really does a lot for RMS
	//paramsFrame.minThreshold = 200;
	paramsFrame.minCircularity = 0.5;

	circlesDetector = SimpleBlobDetector::create(paramsFrame);
	
	std::cout << "OpenCV version: " << CV_VERSION << std::endl;
}

void ProcamCalibrator::calibrate(bool debug)
{
	bool mirrored = false;
	std::cout << "Mirror calib name: " << mirrorCalibName << std::endl;
	if (mirrorCalibName != "")
	{
		mirrored = true;
	}

	int imgId = -1;
	Mat refImg;

	auto images = Utils::loadImages(imgsFolder);
	capPerPattern = images.size() / proj->getNrPatterns();

	int debugDelay = -1;
	if (debug)
		debugDelay = 0;

	for (const auto& entry : images)
	{
		++imgId;

		if (imgId % capPerPattern == 0)
		{
			proj->nextPattern();
		}

		Mat pattern = proj->getCurrentPattern();

		Mat img = imread(entry);
		if (imgId == 0)
			refImg = img;

		std::cout << "Loaded image " << imgId << std::endl;

		bool detected = detectAll(pattern, img, mirrored, debugDelay);
		if (detected)
		{
			++detections;
		}


		//if(debug)
			//calibrateInternal(mirrored, proj->getCurrentPattern().size(), refImg.size());
	}

	destroyAllWindows();

	std::cout << "==== Number detections: " << detections << std::endl;

	calibrateInternal(mirrored, proj->getCurrentPattern().size(), refImg.size());
}

void ProcamCalibrator::calibrate(std::shared_ptr<DeviceFactory::Device> physCamera, int capPerPattern)
{
	bool mirrored = false;
	if (imgsFolder[0] == 'S')
		mirrored = true;

	int imgId = 0;
	Mat refImg;
	bool patternChanged = false;

	while (imgId < capPerPattern * proj->getNrPatterns())
	{
		if (imgId % capPerPattern == 0 && !patternChanged)
		{
			proj->nextPattern();
			proj->showCurrentPattern(false);
			patternChanged = true;
		}

		Mat pattern = proj->getCurrentPattern();

		Mat cap; double timestamp;
		physCamera->captureImages(cap, timestamp);

		Mat img = cap.clone();
		Mat sImg = cap.clone();

		if (imgId == 0)
			refImg = img;

		std::cout << "Trying new image..\n";

		imshow("Camera", img);
		auto c = waitKey(1);
		if (c == 'q')
		{
			destroyAllWindows();
			exit(1);
		}

		bool detected = detectAll(pattern, img, mirrored, 2000);
		if (detected || c == 's')
		{
			std::stringstream ss;
			ss << std::setfill('0') << std::setw(2) << imgId << ".png";
			Utils::verifyDirectories(imgsFolder + "/" + ss.str());
			imwrite(imgsFolder + "/" + ss.str(), sImg);
			++imgId;
			patternChanged = false;
		}
	}

	destroyAllWindows();

	calibrateInternal(mirrored, proj->getCurrentPattern().size(), refImg.size());
}


void ProcamCalibrator::saveToJSON()
{
	std::string seqName = std::filesystem::path(imgsFolder).filename().string();
	const std::string filePath = Config::procamCalibrationFolder + "/" + seqName + ".json";
	Utils::verifyDirectories(filePath);

	FileStorage fs{ filePath, FileStorage::WRITE + FileStorage::FORMAT_JSON };
	if (!fs.isOpened())
	{
		std::cerr << "[ProcamCalibrator] Error: Could not open the input file " << filePath << std::endl;
		exit(1);
	}

	fs << "cam_int" << camCalib.getIntrinsicsMatrix();
	fs << "cam_dist" << camCalib.getDistortionParameters();
	fs << "cam_width" << camCalib.getWidth();
	fs << "cam_height" << camCalib.getHeight();
	fs << "cam_fisheye" << camCalib.isFishEye();
	fs << "proj_int" << projInt;
	fs << "proj_dist" << projDist;
	fs << "proj_width" << proj->getCurrentPattern().size().width;
	fs << "proj_height" << proj->getCurrentPattern().size().height;
	fs << "proj_RMS" << projRMS;
	fs << "cam2proj" << cam2Proj;
	fs << "stereo_RMS" << stereoRMS;
	fs << "detections" << detections;

	if (imgsFolder[0] == 'S')
	{
		fs << "plane" << mp.getPlaneParams();
		fs << "virtualProj2Cam" << virtualProj2Cam;
	}

	fs.release();
}


