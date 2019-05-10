/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   LICBrush.cpp
 * Author: leo
 * 
 * Created on May 8, 2019, 1:14 PM
 */

#include "LICBrush.h"

#include <GL/gl.h>
#include <GL/GLVertexTemplates.h>
#include <GL/GLColorTemplates.h>
#include <GL/GLGeometryWrappers.h>
#include <Geometry/OrthogonalTransformation.h>
#include <GLMotif/WidgetManager.h>
#include <GLMotif/PopupWindow.h>
#include <GLMotif/RowColumn.h>
#include <GLMotif/Label.h>
#include <GLMotif/TextField.h>
#include <Vrui/Vrui.h>

#include <Abstract/DataSet.h>
#include <Abstract/DataSetRenderer.h>
#include <Abstract/CoordinateTransformer.h>

#include "GLRenderState.h"
#include "Visualizer.h"
#include "ElementList.h"
#include "Wrappers/Vector3DLICRenderer.h"

LICBrush::LICBrush(Vrui::LocatorTool* sLocatorTool,Visualizer* sApplication, const Misc::ConfigurationFileSection* cfg) 
        :BaseLocator(sLocatorTool,sApplication),
        locator(application->dataSet->getLocator()),
        dragging(false),hasPoint(false)
        {
        brushSize = 0.03;
        }

LICBrush::~LICBrush() 
        {
        delete locator;
        }

void LICBrush::motionCallback(Vrui::LocatorTool::MotionCallbackData* cbData)
        {
        locator->setPosition(cbData->currentTransformation.getOrigin());
	locator->setOrientation(cbData->currentTransformation.getRotation());
        
        if(dragging)
                {
                brushPos=locator->getPosition();
                if(locator->isValid())
                        {
                        std::cout<<"Press Pos: "<<brushPos[0]<<" "<<brushPos[1]<<" "<<brushPos[2]<<std::endl;
                        updateMask(application->variableManager->getLICMask());
                        }
                }
        }

void LICBrush::buttonPressCallback(Vrui::LocatorTool::ButtonPressCallbackData* cbData)
	{
	/* Create a new evaluation point and start dragging it: */
	dragging=true;
        std::cout<<"brush press"<<std::endl;
	}

void LICBrush::buttonReleaseCallback(Vrui::LocatorTool::ButtonReleaseCallbackData* cbData)
	{
	/* Stop dragging the current evaluation point: */
	dragging=false;
	}

void LICBrush::highlightLocator(GLRenderState& renderState) const
        {

        }

void LICBrush::updateMask(LICBrushMask* mask)
        {
        mask->updateMaskByBrush(brushPos, brushSize, 1.0);
        }