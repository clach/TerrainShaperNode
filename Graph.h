#pragma once

//#include <bits/stdc++.h>

#include <iostream>
#include <list>
#include <queue>
#include <vec.h>

#include "CImg.h"

using namespace cimg_library;

typedef CImg<unsigned char> Image;
typedef std::pair<int, int> Point;

class Node
{
public:
	Node(int x, int y) : processed(false), coords(Point(x, y)), height(0) {}
	Node(const Node& n) : processed(n.processed), coords(n.coords), height(n.height) {}

	void operator=(Node n) {
		this->coords = n.coords;
		this->processed = n.processed;
		this->height = n.height;
	}

	bool processed;
	std::pair<int, int> coords;
	float height;

};

class Graph
{

public:
	Graph(int x, int y); // takes in grid dimensions
	~Graph();

	Image run(); // return height map produced by algorithm

	void setDetailMaps(std::vector<std::string> detailMapsFilenames);
	void setStartPointsMap(std::string startPointsFilenames);
	void setNumStartPoints(int numStartPoints);

protected:
	std::vector<std::vector<float>> shortestPath(std::vector<Point> startCoords);

	float weightFunctionWithMaps(int x, int y);
	float weightFunctionDunes(int x1, int y1, int x2, int y2);

	std::vector<Image> detailMaps;
	std::vector<Point> startPoints;

	// representation of graph/grid
	std::vector<std::vector<Node*>> nodes; 

	// dimensions of grid
	int xDim;
	int yDim;

	// maximum height of any grid coordinate
	// (all start coords will have max height)
	float maxHeight;

	int numStartPoints;

};
