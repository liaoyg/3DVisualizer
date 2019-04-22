/***********************************************************************
Vector3DLICRenderer - Wrapper class for 3D Line integral convolution
renderers as visualization elements.
Part of the wrapper layer of the templatized visualization
components.
Copyright (c) 20017-2019 Yangguang Liao

This file is part of the 3D Data Visualizer (Visualizer).

The 3D Data Visualizer is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as published
by the Free Software Foundation; either version 2 of the License, or (at
your option) any later version.

The 3D Data Visualizer is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License along
with the 3D Data Visualizer; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
***********************************************************************/

#ifndef VISUALIZATION_WRAPPERS_VECTOR3DLICRENDERER_INCLUDED
#define VISUALIZATION_WRAPPERS_VECTOR3DLICRENDERER_INCLUDED

#include <GLMotif/ToggleButton.h>
#include <GLMotif/TextFieldSlider.h>

#include <Abstract/Element.h>

/* Forward declarations: */
class LICRaycaster;

namespace Visualization {

namespace Wrappers {

template <class DataSetWrapperParam>
class Vector3DLICRenderer:public Visualization::Abstract::Element
	{
	/* Embedded classes: */
	public:
	typedef Visualization::Abstract::Element Base; // Base class
	typedef DataSetWrapperParam DataSetWrapper; // Compatible data set type
	typedef typename DataSetWrapper::DS DS; // Type of templatized data set
	typedef typename DS::Scalar Scalar; // Scalar type of data set's domain
	typedef typename DataSetWrapper::SE SE; // Type of templatized scalar extractor
	typedef typename DataSetWrapper::VE VE; // Type of templatized vector extractor
	typedef typename DataSetWrapper::ScalarExtractor ScalarExtractor; // Compatible scalar extractor wrapper class
	typedef typename DataSetWrapper::VectorExtractor VectorExtractor; // Compatible scalar extractor wrapper class
        
	
	/* Elements: */
	private:
	LICRaycaster* raycaster; // A raycasting volume renderer
	
	/* UI components: */
	GLMotif::ToggleButton* channelEnabledToggles[3]; // Toggle buttons to enable/disable individual channels
	GLMotif::TextFieldSlider* transparencyGammaSlider; // Slider to change current gamma correction factor for each channel
	
	/* Constructors and destructors: */
	public:
	Vector3DLICRenderer(Visualization::Abstract::Algorithm* algorithm,Visualization::Abstract::Parameters* sParameters); // Creates a volume renderer for the given extractor and parameters
	private:
	Vector3DLICRenderer(const Vector3DLICRenderer& source); // Prohibit copy constructor
	Vector3DLICRenderer& operator=(const Vector3DLICRenderer& source); // Prohibit assignment operator
	public:
	virtual ~Vector3DLICRenderer(void);
	
	/* Methods from Visualization::Abstract::Element: */
	virtual std::string getName(void) const;
	virtual size_t getSize(void) const;
	virtual bool usesTransparency(void) const
		{
		return true;
		}
	virtual GLMotif::Widget* createSettingsDialog(GLMotif::WidgetManager* widgetManager); // Returns a new UI widget to change internal settings of the element
	virtual void glRenderAction(GLRenderState& renderState) const;
	
	/* New methods: */
	void sliceFactorCallback(GLMotif::TextFieldSlider::ValueChangedCallbackData* cbData);
	void transparencyGammaCallback(GLMotif::TextFieldSlider::ValueChangedCallbackData* cbData);
	};

}

}

#ifndef VISUALIZATION_WRAPPERS_VECTOR3DLICRENDERER_IMPLEMENTATION
#include <Wrappers/Vector3DLICRenderer.icpp>
#endif

#endif
