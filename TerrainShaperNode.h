#pragma once
#include <maya/MPxNode.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MTime.h>
#include <maya/MAngle.h>
#include <maya/MFnMesh.h>
#include <maya/MFnMeshData.h>
#include <maya/MFloatPointArray.h>
#include <maya/MGlobal.h>
#include <maya/MImage.h>
#include "Graph.h"

//#include "opencv2\highgui\highgui.hpp"

class TerrainShaperNode : public MPxNode
{
public:
	TerrainShaperNode();
	virtual ~TerrainShaperNode() {}
	static  void*	creator();
	static  MStatus initialize();
	virtual MStatus compute(const MPlug& plug, MDataBlock& data);

	static MTypeId	id;

	// node input and output
	static MObject detailMap;
	static MObject startPointsMap;
	static MObject numStartPoints;
	static MObject	outMesh;

	Image runAlgorithm(std::vector<std::string> inDetailMapFilenames, std::string inStartPointsFilenames, int numStartPoints);

protected:
	

};

