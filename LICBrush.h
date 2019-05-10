/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   LICBrush.h
 * Author: leo
 *
 * Created on May 8, 2019, 1:14 PM
 */

#ifndef LICBRUSH_H
#define LICBRUSH_H

#include <Geometry/Point.h>
#include <Vrui/Geometry.h>

#include <Abstract/DataSet.h>
#include "BaseLocator.h"
#include "LICBrushMask.h"

namespace GLMotif {
class PopupWindow;
class RowColumn;
class TextField;
}
namespace Visualization {
namespace Abstract {
class CoordinateTransformer;
class Element;
}
}

class LICBrush:public BaseLocator
{
        /* Embedded classes: */
	protected:
                typedef Visualization::Abstract::Element Element;
	typedef Visualization::Abstract::DataSet::Locator Locator;
	typedef Visualization::Abstract::CoordinateTransformer CoordinateTransformer;
        
        /* Elements: */
	GLMotif::PopupWindow* evaluationDialogPopup; // Pointer to the evaluation dialog window
	GLMotif::RowColumn* evaluationDialog; // Pointer to the evaluation dialog
	GLMotif::TextField* pos[3]; // The coordinate labels for the evaluation position
	Locator* locator; // A locator for evaluation
	Vrui::Point brushPos; // The evaluation point
        Vrui::Scalar brushSize;
	bool dragging; // Flag if the locator is currently dragging the evaluation point
	bool hasPoint; // Flag whether the locator has a position
        
        public:
        LICBrush(Vrui::LocatorTool* sTool,Visualizer* sApplication, const Misc::ConfigurationFileSection* cfg =0);
        virtual ~LICBrush();
        
        /* Methods from Vrui::LocatorToolAdapter: */
	virtual void motionCallback(Vrui::LocatorTool::MotionCallbackData* cbData);
	virtual void buttonPressCallback(Vrui::LocatorTool::ButtonPressCallbackData* cbData);
	virtual void buttonReleaseCallback(Vrui::LocatorTool::ButtonReleaseCallbackData* cbData);
	
	/* Methods from class BaseLocater: */
	virtual void highlightLocator(GLRenderState& renderState) const;

        private:
        void updateMask(LICBrushMask* mask);

};

#endif /* LICBRUSH_H */

