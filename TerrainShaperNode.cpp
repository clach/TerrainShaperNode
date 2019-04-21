#include "TerrainShaperNode.h"

#define McheckErr(stat,msg)			\
	if ( MS::kSuccess != stat ) {	\
		cerr << msg;				\
		return MS::kFailure;		\
	}

using namespace cimg_library;

MObject TerrainShaperNode::detailMap;
MObject TerrainShaperNode::startPointsMap;
MObject TerrainShaperNode::numStartPoints;
MObject TerrainShaperNode::strength;
MTypeId TerrainShaperNode::id(0x80000);

void* TerrainShaperNode::creator()
{
	return new TerrainShaperNode;
}

MStatus TerrainShaperNode::initialize()
{
	MFnTypedAttribute inputDetailMapAttr;
	MFnTypedAttribute inputStartPointsMapAttr;
	MFnNumericAttribute inputNumStartPointsAttr;
	MFnNumericAttribute inputStrengthAttr;

	MStatus returnStatus;

	// create input/output attributes
	TerrainShaperNode::detailMap = inputDetailMapAttr.create("detailMap", "dm", MFnData::kString, 0);
	McheckErr(returnStatus, "ERROR creating TerrainShaperNode detail map attribute\n");

	TerrainShaperNode::startPointsMap = inputStartPointsMapAttr.create("startPointsMap", "spm", MFnData::kString, 0);
	McheckErr(returnStatus, "ERROR creating TerrainShaperNode start points map attribute\n");

	TerrainShaperNode::numStartPoints = inputNumStartPointsAttr.create("numberStartPoints", "nsp", MFnNumericData::kInt, 5);
	McheckErr(returnStatus, "ERROR creating TerrainShaperNode number of start points attribute\n");

	TerrainShaperNode::strength = inputStrengthAttr.create("strength", "s", MFnNumericData::kFloat, 5);
	McheckErr(returnStatus, "ERROR creating TerrainShaperNode strength attribute\n");

	// add attributes
	returnStatus = addAttribute(TerrainShaperNode::detailMap);
	McheckErr(returnStatus, "ERROR adding detail map attribute\n");

	returnStatus = addAttribute(TerrainShaperNode::startPointsMap);
	McheckErr(returnStatus, "ERROR adding start points map attribute\n");

	returnStatus = addAttribute(TerrainShaperNode::numStartPoints);
	McheckErr(returnStatus, "ERROR adding num start points attribute\n");

	returnStatus = addAttribute(TerrainShaperNode::strength);
	McheckErr(returnStatus, "ERROR adding strength attribute\n");

	// attribute effects
	returnStatus = attributeAffects(TerrainShaperNode::detailMap, TerrainShaperNode::outputGeom);
	McheckErr(returnStatus, "ERROR in attributeAffects for detail map\n");

	returnStatus = attributeAffects(TerrainShaperNode::startPointsMap, TerrainShaperNode::outputGeom);
	McheckErr(returnStatus, "ERROR in attributeAffects for start points map\n");

	returnStatus = attributeAffects(TerrainShaperNode::numStartPoints, TerrainShaperNode::outputGeom);
	McheckErr(returnStatus, "ERROR in attributeAffects for number of start points\n");

	returnStatus = attributeAffects(TerrainShaperNode::strength, TerrainShaperNode::outputGeom);
	McheckErr(returnStatus, "ERROR in attributeAffects for strength\n");

	return MS::kSuccess;
}

MStatus TerrainShaperNode::deform(MDataBlock& data, MItGeometry& itGeo,
	const MMatrix &localToWorldMatrix, unsigned int mIndex) {
	MStatus returnStatus;

	// get input

	// Get the envelope
	float env = data.inputValue(envelope).asFloat();

	// det detail map
	MDataHandle detailMapData = data.inputValue(detailMap, &returnStatus);
	McheckErr(returnStatus, "ERROR getting detail map data handle\n");
	MString detailMapFilename = detailMapData.asString();

	// get start points map
	MDataHandle startPointsMapData = data.inputValue(startPointsMap, &returnStatus);
	McheckErr(returnStatus, "ERROR getting start points data handle\n");
	MString startPointsMapFilename = startPointsMapData.asString();

	// get num start points
	MDataHandle numStartPointsData = data.inputValue(numStartPoints, &returnStatus);
	McheckErr(returnStatus, "ERROR getting number of start points data handle\n");
	int numStartPointsVal = numStartPointsData.asInt();

	// get strength
	MDataHandle strengthData = data.inputValue(strength, &returnStatus);
	McheckErr(returnStatus, "ERROR getting strength data handle\n");
	float strengthVal = strengthData.asFloat();

	// create height map
	std::vector<std::string> detailMaps;
	detailMaps.push_back(detailMapFilename.asChar());
	Image heightMap = runAlgorithm(detailMaps, startPointsMapFilename.asChar(), numStartPointsVal);

	// save height map as image
	heightMap.save("C:\\Users\\caroline\\Documents\\CIS_660_windows\\TerrainShaperNode\\Images\\OutHeightMap.bmp");

	std::vector<float> heightsVector;
	float weightedStrength = strengthVal / 255.f;
	// fill point array with height values from height map
	//cimg_forXY(heightMap, x, y) {
	//	int r = (int)heightMap(x, y, 0, 0);
		//int g = (int)heightMap(x, y, 0, 1);
		///int b = (int)heightMap(x, y, 0, 2);
		//float height = 0.21 * r + 0.72 * g + 0.07 * b; // height is from [0, 255]
		//height *= strength; // weight height by strength
		//floatArray.append(height);
		//heightsVector.push_back(height);
	//}

	// TODO: can probably combine these two following blocks of code
	for (int x = 0; x < 512; x++) {
		for (int y = 511; y >= 0; y--) {
			int r = (int)heightMap(x, y, 0, 0);
			int g = (int)heightMap(x, y, 0, 1);
			int b = (int)heightMap(x, y, 0, 2);
			float height = 0.21 * r + 0.72 * g + 0.07 * b; // height is from [0, 255]
			height *= weightedStrength; // weight height by strength
			heightsVector.push_back(height);
		}
	}

	MPoint pt;
	for (; !itGeo.isDone(); itGeo.next()) {
		// get the input point
		pt = itGeo.position();

		// perform the deformation
		pt = pt + MPoint(0, heightsVector[itGeo.index()], 0);

		// set the new output point
		itGeo.setPosition(pt);
	}

	return MS::kSuccess;
}

Image TerrainShaperNode::runAlgorithm(std::vector<std::string> inDetailMapFilenames, std::string inStartPointsFilenames,
	int numStartPoints) {
	// run algorithm and display height map
	Graph g = Graph(512, 512);
	g.setDetailMaps(inDetailMapFilenames);
	g.setStartPointsMap(inStartPointsFilenames);
	g.setNumStartPoints(numStartPoints);
	Image heightMap = g.run();

	return heightMap;
}