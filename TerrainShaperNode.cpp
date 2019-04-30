#include "TerrainShaperNode.h"

#define McheckErr(stat,msg)			\
	if ( MS::kSuccess != stat ) {	\
		cerr << msg;				\
		return MS::kFailure;		\
	}

using namespace cimg_library;

MObject TerrainShaperNode::weightFunction;
MObject TerrainShaperNode::detailMap;
MObject TerrainShaperNode::startPointsMap;
MObject TerrainShaperNode::numStartPoints;
MObject TerrainShaperNode::maxHeight;
MObject TerrainShaperNode::steepness;
MTypeId TerrainShaperNode::id(0x80000);

void* TerrainShaperNode::creator()
{
	return new TerrainShaperNode;
}

MStatus TerrainShaperNode::initialize()
{
	MFnEnumAttribute inputWeightFunctionAttr;
	MFnTypedAttribute inputDetailMapAttr;
	MFnTypedAttribute inputStartPointsMapAttr;
	MFnNumericAttribute inputNumStartPointsAttr;
	MFnNumericAttribute inputMaxHeightAttr;
	MFnNumericAttribute inputSteepnessAttr;

	MStatus returnStatus;

	// create input/output attributes
	TerrainShaperNode::weightFunction = inputWeightFunctionAttr.create("weightFunction", "wf", 0);
	McheckErr(returnStatus, "ERROR creating TerrainShaperNode weight function attribute\n");
	returnStatus = inputWeightFunctionAttr.addField("Basic", 0);
	McheckErr(returnStatus, "ERROR creating TerrainShaperNode weight function enum type basic\n");
	returnStatus = inputWeightFunctionAttr.addField("Dunes", 1);
	McheckErr(returnStatus, "ERROR creating TerrainShaperNode weight function enum type dunes\n");
	returnStatus = inputWeightFunctionAttr.addField("Dunes2", 2);
	McheckErr(returnStatus, "ERROR creating TerrainShaperNode weight function enum type dunes2\n");
	returnStatus = inputWeightFunctionAttr.addField("Canyons", 3);
	McheckErr(returnStatus, "ERROR creating TerrainShaperNode weight function enum type canyon\n");

	TerrainShaperNode::detailMap = inputDetailMapAttr.create("detailMap", "dm", MFnData::kString, 0);
	McheckErr(returnStatus, "ERROR creating TerrainShaperNode detail map attribute\n");

	TerrainShaperNode::startPointsMap = inputStartPointsMapAttr.create("startPointsMap", "spm", MFnData::kString, 0);
	McheckErr(returnStatus, "ERROR creating TerrainShaperNode start points map attribute\n");

	TerrainShaperNode::numStartPoints = inputNumStartPointsAttr.create("numberStartPoints", "nsp", MFnNumericData::kInt, 5);
	McheckErr(returnStatus, "ERROR creating TerrainShaperNode number of start points attribute\n");

	TerrainShaperNode::maxHeight = inputMaxHeightAttr.create("maxHeight", "mh", MFnNumericData::kFloat, 5);
	McheckErr(returnStatus, "ERROR creating TerrainShaperNode max height attribute\n");
	
	TerrainShaperNode::steepness = inputSteepnessAttr.create("steepness", "s", MFnNumericData::kFloat, 0.005);
	McheckErr(returnStatus, "ERROR creating TerrainShaperNode steepness attribute\n");

	// add attributes
	returnStatus = addAttribute(TerrainShaperNode::weightFunction);
	McheckErr(returnStatus, "ERROR adding weight function attribute\n");

	returnStatus = addAttribute(TerrainShaperNode::detailMap);
	McheckErr(returnStatus, "ERROR adding detail map attribute\n");

	returnStatus = addAttribute(TerrainShaperNode::startPointsMap);
	McheckErr(returnStatus, "ERROR adding start points map attribute\n");

	returnStatus = addAttribute(TerrainShaperNode::numStartPoints);
	McheckErr(returnStatus, "ERROR adding num start points attribute\n");

	returnStatus = addAttribute(TerrainShaperNode::maxHeight);
	McheckErr(returnStatus, "ERROR adding max height attribute\n");

	returnStatus = addAttribute(TerrainShaperNode::steepness);
	McheckErr(returnStatus, "ERROR adding steepness attribute\n");

	// attribute effects
	returnStatus = attributeAffects(TerrainShaperNode::weightFunction, TerrainShaperNode::outputGeom);
	McheckErr(returnStatus, "ERROR in attributeAffects for weight function\n");

	returnStatus = attributeAffects(TerrainShaperNode::detailMap, TerrainShaperNode::outputGeom);
	McheckErr(returnStatus, "ERROR in attributeAffects for detail map\n");

	returnStatus = attributeAffects(TerrainShaperNode::startPointsMap, TerrainShaperNode::outputGeom);
	McheckErr(returnStatus, "ERROR in attributeAffects for start points map\n");

	returnStatus = attributeAffects(TerrainShaperNode::numStartPoints, TerrainShaperNode::outputGeom);
	McheckErr(returnStatus, "ERROR in attributeAffects for number of start points\n");

	returnStatus = attributeAffects(TerrainShaperNode::maxHeight, TerrainShaperNode::outputGeom);
	McheckErr(returnStatus, "ERROR in attributeAffects for max height\n");

	returnStatus = attributeAffects(TerrainShaperNode::steepness, TerrainShaperNode::outputGeom);
	McheckErr(returnStatus, "ERROR in attributeAffects for steepness\n");

	return MS::kSuccess;
}

MStatus TerrainShaperNode::deform(MDataBlock& data, MItGeometry& itGeo,
	const MMatrix &localToWorldMatrix, unsigned int mIndex)
{
	MStatus returnStatus;

	// get input

	// get the envelope
	float env = data.inputValue(envelope).asFloat();

	// get weight function
	MDataHandle weightFunctionData = data.inputValue(weightFunction, &returnStatus);
	McheckErr(returnStatus, "ERROR getting weight function data handle\n");
	short weightFunctionVal = weightFunctionData.asShort();

	// get detail map
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

	// get max height
	MDataHandle maxHeightData = data.inputValue(maxHeight, &returnStatus);
	McheckErr(returnStatus, "ERROR getting max height data handle\n");
	float maxHeightVal = maxHeightData.asFloat();

	// get steepness
	MDataHandle steepnessData = data.inputValue(steepness, &returnStatus);
	McheckErr(returnStatus, "ERROR getting steepness data handle\n");
	float steepnessVal = steepnessData.asFloat();

	// create height map
	int numVertices = itGeo.exactCount();
	int numSubdivisions = ceil(sqrt(numVertices)); // gently assume geometry is square plane
	std::vector<std::string> detailMaps;
	detailMaps.push_back(detailMapFilename.asChar());
	Image heightMap = runAlgorithm(numSubdivisions, weightFunctionVal, steepnessVal, detailMaps, startPointsMapFilename.asChar(), numStartPointsVal);

	// save height map as image
	heightMap.save("C:\\Users\\caroline\\Documents\\CIS_660_windows\\TerrainShaperNode\\Images\\OutHeightMapAFTER.bmp");

	// TODO: can probably combine these two following blocks of code
	std::vector<float> heightsVector;
	// fill point array with height values from height map
	cimg_forXY(heightMap, x, y)
	{
		int r = (int)heightMap(x, y, 0, 0);
		int g = (int)heightMap(x, y, 0, 1);
		int b = (int)heightMap(x, y, 0, 2);
		float height = 0.21 * r + 0.72 * g + 0.07 * b; // height is from [0, 255]
		height -= 128.f; // remap [0, 255] to [-128, 128] (0 is sea level)
		height *= maxHeightVal / 128.f; // scale using maxHeight
		heightsVector.push_back(height);
	}

	MPoint pt;
	for (; !itGeo.isDone(); itGeo.next()) 
	{
		// get the input point
		pt = itGeo.position();

		// perform the deformation
		int index = itGeo.index();
		pt = pt + MPoint(0, heightsVector[itGeo.index()], 0);

		// set the new output point
		itGeo.setPosition(pt);
	}

	return MS::kSuccess;
}

Image TerrainShaperNode::runAlgorithm(int numSubdivisions, short weightFunction, float steepness,
	std::vector<std::string> inDetailMapFilenames, 
	std::string inStartPointsFilenames, int numStartPoints) 
{
	// run algorithm and display height map
	Graph g = Graph(numSubdivisions, numSubdivisions);
	g.setDetailMaps(inDetailMapFilenames);
	g.setStartPointsMap(inStartPointsFilenames);
	g.setNumStartPoints(numStartPoints);
	g.setSteepness(steepness);
	Image heightMap = g.run(weightFunction);

	heightMap.save("C:\\Users\\caroline\\Documents\\CIS_660_windows\\TerrainShaperNode\\Images\\OutHeightMapBEFORE.bmp");

	return heightMap;
}