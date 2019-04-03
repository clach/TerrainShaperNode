#include "graph.h"

Graph::Graph(int x, int y) : xDim(x), yDim(y), maxHeight(1000), detailMaps(std::vector<std::vector<std::vector<vec3>>>()), 
startPoints(std::vector<std::pair<int, int>>())
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
			vec3 color = detailMaps[i][x][y];
			heightSum += 0.21 * color[0] + 0.72 * color[1] + 0.07 * color[2];
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
	}
};

void Graph::shortestPath(std::vector<std::pair<int, int>> startCoords)
{
	std::priority_queue<Node*, std::vector<Node*>, nodeCompare> queue;
	
	// push back start coordinates
	for (int i = 0; i < startCoords.size(); i++)
	{
		Node* n = nodes[startCoords[i].first][startCoords[i].second];
		// assign some height between maxHeight/2 and maxHeight
		float height = maxHeight / 2.f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (maxHeight - maxHeight / 2.f)));
		n->height = height;
		queue.push(n);
	}

	while (!queue.empty())
	{
		// pop current highest node
		Node* n = queue.top();
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
						float d = weightFunctionWithMaps(x, y); // TODO: update this w/ real weight fxn
						if (neighbor->height < n->height - d)
						{
							neighbor->height = n->height - d;
							Node* newNeighbor = new Node(*neighbor);
							queue.push(newNeighbor);
						}
					}
				}
			}
		}
	}
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

		int numStartCoords = 15; // TODO: better way of determining this number
		for (int i = 0; i < numStartCoords; i++)
		{
			Point coords = Point(rand() % xDim, rand() % yDim);
			startCoords.push_back(coords);
		}
	}

	
	// run modified Dijkstra's
	shortestPath(startCoords);
	
	// convert grid to height map (Image) representation
	// TODO: this is kind of ugly
	Image heightMap = Image();
	for (int x = 0; x < xDim; x++)
	{
		std::vector<vec3> heightMapY;
		for (int y = 0; y < yDim; y++)
		{
			heightMapY.push_back(vec3(0, 0, 0));
		}
		heightMap.push_back(heightMapY);
	}

	for (int x = 0; x < xDim; x++)
	{
		for (int y = 0; y < yDim; y++)
		{
			Node* n = nodes[x][y];
			// remap height value from (0, maxHeight) to (0, 255)
			float heightRemapped = n->height * (255.f / maxHeight);
			vec3 pixelColor = vec3(heightRemapped, heightRemapped, heightRemapped);
			heightMap[x][y] = pixelColor;
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
	//return Image();
}

void Graph::setDetailMaps(std::vector<Image> detailMaps) {
	this->detailMaps = detailMaps;
}

void Graph::setStartPoints(std::vector<Point> startPoints) {
	this->startPoints = startPoints;
}

