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
MObject TerrainShaperNode::startPointsMap;
MObject TerrainShaperNode::numStartPoints;
//MObject TerrainShaperNode::outMesh;
MObject TerrainShaperNode::outPoints;
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

	//MFnTypedAttribute outputGeometryAttr;

	MFnTypedAttribute outputPointsAttr; ///////////////////

	MStatus returnStatus;

	// create input/output attributes
	TerrainShaperNode::detailMap = inputDetailMapAttr.create("detailMap", "dm", MFnData::kString, 0);
	McheckErr(returnStatus, "ERROR creating TerrainShaperNode detail map attribute\n");

	TerrainShaperNode::startPointsMap = inputStartPointsMapAttr.create("startPointsMap", "spm", MFnData::kString, 0);
	McheckErr(returnStatus, "ERROR creating TerrainShaperNode start points map attribute\n");

	TerrainShaperNode::numStartPoints = inputNumStartPointsAttr.create("numberStartPoints", "nsp", MFnNumericData::kInt, 1);
	McheckErr(returnStatus, "ERROR creating TerrainShaperNode number of start points attribute\n");

	//TerrainShaperNode::outMesh = outputGeometryAttr.create("outMesh", "om", MFnData::kMesh, &returnStatus);
	//McheckErr(returnStatus, "ERROR creating TerrainShaperNode output mesh attribute\n");

	TerrainShaperNode::outPoints = outputPointsAttr.create("outPoints", "op", MFnData::kFloatArray, &returnStatus);
	McheckErr(returnStatus, "ERROR creating TerrainShaperNode output points attribute\n");

	// add attributes
	returnStatus = addAttribute(TerrainShaperNode::detailMap);
	McheckErr(returnStatus, "ERROR adding detail map attribute\n");

	returnStatus = addAttribute(TerrainShaperNode::startPointsMap);
	McheckErr(returnStatus, "ERROR adding start points map attribute\n");

	returnStatus = addAttribute(TerrainShaperNode::numStartPoints);
	McheckErr(returnStatus, "ERROR adding num start points attribute\n");

	//returnStatus = addAttribute(TerrainShaperNode::outMesh);
	//McheckErr(returnStatus, "ERROR adding output mesh attribute\n");

	returnStatus = addAttribute(TerrainShaperNode::outPoints);
	McheckErr(returnStatus, "ERROR adding output points attribute\n");

	// attribute effects
	//returnStatus = attributeAffects(TerrainShaperNode::detailMap, TerrainShaperNode::outMesh);
	//McheckErr(returnStatus, "ERROR in attributeAffects for detail map\n");

	//returnStatus = attributeAffects(TerrainShaperNode::startPointsMap, TerrainShaperNode::outMesh);
//	McheckErr(returnStatus, "ERROR in attributeAffects for start points map\n");

	//returnStatus = attributeAffects(TerrainShaperNode::numStartPoints, TerrainShaperNode::outMesh);
	//McheckErr(returnStatus, "ERROR in attributeAffects for number of start points\n");

	returnStatus = attributeAffects(TerrainShaperNode::detailMap, TerrainShaperNode::outPoints);
	McheckErr(returnStatus, "ERROR in attributeAffects for detail map\n");

	returnStatus = attributeAffects(TerrainShaperNode::startPointsMap, TerrainShaperNode::outPoints);
	McheckErr(returnStatus, "ERROR in attributeAffects for start points map\n");

	returnStatus = attributeAffects(TerrainShaperNode::numStartPoints, TerrainShaperNode::outPoints);
	McheckErr(returnStatus, "ERROR in attributeAffects for number of start points\n");

	return MS::kSuccess;
}

MStatus TerrainShaperNode::compute(const MPlug & plug, MDataBlock & data)
{
	MStatus returnStatus;

	if (plug == outPoints) {

		// get input

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
		int numStartPoints = numStartPointsData.asInt();

		// get output geometry //////
		//MDataHandle outputHandle = data.outputValue(outMesh, &returnStatus);
		//McheckErr(returnStatus, "ERROR getting polygon data handle\n");

		MDataHandle outputHandle = data.outputValue(outPoints, &returnStatus);
		McheckErr(returnStatus, "ERROR getting output points data handle\n");

		//MFnMeshData dataCreator;
		//MObject newOutputData = dataCreator.create(&returnStatus);
		//McheckErr(returnStatus, "ERROR creating outputData\n");

		// TODO: create new output data
		
		std::vector<std::string> detailMaps;
		detailMaps.push_back(detailMapFilename.asChar());
		Image heightMap = runAlgorithm(detailMaps, startPointsMapFilename.asChar(), numStartPoints);

		heightMap.save("C:\\Users\\caroline\\Documents\\CIS_660_windows\\TerrainShaperNode\\Images\\OutHeightMap.bmp");


		// TODO: use heightMap to make terrain

		MFloatArray floatArray;

		std::vector<float> heightsVector;

		float strength = 5;
		// fill point array with height values from height map
		cimg_forXY(heightMap, x, y) {
			int r = (int)heightMap(x, y, 0, 0);
			int g = (int)heightMap(x, y, 0, 1);
			int b = (int)heightMap(x, y, 0, 2);
			float height = 0.21 * r + 0.72 * g + 0.07 * b; // height is from [0, 255]
			height *= strength; // weight height by strength
			floatArray.append(height);
			heightsVector.push_back(height);
		}

		MFnFloatArrayData dataCreator;
		MObject newOutputData = dataCreator.create(floatArray, &returnStatus);
		McheckErr(returnStatus, "ERROR creating newOutputData\n");


		float lo = 0;
		float hi = 50;
		float val = lo + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (hi - lo)));

		MString testCommand =
			MString("int $count;\n") +
			MString("int $nVerts[] = `polyEvaluate -v myPolyPlane`;\n") +
			MString("for ($count = 0; $count < $nVerts[0]; $count++) {\n") +
			MString("float $pos[] = `xform -q -ws -t myPolyPlane.vtx[$count]`;\n") +
			MString("float $randVal = `rand 10`;\n") +
			MString("xform -ws -t $pos[0] $randVal $pos[2] myPolyPlane.vtx[$count];\n") +
			MString("}\n");

		MGlobal::executeCommand(testCommand);

		outputHandle.set(newOutputData);
		data.setClean(plug);
	}
	else {
		return MS::kUnknownParameter;
	}

	return MStatus::kSuccess;
}


Image TerrainShaperNode::runAlgorithm(std::vector<std::string> inDetailMapFilenames, std::string inStartPointsFilenames,
	int numStartPoints) {
	// run algorithm and display height map
	Graph g = Graph(512, 512);
	g.setDetailMaps(inDetailMapFilenames);
	g.setStartPointsMap(inStartPointsFilenames);
	g.setNumStartPoints(numStartPoints);
	Image heightMap = g.run();

	//Image heightMap = Image();
	return heightMap;

	//displayQImage(heightMap);
	//detailMaps.clear();
	//startPoints.clear();
}