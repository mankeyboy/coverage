//NOT TO BE USED NOW

#include <cstdio>
#include <search.h>
#include <cstdlib>
#include <iostream>
#include "VoronoiDiagramGenerator.h"


using namespace cv;
using namespace std;
const int XMAX = 500;
const int YMAX = 500;

struct PointPair{
	Point2d start;
	Point2d end;
	PointPair(Point2d pt1, Point2d pt2){
		start.x = pt1.x; start.y=pt1.y;
		end.x = pt2.x;end.y = pt2.y;
	}
};

void DisplayPt(Point2d pt){
	cout<<" ("<<pt.x<<","<<pt.y<<") ";
}

bool AreEqual(Point2d pt1, Point2d pt2){
	if(fabs(pt1.x-pt2.x)>1||fabs(pt1.y-pt2.y)>1){
		return false;
	}
	return true;
}

void InsertIfNew(vector<Point2d>& pts, Point2d newpt)
{
	bool is_new = true;
	for(int i=0;i<pts.size();i++){
		if((int)pts[i].x==(int)newpt.x&&(int)pts[i].y==(int)newpt.y){
			is_new=false;
			break;
		}
	}

	if(is_new)
		pts.push_back(newpt);
}

void GetPointsInOrder(vector<PointPair>& pt_pairs, vector<Point2d>& vertices)
{
	int i,j,npairs = pt_pairs.size(),count=2;
	vector<bool> visited(npairs,false);

	cout<<"To Consider "<<endl;

	for(i=0;i<npairs;i++){
		DisplayPt(pt_pairs[i].start);
		DisplayPt(pt_pairs[i].end);
		cout<<endl;
	}

	vertices.push_back(pt_pairs[0].start);
	vertices.push_back(pt_pairs[0].end);
	visited[0]=true;

	Point2d prev(vertices[1].x,vertices[1].y),next;


	do{
		for(i=1;i<npairs;i++){

			if(visited[i]==true)
				continue;

			if(AreEqual(pt_pairs[i].start,prev)==true){
				vertices.push_back(pt_pairs[i].end);
				prev.x=pt_pairs[i].end.x;prev.y=pt_pairs[i].end.y;
				count++;
				visited[i]=true;
			}
			else if(AreEqual(pt_pairs[i].end,prev)==true){
				vertices.push_back(pt_pairs[i].start);
				prev.x=pt_pairs[i].start.x;prev.y=pt_pairs[i].start.y;
				visited[i]=true;
				count++;
			}
			if(count==npairs)
				break;
		}
	}while(count<npairs);
}


Point2d GetCentroid(vector<Point2d>& vertices)
{

	double A=0,num_x=0,num_y=0;
	int n = vertices.size();
	for(int i=0;i<n;i++){

		A+=vertices[i].x*vertices[(i+1)%n].y - vertices[(i+1)%n].x*vertices[i].y;
		num_x+=(vertices[i].x+vertices[(i+1)%n].x)*(vertices[i].x*vertices[(i+1)%n].y - vertices[(i+1)%n].x*vertices[i].y);
		num_y+=(vertices[i].y+vertices[(i+1)%n].y)*(vertices[i].x*vertices[(i+1)%n].y - vertices[(i+1)%n].x*vertices[i].y);
	}

	Point2d result;
	A = A/2.0;
	result.x = (num_x)/(6*A);
	result.y = (num_y)/(6*A);
	return result;
}

int main(int argc,char **argv) 
{	

	float xValues[4] = {55, 235,155,300};
	float yValues[4] = {89, 375,145,255};
	
	long count = 4;
	int i,j;
	double sumdist = 1000000;


	while(1)
	{
		VoronoiDiagramGenerator vdg;
		vector<Point2d> sites(4);
		vdg.generateVoronoi(xValues,yValues,count, 0,XMAX,0,YMAX,3,sites);


		vdg.resetIterator();
		vector< vector<Point2d> > site_vertices;
		vector< vector<Point2d> > temp_site_vertices;
		vector< vector<PointPair> > site_pt_pairs;
		site_pt_pairs.resize(4);
		site_vertices.resize(4);
		temp_site_vertices.resize(4);

		float x1,y1,x2,y2;
		int siteid1,siteid2;

		Mat img = Mat::zeros(YMAX,XMAX,CV_8UC1);
		for(i=0;i<4;i++){
			circle(img,Point2d(xValues[i],yValues[i]),3, Scalar(255),-1);
			cout<<"Site "<<i<<": "<<sites[i].x<<","<<sites[i].y<<endl;
		}

		//printf("\n-------------------------------\n");
		while(vdg.getNext(x1,y1,x2,y2,siteid1,siteid2))
		{
			printf("GOT Line (%f,%f)->(%f,%f) for sites %d,%d\n",x1,y1,x2, y2,siteid1,siteid2);
			line(img, Point2d(x1,y1),Point2d(x2,y2),Scalar(255),2);
			if(siteid1==-1||siteid2==-1)
				continue;
			site_pt_pairs[siteid1].push_back(PointPair(Point2d(x1,y1),Point2d(x2,y2)));
			site_pt_pairs[siteid2].push_back(PointPair(Point2d(x1,y1),Point2d(x2,y2)));
			InsertIfNew(temp_site_vertices[siteid1],Point2d(x1,y1));
			InsertIfNew(temp_site_vertices[siteid1],Point2d(x2,y2));
			InsertIfNew(temp_site_vertices[siteid2],Point2d(x1,y1));
			InsertIfNew(temp_site_vertices[siteid2],Point2d(x2,y2));
		}

		imshow("Field",img);
		waitKey(0);
		//img.release();
		//destroyAllWindows();
		if(sumdist<30){
			waitKey(0);
			break;
		}

		double mindist = XMAX*XMAX + YMAX*YMAX;
		double dist;
		int ind,nsite_pairs,k;
		Point2d copy,copy1,copy2;

		for(i=0;i<4;i++){
			nsite_pairs=temp_site_vertices[i].size();
			for(j=0;j<nsite_pairs-1;j++){
				if(temp_site_vertices[i][j].x==0||temp_site_vertices[i][j].x==XMAX){
					for(k=j+1;k<nsite_pairs;k++){
						if(temp_site_vertices[i][j].x==temp_site_vertices[i][k].x)
							site_pt_pairs[i].push_back(PointPair(temp_site_vertices[i][j],temp_site_vertices[i][k]));
					}
				}
				else if(temp_site_vertices[i][j].y==0||temp_site_vertices[i][j].y==YMAX){
					for(k=j+1;k<nsite_pairs;k++){
						if(temp_site_vertices[i][j].y==temp_site_vertices[i][k].y)
							site_pt_pairs[i].push_back(PointPair(temp_site_vertices[i][j],temp_site_vertices[i][k]));
					}
				}
			}
		}


		for(i=0;i<4;i++){
			dist = sites[i].x*sites[i].x + sites[i].y*sites[i].y;
			if(dist<mindist){
				mindist = dist;
				ind=i;
			} 
		}
		nsite_pairs = site_pt_pairs[ind].size();
		for(i=0;i<nsite_pairs;i++){
			if(site_pt_pairs[ind][i].start.x==0||site_pt_pairs[ind][i].start.y==0)
				site_pt_pairs[ind].push_back(PointPair(site_pt_pairs[ind][i].start,Point2d(0,0)));
			if(site_pt_pairs[ind][i].end.x==0||site_pt_pairs[ind][i].end.y==0)
				site_pt_pairs[ind].push_back(PointPair(site_pt_pairs[ind][i].end,Point2d(0,0)));
		}

		mindist = XMAX*XMAX + YMAX*YMAX;
		for(i=0;i<4;i++){
			dist = (sites[i].x-XMAX)*(sites[i].x-XMAX) + sites[i].y*sites[i].y;
			if(dist<mindist){
				mindist = dist;
				ind=i;
			}
		}
		nsite_pairs = site_pt_pairs[ind].size();
		for(i=0;i<nsite_pairs;i++){
			if(site_pt_pairs[ind][i].start.x==XMAX||site_pt_pairs[ind][i].start.y==0)
				site_pt_pairs[ind].push_back(PointPair(site_pt_pairs[ind][i].start,Point2d(XMAX,0)));
			if(site_pt_pairs[ind][i].end.x==XMAX||site_pt_pairs[ind][i].end.y==0)
				site_pt_pairs[ind].push_back(PointPair(site_pt_pairs[ind][i].end,Point2d(XMAX,0)));
		}


		mindist = XMAX*XMAX + YMAX*YMAX;
		for(i=0;i<4;i++){
			dist = (sites[i].x-XMAX)*(sites[i].x-XMAX) + (sites[i].y-YMAX)*(sites[i].y-YMAX);
			if(dist<mindist){
				mindist = dist;
				ind=i;
			}
		}
		nsite_pairs = site_pt_pairs[ind].size();
		for(i=0;i<nsite_pairs;i++){
			if(site_pt_pairs[ind][i].start.x==XMAX||site_pt_pairs[ind][i].start.y==YMAX)
				site_pt_pairs[ind].push_back(PointPair(site_pt_pairs[ind][i].start,Point2d(XMAX,YMAX)));
			if(site_pt_pairs[ind][i].end.x==XMAX||site_pt_pairs[ind][i].end.y==YMAX)
				site_pt_pairs[ind].push_back(PointPair(site_pt_pairs[ind][i].end,Point2d(XMAX,YMAX)));
		}


		mindist = XMAX*XMAX + YMAX*YMAX;
		for(i=0;i<4;i++){
			dist = (sites[i].x)*(sites[i].x) + (sites[i].y-YMAX)*(sites[i].y-YMAX);
			if(dist<mindist){
				mindist = dist;
				ind=i;
			}
		}
		nsite_pairs = site_pt_pairs[ind].size();
		for(i=0;i<nsite_pairs;i++){
			if(site_pt_pairs[ind][i].start.x==0||site_pt_pairs[ind][i].start.y==YMAX)
				site_pt_pairs[ind].push_back(PointPair(site_pt_pairs[ind][i].start,Point2d(0,YMAX)));
			if(site_pt_pairs[ind][i].end.x==0||site_pt_pairs[ind][i].end.y==YMAX)
				site_pt_pairs[ind].push_back(PointPair(site_pt_pairs[ind][i].end,Point2d(0,YMAX)));
		}


		for(i=0;i<4;i++){
			cout<<"Site "<<i<<": "<<sites[i].x<<","<<sites[i].y<<endl;
			GetPointsInOrder(site_pt_pairs[i],site_vertices[i]);
			for(j=0;j<site_vertices[i].size();j++)
				cout<<site_vertices[i][j].x<<","<<site_vertices[i][j].y<<" ";
			cout<<endl;
		}

		sumdist = 0;
		for(i=0;i<4;i++){
			sites[i]=GetCentroid(site_vertices[i]);
			dist = sqrt((sites[i].x-xValues[i])*(sites[i].x-xValues[i]) + (sites[i].y-yValues[i])*(sites[i].y-yValues[i]));
			sumdist+=dist;
			xValues[i] = sites[i].x;
			yValues[i] = sites[i].y;
			//xValues[i]+=(sites[i].x-xValues[i])/(1+dist);
			//yValues[i]+=(sites[i].y-yValues[i])/(1+dist);

		}
		cout<<"Sumdist is "<<sumdist<<endl;

		
		
	}

	return 0;

	
}



