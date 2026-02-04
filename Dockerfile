FROM ubuntu:22.04

RUN apt update && apt install -y \
    cmake \
    g++ \
    wget \
    unzip \
    python3-dev \
    python3-pip \
    git \
    build-essential \
    pkg-config \
    libgtk-3-dev \
    libavcodec-dev \
    libavformat-dev \
    libswscale-dev \
    libv4l-dev \
    libxvidcore-dev \
    libx264-dev \
    libjpeg-dev \
    libpng-dev \
    libtiff-dev \
    libatlas-base-dev \
    gfortran \
    libusb-1.0-0-dev \
    libudev-dev \
    libglm-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /tmp/opencv_build
RUN wget -O opencv.zip https://github.com/opencv/opencv/archive/4.11.0.zip && \
    wget -O opencv_contrib.zip https://github.com/opencv/opencv_contrib/archive/4.11.0.zip && \
    unzip opencv.zip && \
    unzip opencv_contrib.zip && \
    mkdir -p build

WORKDIR /tmp/opencv_build/build
RUN cmake -DOPENCV_EXTRA_MODULES_PATH=../opencv_contrib-4.11.0/modules ../opencv-4.11.0

RUN cmake --build .
RUN make install

COPY . /workspace
WORKDIR /workspace/src
RUN mkdir -p build

WORKDIR /workspace/src/build
RUN cmake ..
RUN make
RUN make install

WORKDIR /workspace

RUN export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
ENTRYPOINT ["python3", "/workspace/calibrate.py"]
