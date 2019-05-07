#include <maya/MPxCommand.h>
#include <maya/MFnPlugin.h>
#include <maya/MIOStream.h>
#include <maya/MString.h>
#include <maya/MArgList.h>
#include <maya/MGlobal.h>
#include <maya/MSimple.h>
#include <maya/MDoubleArray.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MFnNurbsCurve.h>
#include <maya/MDGModifier.h>
#include <maya/MPlugArray.h>
#include <maya/MVector.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MStringArray.h>
#include <list>

#include "TerrainShaperNode.h"
#include "TerrainShaperNode.h"


MStatus initializePlugin(MObject obj)
{
	MStatus   status = MStatus::kSuccess;
	MFnPlugin plugin(obj, "MyPlugin", "1.0", "Any");

	// register node
	status = plugin.registerNode("TerrainShaperNode", TerrainShaperNode::id, TerrainShaperNode::creator,
		TerrainShaperNode::initialize, MPxNode::kDeformerNode);
	if (!status) {
		status.perror("registerNode");
		return status;
	}
	MString nodeClassName("TerrainShaperNode");


	// auto-register TerrainShaper menu command
	MString menuCmd = "source \"" + plugin.loadPath() + "/TerrainShaperPluginWindow\";";
	MGlobal::executeCommand(menuCmd, true);


	MString global_proc = "source \"" + plugin.loadPath() + "/TextureDeformerTest\";";
	MGlobal::executeCommand(global_proc, true);


	return status;
}

MStatus uninitializePlugin(MObject obj)
{
	MStatus   status = MStatus::kSuccess;
	MFnPlugin plugin(obj);

	status = plugin.deregisterNode(TerrainShaperNode::id);
	if (!status) {
		status.perror("deregisterNode");
		return status;
	}

	MGlobal::executeCommand(MString("if(`menu -exists $tsMenu`) deleteUI -m $tsMenu;"));
	MGlobal::executeCommand(MString("flushUndo;"));
	
	return status;
}


