/*This is the class for optimal coverage of a given map,
using a known number of agents*/

#ifndef COVERAGE
#define COVERAGE

#include <vector>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace cv;

inline bool IsValidPoint(int x, int y, int r, int c)
{
	//Checks if inside the grid - for corner cases
	if(x<0||y<0||x>=c||y>=r)
		return false;
	return true;
}

inline double EuclideanDist(Point p1, Point p2)
{
	//OpenCV norm is too clunky but see if you can use it
	double sqdist = (p1.x-p2.x)*(p1.x-p2.x) + (p1.y-p2.y)*(p1.y-p2.y);
	return sqrt(sqdist);
}

class Coverage
{
public:
	Coverage();
	Coverage(Mat& img, int n):map(img), n_agents(n){};
	~Coverage();

	void GetBestPositions(vector<Point>& positions);

private:
	Mat map;
	int n_agents;
	void GetCentroids(Mat& img,vector<Point>& sites, vector<Point>& centroids);
	void GetStartingPoints(vector<Point>& sites);
	void DisplayPoints(vector<Point>& locations);
};

#endif