#include "ofApp.h"
#include <future>
#include <chrono>

#define MAIN_X_OFFSET sb.getWidth()
#define MAIN_WIDTH winWidth - sb.getWidth()
#define SIDE_BUTTON_WIDTH 65
#define SIDE_BUTTON_MARGIN 1

#define LEFT_CLICK_CODE 0

using cv::ORB;

const Mat ver_edge_filter = (Mat1f(2, 2) << 1, 1, -1, -1);
const Mat hor_edge_filter = (Mat1f(2, 2) << 1, 1, -1, -1);
const Mat dia45_edge_filter = (Mat1f(2, 2) << sqrt(2), 0, 0, -sqrt(2));
const Mat dia135_edge_filter = (Mat1f(2, 2) << 0, sqrt(2), -sqrt(2), 0);
const Mat nond_edge_filter = (Mat1f(2, 2) << 2, -2, -2, 2);

//--------------------------------------------------------------
void setupXML(ofImage image, ofxXmlSettings XML)
{

	Mat imgMat;
	cvtColor(Mat(toCv(image)), imgMat, COLOR_BGR2GRAY);
	/* ofxCvGrayscaleImage grayImg;
	grayImg = image;
	Mat imgMat(toCv(grayImg)); */

	double luminanceVer = 0;
	double luminanceHor = 0;
	double luminanceDia45 = 0;
	double luminanceDia135 = 0;
	double luminanceNond = 0;

	double whiteVer = 0;
	double whiteHor = 0;
	double whiteDia45 = 0;
	double whiteDia135 = 0;
	double whiteNond = 0;

	double gabor0 = 0;
	double gabor0White = 0;

	double gabor1 = 0;
	double gabor1White = 0;

	double gabor2 = 0;
	double gabor2White = 0;

	double gabor3 = 0;
	double gabor3White = 0;

	/* double gabor4 = 0;
	double gabor4White = 0;

	double gabor5 = 0;
	double gabor5White = 0;

	double gabor6 = 0;
	double gabor6White = 0;

	double gabor7 = 0;
	double gabor7White = 0; */

	// TODO: Use function buildFilters to get useful info about edge distribution, sum of white points, etc...

	const Mat filters[5] = { ver_edge_filter, hor_edge_filter, dia45_edge_filter, dia135_edge_filter, nond_edge_filter };
	ofImage imageFilter[5];

	for (unsigned int i = 0; i < 5; i++)
	{
		Mat filter = filters[i];
		Mat resMat; // = useFilter(imgMat, filter, CV_8U);
		filter2D(imgMat, resMat, CV_8U, filter);
		ofImage resImg;
		resImg.allocate(image.getWidth(), image.getHeight(), OF_IMAGE_GRAYSCALE);
		toOf(resMat, resImg);

		imageFilter[i] = resImg;

		// TODO: Get info from result image
	}

	// Apply gabor filter
	const int ksize = 9;	  // Size of the kernel
	const double sigma = 2.0; // Standard deviation of the Gaussian envelope
	const double gamma = 0.5;
	const int n_filters = 4;
	ofImage gaborImages[n_filters];

	for (unsigned int i = 2; i < n_filters + 2; i++)
	{
		double rad = i * CV_PI / n_filters;
		// calculate Wavelength of the sinusoidal factor for each i
		// FIXME: get lambd right!
		double lambd = pow(2, i);

		cout << "rad: " << rad << endl;
		cout << "lambd: " << lambd << endl;

		Mat gaborFilter = getGaborKernel(Size(ksize, ksize), sigma, rad, lambd, gamma);
		Mat gaborResult; // = useFilter(imgMat, gaborFilter, CV_8U);
		filter2D(imgMat, gaborResult, CV_8U, gaborFilter);

		ofImage gaborImg;
		gaborImg.allocate(image.getWidth(), image.getHeight(), OF_IMAGE_GRAYSCALE);
		toOf(gaborResult, gaborImg);

		// save gaborimg localy
		string path = "media/gabor/gabor" + to_string(i) + ".png";
		gaborImg.save(path);

		gaborImages[i - 2] = gaborImg;
	}

	double redAverage = 0;
	double greenAverage = 0;
	double blueAverage = 0;
	double luminanceAverage = 0;

	// for each pixel in the image
	ofPixels pixels = image.getPixels();
	int width = image.getWidth();
	int height = image.getHeight();

	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
		{
			ofColor color = image.getColor(i, j);

			// check if pixel is white

			ofColor colorVer = imageFilter[0].getColor(i, j);

			if ((colorVer.r + colorVer.g + colorVer.b) / 3 > 200)
				whiteVer++;

			ofColor colorHor = imageFilter[1].getColor(i, j);

			if ((colorHor.r + colorHor.g + colorHor.b) / 3 > 200)
				whiteHor++;

			ofColor colorDia45 = imageFilter[2].getColor(i, j);

			if ((colorDia45.r + colorDia45.g + colorDia45.b) / 3 > 200)
				whiteDia45++;

			ofColor colorDia135 = imageFilter[3].getColor(i, j);

			if ((colorDia135.r + colorDia135.g + colorDia135.b) / 3 > 200)
				whiteDia135++;

			ofColor colorNond = imageFilter[4].getColor(i, j);

			if ((colorNond.r + colorNond.g + colorNond.b) / 3 > 200)
				whiteNond++;

			luminanceAverage += 0.2125 * color.r + 0.7154 * color.g + 0.0721 * color.b;
			luminanceVer += 0.2125 * colorVer.r + 0.7154 * colorVer.g + 0.0721 * colorVer.b;
			luminanceHor += 0.2125 * colorHor.r + 0.7154 * colorHor.g + 0.0721 * colorHor.b;
			luminanceDia45 += 0.2125 * colorDia45.r + 0.7154 * colorDia45.g + 0.0721 * colorDia45.b;
			luminanceDia135 += 0.2125 * colorDia135.r + 0.7154 * colorDia135.g + 0.0721 * colorDia135.b;
			luminanceNond += 0.2125 * colorNond.r + 0.7154 * colorNond.g + 0.0721 * colorNond.b;

			redAverage += color.r;
			greenAverage += color.g;
			blueAverage += color.b;

			ofColor gabor0Color = gaborImages[0].getColor(i, j);
			gabor0 += 0.2125 * gabor0Color.r + 0.7154 * gabor0Color.g + 0.0721 * gabor0Color.b;

			if ((gabor0Color.r + gabor0Color.g + gabor0Color.b) / 3 > 200)
				gabor0White++;

			ofColor gabor1Color = gaborImages[1].getColor(i, j);
			gabor1 += 0.2125 * gabor1Color.r + 0.7154 * gabor1Color.g + 0.0721 * gabor1Color.b;

			if ((gabor1Color.r + gabor1Color.g + gabor1Color.b) / 3 > 200)
				gabor1White++;

			ofColor gabor2Color = gaborImages[2].getColor(i, j);
			gabor2 += 0.2125 * gabor2Color.r + 0.7154 * gabor2Color.g + 0.0721 * gabor2Color.b;

			if ((gabor2Color.r + gabor2Color.g + gabor2Color.b) / 3 > 200)
				gabor2White++;

			ofColor gabor3Color = gaborImages[3].getColor(i, j);
			gabor3 += 0.2125 * gabor3Color.r + 0.7154 * gabor3Color.g + 0.0721 * gabor3Color.b;

			if ((gabor3Color.r + gabor3Color.g + gabor3Color.b) / 3 > 200)
				gabor3White++;

			/* ofColor gabor4Color = gaborImages[4].getColor(i, j);
			gabor4 += 0.2125 * gabor4Color.r + 0.7154 * gabor4Color.g + 0.0721 * gabor4Color.b;

			ofColor gabor5Color = gaborImages[5].getColor(i, j);
			gabor5 += 0.2125 * gabor5Color.r + 0.7154 * gabor5Color.g + 0.0721 * gabor5Color.b;

			ofColor gabor6Color = gaborImages[6].getColor(i, j);
			gabor6 += 0.2125 * gabor6Color.r + 0.7154 * gabor6Color.g + 0.0721 * gabor6Color.b;

			ofColor gabor7Color = gaborImages[7].getColor(i, j);
			gabor7 += 0.2125 * gabor7Color.r + 0.7154 * gabor7Color.g + 0.0721 * gabor7Color.b; */
		}

	gabor0 /= pixels.getWidth() * pixels.getHeight();
	gabor1 /= pixels.getWidth() * pixels.getHeight();
	gabor2 /= pixels.getWidth() * pixels.getHeight();
	gabor3 /= pixels.getWidth() * pixels.getHeight();
	/* gabor4 /= pixels.getWidth() * pixels.getHeight();
	gabor5 /= pixels.getWidth() * pixels.getHeight();
	gabor6 /= pixels.getWidth() * pixels.getHeight();
	gabor7 /= pixels.getWidth() * pixels.getHeight();
 */
	cout << "gabor0: " << gabor0 << endl;
	cout << "gabor1: " << gabor1 << endl;
	cout << "gabor2: " << gabor2 << endl;
	cout << "gabor3: " << gabor3 << endl;
	/* cout << "gabor4: " << gabor4 << endl;
	cout << "gabor5: " << gabor5 << endl;
	cout << "gabor6: " << gabor6 << endl;
	cout << "gabor7: " << gabor7 << endl; */

	// divide the average by the number of pixels

	redAverage /= pixels.getWidth() * pixels.getHeight();
	greenAverage /= pixels.getWidth() * pixels.getHeight();
	blueAverage /= pixels.getWidth() * pixels.getHeight();
	luminanceAverage /= pixels.getWidth() * pixels.getHeight();
	luminanceVer /= pixels.getWidth() * pixels.getHeight();
	luminanceHor /= pixels.getWidth() * pixels.getHeight();
	luminanceDia45 /= pixels.getWidth() * pixels.getHeight();
	luminanceDia135 /= pixels.getWidth() * pixels.getHeight();
	luminanceNond /= pixels.getWidth() * pixels.getHeight();

	// calculate luminance standart deviation
	// calculate red standart deviation
	// calculate green standart deviation
	// calculate blue standart deviation

	double luminanceStandartDeviation = 0;
	double redStandartDeviation = 0;
	double greenStandartDeviation = 0;
	double blueStandartDeviation = 0;
	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
		{
			ofColor color = image.getColor(i, j);
			luminanceStandartDeviation += pow(0.2125 * color.r + 0.7154 * color.g + 0.0721 * color.b - luminanceAverage, 2);
			redStandartDeviation += pow(color.r - redAverage, 2);
			greenStandartDeviation += pow(color.g - greenAverage, 2);
			blueStandartDeviation += pow(color.b - blueAverage, 2);
		}

	// sqrt the standart deviation
	luminanceStandartDeviation = sqrt(luminanceStandartDeviation / (pixels.getWidth() * pixels.getHeight()));
	redStandartDeviation = sqrt(redStandartDeviation / (pixels.getWidth() * pixels.getHeight()));
	greenStandartDeviation = sqrt(greenStandartDeviation / (pixels.getWidth() * pixels.getHeight()));
	blueStandartDeviation = sqrt(greenStandartDeviation / (pixels.getWidth() * pixels.getHeight()));
	// debug
	cout << "redAverage: " << redAverage << endl;
	cout << "greenAverage: " << greenAverage << endl;
	cout << "blueAverage: " << blueAverage << endl;
	cout << "luminanceAverage: " << luminanceAverage << endl;

	cout << "luminanceVer: " << luminanceVer << endl;
	cout << "luminanceHor: " << luminanceHor << endl;
	cout << "luminanceDia45: " << luminanceDia45 << endl;
	cout << "luminanceDia135: " << luminanceDia135 << endl;
	cout << "luminanceNond: " << luminanceNond << endl;

	XML.addTag("LUMINANCE");
	XML.pushTag("LUMINANCE");
	XML.addValue("AVERAGE", luminanceAverage);
	XML.popTag();
	XML.addTag("COLOR");
	XML.pushTag("COLOR");
	XML.addTag("AVERAGE");
	XML.pushTag("AVERAGE");
	XML.addValue("RED", redAverage);
	XML.addValue("GREEN", greenAverage);
	XML.addValue("BLUE", blueAverage);
	XML.popTag();
	XML.addTag("STANDART_DEVIATION");
	XML.pushTag("STANDART_DEVIATION");
	XML.addValue("LUMINANCE", luminanceStandartDeviation);
	XML.addValue("RED", redStandartDeviation);
	XML.addValue("GREEN", greenStandartDeviation);
	XML.addValue("BLUE", blueStandartDeviation);
	XML.popTag();
	XML.addTag("FILTER_INFO");
	XML.pushTag("FILTER_INFO");
	XML.addTag("VERTICAL_FILTER");
	XML.pushTag("VERTICAL_FILTER");
	XML.addValue("LUMINANCE_AVG", luminanceVer);
	XML.addValue("WHITE_COUNT", whiteVer);
	XML.popTag();
	XML.addTag("HORIZONTAL_FILTER");
	XML.pushTag("HORIZONTAL_FILTER");
	XML.addValue("LUMINANCE_AVG", luminanceHor);
	XML.addValue("WHITE_COUNT", whiteHor);
	XML.popTag();
	XML.addTag("DIAGONAL45_FILTER");
	XML.pushTag("DIAGONAL45_FILTER");
	XML.addValue("LUMINANCE_AVG", luminanceDia45);
	XML.addValue("WHITE_COUNT", whiteDia45);
	XML.popTag();
	XML.addTag("DIAGONAL135_FILTER");
	XML.pushTag("DIAGONAL135_FILTER");
	XML.addValue("LUMINANCE_AVG", luminanceDia135);
	XML.addValue("WHITE_COUNT", whiteDia135);
	XML.popTag();
	XML.addTag("NONDIRECTIONAL_FILTER");
	XML.pushTag("NONDIRECTIONAL_FILTER");
	XML.addValue("LUMINANCE_AVG", luminanceNond);
	XML.addValue("WHITE_COUNT", whiteNond);
	XML.popTag();
	XML.popTag();
	XML.addTag("GABOR_FILTER");
	XML.pushTag("GABOR_FILTER");
	XML.addTag("GABOR0");
	XML.pushTag("GABOR0");
	XML.addValue("LUMINANCE_AVG", gabor0);
	XML.addValue("WHITE_COUNT", gabor0White);
	XML.popTag();
	XML.addTag("GABOR1");
	XML.pushTag("GABOR1");
	XML.addValue("LUMINANCE_AVG", gabor1);
	XML.addValue("WHITE_COUNT", gabor1White);
	XML.popTag();
	XML.addTag("GABOR2");
	XML.pushTag("GABOR2");
	XML.addValue("LUMINANCE_AVG", gabor2);
	XML.addValue("WHITE_COUNT", gabor2White);
	XML.popTag();
	XML.addTag("GABOR3");
	XML.pushTag("GABOR3");
	XML.addValue("LUMINANCE_AVG", gabor3);
	XML.addValue("WHITE_COUNT", gabor3White);
	XML.popTag();
	XML.popTag();
}

//--------------------------------------------------------------
vector<string> compareColorAndLuminance(ofImage frame, ofDirectory dir)
{
	vector<string> paths;

	double luminanceVer = 0;
	double luminanceHor = 0;
	double luminanceDia45 = 0;
	double luminanceDia135 = 0;
	double luminanceNond = 0;

	double gabor0 = 0;
	double gabor0White = 0;

	double gabor1 = 0;
	double gabor1White = 0;

	double gabor2 = 0;
	double gabor2White = 0;

	double gabor3 = 0;
	double gabor3White = 0;

	double whiteVer = 0;
	double whiteHor = 0;
	double whiteDia45 = 0;
	double whiteDia135 = 0;
	double whiteNond = 0;

	int imgWidth = frame.getWidth();
	int imgHeight = frame.getHeight();
	int imgSize = imgWidth * imgHeight;

	// Luminance and Color Average
	double luminanceSum = 0;
	double redSum = 0;
	double greenSum = 0;
	double blueSum = 0;
	double colorSum = 0;
	for (int i = 0; i < imgWidth; i++)
		for (int j = 0; j < imgHeight; j++)
		{
			ofColor color = frame.getColor(i, j);
			luminanceSum += 0.2125 * color.r + 0.7154 * color.g + 0.0721 * color.b;
			redSum += color.r;
			greenSum += color.g;
			blueSum += color.b;
		}
	double luminance_average = luminanceSum / imgSize;
	double red_average = redSum / imgSize;
	double green_average = greenSum / imgSize;
	double blue_average = blueSum / imgSize;

	// Luminance and Color Standard Deviation
	double red_varianceSum = 0;
	double green_varianceSum = 0;
	double blue_varianceSum = 0;
	double luminance_varianceSum = 0;
	for (int i = 0; i < imgWidth; i++)
		for (int j = 0; j < imgHeight; j++)
		{
			ofColor color = frame.getColor(i, j);
			red_varianceSum += pow(color.r - red_average, 2);
			green_varianceSum += pow(color.g - green_average, 2);
			blue_varianceSum += pow(color.b - blue_average, 2);
			double pixelLuminance = 0.2125 * color.r + 0.7154 * color.g + 0.0721 * color.b;
			luminance_varianceSum += pow(pixelLuminance - luminance_average, 2);
		}

	double red_std_deviation = sqrt(red_varianceSum / imgSize);
	double green_std_deviation = sqrt(green_varianceSum / imgSize);
	double blue_std_deviation = sqrt(blue_varianceSum / imgSize);
	double luminance_std_deviation = sqrt(luminance_varianceSum / imgSize);

	int acceptableMatches = 6;
	int acceptableDiffAverage = 45;//60
	//DEVIATION HERE 20
	int acceptableDiffStdDeviation = 5;

	for (int i = 0; i < dir.size(); i++)
	{
		ofxXmlSettings imgXML;
		imgXML.loadFile(dir.getPath(i));

		int matches = 0;

		int imgInfo = imgXML.getValue("IMAGE:LUMINANCE:AVERAGE", 0.000000000);

		if (imgInfo - acceptableDiffAverage < luminance_average &&
			imgInfo + acceptableDiffAverage > luminance_average)
			matches++;

		imgInfo = imgXML.getValue("IMAGE:COLOR:AVERAGE:RED", 0.000000000);

		if (imgInfo - acceptableDiffAverage < red_average &&
			imgInfo + acceptableDiffAverage > red_average)
			matches++;

		imgInfo = imgXML.getValue("IMAGE:COLOR:AVERAGE:GREEN", 0.000000000);

		if (imgInfo - acceptableDiffAverage < green_average &&
			imgInfo + acceptableDiffAverage > green_average)
			matches++;

		imgInfo = imgXML.getValue("IMAGE:COLOR:AVERAGE:BLUE", 0.000000000);

		if (imgInfo - acceptableDiffAverage < blue_average &&
			imgInfo + acceptableDiffAverage > blue_average)
			matches++;

		imgInfo = imgXML.getValue("IMAGE:COLOR:STANDART_DEVIATION:LUMINANCE", 0.000000000);

		if (imgInfo - acceptableDiffStdDeviation < luminance_std_deviation &&
			imgInfo + acceptableDiffStdDeviation > luminance_std_deviation)
			matches++;

		imgInfo = imgXML.getValue("IMAGE:COLOR:STANDART_DEVIATION:RED", 0.000000000);

		if (imgInfo - acceptableDiffStdDeviation < red_std_deviation &&
			imgInfo + acceptableDiffStdDeviation > red_std_deviation)
			matches++;

		imgInfo = imgXML.getValue("IMAGE:COLOR:STANDART_DEVIATION:GREEN", 0.000000000);

		if (imgInfo - acceptableDiffStdDeviation < green_std_deviation &&
			imgInfo + acceptableDiffStdDeviation > green_std_deviation)
			matches++;

		imgInfo = imgXML.getValue("IMAGE:COLOR:STANDART_DEVIATION:GREEN", 0.000000000);

		if (imgInfo - acceptableDiffStdDeviation < blue_std_deviation &&
			imgInfo + acceptableDiffStdDeviation > blue_std_deviation)
			matches++;

		if (matches >= acceptableMatches)
		{
			paths.push_back(dir.getPath(i));
		}
	}

	// FILTRAR POR FILTROS

	frame.setImageType(OF_IMAGE_GRAYSCALE);
	Mat frameMat(toCv(frame));
	const Mat filters[5] = { ver_edge_filter, hor_edge_filter, dia45_edge_filter, dia135_edge_filter, nond_edge_filter };
	ofImage imageFilter[5];

	for (unsigned int i = 0; i < 5; i++)
	{
		Mat filter = filters[i];
		Mat resMat; // = useFilter(imgMat, filter, CV_8U);
		filter2D(frameMat, resMat, CV_8U, filter);
		ofImage resImg;
		resImg.allocate(frame.getWidth(), frame.getHeight(), OF_IMAGE_GRAYSCALE);
		toOf(resMat, resImg);

		imageFilter[i] = resImg;

		// TODO: Get info from result image
	}

	int width = frame.getWidth();
	int height = frame.getHeight();

	vector<string> newPaths;

	int acceptableDiffLuminance = 5; //4
	int acceptableDiffWhites = 25; //15

	for (int i = 0; i < width; i++)
		for (int j = 0; j < height; j++)
		{
			ofColor color = frame.getColor(i, j);

			// check if pixel is white

			ofColor colorVer = imageFilter[0].getColor(i, j);

			if ((colorVer.r + colorVer.g + colorVer.b) / 3 > 200)
				whiteVer++;

			ofColor colorHor = imageFilter[1].getColor(i, j);

			if ((colorHor.r + colorHor.g + colorHor.b) / 3 > 200)
				whiteHor++;

			ofColor colorDia45 = imageFilter[2].getColor(i, j);

			if ((colorDia45.r + colorDia45.g + colorDia45.b) / 3 > 200)
				whiteDia45++;

			ofColor colorDia135 = imageFilter[3].getColor(i, j);

			if ((colorDia135.r + colorDia135.g + colorDia135.b) / 3 > 200)
				whiteDia135++;

			ofColor colorNond = imageFilter[4].getColor(i, j);

			if ((colorNond.r + colorNond.g + colorNond.b) / 3 > 200)
				whiteNond++;

			luminanceVer += 0.2125 * colorVer.r + 0.7154 * colorVer.g + 0.0721 * colorVer.b;
			luminanceHor += 0.2125 * colorHor.r + 0.7154 * colorHor.g + 0.0721 * colorHor.b;
			luminanceDia45 += 0.2125 * colorDia45.r + 0.7154 * colorDia45.g + 0.0721 * colorDia45.b;
			luminanceDia135 += 0.2125 * colorDia135.r + 0.7154 * colorDia135.g + 0.0721 * colorDia135.b;
			luminanceNond += 0.2125 * colorNond.r + 0.7154 * colorNond.g + 0.0721 * colorNond.b;
		}

	luminanceVer /= frame.getWidth() * frame.getHeight();
	luminanceHor /= frame.getWidth() * frame.getHeight();
	luminanceDia45 /= frame.getWidth() * frame.getHeight();
	luminanceDia135 /= frame.getWidth() * frame.getHeight();
	luminanceNond /= frame.getWidth() * frame.getHeight();

	for (int i = 0; i < paths.size(); i++)
	{

		ofxXmlSettings imgXML;
		imgXML.loadFile(paths[i]);

		int matches = 0;

		int imgInfo = imgXML.getValue("IMAGE:COLOR:FILTER_INFO:VERTICAL_FILTER:LUMINANCE_AVG", 0.000000000);

		cout << "imgInfo: " << imgInfo << endl;
		cout << "luminance_average: " << luminanceVer << endl;

		if (imgInfo - acceptableDiffLuminance < luminanceVer &&
			imgInfo + acceptableDiffLuminance > luminanceVer)
			matches++;

		imgInfo = imgXML.getValue("IMAGE:COLOR:FILTER_INFO:VERTICAL_FILTER:WHITES_COUNT", 0.000000000);

		cout << "imgInfo: " << imgInfo << endl;
		cout << "whiteVer: " << whiteVer << endl;

		if (imgInfo - acceptableDiffWhites < whiteVer &&
			imgInfo + acceptableDiffWhites > whiteVer)
			matches++;

		imgInfo = imgXML.getValue("IMAGE:COLOR:FILTER_INFO:HORIZONTAL_FILTER:LUMINANCE_AVG", 0.000000000);

		if (imgInfo - acceptableDiffLuminance < luminanceHor &&
			imgInfo + acceptableDiffLuminance > luminanceHor)
			matches++;

		imgInfo = imgXML.getValue("IMAGE:COLOR:FILTER_INFO:HORIZONTAL_FILTER:WHITES_COUNT", 0.000000000);

		if (imgInfo - acceptableDiffWhites < whiteHor &&
			imgInfo + acceptableDiffWhites > whiteHor)
			matches++;

		imgInfo = imgXML.getValue("IMAGE:COLOR:FILTER_INFO:DIAGONAL45_FILTER:LUMINANCE_AVG", 0.000000000);

		if (imgInfo - acceptableDiffLuminance < luminanceDia45 &&
			imgInfo + acceptableDiffLuminance > luminanceDia45)
			matches++;

		imgInfo = imgXML.getValue("IMAGE:COLOR:FILTER_INFO:DIAGONAL45_FILTER:WHITES_COUNT", 0.000000000);

		if (imgInfo - acceptableDiffWhites < whiteDia45 &&
			imgInfo + acceptableDiffWhites > whiteDia45)
			matches++;

		imgInfo = imgXML.getValue("IMAGE:COLOR:FILTER_INFO:DIAGONAL135_FILTER:LUMINANCE_AVG", 0.000000000);

		if (imgInfo - acceptableDiffLuminance < luminanceDia135 &&
			imgInfo + acceptableDiffLuminance > luminanceDia135)
			matches++;

		imgInfo = imgXML.getValue("IMAGE:COLOR:FILTER_INFO:DIAGONAL135_FILTER:WHITES_COUNT", 0.000000000);

		if (imgInfo - acceptableDiffWhites < whiteDia135 &&
			imgInfo + acceptableDiffWhites > whiteDia135)
			matches++;

		imgInfo = imgXML.getValue("IMAGE:COLOR:FILTER_INFO:NONDIRECTIONAL_FILTER:LUMINANCE_AVG", 0.000000000);

		if (imgInfo - acceptableDiffLuminance < luminanceNond &&
			imgInfo + acceptableDiffLuminance > luminanceNond)
			matches++;

		imgInfo = imgXML.getValue("IMAGE:COLOR:FILTER_INFO:NONDIRECTIONAL_FILTER:WHITES_COUNT", 0.000000000);

		if (imgInfo - acceptableDiffWhites < whiteNond &&
			imgInfo + acceptableDiffWhites > whiteNond)
			matches++;

		cout << "MatchesINFO: " << matches << endl;

		if (matches >= 5)
			newPaths.push_back(imgXML.getValue("IMAGE:NAME", "NULL"));
	}

	return newPaths;
}

//--------------------------------------------------------------
void ofApp::setup(){

    //Screen Size
    // Set the window size to a larger aspect ratio below the portable computer
    int screenWidth = 1366;  // Choose a suitable width
    int screenHeight = 768;   // Choose a suitable height
    ofSetWindowShape(screenWidth, screenHeight);

    ofSetWindowTitle("Custom Button GUI");
    ofSetFrameRate(60);
    ofBackground(0x444444);

    //Load xml
    if (xml.loadFile("xmls/teams.xml")) {
        ofLogError() << "Successfull load teams.xml";
        loadXml(xml);
    }
    else {
        ofLogError() << "Failed to load teams.xml";
    }

    //Current Page
    currentPage = Page::Idle;

    //Home Page
    homeIcon1.load("icons/Strategy.png");
    homeIcon2.load("icons/Video Clip.png");
    homeIcon3.load("icons/Hand Drag.png");

    float buttonWidth = 250;
    float buttonHeight = 250;
    float spacing = 50;
    float startX = (ofGetWidth() - 3 * buttonWidth - 2 * spacing) / 2;
    float startY = ofGetHeight() / 2 - buttonHeight / 2;

    
    homeButton1 = new CustomButton(startX, startY, buttonWidth, buttonHeight, homeIcon1);
    homeButton2 = new CustomButton(startX + buttonWidth + spacing, startY, buttonWidth, buttonHeight, homeIcon2);
    homeButton3 = new CustomButton(startX + 2 * (buttonWidth + spacing), startY, buttonWidth, buttonHeight, homeIcon3);

    homeButton1->setOnClickListener([this]() { 
        currentPage = Page::Select;
        nextPage = Page::Strategy;
        });
    homeButton2->setOnClickListener([this]() { 
        currentPage = Page::Select; 
        nextPage = Page::Videos;
        });
    homeButton3->setOnClickListener([this]() { currentPage = Page::Tutorial; });

    //Select Page
    selectIcon1.load("icons/Palm Scan.png");
    selectIcon2.load("icons/Clipboard List.png");

    buttonWidth = screenWidth / 2;
    buttonHeight = screenHeight;
    spacing = 10;
    startX = (ofGetWidth() - 2 * buttonWidth - spacing) / 2;
    startY = ofGetHeight() / 2 - buttonHeight / 2;

    selectButton1 = new CustomButton(startX, startY, buttonWidth, buttonHeight, selectIcon1);
    selectButton2 = new CustomButton(startX + buttonWidth + spacing, startY, buttonWidth, buttonHeight, selectIcon2, ofColor(0x444444));

    selectButton1->setOnClickListener([this]() { currentPage = Page::Camera;});
    selectButton2->setOnClickListener([this]() { currentPage = Page::List; });

    //List Page
    scrollableButtonList = new ScrollableButtonList(0, screenHeight / 2 - 125, 250, 250, teams);
    scrollableButtonList->setOnClickListener([&]() { 
        if (nextPage == Page::Strategy) {

            currentPage = Page::Strategy;
            currentTeam = scrollableButtonList->getSelectedTeam();

            //Strategy Page
            fieldImage.load("icons/Soccer Field.png");
            customField = new CustomField(80, 50, fieldImage);
            customField->setup();

            verticalElementList = new VerticalElementList((1366 - (400 + 80)), 0, 400, 80, currentTeam.players);
            verticalElementList->setOnClickListener([&]() {
                currentPlayer = verticalElementList->getSelectedPlayer();
                });
        }
        else if (nextPage == Page::Videos) {

            currentPage = Page::Videos;
            currentTeam = scrollableButtonList->getSelectedTeam();

            //Videos Page
            currentVideoIndex = 0;
            loadTeamVideos(currentTeam);

            if (!videos.empty()) {
                videos[currentVideoIndex].play();

                videoIcon1.load("icons/Circled Play.png");


                for (size_t i = 0; i < videos.size(); i++) {
                    float buttonX = 100 + i * (150 + 50);

                    // Dynamically allocate a new CustomButton and store its pointer in videoButtons vector
                    CustomButton* button = new CustomButton(buttonX, 500, 150, 150, videoIcon1);

                    // Use lambda to capture current 'i' value for each button
                    button->setOnClickListener([&, i]() {
                        setVideoIndex(i); // Set the current video index
                        });

                    videoButtons.push_back(button);
                }
            }
        }
    });    

    //Tutorial Page
    tutorialIcon1.load("icons/Hand Up.png");
    tutorialIcon2.load("icons/Clenched Fist.png");
    tutorialIcon3.load("icons/Hand.png");

    std::string text1 = "Pointing hand gesture, use your \n indicative finger, represents a \nselection interaction.";
    std::string text2 = "Clenched fist gesture, it is not\n used as interaction gesture,\n being a safe gesture as rest\n position.";
    std::string text3 = "Hand swipe motion gesture,\n represents a dragging motion in \norder to draw or navigate lists.";

    float labelWidth = 300;
    float labelHeight = 600;
    spacing = 50;
    startX = (ofGetWidth() - 3 * labelWidth - 2 * spacing) / 2;
    startY = ofGetHeight() / 2 - labelHeight / 2;

    tutorialLabel1 = new CustomLabel(startX, startY, labelWidth, labelHeight, tutorialIcon1, text1);
    tutorialLabel2 = new CustomLabel(startX + labelWidth + spacing, startY, labelWidth, labelHeight, tutorialIcon2, text2);
    tutorialLabel3 = new CustomLabel(startX + 2 * (labelWidth + spacing), startY, labelWidth, labelHeight, tutorialIcon3, text3);

	//Camera Page
	setupCameraPage();
    
	//Idle Page	
	// Setup Webcam Grabber
	vidGrabber.setup(320, 240);
	color.allocate(vidGrabber.getWidth(), vidGrabber.getHeight());
	gray.allocate(vidGrabber.getWidth(), vidGrabber.getHeight());
	haar.setup("haarcascade_frontalface_default.xml");	

	idleFont.load("fonts/Roboto-Bold.ttf", 64);
}

//--------------------------------------------------------------
void ofApp::setupCameraPage() {

	
	// Directory Setup
	defaultDir = "media/images";
	dir.listDir(defaultDir);
	dir.sort();
	int nImages = dir.size();

	// Assign Images
	if (nImages)
	{
		images.assign(nImages, ofImage());
	}

	// Load Images
	for (int i = 0; i < nImages; i++)
	{
		cout << "Image " << i << " dir: " << dir.getPath(i) << endl;
		images[i].load(dir.getPath(i));
	}

	// Check if the XML directory is empty
	ofDirectory xmlDir("media/xml");
	xmlDir.allowExt("xml");
	xmlDir.listDir();
	if (xmlDir.size() == 0) {
		// setXMLSettings
		cout << "DIRSIZE " << xmlDir.size() << endl;
		for (unsigned int i = 0; i < nImages; i++) {
			// TODO: Check if xml of image exist, otherwise create it

			xml.addTag("IMAGE");
			xml.pushTag("IMAGE");
			xml.addValue("NAME", dir.getPath(i));
			// xml.addValue("VIDEO", "media/videos/promo/" + to_string(i));

			ofImage target = images[i];
			setupXML(target, xml);
			xml.popTag();
			// FIXME: save xml inside function
			xml.saveFile("media/xml/" + to_string(i) + ".xml");
			xml.clear();
		}
	}

	//Execution time variable
	lastExecutionTime = 0;
}

//--------------------------------------------------------------
void ofApp::update(){
	if (currentPage == Page::Idle) {
		vidGrabber.update();
		color.setFromPixels(vidGrabber.getPixels());
		if (vidGrabber.isFrameNew()) {
			color.setFromPixels(vidGrabber.getPixels());
			gray = color;
			haar.findHaarObjects(gray);
			// Log the size of haar.blobs
			ofLog() << "Number of blobs detected: " << haar.blobs.size();
			if (haar.blobs.size() != 0) {
				faceDetectionTime ++;  // Add the time of the last frame
				ofLog() << "counter: " << faceDetectionTime;
				if (faceDetectionTime >= 10) {
				
					vidGrabber.close();

					// Setup Camera
					camWidth = 640;
					camHeight = 500;

					// Setup Webcam Grabber
					vidGrabber.setVerbose(true);
					vidGrabber.setup(camWidth, camHeight);

					currentPage = Page::Home;
				}
			}
			else {
				// Reset the timer if the condition is not met
				faceDetectionTime = 0;
			}
		}
	}
	else if (currentPage == Page::Videos) {
		if (!videos.empty()) {
			videos[currentVideoIndex].update();
		}
	}
	else if (currentPage == Page::Camera) {
		vidGrabber.update();

		uint64_t currentTime = ofGetElapsedTimeMillis(); // Get the current time in milliseconds

		if (currentTime - lastExecutionTime >= 4000) { // Check if 4 seconds have passed
			lastExecutionTime = currentTime; // Update the last execution time

			ofDirectory xmlDir;
			xmlDir.listDir("media/xml");

			ofImage frame;
			frame.setFromPixels(vidGrabber.getPixels());
			// Log image properties to check its structure
			vector<string> compareResult = compareColorAndLuminance(frame, xmlDir);

			vector<ofImage> compareImgs;
			compareImgs.assign(compareResult.size(), ofImage());

			cout << "compareResult.size(): " << compareResult.size() << endl;
			cout << "compareImgs.size(): " << compareImgs.size() << endl;

			for (unsigned int i = 0; i < compareResult.size(); i++) {
				cout << "Loading image " << compareResult[i] << endl;
				ofImage img;
				compareImgs[i].load(compareResult[i]);
			}

			for (unsigned int i = 0; i < compareImgs.size(); i++) {
				ofImage img;

				img.setFromPixels(vidGrabber.getPixels());
				orb.setOrigin(img);

				orb.setTarget(compareImgs[i]);
				orb.detectAndCompute();
				if (orb.match() > 0) {
					std::string detectedItemPath = xmlDir.getPath(i);
					cout << "detectedItem: " << dir.getPath(i) << endl;
					
					// Check if the path contains a team name
					for (const auto& team : teams) {
						if (detectedItemPath.find(team.name) != std::string::npos) {
							cout << "Matched team: " << team.name << endl;
							// TODO: CHANGE PAGE DEPENDING ON DIR PATH AND TEAM NAME
							// Page logic
							if (nextPage == Page::Strategy) {

								currentPage = Page::Strategy;
								currentTeam = team;

								// Strategy Page
								fieldImage.load("icons/Soccer Field.png");
								customField = new CustomField(80, 50, fieldImage);
								customField->setup();

								verticalElementList = new VerticalElementList((1366 - (400 + 80)), 0, 400, 80, currentTeam.players);
								verticalElementList->setOnClickListener([&]() {
									currentPlayer = verticalElementList->getSelectedPlayer();
									});
							}
							else if (nextPage == Page::Videos) {

								currentPage = Page::Videos;
								currentTeam = team;

								// Videos Page
								currentVideoIndex = 0;
								loadTeamVideos(currentTeam);

								if (!videos.empty()) {
									videos[currentVideoIndex].play();

									videoIcon1.load("icons/Circled Play.png");

									for (size_t i = 0; i < videos.size(); i++) {
										float buttonX = 100 + i * (150 + 50);

										// Dynamically allocate a new CustomButton and store its pointer in videoButtons vector
										CustomButton* button = new CustomButton(buttonX, 500, 150, 150, videoIcon1);

										// Use lambda to capture current 'i' value for each button
										button->setOnClickListener([&, i]() {
											setVideoIndex(i); // Set the current video index
											});

										videoButtons.push_back(button);
									}
								}
							}
							break; // Exit the loop once a match is found
						}
					}
				}
			}
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	if (currentPage == Page::Idle) {
		color.draw(0, 0);
		for (int i = 0; i < haar.blobs.size(); i++)
		{
			ofSetColor(255);
			ofNoFill();
			ofDrawRectangle(haar.blobs[i].boundingRect);
		}

		ofFill();
		// Calculate the position to center the text
		std::string text = "Greetings";
		ofRectangle textBounds = idleFont.getStringBoundingBox(text, 0, 0);
		int textX = (ofGetWidth() - textBounds.getWidth()) / 2;
		int textY = (ofGetHeight() - textBounds.getHeight()) / 2; ;

		// Draw the text
		idleFont.drawString(text, textX, textY);
	}
    else if (currentPage == Page::Home) {
        homeButton1->draw();
        homeButton2->draw();
        homeButton3->draw();
    }
    else if (currentPage == Page::Select) {
        selectButton1->draw();
        selectButton2->draw();
    }
    else if (currentPage == Page::List) {
        scrollableButtonList->draw();
    }
    else if (currentPage == Page::Videos) {
        if (!videos.empty()) {
            videos[currentVideoIndex].draw(((1366 - 800) / 2) , 50, 800, 400);
        }
        
        for (auto button : videoButtons) {
            button->draw();
        }
    }
    else if (currentPage == Page::Tutorial) {
        tutorialLabel1->draw();
        tutorialLabel2->draw();
        tutorialLabel3->draw();
    }
    else if (currentPage == Page::Camera) {
		// Calculate the position to center the vidGrabber in the window
		int windowWidth = ofGetWidth();
		int windowHeight = ofGetHeight();
		int videoWidth = vidGrabber.getWidth();
		int videoHeight = vidGrabber.getHeight();

		int x = (windowWidth - videoWidth) / 2;
		int y = (windowHeight - videoHeight) / 2;

		// Draw the vidGrabber at the calculated position
		vidGrabber.draw(x, y);
    }
    else if (currentPage == Page::Strategy) {
        customField->draw();
        verticalElementList->draw();
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == 'b') {
        ofLog() << "Key 'b' pressed, setting current page to Home";
        currentPage = Page::Home;
        videos.clear();
    }

	if (currentPage == Page::Camera) {
		if (key == 'p' || key == 'P') {
			// Get the current frame from the video grabber
			ofImage image;
			image.setFromPixels(vidGrabber.getPixels());

			// Define the path and filename
			std::string filename = "media/images/captured_frame_" + ofGetTimestampString() + ".png";

			// Save the image
			image.save(filename);

			// Print a message to the console
			ofLogNotice() << "Saved frame to " << filename;
		}
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    if (currentPage == Page::List) {
        scrollableButtonList->mouseDragged(x, y);
    }
    else if (currentPage == Page::Strategy) {
        customField->mouseDragged(x, y);
        verticalElementList->mouseDragged(x, y);
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
    if (currentPage == Page::Home) {
        homeButton1->mousePressed(x, y);
        homeButton2->mousePressed(x, y);
        homeButton3->mousePressed(x, y);
    }
    else if (currentPage == Page::Select) {
        selectButton1->mousePressed(x, y);
        selectButton2->mousePressed(x, y);
    }
    else if (currentPage == Page::List) {
        scrollableButtonList->mousePressed(x, y);
    }
    else if (currentPage == Page::Strategy) {
        customField->mousePressed(x, y, currentPlayer);
        verticalElementList->mousePressed(x, y);
    }
    else if (currentPage == Page::Videos) {
        for (auto button : videoButtons) {
            button->mousePressed(x, y);
        }
    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    if (currentPage == Page::List) {
    scrollableButtonList->mouseReleased(x, y);
    }  
    else if (currentPage == Page::Strategy) {
        customField->mouseReleased(x, y);
        verticalElementList->mouseReleased(x, y);
    }
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

//--------------------------------------------------------------
void ofApp::loadXml(ofxXmlSettings xml) {
    // Change the root to the <teams> node
    xml.pushTag("teams");

    // Count the <team> tags
    int numTeams = xml.getNumTags("team");

    for (int i = 0; i < numTeams; ++i) {
        Team team;
        xml.pushTag("team", i);

        // Load team name and logo
        team.name = xml.getValue("name", "");
        std::string logoPath = "images/teams/" + xml.getValue("logo", "");
        if (!team.logo.load(logoPath)) {
            ofLogError() << "Failed to load logo: " << logoPath;
        }


        // Load players
        xml.pushTag("players");
        int numPlayers = xml.getNumTags("player");
        for (int j = 0; j < numPlayers; ++j) {
            Player player;
            xml.pushTag("player", j);
            player.number = xml.getValue("number", 0);
            player.name = xml.getValue("name", "");
            std::string playerImagePath = "images/players/" + xml.getValue("image", "");
            if (!player.image.load(playerImagePath)) {
                ofLogError() << "Failed to load player image: " << playerImagePath;
            }
            team.players.push_back(player);
            xml.popTag(); // player
        }
        xml.popTag(); // players

        // Load videos
        xml.pushTag("videos");
        int numVideos = xml.getNumTags("video");
        for (int k = 0; k < numVideos; ++k) {
            team.videos.push_back("videos/" + xml.getValue("video", "", k));
        }
        xml.popTag(); // videos

        teams.push_back(team);
        xml.popTag(); // team
    }

    // Don't forget to pop the <teams> tag when done
    xml.popTag();
}

//--------------------------------------------------------------
void ofApp::loadTeamVideos(Team team) {
    videos.clear();
    
    for (const auto& videoFile : team.videos) {
        ofVideoPlayer video;
        video.load(videoFile);
        videos.push_back(video);
    }
    
}

//--------------------------------------------------------------
void ofApp::setVideoIndex(int newIndex) {
    if (!videos.empty() && newIndex >= 0 && newIndex < videos.size()) {
        videos[currentVideoIndex].stop();
        currentVideoIndex = newIndex;
        videos[currentVideoIndex].play();
    }
}

