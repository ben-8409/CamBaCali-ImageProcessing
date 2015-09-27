# CamBaCali-ImageProcessing
Image processing component for camera based calibration of large high resolution display walls.

# Abstract
This software is part of a software collection developed during my masters thesis at Bonn-Rhine-Sieg University.
The goal of the collection is to facilicate camera based calibration of Large-High-Resolution display walls. The image processing component takes images recorded and downloaded to the filesystem, detects individual screens and their mean color. It is designed to post it results to the camera based calibration server CamBaCali-Server. Run the makefile to compile and launch the binary with to learn about the command line parameters used.
The calibration server runs this software via SSH, so make sure it is in the path of the SSH session configured in the server.

# Mission statement
I hope to make it easier to developed turnkey color calibration solutions via this framework. I release my code under a free license. If you find the software useful, consider to give back by contributing your knowledge or time.

# Third party code
This software relies on third-party software for various functionality. This software is not included with the source code, but has to be downloaded manually. Check the third party directory.
This component depends on OpenCV on the target system. It was developed with OpenCV 2.4 but aimed to be ported to OpenCV 3.0.