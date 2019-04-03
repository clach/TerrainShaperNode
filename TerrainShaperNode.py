# TerrainShaperNode.py

import sys
import random
import os

import maya.OpenMaya as OpenMaya
import maya.OpenMayaAnim as OpenMayaAnim
import maya.OpenMayaMPx as OpenMayaMPx
import maya.cmds as cmds
import maya.mel as mel

# Useful functions for declaring attributes as inputs or outputs.
def MAKE_INPUT(attr):
    attr.setKeyable(True)
    attr.setStorable(True)
    attr.setReadable(True)
    attr.setWritable(True)
def MAKE_OUTPUT(attr):
    attr.setKeyable(False)
    attr.setStorable(False)
    attr.setReadable(True)
    attr.setWritable(False)

# Define the name of the node
kPluginNodeTypeName = "TerrainShaperNode"

# Give the node a unique ID. Make sure this ID is different from all of your
# other nodes!
TerrainShaperNodeId = OpenMaya.MTypeId(0x8705)

# Node definition
class TerrainShaperNode(OpenMayaMPx.MPxNode):
    # Declare class variables:
    inDetailMap = OpenMaya.MObject()
    inStartPoints = OpenMaya.MObject()

    outMesh = OpenMaya.MObject()
    
    # constructor
    def __init__(self):
        OpenMayaMPx.MPxNode.__init__(self)

    # compute
    def compute(self,plug,data):
        # get input
        inDetailMapDataHandle = data.inputValue(TerrainShaperNode.inDetailMap)
        inDetailMapData = inDetailMapDataHandle.asString()

        inStartPointsDataHandle = data.inputValue(TerrainShaperNode.inStartPoints)
        inStartPointsData = inStartPointsDataHandle.asString()

        # get output
        # TODO: fix this
        outMeshDataHandle = data.outputValue(TerrainShaperNode.outMesh)
        dataCreator = OpenMaya.MFnMeshData()
        outMeshData = dataCreator.create()
        #mesh = OpenMaya.MFnMesh()
        #meshObject = mesh.create()

        # TODO: use input to generate terrain mesh
     
        # Finally set the output data handle
        outMeshDataHandle.setMObject(outMeshData)

        data.setClean(plug)
    
# initializer
def nodeInitializer():
    tAttr = OpenMaya.MFnTypedAttribute()
    nAttr = OpenMaya.MFnNumericAttribute()

    # Initialize the input and output attributes
    stringDataFn = OpenMaya.MFnStringData()
    defaultDetailMap = cmds.workspace(q=True, dir=True) + "plants/simple1.txt"
    stringDataObj = stringDataFn.create(defaultDetailMap)
    TerrainShaperNode.inDetailMap = tAttr.create("detailMap", "dm", OpenMaya.MFnData.kString, stringDataObj)
    MAKE_INPUT(tAttr)

    defaultStartPoints = cmds.workspace(q=True, dir=True) + "plants/simple2.txt"
    stringDataObj = stringDataFn.create(defaultStartPoints)
    TerrainShaperNode.inStartPoints = tAttr.create("startPoints", "sp", OpenMaya.MFnData.kString, stringDataObj)
    MAKE_INPUT(tAttr)

    TerrainShaperNode.outMesh = tAttr.create("outMesh", "m", OpenMaya.MFnData.kMesh)
    MAKE_OUTPUT(tAttr)

    try:
        # Add the attributes to the node and set up the attributeAffects
        print "Initialization!\n"
        TerrainShaperNode.addAttribute(TerrainShaperNode.inDetailMap)
        TerrainShaperNode.addAttribute(TerrainShaperNode.inStartPoints)

        TerrainShaperNode.addAttribute(TerrainShaperNode.outMesh)

        TerrainShaperNode.attributeAffects(TerrainShaperNode.inDetailMap, TerrainShaperNode.outMesh)
        TerrainShaperNode.attributeAffects(TerrainShaperNode.inStartPoints, TerrainShaperNode.outMesh)
        
    except:
        sys.stderr.write( ("Failed to create attributes of %s node\n", kPluginNodeTypeName) )

# creator
def nodeCreator():
    return OpenMayaMPx.asMPxPtr( TerrainShaperNode() )

# initialize the script plug-in
def initializePlugin(mobject):
    mplugin = OpenMayaMPx.MFnPlugin(mobject)
    try:
        mplugin.registerNode( kPluginNodeTypeName, TerrainShaperNodeId, nodeCreator, nodeInitializer )
    except:
        sys.stderr.write( "Failed to register node: %s\n" % kPluginNodeTypeName )

    menuCmd = "source \"" + mplugin.loadPath() + "/TerrainShaperPluginWindow\";"
    mel.eval(menuCmd)

# uninitialize the script plug-in
def uninitializePlugin(mobject):
    mplugin = OpenMayaMPx.MFnPlugin(mobject)
    try:
        mplugin.deregisterNode( TerrainShaperNodeId )
    except:
        sys.stderr.write( "Failed to unregister node: %s\n" % kPluginNodeTypeName )
