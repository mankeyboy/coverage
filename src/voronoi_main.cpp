#include <cstdio>
#include <search.h>
#include <cstdlib>
#include <queue>
#include <cmath>
#include <iostream>
#include "VoronoiDiagramGenerator.h"


using namespace cv;
using namespace std;



bool IsValidPoint(int x, int y, int r, int c)
{
	//Checks if inside the grid - for corner cases
	if(x<0||y<0||x>=c||y>=r)
		return false;
	return true;
}

double EuclideanDist(Point p1, Point p2)
{
	//OpenCV norm is too clunky but see if you can use it
	double sqdist = (p1.x-p2.x)*(p1.x-p2.x) + (p1.y-p2.y)*(p1.y-p2.y);
	return sqrt(sqdist);
}


void GetCentroids(Mat& img, vector<Point>& sites, vector<Point>& centroids)
{
	//Obtain Centroids of partitions using blob method
	//Partitions will be formed by lines in the image

	int n_sites = sites.size(),i,r=img.rows,c=img.cols,k,l;

	//To remove adding duplicates to BFS queue
	vector< vector<bool> > visited(r,vector<bool>(c,false));

	for(i=0;i<n_sites;i++){

		//Begin BFS
		queue<Point> frontier;
		frontier.push(sites[i]);

		visited[sites[i].y][sites[i].x] = true;
		double avgx=0,avgy=0;
		int n_pix=0;

		while(!frontier.empty()){
			Point top = frontier.front();
			frontier.pop();
			n_pix++;
			avgx += top.x;
			avgy += top.y;

			for(k=top.y-1;k<=top.y+1;k++){
				for(l=top.x-1;l<=top.x+1;l++){
					if(IsValidPoint(l,k,r,c)==false||img.at<uchar>(k,l)==0||visited[k][l]==true)
						continue;
					Point nbr(l,k);
					visited[k][l] = true;
					frontier.push(nbr);
				}
			}
		}

		avgx/=n_pix;
		avgy/=n_pix;

		//Get avg values and centroid
		Point centroid(avgx,avgy);
		centroids[i] = centroid;

	}
}


void GetBestPositions(Mat& img, vector<Point>& sites, vector<Point>& centres, Mat& result)
{
	//Assumes greyscale image of arena - white for free, black for occupied
	int n_sites = sites.size();
	int i,j;
	double sumdist = 1000000;

	vector<float> xValues(n_sites); //Voronoi object takes two separate arrays as args
	vector<float> yValues(n_sites);
	vector<Point> temp_sites(sites); //Copy the site points to temp variable
	vector<Point> temp_centres(n_sites);

	VoronoiDiagramGenerator vdg;

	Mat work_copy;

	while(sumdist > 4*n_sites) //Threshold for convergence
	{
		work_copy = img.clone();

		for(i=0;i<n_sites;i++){
			xValues[i] = temp_sites[i].x;
			yValues[i] = temp_sites[i].y;
		}

		//Get Voronoi Diagram from object
		vdg.generateVoronoi(xValues,yValues,n_sites,0,img.cols-1,0,img.rows-1,3);
		vdg.resetIterator();

		float x1,x2,y1,y2; //To get co-ords of returned lines

		while(vdg.getNext(x1,y1,x2,y2)){
			//Extract truncated lines one by one 
			line(work_copy,Point(x1,y1),Point(x2,y2),Scalar(0),2);
		}

		GetCentroids(work_copy,temp_sites,temp_centres);

		sumdist = 0.0;
		//Set sites to centroids
		for(i=0;i<n_sites;i++){
			sumdist+= EuclideanDist(temp_sites[i],temp_centres[i]);
			temp_sites[i] = temp_centres[i];
		}
	}

	//Final answer is the best estimate
	centres = temp_centres;
	result = work_copy.clone(); //This has the final diagram with lines

}

int main(int argc,char **argv) 
{	

	int n_sites,i;
	string fname;

	//Read image name
	cout<<"Enter image filename: ";
	cin>>fname;
	Mat img = imread(fname,0);

	//Read number of agents (sites)
	cout<<"Number of agents: ";
	cin>>n_sites;

	vector<Point> sites(n_sites); //To store co-ordinates of agents/sites

	//Read starting points of sites - STARTING POSITIONS SHOULD NOT BE IN A BLACK AREA
	for(i=0;i<n_sites;i++){
		int x,y;
		cout<<"Enter x-y of point "<<(i+1)<<" with spaces : ";
		cin>>x>>y;
		sites[i] = Point(x,y);
	}

	Mat result;
	vector<Point> centres(n_sites);

	GetBestPositions(img,sites,centres,result);

	imshow("Result",result);
	waitKey(0);
	
}



