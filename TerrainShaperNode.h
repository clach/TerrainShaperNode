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

class TerrainShaperNode : public MPxNode
{
public:
	TerrainShaperNode();
	virtual ~TerrainShaperNode() {}
	static  void*	creator();
	static  MStatus initialize();
	virtual MStatus compute(const MPlug& plug, MDataBlock& data);

	static MTypeId	id;

	// TODO: more input here
	static MObject	time;
	static MObject detailMap;
	static MObject startPoints;
	static MObject	outputMesh;

	Image runAlgorithm(std::vector<Image> inDetailMaps, std::vector<Point> inStartPoints);

protected:
	

};
