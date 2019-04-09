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
struct nodeCompare
{
	bool operator()(const Node* n1, const Node* n2) const
	{
		return n1->height < n2->height;
		//return dist[n1->coords.first * n1->coords.second] < dist[n2->coords.first * n2->coords.second];
	}
};

bool Graph::Compare(Node* n1, Node* n2) {
	//return dist[n1->coords.first * n1->coords.second] < dist[n2->coords.first * n2->coords.second];'
	return false;
}




std::vector<std::vector<float>> Graph::shortestPath(std::vector<Point> startCoords)
{
	// make array of size xDim * yDim with all values initialized to 0
	std::vector<float> yHeights(yDim, 0);
	std::vector<std::vector<float>> heights(xDim, yHeights);

	std::priority_queue<Node*, std::vector<Node*>, nodeCompare> queue;

	// push back start coordinates
	for (int i = 0; i < startCoords.size(); i++)
	{
		Node* n = nodes[startCoords[i].first][startCoords[i].second];
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
						// calculate "edge weight" d dynamically
						float weight = weightFunctionWithMaps(x, y); // TODO: update this w/ real weight fxn

						float neighborHeight = heights[neighbor->coords.first][neighbor->coords.second];
						float nHeight = heights[n->coords.first][n->coords.second];

						//if (neighbor->height < n->height - weight)
						if (neighborHeight < nHeight - weight)
						{
							//neighbor->height = n->height - weight;
							neighborHeight = nHeight - weight;
							heights[neighbor->coords.first][neighbor->coords.second] = neighborHeight;

							//Node* newNeighbor = new Node(*neighbor);
							//Node* newNeighbor = new Node(neighbor->coords.first, neighbor->coords.second);
							//newNeighbor->height = neighbor->height;
							Node* newNeighbor = new Node(*neighbor);
							newNeighbor->height = neighborHeight;
							queue.push(newNeighbor);


							//queue.push(neighbor);
						}
					}
				}
			}
		}
	}


	return heights;
}


Image Graph::run()
{
	//srand(time(NULL));

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

	
	std::vector<Point> startCoords;
	if (!startPoints.empty()) {
		startCoords = startPoints;
	}
	else {
		// push back a number of random starting points

		int numStartCoords = 2; // TODO: better way of determining this number
		for (int i = 0; i < numStartCoords; i++)
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

	for (int x = 0; x < xDim; x++)
	{
		for (int y = 0; y < yDim; y++)
		{
			//Node* n = nodes[x][y];
			float height = heights[x][y];

			// add some random value to avoid completely flat regions
			//float lo = -10;
			//float hi = 10;
			//float randVal = lo + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (hi - lo)));
			float randVal = 0;

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
	}

	for (int x = 0; x < xDim; x++)
	{
		for (int y = 0; y < yDim; y++)
		{
			Node* n = nodes[x][y];
			delete n;
		}
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

void Graph::setStartPoints(std::string startPointsFilename)
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

