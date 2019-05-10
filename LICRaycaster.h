/***********************************************************************
LICRaycaster - Class for 3D LIC renderers with volumetric vector field
data.
Copyright (c) 2017-2019 Yangguang Liao

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

#ifndef LICRAYCASTER_INCLUDED
#define LICRAYCASTER_INCLUDED

#include <GL/gl.h>
#include <GL/GLColor.h>
#include <GL/GLColorMap.h>

#include <Raycaster.h>
#include <LICBrushMask.h>

class LICRaycaster:public Raycaster
	{
	/* Embedded classes: */
	protected:
	typedef GLfloat Voxel; // Type for voxel data
	
	struct DataItem:public Raycaster::DataItem
		{
		/* Elements: */
		public:
		bool haveFloatTextures; // Flag whether the local OpenGL supports floating-point textures
		
                GLuint maskTextureID;
                unsigned int maskTextureVersion;
                GLuint noiseTextureID; // Texture object ID for noise data texture
                GLuint kernalTextureID; // Texture object ID for kernel texture
		GLuint volumeTextureID; // Texture object ID for volume data texture
		unsigned int volumeTextureVersion; // Version number of volume data texture
		GLuint colorMapTextureID; // Texture object IDs for per-channel stepsize-adjusted color map textures
                
                // 3D LIC parameter value
                GLfloat invKernelArea;
                GLfloat gradient[3];
                GLfloat licKernel[3],licParams[3]; 
		
                int gradientLoc; // Location of the gradient and scale factors of 3D LIC
		int licKernelLoc; // Location of kernel step width and inverse funtion area
                int licParamsLoc; // Location of LIC steps number and step size
		int volumeSamplerLoc; // Location of the volume data texture sampler
                int maskSamplerLoc; // Location of the mask texture sampler
                int noiseSamplerLoc; // Location of the random noise texture sampler
                int kernalSamplerLoc; // Location of the random noise texture sampler
		int colorMapSamplerLoc; // Location of the three color map texture samplers
		
		/* Constructors and destructors: */
		DataItem(void);
		virtual ~DataItem(void);
		};
	
	/* Elements: */
	protected:
	Voxel* data; // Pointer to the volume dataset
	unsigned int dataVersion; // Version number of the volume dataset to track changes
        unsigned int noiseSize[3];
        Voxel* nData; // Pointer to the noise texture dataset
        LICBrushMask* mask;
	const GLColorMap* colorMap; // Pointers to the three channel color maps
	GLfloat transparencyGamma; // Adjustment factor for each color map's overall opacity
        Scalar licStepSize;
	
	/* Protected methods: */
	protected:
	virtual void initDataItem(Raycaster::DataItem* dataItem) const;
	virtual void initShader(Raycaster::DataItem* dataItem) const;
	virtual void bindShader(const PTransform& pmv,const PTransform& mv,Raycaster::DataItem* dataItem) const;
	virtual void unbindShader(Raycaster::DataItem* dataItem) const;
        
        void loadKernelFunction(DataItem* dataItem) const;
        bool loadNoiseTexture();
        
        float* paddingData(const unsigned int dataSize[3], Voxel* data) const;
	
	/* Constructors and destructors: */
	public:
	LICRaycaster(const unsigned int sDataSize[3],const Box& sDomain); // Creates a volume renderer
	virtual ~LICRaycaster(void); // Destroys the raycaster
	
	/* Methods from GLObject: */
	virtual void initContext(GLContextData& contextData) const;
	
	/* Methods from Raycaster: */
	virtual void setStepSize(Scalar newStepSize);
        
        void setLICStepSize(Scalar newStepSize)
                {
                licStepSize = newStepSize;
                }
        Scalar getLICStepSize()
                {
                return licStepSize;
                }
	
	/* New methods: */
	const Voxel* getData(int channel) const // Returns pointer to the volume dataset for the given channel
		{
		return data+channel;
		}
	Voxel* getData(int channel) // Ditto
		{
		return data+channel;
		}
	virtual void updateData(void); // Notifies the raycaster that the volume dataset has changed

	const GLColorMap* getColorMap() const // Returns the raycaster's color map
		{
		return colorMap;
		}
	void setColorMap(const GLColorMap* newColorMap); // Sets the raycaster's color map
	GLfloat getTransparencyGamma() const // Returns the opacity adjustment factor 
		{
		return transparencyGamma;
		}
	void setTransparencyGamma(GLfloat newTransparencyGamma); // Sets the opacity adjustment factor for the given scalar channel
        
        void setLICMask(LICBrushMask* sMask);
         
	};
        

#endif
