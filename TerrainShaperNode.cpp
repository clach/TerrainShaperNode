#include "TerrainShaperNode.h"

#define McheckErr(stat,msg)			\
	if ( MS::kSuccess != stat ) {	\
		cerr << msg;				\
		return MS::kFailure;		\
	}

using namespace cimg_library;

TerrainShaperNode::TerrainShaperNode()
{
}


MObject TerrainShaperNode::time;
MObject TerrainShaperNode::detailMap;
MObject TerrainShaperNode::startPoints;
MObject TerrainShaperNode::outMesh;
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

	TerrainShaperNode::outMesh = outputGeometryAttr.create("outMesh", "out", MFnData::kMesh, &returnStatus);
	McheckErr(returnStatus, "ERROR creating LSystemNode output attribute\n");

	// TODO: add attributes
	returnStatus = addAttribute(TerrainShaperNode::time);
	McheckErr(returnStatus, "ERROR adding time attribute\n");

	returnStatus = addAttribute(TerrainShaperNode::detailMap);
	McheckErr(returnStatus, "ERROR adding detail map attribute\n");

	returnStatus = addAttribute(TerrainShaperNode::startPoints);
	McheckErr(returnStatus, "ERROR adding start points attribute\n");

	returnStatus = addAttribute(TerrainShaperNode::outMesh);
	McheckErr(returnStatus, "ERROR adding output attribute\n");

	// TODO: attribute effects
	returnStatus = attributeAffects(TerrainShaperNode::time, TerrainShaperNode::outMesh);
	McheckErr(returnStatus, "ERROR in attributeAffects for time\n");

	returnStatus = attributeAffects(TerrainShaperNode::detailMap, TerrainShaperNode::outMesh);
	McheckErr(returnStatus, "ERROR in attributeAffects for detail map\n");

	returnStatus = attributeAffects(TerrainShaperNode::startPoints, TerrainShaperNode::outMesh);
	McheckErr(returnStatus, "ERROR in attributeAffects for start point\n");

	return MS::kSuccess;
}

MStatus TerrainShaperNode::compute(const MPlug & plug, MDataBlock & data)
{
	MStatus returnStatus;

	if (plug == outMesh) {

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
		MDataHandle outputHandle = data.outputValue(outMesh, &returnStatus);
		McheckErr(returnStatus, "ERROR getting polygon data handle\n");

		MFnMeshData dataCreator;
		MObject newOutputData = dataCreator.create(&returnStatus);
		McheckErr(returnStatus, "ERROR creating outputData\n");

		// TODO: create new output data

		std::vector<Image> detailMaps;
		std::vector<Point> startPoints;
		Image heightMap = runAlgorithm(detailMaps, startPoints);

		//cv::Mat image(512, 512, CV_8UC3, cv::Scalar(0, 255, 0));
		//cv::imwrite("../CVTest.jpg", image);

		CImg<unsigned char> testImage(512, 512, 1, 3, 0);

		//float color[] = { 255.0,0.0,0.0 };
		cimg_forXY(testImage, x, y) {
			vec3 c = heightMap[x][y];
			float const color[] = { c[0], c[1], c[2] };
			testImage.draw_point(x, y, color);
		}

		for (int x = 0; x < 512; x++)
		{
			for (int y = 0; y < 512; y++)
			{
				vec3 c = heightMap[x][y];
				float const color[] = { c[0], c[1], c[2] };
				testImage.draw_point(x, y, color);
			}
		}
		testImage.save("C:\\Users\\caroline\\Documents\\CIS_660_windows\\CImg_Test.bmp");


		// TODO: use heightMap to make terrain

		MGlobal::executeCommand(MString("test();"));

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