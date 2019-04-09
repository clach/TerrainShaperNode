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


MObject TerrainShaperNode::detailMap;
MObject TerrainShaperNode::startPoints;
MObject TerrainShaperNode::outMesh;
MTypeId TerrainShaperNode::id(0x80000);

void* TerrainShaperNode::creator()
{
	return new TerrainShaperNode;
}

MStatus TerrainShaperNode::initialize()
{
	MFnTypedAttribute inputDetailMapAttr;
	MFnTypedAttribute inputStartPointsAttr;
	MFnTypedAttribute outputGeometryAttr;

	MStatus returnStatus;

	// create input/output attributes
	TerrainShaperNode::detailMap = inputDetailMapAttr.create("detailMap", "dm", MFnData::kString, 0);
	McheckErr(returnStatus, "ERROR creating LSystemNode detail map attribute\n");

	TerrainShaperNode::startPoints = inputStartPointsAttr.create("startPoints", "sp", MFnData::kString, 0);
	McheckErr(returnStatus, "ERROR creating LSystemNode startPoints attribute\n");

	TerrainShaperNode::outMesh = outputGeometryAttr.create("outMesh", "out", MFnData::kMesh, &returnStatus);
	McheckErr(returnStatus, "ERROR creating LSystemNode output attribute\n");

	// add attributes
	returnStatus = addAttribute(TerrainShaperNode::detailMap);
	McheckErr(returnStatus, "ERROR adding detail map attribute\n");

	returnStatus = addAttribute(TerrainShaperNode::startPoints);
	McheckErr(returnStatus, "ERROR adding start points attribute\n");

	returnStatus = addAttribute(TerrainShaperNode::outMesh);
	McheckErr(returnStatus, "ERROR adding output attribute\n");

	// attribute effects
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

		// get input

		// det detail map
		MDataHandle detailMapData = data.inputValue(detailMap, &returnStatus);
		McheckErr(returnStatus, "ERROR getting detail map data handle\n");
		MString detailMapFilename = detailMapData.asString();

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

		std::vector<std::string> detailMaps;
		detailMaps.push_back(detailMapFilename.asChar());
		Image heightMap = runAlgorithm(detailMaps, startPointsFilename.asChar());

		heightMap.save("C:\\Users\\caroline\\Documents\\CIS_660_windows\\TerrainShaperNode\\Images\\OutHeightMap.bmp");


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


Image TerrainShaperNode::runAlgorithm(std::vector<std::string> inDetailMapFilenames, std::string inStartPointsFilenames) {
	// run algorithm and display height map
	Graph g = Graph(512, 512);
	g.setDetailMaps(inDetailMapFilenames);
	g.setStartPoints(inStartPointsFilenames);
	Image heightMap = g.run();

	//Image heightMap = Image();
	return heightMap;

	//displayQImage(heightMap);
	//detailMaps.clear();
	//startPoints.clear();
}