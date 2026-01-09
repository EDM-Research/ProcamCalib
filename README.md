# Projector-camera calibration with non-overlapping fields of view using a planar mirror

Implementation of the Springer Virtual Reality journal publication **Projector-camera calibration with non-overlapping fields of view using a planar mirror**: [doi:10.1007/s10055-024-01089-7](https://doi.org/10.1007/s10055-024-01089-7)

## Installation

Tested on Ubuntu 22.04 LTS, with a Realsense D455f camera and a Kodak LUMA 450 projector.

### Prerequisites

- OpenCV (tested with 4.13.0)


```
git clone https://github.com/EDM-Research/ProcamCalib.git
cd ProcamCalib/src
mkdir build
cd build
cmake .. 
sudo make install
```

## Usage

In `ProcamCalib/` run:
```
python calibrate.py ./data/recordings/recording/S0_0 ./data/patterns/Asym_4_9 -m ./data/recordings/mirrorRecording/M_14_0
```

This will use the saved recordings to calibrate the camera, the mirror and the projector and saves the results under `./data/estimation`

## Citation
```
@article{projectorCameraCalibrationNonOverlappingFOV,
  author = {Vanherck, Joni and Jorissen, Lode and Zoomers, Brent and Michiels, Nick},
  title = {Projector-camera calibration with non-overlapping fields of view using a planar mirror},
  year = {2025},
  issue_date = {Dec 2024},
  publisher = {Springer-Verlag},
  address = {Berlin, Heidelberg},
  volume = {29},
  number = {1},
  issn = {1359-4338},
  url = {https://doi.org/10.1007/s10055-024-01089-7},
  doi = {10.1007/s10055-024-01089-7},
  journal = {Virtual Real.},
  month = jan,
  numpages = {13}
}
```

## About
![](/DFL_FM.jpg)

Developed by [Hasselt University](https://www.uhasselt.be/), [Digital Future Lab](https://www.uhasselt.be/en/instituten-en/digitalfuturelab) in the scope of the Flanders Make funded project ALARMM SBO, FWO and funds on behalf of Hasselt University.


