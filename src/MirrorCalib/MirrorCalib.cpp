#include <iostream>
#include <vector>
#include "MirrorCalibrator.h"
#include "DeviceFactory/DeviceFactory.h"

using namespace std;

class CmdLineParser {

private:
    int argc; char** argv;

public:
    CmdLineParser(int _argc, char** _argv) :argc(_argc), argv(_argv) {}  bool operator[] (string param) { int idx = -1;  for (int i = 0; i < argc && idx == -1; i++) if (string(argv[i]) == param) idx = i;	return (idx != -1); } string operator()(string param, string defvalue = "") { int idx = -1;	for (int i = 0; i < argc && idx == -1; i++) if (string(argv[i]) == param) idx = i; if (idx == -1) return defvalue;   else  return (argv[idx + 1]); }
    std::vector<std::string> getAllInstances(string str)
    {
        std::vector<std::string> ret;
        for (int i = 0; i < argc - 1; i++)
        {
            if (string(argv[i]) == str)
                ret.push_back(argv[i + 1]);
        }
        return ret;
    }
};



int main(int argc, char** argv)
{
    CmdLineParser cml(argc, argv);
    if (argc < 2 || cml["-h"]) {
        cerr << std::endl << "Usage: ./MirrorCalib recording [-c calibPath] [-d]" << std::endl;
        cerr << std::endl << "recording: folder containing the recording." << std::endl;
        cerr << std::endl << "calibPath: path to camera calibration data." << std::endl;
        cerr << std::endl << "[-p]: captures per pattern, only use when physical camera is connected." << std::endl;
        cerr << std::endl << "[-camid]: camera id to use. Only use when physical camera is connected." << std::endl;
        cerr << std::endl << "[-d]: whether to use debug mode or not." << std::endl;
        return 0;
    }

    std::string recordingFolder = argv[1];
    std::string calibPath = argv[2];

    MirrorCalibrator calibrator;
    calibrator.init(recordingFolder, calibPath);

    if (cml["-p"] && cml["-camid"])
    {
        DeviceFactory::DeviceFactory df;
        df.listAvailableDevices();
        std::shared_ptr<DeviceFactory::Device> cam = df.createDevices("RealSense2", cml("-camid"));
        if (cam.get() == nullptr)
            exit(1);
        calibrator.calibrate(cam, std::stoi(cml("-p")));
    }
    else if (!cml["-p"] && !cml["-camid"])
    {
        std::cout << "[MirrorCalib]: Running calibration with offline recording" << std::endl;
        calibrator.calibrate(cml["-d"]);
    }
    else
    {
        cerr << "[MirrorCalib]: [-p] and [-camid] should be used simultaneously" << endl;
        exit(1);
    }

    calibrator.saveToJSON();

    return 0;
}