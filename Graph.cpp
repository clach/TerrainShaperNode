#include "graph.h"

#include <deque>
#include <functional> 

Graph::Graph(int x, int y) : xDim(x), yDim(y), maxHeight(1000), detailMaps(std::vector<Image>()), 
startPoints(std::vector<std::pair<int, int>>())//, dist(std::vector<float>(xDim * yDim, INFINITY))
{}

Graph::~Graph()
{}

float weightFunction()
{
	// TODO: placeholder fxn
	// return some float between (lo, hi)
	float lo = 5;
	float hi = 50;
	return lo + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (hi - lo)));
}

float Graph::weightFunctionDunes(int x1, int y1, int x2, int y2) {
	float deltaMin = 5.0;
	float deltaMax = 50.0;

	float windWeight = weightFunctionWithMaps(x1, y1);

	vec3 windDir = vec3(1, 1, 1);
	windDir.Normalize();
	//vec3 dist = Distance(vec3(x1, y2, 0), vec3(x2, y2, 0));
	vec3 dist = vec3(x1, y2, 0) - vec3(x2, y2, 0);

	float dot = Dot(windDir, dist);
	dot = (dot + 1.0) / 2.0; // remap from (-1, 1) to (0, 1)
	dot = (dot * (deltaMax - deltaMin)) + deltaMin; // remap from (0, 1) to (deltaMin, deltaMax)
	dot *= windWeight;

	return dot;
}

float Graph::weightFunctionWithMaps(int x, int y)
{
	if (!detailMaps.empty())
	{
		float heightSum = 0;
		for (int i = 0; i < detailMaps.size(); i++)
		{
			int r = (int)(detailMaps[i])(x, y, 0, 0);
			int g = (int)(detailMaps[i])(x, y, 0, 1);
			int b = (int)(detailMaps[i])(x, y, 0, 2);
			heightSum += 0.21 * r + 0.72 * g + 0.07 * b;
		}

		float avgHeight = (weightFunction() / 500) * heightSum / detailMaps.size();

		return avgHeight * (1000.0 / 256.0);
	}
	else {
		return weightFunction();
	}

}

// sorts nodes by height, from tallest to shortest heights
struct NodeCompare
{
	bool operator()(const Node* n1, const Node* n2) const
	{
		return n1->height < n2->height;
	}
};

std::vector<std::vector<float>> Graph::shortestPath(std::vector<Point> startCoords)
{
	// make array of size xDim * yDim with all values initialized to 0
	std::vector<float> heightsInner(yDim, 0);
	std::vector<std::vector<float>> heights(xDim, heightsInner);

	std::priority_queue<Node*, std::vector<Node*>, NodeCompare> queue;

	// create grid representation of nodes
	for (int x = 0; x < xDim; x++)
	{
		std::vector<Node*> nodesInner;
		for (int y = 0; y < yDim; y++)
		{
			Node* n = new Node(x, y);
			nodesInner.push_back(n);
		}
		nodes.push_back(nodesInner);
	}

	// push back start coordinates
	for (int i = 0; i < startCoords.size(); i++)
	{
		Node* n = new Node(*(nodes[startCoords[i].first][startCoords[i].second]));
		// assign some height between maxHeight/2 and maxHeight
		float height = maxHeight / 2.f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxHeight - maxHeight / 2.f)));
		n->height = height;
		queue.push(n);

		heights[startCoords[i].first][startCoords[i].second] = height;
	}

	while (!queue.empty())
	{
		// pop current highest node
		Node* n = new Node(*queue.top());
		queue.pop();
		n->processed = true;

		// get 8 surrounding neighbors of node
		int leftIndex = std::max(n->coords.first - 1, 0);
		int rightIndex = std::min(n->coords.first + 1, xDim - 1);
		int downIndex = std::max(n->coords.second - 1, 0);
		int upIndex = std::min(n->coords.second + 1, yDim - 1);

		for (int x = leftIndex; x <= rightIndex; x++)
		{
			for (int y = downIndex; y <= upIndex; y++)
			{
				// skip current node
				if (!(x == n->coords.first && y == n->coords.second))
				{
					Node* neighbor = nodes[x][y];
					
					if (!neighbor->processed)
					{
						float neighborHeight = heights[neighbor->coords.first][neighbor->coords.second];
						float nHeight = heights[n->coords.first][n->coords.second];

						// calculate "edge weight" d dynamically
						//float weight = weightFunctionWithMaps(x, y); // TODO: update this w/ real weight fxn
						float weight = weightFunctionDunes(neighbor->coords.first, neighbor->coords.first, n->coords.first, n->coords.first);

						if (neighborHeight < nHeight - weight)
						{
							neighborHeight = nHeight - weight;
							heights[neighbor->coords.first][neighbor->coords.second] = neighborHeight;

							Node* neighborCopy = new Node(*neighbor);
							neighborCopy->height = neighborHeight;
							queue.push(neighborCopy);
						}
					}
				}
			}
		}
	}

	for (int x = 0; x < xDim; x++)
	{
		for (int y = 0; y < yDim; y++)
		{
			Node* n = nodes[x][y];
			delete n;
		}
	}

	return heights;
}

Image Graph::run()
{
	//srand(time(NULL));
	
	std::vector<Point> startCoords;
	if (!startPoints.empty()) {
		startCoords = startPoints;
	}
	else {
		for (int i = 0; i < numStartPoints; i++)
		{
			Point coords = Point(rand() % xDim, rand() % yDim);
			startCoords.push_back(coords);
		}
	}
	
	// run modified Dijkstra's
	std::vector<std::vector<float>> heights = shortestPath(startCoords);
	
	// convert grid to height map (Image) representation
	Image heightMap(xDim, yDim, 1, 3, 0);
	heightMap.fill(0);

	cimg_forXY(heightMap, x, y) {
		float height = heights[x][y];

		// add some random value to avoid completely flat regions
		float lo = -10;
		float hi = 10;
		float randVal = lo + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (hi - lo)));
		//float randVal = 0;

		// remap height value from (0, maxHeight) to (0, 255)
		float heightRemapped = (height + randVal) * (255.f / maxHeight);

		// TODO: replace with clamp function
		if (heightRemapped > 255) {
			heightRemapped = 255;
		}
		if (heightRemapped < 0) {
			heightRemapped = 0;
		}

		const float color[] = { heightRemapped, heightRemapped, heightRemapped };
		heightMap.draw_point(x, y, color);
	}

	return heightMap;
}

void Graph::setDetailMaps(std::vector<std::string> detailMapsFilenames) {
	for (int i = 0; i < detailMapsFilenames.size(); i++) 
	{
		if (detailMapsFilenames[i] != "") {
			const char* detailMapFilename = (detailMapsFilenames[i]).c_str();
			CImg<unsigned char> detailMap(detailMapFilename);
			detailMap.save("C:\\Users\\caroline\\Documents\\CIS_660_windows\\CImg_detailMapLoadTest.bmp");

			this->detailMaps.push_back(detailMap);
		}
	}
}

void Graph::setStartPointsMap(std::string startPointsFilename)
{
	if (startPointsFilename != "") {
		const char* startPointsFilenameChar = startPointsFilename.c_str();
		CImg<unsigned char> startPointsImage(startPointsFilenameChar);
		startPointsImage.save("C:\\Users\\caroline\\Documents\\CIS_660_windows\\CImg_startPointsLoadTest.bmp");

		cimg_forXY(startPointsImage, x, y) {
			int r = (int)startPointsImage(x, y, 0, 0);
			int g = (int)startPointsImage(x, y, 0, 1);
			int b = (int)startPointsImage(x, y, 0, 2);
			if (r == 0 && b == 0 && g == 0) {
				startPoints.push_back(std::pair<int, int>(x, y));
			}
		}
	}
}

void Graph::setNumStartPoints(int numStartPoints) {
	this->numStartPoints = numStartPoints;
}


