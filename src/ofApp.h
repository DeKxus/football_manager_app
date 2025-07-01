#pragma once
#include "ofMain.h"
#include "ofxGui.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxXmlSettings.h"
#include "ofxCvHaarFinder.h"

#include <string>

#include "Team.h"
#include "CustomButton.h"
#include "ScrollableButtonList.h"
#include "VerticalElementList.h"
#include "CustomField.h"
#include "CustomLabel.h"

using namespace cv;
using namespace ofxCv;
using cv::ORB;

class OrbClass
{
private:
	Ptr<ORB> orb;
	ofImage origin, target;
	Mat descriptorsOrigin, descriptorsTarget;
	std::vector<cv::KeyPoint> keypointsOrigin, keypointsTarget;
	vector<DMatch> matches;
	Mat res;
	BFMatcher matcher;

public:
	OrbClass()
	{
		orb = ORB::create();
		matcher = BFMatcher(NORM_HAMMING);
	}

	void setOrigin(ofImage img)
	{
		origin = img;
		origin.allocate(img.getWidth(), img.getHeight(), OF_IMAGE_COLOR);
	}

	void setTarget(ofImage img)
	{
		target = img;
		target.allocate(img.getWidth(), img.getHeight(), OF_IMAGE_COLOR);
	}

	void detectAndCompute()
	{
		std::cout << "Starting detectAndCompute()" << std::endl;

		origin.setImageType(OF_IMAGE_GRAYSCALE);
		ofxCvGrayscaleImage originCv;
		ofPixels originPixels = origin.getPixels();
		originCv.setFromPixels(originPixels);
		std::cout << "Origin image converted to grayscale" << std::endl;

		target.setImageType(OF_IMAGE_GRAYSCALE);
		ofxCvGrayscaleImage targetCv;
		ofPixels targetPixels = target.getPixels();
		targetCv.setFromPixels(targetPixels);
		std::cout << "Target image converted to grayscale" << std::endl;

		// Convert input images to grayscale
		cv::Mat grayImageOrigin = toCv(originCv), grayImageTarget = toCv(targetCv);
		std::cout << "Converted ofxCvGrayscaleImage to cv::Mat" << std::endl;

		// Detect keypoints using ORB detector
		try {
			std::cout << "Detecting keypoints in origin image. grayImageOrigin: " << grayImageOrigin.size() << " keypointsOrigin: " << keypointsOrigin.size() << std::endl;
			orb->detect(grayImageOrigin, keypointsOrigin);
			std::cout << "Keypoints in origin image detected, count: " << keypointsOrigin.size() << std::endl;

			std::cout << "Detecting keypoints in target image. grayImageTarget: " << grayImageTarget.size() << " keypointsTarget: " << keypointsTarget.size() << std::endl;
			orb->detect(grayImageTarget, keypointsTarget);
			std::cout << "Keypoints in target image detected, count: " << keypointsTarget.size() << std::endl;

			if (keypointsOrigin.empty()) {
				std::cerr << "No keypoints detected in origin image, skipping descriptor computation for origin image." << std::endl;
			}
			else {
				// Compute descriptors
				std::cout << "Computing descriptors for origin image" << std::endl;
				orb->compute(grayImageOrigin, keypointsOrigin, descriptorsOrigin);
				std::cout << "Descriptors for origin image computed, size: " << descriptorsOrigin.size() << std::endl;
			}

			if (keypointsTarget.empty()) {
				std::cerr << "No keypoints detected in target image, skipping descriptor computation for target image." << std::endl;
			}
			else {
				std::cout << "Computing descriptors for target image" << std::endl;
				orb->compute(grayImageTarget, keypointsTarget, descriptorsTarget);
				std::cout << "Descriptors for target image computed, size: " << descriptorsTarget.size() << std::endl;
			}
		}
		catch (const cv::Exception& e) {
			std::cerr << "Error during ORB detection and computation: " << e.what() << std::endl;
		}

		std::cout << "Finished detectAndCompute()" << std::endl;
	}

	float match()
	{
		const double kDistanceCoef = 30.0;
		const int kMaxMatchingSize = 30;

		if (descriptorsOrigin.empty() || descriptorsTarget.empty()) {
			std::cerr << "Descriptors are empty, cannot perform matching." << std::endl;
			return 0.0f;
		}

		matches.clear();
		matcher.match(descriptorsOrigin, descriptorsTarget, matches);

		std::sort(matches.begin(), matches.end());
		if (matches.size() > 0)
		{
			while (matches.front().distance * kDistanceCoef < matches.back().distance)
			{
				matches.pop_back();
			}

			while (matches.size() > kMaxMatchingSize)
			{
				matches.pop_back();
			}
		}

		int counterObjects = 0;

		for (size_t i = 0; i < matches.size(); i++)
		{
			if (matches[i].distance < kDistanceCoef)
			{
				counterObjects++;
			}
		}

		std::cout << "Matches found: " << counterObjects << std::endl;
		std::cout << "Matches size: " << matches.size() << std::endl;

		return ((float)counterObjects) / ((float)kMaxMatchingSize);
	}

	void draw()
	{
		// Draw matches

		if (matches.size() > 0)
		{
			Mat originMat = toCv(origin);
			Mat targetMat = toCv(target);
			Mat img_matches;
		}
	}
};

class ofApp : public ofBaseApp{

	public:
		void setup();
		//Camera related
		void setupCameraPage();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		//Videos related
		void loadTeamVideos(Team team);
		void setVideoIndex(int newIndex);

		//Load xml
		void loadXml(ofxXmlSettings xml);
		//List of all teams
		std::vector<Team> teams;
		//Current selected team
		Team currentTeam;
		//Current selected player
		Player currentPlayer;
		//xml with teams info
		ofxXmlSettings xml;

		//Pages
		enum class Page {
			Idle,
			Greetings,
			Home,
			Select,
			Camera,
			List,
			Strategy,
			Videos,
			Tutorial
		};

		Page currentPage;
		Page nextPage;

		//Idle
		ofVideoGrabber vidGrabberFace;
		ofxCvColorImage color;
		ofxCvGrayscaleImage gray;
		ofxCvHaarFinder haar;
		ofTrueTypeFont idleFont;
		int faceDetectionTime;

		//Home
		ofImage homeIcon1, homeIcon2, homeIcon3;
		CustomButton* homeButton1;
		CustomButton* homeButton2;
		CustomButton* homeButton3;

		//Select
		ofImage selectIcon1, selectIcon2;
		CustomButton* selectButton1;
		CustomButton* selectButton2;

		//Camera
			// Directory class
		ofDirectory dir;
		string defaultDir;
			// Images information
		vector<ofImage> images;
			// Camera information
		ofVideoGrabber vidGrabber;
		int camWidth;
		int camHeight;
			// OpenCV Information
		OrbClass orb;
		uint64_t lastExecutionTime;

		//List
		ScrollableButtonList* scrollableButtonList;

		//Strategy
		ofImage fieldImage;
		CustomField* customField;
		VerticalElementList* verticalElementList;

		//Videos
		ofImage videoIcon1;
		std::vector<CustomButton*> videoButtons;
		vector<ofVideoPlayer> videos;
		int currentVideoIndex;

		//Tutorial
		ofImage tutorialIcon1, tutorialIcon2, tutorialIcon3;
		CustomLabel* tutorialLabel1;
		CustomLabel* tutorialLabel2;
		CustomLabel* tutorialLabel3;

};
