# Projector-camera calibration without overlapping fields of view using a planar mirror

## Installation

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