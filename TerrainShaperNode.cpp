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
MObject TerrainShaperNode::noise;
MObject TerrainShaperNode::windDirX;
MObject TerrainShaperNode::windDirY;
MObject TerrainShaperNode::additiveDetailMap;
MTypeId TerrainShaperNode::id(0x80000);

void* TerrainShaperNode::creator()
{
	return new TerrainShaperNode;
}

MStatus TerrainShaperNode::initialize()
{
	MFnEnumAttribute inWeightFunctionAttr;
	MFnTypedAttribute inDetailMapAttr;
	MFnTypedAttribute inStartPointsMapAttr;
	MFnNumericAttribute inNumStartPointsAttr;
	MFnNumericAttribute inMaxHeightAttr;
	MFnNumericAttribute inSteepnessAttr;
	MFnNumericAttribute inNoiseAttr;
	MFnNumericAttribute inWindDirXAttr;
	MFnNumericAttribute inWindDirYAttr;
	MFnNumericAttribute inAdditiveDetailMapAttr;

	MStatus returnStatus;

	// create input/output attributes
	TerrainShaperNode::weightFunction = inWeightFunctionAttr.create("weightFunction", "wf", 0);
	McheckErr(returnStatus, "ERROR creating TerrainShaperNode weight function attribute\n");
	returnStatus = inWeightFunctionAttr.addField("Basic", 0);
	McheckErr(returnStatus, "ERROR creating TerrainShaperNode weight function enum type basic\n");
	returnStatus = inWeightFunctionAttr.addField("Dunes", 1);
	McheckErr(returnStatus, "ERROR creating TerrainShaperNode weight function enum type dunes\n");
	returnStatus = inWeightFunctionAttr.addField("Peaks", 2);
	McheckErr(returnStatus, "ERROR creating TerrainShaperNode weight function enum type peaks\n");
	returnStatus = inWeightFunctionAttr.addField("Canyons", 3);
	McheckErr(returnStatus, "ERROR creating TerrainShaperNode weight function enum type canyon\n");

	TerrainShaperNode::detailMap = inDetailMapAttr.create("detailMap", "dm", MFnData::kString, 0);
	McheckErr(returnStatus, "ERROR creating TerrainShaperNode detail map attribute\n");

	TerrainShaperNode::startPointsMap = inStartPointsMapAttr.create("startPointsMap", "spm", MFnData::kString, 0);
	McheckErr(returnStatus, "ERROR creating TerrainShaperNode start points map attribute\n");

	TerrainShaperNode::numStartPoints = inNumStartPointsAttr.create("numberStartPoints", "nsp", MFnNumericData::kInt, 5);
	McheckErr(returnStatus, "ERROR creating TerrainShaperNode number of start points attribute\n");

	TerrainShaperNode::maxHeight = inMaxHeightAttr.create("maxHeight", "mh", MFnNumericData::kFloat, 5);
	McheckErr(returnStatus, "ERROR creating TerrainShaperNode max height attribute\n");
	
	TerrainShaperNode::steepness = inSteepnessAttr.create("steepness", "s", MFnNumericData::kFloat, 0.5);
	McheckErr(returnStatus, "ERROR creating TerrainShaperNode steepness attribute\n");

	TerrainShaperNode::noise = inNoiseAttr.create("noise", "noise", MFnNumericData::kInt, 10);
	McheckErr(returnStatus, "ERROR creating TerrainShaperNode noise attribute\n");

	TerrainShaperNode::windDirX = inWindDirXAttr.create("windDirectionX", "wDirX", MFnNumericData::kFloat, 1);
	McheckErr(returnStatus, "ERROR creating TerrainShaperNode wind direction x attribute\n");

	TerrainShaperNode::windDirY = inWindDirYAttr.create("windDirectionY", "wDirY", MFnNumericData::kFloat, 0);
	McheckErr(returnStatus, "ERROR creating TerrainShaperNode wind direction y attribute\n");

	TerrainShaperNode::additiveDetailMap = inAdditiveDetailMapAttr.create("additiveDetailMap", "adm", MFnNumericData::kBoolean, 0);
	McheckErr(returnStatus, "ERROR creating TerrainShaperNode additive detail map attribute\n");

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

	returnStatus = addAttribute(TerrainShaperNode::windDirX);
	McheckErr(returnStatus, "ERROR adding wind direction x attribute\n");

	returnStatus = addAttribute(TerrainShaperNode::windDirY);
	McheckErr(returnStatus, "ERROR adding wind direction y attribute\n");

	returnStatus = addAttribute(TerrainShaperNode::noise);
	McheckErr(returnStatus, "ERROR adding noise attribute\n");

	returnStatus = addAttribute(TerrainShaperNode::additiveDetailMap);
	McheckErr(returnStatus, "ERROR adding additive detail map attribute\n");

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

	returnStatus = attributeAffects(TerrainShaperNode::windDirX, TerrainShaperNode::outputGeom);
	McheckErr(returnStatus, "ERROR in attributeAffects for wind direction x\n");

	returnStatus = attributeAffects(TerrainShaperNode::windDirY, TerrainShaperNode::outputGeom);
	McheckErr(returnStatus, "ERROR in attributeAffects for wind direction y\n");

	returnStatus = attributeAffects(TerrainShaperNode::noise, TerrainShaperNode::outputGeom);
	McheckErr(returnStatus, "ERROR in attributeAffects for noise\n");

	returnStatus = attributeAffects(TerrainShaperNode::additiveDetailMap, TerrainShaperNode::outputGeom);
	McheckErr(returnStatus, "ERROR in attributeAffects for additive detail map\n");

	return MS::kSuccess;
}

string ExePath() {
	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
	string::size_type pos = string(buffer).find_last_of("\\/");
	return string(buffer).substr(0, pos);
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

	// get wind direction
	MDataHandle windDirXData = data.inputValue(windDirX, &returnStatus);
	McheckErr(returnStatus, "ERROR getting wind direction x data handle\n");
	float windDirXVal = windDirXData.asFloat();
	MDataHandle windDirYData = data.inputValue(windDirY, &returnStatus);
	McheckErr(returnStatus, "ERROR getting wind direction y data handle\n");
	float windDirYVal = windDirYData.asFloat();

	// get noise
	MDataHandle noiseData = data.inputValue(noise, &returnStatus);
	McheckErr(returnStatus, "ERROR getting steepness data handle\n");
	int noiseVal = noiseData.asInt();

	// get additive detail map
	MDataHandle additiveDetailMapData = data.inputValue(additiveDetailMap, &returnStatus);
	McheckErr(returnStatus, "ERROR getting additive detail map data handle\n");
	bool additiveDetailMapVal = additiveDetailMapData.asBool();

	// create height map
	int numVertices = itGeo.exactCount();
	int numSubdivisions = ceil(sqrt(numVertices)); // gently assume geometry is square plane
	std::vector<std::string> detailMaps;
	detailMaps.push_back(detailMapFilename.asChar());
	Image heightMap = runAlgorithm(numSubdivisions, weightFunctionVal, steepnessVal, additiveDetailMapVal, 
		detailMaps, startPointsMapFilename.asChar(), numStartPointsVal, windDirXVal, windDirYVal, noiseVal);

	// save height map as image
	try {
		std::string exePath = ExePath();
		heightMap.save("C:\\Spring2019\\CIS660\\TerrainShaper\\Images\\outHeightMap.bmp");
	}
	catch (...)
	{
		// TODO
	}

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

Image TerrainShaperNode::runAlgorithm(int numSubdivisions, short weightFunction, float steepness, bool additiveDetailMap,
	std::vector<std::string> inDetailMapFilenames, std::string inStartPointsFilenames, int numStartPoints,
	float windDirX, float windDirY,
	int noise) 
{
	// run algorithm and display height map
	Graph g = Graph(numSubdivisions, numSubdivisions);
	g.setDetailMaps(inDetailMapFilenames);
	g.setStartPointsMap(inStartPointsFilenames);
	g.setNumStartPoints(numStartPoints);
	g.setSteepness(steepness);
	g.setNoise(noise);
	g.setAdditiveDetailMap(additiveDetailMap);
	Image heightMap = g.run(weightFunction);

	return heightMap;
}