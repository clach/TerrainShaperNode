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

class NodeCompareClass {
public:

	bool operator()(const Node* n1, const Node* n2) const
	{
		//return n1->height < n2->height;
		//return Graph::dist[n1->coords.first * n1->coords.second] < dist[n2->coords.first * n2->coords.second];
	}
};

class Graph
{
protected:
	std::vector<std::vector<float>> shortestPath(std::vector<Point> startCoords);

	float weightFunctionWithMaps(int x, int y);

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

public:
	Graph(int x, int y); // takes in grid dimensions
	~Graph();

	Image run(); // return height map produced by algorithm

	void setDetailMaps(std::vector<std::string> detailMapsFilenames);
	void setStartPoints(std::string startPointsFilenames);


};
