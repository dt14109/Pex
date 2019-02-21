#include "pch.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio/legacy/constants_c.h>
#include <fstream>
#include <vector>
#include <functional>

using namespace std;
using namespace cv;

//Given a video file and dimensions(e.g. 32x32, 64x64, 128x128),
//  c:\pex\pex\World_480_15.mp4  480x270
// extract keyframes from the video
// convert the frame into grayscale,
// split each frame into a grid of said dimensions, 
// calculate median value of all the pixels of each cell of the grid
// write the values to a CSV file together with the timestamp of the frame.
//
//Example:
//
//If a frame timestamp(in seconds) is 3.14 and the dimensions are 3x3 an example line might look like :
//
//3.14, 42, 255, 9, 13, 67, 0, 27, 33, 123  // timestamp + 9 values (3x3) 
int main()
{
	try 
	{
		ofstream ofs;
		string file = "";
		double timestamp = 0.0;
		vector<uchar> eachFramePixelVector; // Each frame has a vector of pixels
		pair<int, double> pair1;			// Median, frame timestamp
		vector<pair <int, double>> resultsVector;

		while(0 == file.size())
		{
			cout << "Enter the output file name: " << endl;
			cin >> file;
		}

		// Open the file for writing and truncate it
		ofs.open(file, ios::out | ios::trunc);

		// open the video file
		vector<Mat> framesVector;
		string videoFile = ""; // image size is 320 x 240
		while (0 == videoFile.size())
		{
			cout << "Enter the input video file name: " << endl;
			cin >> videoFile;
		}

		VideoCapture videoCapture(videoFile);

		if (!videoCapture.isOpened())  // check if we succeeded
		{
			cerr << "Can not open Video file " << videoFile << endl;
			return 0;
		}

		// Process each frame
		for (int frameNum = 0; frameNum < videoCapture.get(CV_CAP_PROP_FRAME_COUNT); frameNum++)
		{
			Mat frame;
			videoCapture >> frame; // get the next frame from video

			// timestamp in milliseconds for each frame
			timestamp = videoCapture.get(CV_CAP_PROP_POS_MSEC);

			// Convert it to grayscale
			Mat grayMat;
			cv::cvtColor(frame, grayMat, COLOR_BGR2GRAY);

			// Get each grayscale pixel and add them to the vector
			for_each(grayMat.begin<uchar>(), grayMat.end<uchar>(), [&](uchar& pixel) {
				eachFramePixelVector.push_back(pixel);
			});

			// Now that we have each pixel for this frame, we want to determine the Median for this frame.
			// Thus we will display the Timestamp and the Median for each frame

			// Get the Median value and put it into the Vector of each Median
			// It's more efficient to do the partial sort
			nth_element(eachFramePixelVector.begin(), eachFramePixelVector.begin() + eachFramePixelVector.size() / 2, eachFramePixelVector.end());

			pair1.first = eachFramePixelVector[eachFramePixelVector.size() / 2];  // Median
			pair1.second = timestamp;
			resultsVector.push_back(pair1);
		}

		// Write the medians to a CSV file together with the timestamp of the frame.
		for_each(resultsVector.begin(), resultsVector.end(), [&](pair<int, double> pair1) {
			ofs << pair1.second << ", " << pair1.first << ", ";
		});

		ofs.close();
		cout << "******************* We are done!! *******************" << endl;
	}
	catch (Exception& e) {
		cerr << e.msg << endl;
		exit(1);
	}

	return 0;
}
