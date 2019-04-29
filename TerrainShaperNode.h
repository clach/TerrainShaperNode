#pragma once
#include <maya/MPxNode.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MTime.h>
#include <maya/MAngle.h>
#include <maya/MFnMesh.h>
#include <maya/MFnMeshData.h>
#include <maya/MFloatPointArray.h>
#include <maya/MGlobal.h>
#include <maya/MFnPointArrayData.h>
#include <maya/MFnFloatArrayData.h>
#include <maya/MFloatArray.h>
#include <maya/MPointArray.h>
#include "Graph.h"
#include <maya/MItGeometry.h>

#pragma comment(lib,"Foundation.lib")
#pragma comment(lib,"OpenMaya.lib")
#pragma comment(lib,"OpenMayaFx.lib")
#pragma comment(lib,"Image.lib")
#pragma comment(lib,"OpenMayaAnim.lib")
#pragma comment(lib,"OpenMayaUi.lib") 

#include <maya/MPxDeformerNode.h>

class TerrainShaperNode : public MPxDeformerNode
{
public:
	TerrainShaperNode() {}
	virtual ~TerrainShaperNode() {}
	static  void*	creator();
	static  MStatus initialize();
	virtual MStatus deform(MDataBlock& data, MItGeometry& itGeo,
		const MMatrix &localToWorldMatrix, unsigned int mIndex);

	static MTypeId	id;

	// node input and output
	static MObject weightFunction;
	static MObject detailMap;
	static MObject startPointsMap;
	static MObject numStartPoints;
	static MObject strength;

	Image runAlgorithm(int numSubdivisions, short weightFunction, std::vector<std::string> inDetailMapFilenames, 
		std::string inStartPointsFilenames, int numStartPoints);
	

};

