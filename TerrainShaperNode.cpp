#include "TerrainShaperNode.h"

#define McheckErr(stat,msg)			\
	if ( MS::kSuccess != stat ) {	\
		cerr << msg;				\
		return MS::kFailure;		\
	}

TerrainShaperNode::TerrainShaperNode()
{
}


MObject TerrainShaperNode::time;
MObject TerrainShaperNode::detailMap;
MObject TerrainShaperNode::startPoints;
MObject TerrainShaperNode::outputMesh;
MTypeId TerrainShaperNode::id(0x80000);
// TODO: more input MObjects

void* TerrainShaperNode::creator()
{
	return new TerrainShaperNode;
}

MStatus TerrainShaperNode::initialize()
{
	// TODO: more input attrs
	MFnUnitAttribute inputTimeAttr;
	MFnTypedAttribute inputDetailMapAttr;
	MFnTypedAttribute inputStartPointsAttr;
	MFnTypedAttribute outputGeometryAttr;

	MStatus returnStatus;

	// TODO: create input/output attributes
	TerrainShaperNode::time = inputTimeAttr.create("time", "tm", MFnUnitAttribute::kTime, 0.0, &returnStatus);
	McheckErr(returnStatus, "ERROR creating LSystemNode time attribute\n");

	TerrainShaperNode::detailMap = inputDetailMapAttr.create("detailMap", "dm", MFnData::kString, 0);
	McheckErr(returnStatus, "ERROR creating LSystemNode detail map attribute\n");

	TerrainShaperNode::startPoints = inputStartPointsAttr.create("startPoints", "sp", MFnData::kString, 0);
	McheckErr(returnStatus, "ERROR creating LSystemNode startPoints attribute\n");

	TerrainShaperNode::outputMesh = outputGeometryAttr.create("outputMesh", "out", MFnData::kMesh, &returnStatus);
	McheckErr(returnStatus, "ERROR creating LSystemNode output attribute\n");

	// TODO: add attributes
	returnStatus = addAttribute(TerrainShaperNode::time);
	McheckErr(returnStatus, "ERROR adding time attribute\n");

	returnStatus = addAttribute(TerrainShaperNode::detailMap);
	McheckErr(returnStatus, "ERROR adding detail map attribute\n");

	returnStatus = addAttribute(TerrainShaperNode::startPoints);
	McheckErr(returnStatus, "ERROR adding start points attribute\n");

	returnStatus = addAttribute(TerrainShaperNode::outputMesh);
	McheckErr(returnStatus, "ERROR adding output attribute\n");

	// TODO: attribute effects
	returnStatus = attributeAffects(TerrainShaperNode::time, TerrainShaperNode::outputMesh);
	McheckErr(returnStatus, "ERROR in attributeAffects for time\n");

	returnStatus = attributeAffects(TerrainShaperNode::detailMap, TerrainShaperNode::outputMesh);
	McheckErr(returnStatus, "ERROR in attributeAffects for detail map\n");

	returnStatus = attributeAffects(TerrainShaperNode::startPoints, TerrainShaperNode::outputMesh);
	McheckErr(returnStatus, "ERROR in attributeAffects for start point\n");

	return MS::kSuccess;
}

MStatus TerrainShaperNode::compute(const MPlug & plug, MDataBlock & data)
{
	MStatus returnStatus;

	if (plug == outputMesh) {

		// TODO: get input 

		// get time
		MDataHandle timeData = data.inputValue(time, &returnStatus);
		McheckErr(returnStatus, "ERROR getting time data handle\n");
		MTime time = timeData.asTime();

		// det detail map
		MDataHandle detailMapData = data.inputValue(detailMap, &returnStatus);
		McheckErr(returnStatus, "ERROR getting detail map data handle\n");
		MString detailMapFilename = detailMapData.asString();

		//MImage detailMap = MImage();
		//detailMap.readFromFile(detailMapFilename);


		// get start points
		MDataHandle startPointsData = data.inputValue(startPoints, &returnStatus);
		McheckErr(returnStatus, "ERROR getting start points data handle\n");
		MString startPointsFilename = startPointsData.asString();

		// get output geometry //////
		MDataHandle outputHandle = data.outputValue(outputMesh, &returnStatus);
		McheckErr(returnStatus, "ERROR getting polygon data handle\n");

		MFnMeshData dataCreator;
		MObject newOutputData = dataCreator.create(&returnStatus);
		McheckErr(returnStatus, "ERROR creating outputData\n");

		// TODO: create new output data

		std::vector<Image> detailMaps;
		std::vector<Point> startPoints;
		Image heightMap = runAlgorithm(detailMaps, startPoints);

		MImage image;
		image.create(10, 10, 3);
		
		unsigned char* pixels = image.pixels();

		unsigned int x, y;
		for (y = 0; y < 10; ++y)
		{
			for (x = 0; x < 10; ++x)
			{
				*pixels++ = 1;
				*pixels++ = 0;
				*pixels++ = 0;
			}
		}
		image.setPixels(pixels, 10, 10);

		image.writeToFile(MString("C:\\Users\\caroline\\Documents\\CIS_660_windows\\imageTest.iff"));

		// TODO: use heightMap to make terrain



		outputHandle.set(newOutputData);
		data.setClean(plug);
	}
	else {
		return MS::kUnknownParameter;
	}

	return MStatus::kSuccess;
}


Image TerrainShaperNode::runAlgorithm(std::vector<Image> inDetailMaps, std::vector<Point> inStartPoints) {
	// run algorithm and display height map
	Graph g = Graph(512, 512);
	g.setDetailMaps(inDetailMaps);
	g.setStartPoints(inStartPoints);
	Image heightMap = g.run();

	//Image heightMap = Image();
	return heightMap;

	//displayQImage(heightMap);
	//detailMaps.clear();
	//startPoints.clear();
}