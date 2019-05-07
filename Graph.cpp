#include "graph.h"
#include <functional> 

Graph::Graph(int x, int y) : numSubdivisions(x), xDim(x), yDim(y), maxHeight((1000.f / 512.f) * (float)x), detailMaps(std::vector<Image>()),
	startPoints(std::vector<std::pair<int, int>>()), steepness(0.005)
{
	cimg::exception_mode(0); // quiet cimg exceptions
}

Graph::~Graph()
{}

// clamps val between lo and hi
float clamp(float val, float lo, float hi) {
	if (val < lo) {
		return lo;
	}
	if (val > hi) {
		return hi;
	}
	return val;
}

// returns float between (lo, hi)
float randRange(float lo, float hi)
{
	return lo + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (hi - lo)));
}

// returns value from [0, 255], gets average pixel value of detail maps at pixel (x, y)
float Graph::getDetailMapValue(int x, int y) {
	float height = 0;
	if (!detailMaps.empty())
	{
		for (int i = 0; i < detailMaps.size(); i++)
		{
			int r = (int)(detailMaps[i])(x, y, 0, 0);
			int g = (int)(detailMaps[i])(x, y, 0, 1);
			int b = (int)(detailMaps[i])(x, y, 0, 2);
			height += 0.21 * r + 0.72 * g + 0.07 * b;
		}

		height /= detailMaps.size();
	}
	return height;
}

// weight function using pixel value of detail map (if not detail maps, use random value)
float Graph::weightFunctionNoise(int x, int y)
{
	if (!detailMaps.empty())
	{
		float height = getDetailMapValue(x, y);
		height *= steepness;
		return height;
	}
	else
	{
		return steepness * randRange(0, 255);
	}
}

// weight function using pixel value of detail map (if not detail maps, use random value)
float Graph::weightFunctionFlat(int x, int y)
{
	if (!detailMaps.empty())
	{
		float height = getDetailMapValue(x, y);
		height *= steepness;
		return height;
	}
	else
	{
		return steepness * randRange(0, 255);
	}
}


// weight function to create dunes
float Graph::weightFunctionDunes(Node const * const currNode, Node const * const childNode)
{
	const float deltaMin = 5.0;
	const float deltaMax = 50.0;

	float windWeight = weightFunctionNoise(currNode->coords.first, currNode->coords.second);

	const vec2 windDir = vec2(windDirX, windDirY).Normalize(); // random vector on circle with length of 1
	vec2 dist = vec2(currNode->coords.first - childNode->coords.first, 
					 currNode->coords.second - childNode->coords.second);

	float dot = Dot(windDir, dist);
	dot = (dot + 1.0f) / 2.0f; // remap from (-1, 1) to (0, 1)
	dot = (dot * (deltaMax - deltaMin)) + deltaMin; // remap from (0, 1) to (deltaMin, deltaMax)
	dot *= windWeight;

	dot /= 50;
	//dot = clamp(dot, 0, maxHeight);

	return dot;
}

// weight function to create dunes
float Graph::weightFunctionPeaks(Node const * const initalFeatureNode, Node const * const currNode, Node const * const childNode)
{
	const vec2 windDir = vec2(randRange(0, 10), randRange(0, 10)).Normalize(); // random vector on circle with length of 1
	vec2 dist = vec2(initalFeatureNode->coords.first - childNode->coords.first, 
		initalFeatureNode->coords.second - childNode->coords.second).Normalize(); //get direction to current point.

	float height = pow(0.25f * (1.1f + 0.5f * Dot(dist, windDir)), 0.75f) * max(currNode->height, 0.f);

	return steepness * height;
}

// weight function to create canyons
float Graph::weightFunctionCanyons(Node const * const initialFeatureNode, Node const * const currNode, 
	Node* const childNode)
{
	const int numTerraces = 5; // how many levels of terraces should be in our canyon
	float dh = 0.1f * clamp(getDetailMapValue(currNode->coords.first, currNode->coords.second) / 255.f, 0, 1) * 4.f;
	float t = initialFeatureNode->height;
	
	float phi = 0.9f;
	while (t >= currNode->height) 
	{
		t *= phi; //actually, you can optimize that by using `log` function
	}

	float height = 0;
	if (ceil(currNode->pathLength * numTerraces) != ceil((currNode->pathLength + dh) * numTerraces))
	{
		height = (currNode->height - t) * 1.01f;
	}
	else
	{
		height = 0;
	}

	//check if we should update pathLegnth if using the new value for childNode
	if (currNode->height - height * 1.05f > childNode->height) 
	{ 
		childNode->pathLength = currNode->pathLength + dh;
	}
	return height;
}

// sorts nodes by height, from tallest to shortest heights
struct NodeCompare
{
	bool operator()(const Node* n1, const Node* n2) const
	{
		return n1->height < n2->height;
	}
};

std::vector<std::vector<float>> Graph::shortestPath(short weightFunction, std::vector<Point> startCoords)
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
		float height = randRange(maxHeight / 2.f, maxHeight);
		if (additiveDetailMap && !detailMaps.empty()) {
			// TODO
			//height += getDetailMapValue(startCoords[i].first, startCoords[i].second);
			//height *= (maxHeight / (maxHeight + 255.f)); // remap height back to [0, 1000]
		}
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
					if (n->initialNode == nullptr) 
					{
						// if n is an initial feature point, tell its children
						neighbor->initialNode = n;
					}
					else {
						neighbor->initialNode = n->initialNode;
					}
					
					if (!neighbor->processed)
					{
						float neighborHeight = heights[neighbor->coords.first][neighbor->coords.second];
						float nHeight = heights[n->coords.first][n->coords.second];

						// calculate "edge weight" dynamically
						float weight = 0;
						switch ((int)weightFunction)
						{
						case 0:
							weight = weightFunctionNoise(x, y);
							break;
						case 1:
							weight = weightFunctionDunes(n, neighbor);
							break;
						case 2:
							if (n->initialNode == nullptr)
							{
								weight = weightFunctionPeaks(n, n, neighbor);
							}
							else {
								weight = weightFunctionPeaks(n->initialNode, n, neighbor);
							}
							break;
						case 3:
							if (n->initialNode == nullptr)
							{
								weight = weightFunctionCanyons(n, n, neighbor);
							}
							else {
								weight = weightFunctionCanyons(n->initialNode, n, neighbor);
							}
							break;
						}

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

Image Graph::run(short weightFunction)
{
	//srand(time(NULL));
	
	std::vector<Point> startCoords;
	if (!startPoints.empty()) 
	{
		startCoords = startPoints;
	}
	else
	{
		for (int i = 0; i < numStartPoints; i++)
		{
			Point coords = Point(rand() % xDim, rand() % yDim);
			startCoords.push_back(coords);
		}
	}
	
	// run modified Dijkstra's
	std::vector<std::vector<float>> heights = shortestPath(weightFunction, startCoords);
	
	// convert grid to height map (Image) representation
	Image heightMap(xDim, yDim, 1, 3, 0);
	heightMap.fill(0);

	cimg_forXY(heightMap, x, y)
	{
		float height = heights[x][y];

		// add some random value to avoid completely flat regions
		float lo = -noise;
		float hi = noise;
		float randVal = lo + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (hi - lo)));
		randVal = (randVal / 512.f) * (float)numSubdivisions;

		// remap height value from [-maxHeight, maxHeight] to [0, 255]
		float heightRemapped = ((height + randVal) + maxHeight) * (255.f / (2.f * maxHeight));
		heightRemapped = clamp(heightRemapped, 0, 255);

		const float color[] = { heightRemapped, heightRemapped, heightRemapped };
		heightMap.draw_point(x, y, color);
	}

	return heightMap;
}

void Graph::setDetailMaps(std::vector<std::string> detailMapsFilenames) {
	for (int i = 0; i < detailMapsFilenames.size(); i++) 
	{
		if (detailMapsFilenames[i] != "")
		{
			const char* detailMapFilename = (detailMapsFilenames[i]).c_str();

			try 
			{
				CImg<unsigned char> detailMap(detailMapFilename);
				detailMap.resize(xDim, yDim);
				detailMap.save("C:\\Spring2019\\CIS660\\TerrainShaper\\Images\\detailMapLoadTest.bmp");
				this->detailMaps.push_back(detailMap);
			}
			catch (...) 
			{
				// TODO
			}

		}
	}
}

void Graph::setStartPointsMap(std::string startPointsFilename)
{
	if (startPointsFilename != "") 
	{
		const char* startPointsFilenameChar = startPointsFilename.c_str();

		try 
		{
			CImg<unsigned char> startPointsImage(startPointsFilenameChar);
			startPointsImage.save("C:\\Spring2019\\CIS660\\TerrainShaper\\Images\\startPointsLoadTestBEFORE.bmp");
			startPointsImage.resize(xDim, yDim);
			startPointsImage.save("C:\\Spring2019\\CIS660\\TerrainShaper\\Images\\startPointsLoadTestAFTER.bmp");

			cimg_forXY(startPointsImage, x, y) 
			{
				int r = (int)startPointsImage(x, y, 0, 0);
				int g = (int)startPointsImage(x, y, 0, 1);
				int b = (int)startPointsImage(x, y, 0, 2);
				if (r == 0 && b == 0 && g == 0)
				{
					startPoints.push_back(std::pair<int, int>(x, y));
				}
			}
		}
		catch (...) 
		{
			// TODO
		}
	}
}

void Graph::setNumStartPoints(int numStartPoints) 
{
	this->numStartPoints = numStartPoints;
}

void Graph::setSteepness(float steepness) 
{
	this->steepness = steepness;
}

void Graph::setWindDir(float x, float y)
{
	windDirX = x;
	windDirY = y;
}

void Graph::setNoise(int noise)
{
	this->noise = noise;
}

void Graph::setAdditiveDetailMap(bool additive) {
	this->additiveDetailMap = additive;
}




