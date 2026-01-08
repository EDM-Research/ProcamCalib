#include "Projector.h"
#include <filesystem>
#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>
#include <map>
#include "Config.h"
#include "Utils.h"

using namespace cv;

Projector::Projector(std::string patternFolder): currentPatternId{-1}
{
	std::cout << "Loading patterns from " << patternFolder << std::endl;
	patternPaths = Utils::loadImages(patternFolder);

	std::string circleGridHeightS, circleGridWidthS;
	int heightidx = patternFolder.find_last_of('_');
	int widthidx = patternFolder.substr(0, heightidx).find_last_of('_');

	circlesGridSize = Size(std::stoi(patternFolder.substr(widthidx + 1, heightidx - (widthidx + 1))), std::stoi(patternFolder.substr(heightidx + 1, patternFolder.size() - heightidx)));
}

void Projector::nextPattern()
{
	currentPatternId++;
	currentPattern = imread(patternPaths[currentPatternId]);
}

void Projector::showCurrentPattern(bool shortDelay)
{
	// std::vector<MONITORINFO> monitors = Utils::getMonitors();
	// MONITORINFO mi = monitors.back();
	namedWindow("Projector", WINDOW_NORMAL);
	// moveWindow("Projector", mi.rcMonitor.left, mi.rcMonitor.top);
	// resizeWindow("Projector", mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top);
	setWindowProperty("Projector", WND_PROP_FULLSCREEN, WINDOW_FULLSCREEN);

	imshow("Projector", currentPattern);
	if (shortDelay)
	{
		if (waitKey(1) == 'q')
			exit(0);
	}
	else 
	{
		if (waitKey(2000) == 'q')
			exit(0);
	}
}

Mat Projector::getCurrentPattern()
{
	return currentPattern;
}

int Projector::getNrPatterns()
{
	return patternPaths.size();
}

cv::Size Projector::getPatternSize()
{
	return circlesGridSize;
}
