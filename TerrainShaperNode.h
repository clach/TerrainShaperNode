#pragma once
#include <maya/MPxNode.h>
#include <maya/MPxDeformerNode.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MTime.h>
#include <maya/MAngle.h>
#include <maya/MFnMesh.h>
#include <maya/MFnMeshData.h>
#include <maya/MGlobal.h>
#include <maya/MItGeometry.h>

#include "Graph.h"

#pragma comment(lib,"Foundation.lib")
#pragma comment(lib,"OpenMaya.lib")
#pragma comment(lib,"OpenMayaFx.lib")
#pragma comment(lib,"Image.lib")
#pragma comment(lib,"OpenMayaAnim.lib")
#pragma comment(lib,"OpenMayaUi.lib") 


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
	static MObject maxHeight;
	static MObject steepness;
	static MObject noise;
	static MObject windDirX;
	static MObject windDirY;
	static MObject windDir;

	Image runAlgorithm(int numSubdivisions, short weightFunction, float steepness,
		std::vector<std::string> inDetailMapFilenames, std::string inStartPointsFilenames, int numStartPoints,
		vec2 windDir, int noise);
	

};

