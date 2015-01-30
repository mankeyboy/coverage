#include <iostream>
#include <cassert>
#include <cstdlib>
#include "VoronoiDiagramGenerator.h"
#include "Coverage.h"


using namespace cv;
using namespace std;


int main(int argc,char *argv[]) 
{	
	//Command line args are the map file and the number of agents
	assert(argc==3);
	char* fname = argv[1];
	int n_agents = atoi(argv[2]);

	//Obtain image for map
	Mat img = imread(fname,0);

	//Initialize object
	Coverage obj(img,n_agents);
	vector<Point> locations;
	obj.GetBestPositions(locations);

	//Now you can use the points in locations to proceed
	return 0;
	
}



