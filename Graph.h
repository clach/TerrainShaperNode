#pragma once

#include <iostream>
#include <queue>
#include <vec.h>

//#define cimg_use_jpeg
#include "CImg.h"

using namespace cimg_library;

typedef CImg<unsigned char> Image;
typedef std::pair<int, int> Point;

class Node
{
public:
	Node(int x, int y) : processed(false), coords(Point(x, y)), initialNode(nullptr), height(0), pathLength(1.f)
	{}
	Node(const Node& n) : processed(n.processed), coords(n.coords), initialNode(n.initialNode), height(n.height), pathLength(n.pathLength)
	{}

	void operator=(Node n) {
		this->coords = n.coords;
		this->processed = n.processed;
		this->height = n.height;
	}

	bool processed;
	std::pair<int, int> coords;
	Node* initialNode;
	float height;
	float pathLength;

};

class Graph
{

public:
	Graph(int x, int y); // takes in grid dimensions

	~Graph();

	Image run(short weightFunction); // return height map produced by algorithm

	void setDetailMaps(std::vector<std::string> detailMapsFilenames);
	void setStartPointsMap(std::string startPointsFilenames);
	void setNumStartPoints(int numStartPoints);
	void setSteepness(float steepness);
	void setWindDir(vec2 windDir);
	void setNoise(int noise);

protected:
	std::vector<std::vector<float>> shortestPath(short weightFunction, std::vector<Point> startCoords);

	float getDetailMapValue(int x, int y);

	float weightFunctionNoise(int x, int y);
	float weightFunctionFlat(int x, int y);
	float weightFunctionDunes(Node const * const currNode, Node const * const childNode);
	float weightFunctionPeaks(Node const * const initalFeatureNodeNode, Node const * const currNode,
		Node const * const childNode);
	float weightFunctionCanyons(Node const * const initialFeatureNode, Node const * const currNode, 
		Node* const childNode);

	std::vector<Image> detailMaps;
	std::vector<Point> startPoints;

	// representation of graph/grid
	std::vector<std::vector<Node*>> nodes; 

	// dimensions of grid
	int xDim;
	int yDim;

	int numSubdivisions;
	float maxHeight;
	float steepness;
	int numStartPoints;
	vec2 windDir;
	int noise;

};
