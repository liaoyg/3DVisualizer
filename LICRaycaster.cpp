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

#include <LICRaycaster.h>

#include <string>
#include <cstdio>
#include <iostream>
#include <GL/gl.h>
#include <GL/GLContextData.h>
#include <GL/Extensions/GLARBMultitexture.h>
#include <GL/Extensions/GLARBTextureFloat.h>
#include <GL/Extensions/GLEXTTexture3D.h>
#include <GL/GLShader.h>
#include <Images/Config.h>
#include <Images/RGBImage.h>
#include <Images/ReadImageFile.h>
#include <Math/Math.h>

/*************************************************
Methods of class LICRaycaster::DataItem:
*************************************************/

LICRaycaster::DataItem::DataItem(void)
	:haveFloatTextures(GLARBTextureFloat::isSupported()),
         maskTextureID(0),maskTextureVersion(0),
	 noiseTextureID(0), kernalTextureID(0),
         volumeTextureID(0),volumeTextureVersion(0), 
	 volumeSamplerLoc(-1), noiseSamplerLoc(-1),
         colorMapSamplerLoc(-1)
	{
	colorMapTextureID=0;
	
	/* Initialize all required OpenGL extensions: */
	GLARBMultitexture::initExtension();
	if(haveFloatTextures)
		GLARBTextureFloat::initExtension();
	GLEXTTexture3D::initExtension();
	
	/* Create the volume texture object: */
	glGenTextures(1,&volumeTextureID);
        
        glGenTextures(1,&maskTextureID);
        glGenTextures(1,&noiseTextureID);
        glGenTextures(1,&kernalTextureID);
	
	/* Create the color map texture objects: */
	glGenTextures(1,&colorMapTextureID);
        
        
	}

LICRaycaster::DataItem::~DataItem(void)
	{
	/* Destroy the volume texture object: */
	glDeleteTextures(1,&volumeTextureID);
        
        glDeleteTextures(1,&maskTextureID);
        glDeleteTextures(1,&noiseTextureID);
        glDeleteTextures(1,&kernalTextureID);
	
	/* Destroy the color map texture objects: */
	glDeleteTextures(1,&colorMapTextureID);
	}

/***************************************
Methods of class LICRaycaster:
***************************************/

void LICRaycaster::initDataItem(Raycaster::DataItem* dataItem) const
	{
	/* Call the base class method: */
	Raycaster::initDataItem(dataItem);
	
	/* Get a pointer to the data item: */
	DataItem* myDataItem=dynamic_cast<DataItem*>(dataItem);
	
	/* Create the data volume texture: */
	glBindTexture(GL_TEXTURE_3D,myDataItem->volumeTextureID);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_S,GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_T,GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_R,GL_CLAMP);
	glTexImage3DEXT(GL_TEXTURE_3D,0,GL_RGBA16F_ARB,myDataItem->textureSize[0],myDataItem->textureSize[1],myDataItem->textureSize[2],0,GL_RGBA,GL_FLOAT,0);
	glBindTexture(GL_TEXTURE_3D,0);
        
        /* Create the mask texture: */
        glBindTexture(GL_TEXTURE_3D,myDataItem->maskTextureID);
        glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_S,GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_T,GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_R,GL_CLAMP);
	glTexImage3DEXT(GL_TEXTURE_3D,0,GL_RGBA16F_ARB,mask->getSize(0),mask->getSize(1),mask->getSize(2),0,GL_RGBA,GL_FLOAT,mask->getData());
	glBindTexture(GL_TEXTURE_3D,0);
        
        /* Create the noise data texture: */
	glBindTexture(GL_TEXTURE_3D,myDataItem->noiseTextureID);
        glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_R,GL_REPEAT);
	glTexImage3DEXT(GL_TEXTURE_3D,0,GL_INTENSITY,noiseSize[0],noiseSize[1],noiseSize[2],0,GL_LUMINANCE,GL_UNSIGNED_BYTE,nData);
	glBindTexture(GL_TEXTURE_3D,0);
        
        /* debug noise data value
        for(int i = 0; i< noiseSize[0]; i+=8)
        {
            for(int j = 0; j < noiseSize[1]; j+=8)
            {
                for(int k = 0; k < noiseSize[2]; k+=8)
                {
                    long idx = k + j*noiseSize[1] + i *noiseSize[2]*noiseSize[1];
                    std::cout << int(nData[idx]) << " ";
                }
                std::cout<<std::endl;
            }
            std::cout<<std::endl<<std::endl;
        }
         */
         
        
	/* Create the color map textures: */

        glBindTexture(GL_TEXTURE_1D,myDataItem->colorMapTextureID);
        glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_1D,0);
        
        myDataItem->gradient[0] = GLfloat(30.0);
        myDataItem->gradient[1] = GLfloat(1.0);
        myDataItem->gradient[2] = GLfloat(6.0);
        
        myDataItem->licParams[0] = GLfloat(32);
        myDataItem->licParams[1] = GLfloat(32);
        myDataItem->licParams[2] = GLfloat(1.0/150.0);
        
        myDataItem->licKernel[0] = GLfloat(0.5/myDataItem->licParams[0]);
        myDataItem->licKernel[1] = GLfloat(0.5/myDataItem->licParams[1]);
        myDataItem->licKernel[2] = GLfloat(myDataItem->invKernelArea / (myDataItem->licParams[0] + myDataItem->licParams[1]));
        
	}

void LICRaycaster::initShader(Raycaster::DataItem* dataItem) const
	{
	/* Call the base class method: */
	Raycaster::initShader(dataItem);
	
	/* Get a pointer to the data item: */
	DataItem* myDataItem=dynamic_cast<DataItem*>(dataItem);
	
	/* Get the shader's uniform locations: */
        
        myDataItem->gradientLoc=myDataItem->shader.getUniformLocation("gradient");
	myDataItem->licKernelLoc=myDataItem->shader.getUniformLocation("licKernel");
	myDataItem->licParamsLoc=myDataItem->shader.getUniformLocation("licParams");
	myDataItem->volumeSamplerLoc=myDataItem->shader.getUniformLocation("volumeSampler");
        myDataItem->maskSamplerLoc=myDataItem->shader.getUniformLocation("maskSampler");
        myDataItem->noiseSamplerLoc=myDataItem->shader.getUniformLocation("noiseSampler");
        myDataItem->kernalSamplerLoc=myDataItem->shader.getUniformLocation("licKernelSampler");
	myDataItem->colorMapSamplerLoc=myDataItem->shader.getUniformLocation("colorMapSampler");
	}

void LICRaycaster::bindShader(const Raycaster::PTransform& pmv,const Raycaster::PTransform& mv,Raycaster::DataItem* dataItem) const
	{
	/* Call the base class method: */
	Raycaster::bindShader(pmv,mv,dataItem);
	
	/* Get a pointer to the data item: */
	DataItem* myDataItem=dynamic_cast<DataItem*>(dataItem);
        
        /* Set up the data space transformation for LIC: */
	glUniform3fvARB(myDataItem->gradientLoc,1,myDataItem->gradient);
	glUniform3fvARB(myDataItem->licKernelLoc,1,myDataItem->licKernel);
	glUniform3fvARB(myDataItem->licParamsLoc,1,myDataItem->licParams);
	
	/* Bind the volume texture: */
	glActiveTextureARB(GL_TEXTURE1_ARB);
	glBindTexture(GL_TEXTURE_3D,myDataItem->volumeTextureID);
	glUniform1iARB(myDataItem->volumeSamplerLoc,1);
	
	/* Check if the volume texture needs to be updated: */
	if(myDataItem->volumeTextureVersion!=dataVersion)
		{
		/* Upload the new volume data: */
                // padding data
                float* pData = paddingData(dataSize, data);
		glTexSubImage3DEXT(GL_TEXTURE_3D,0,0,0,0,dataSize[0],dataSize[1],dataSize[2],GL_RGBA,GL_FLOAT,pData);
		std::cout << "update volume data over." <<std::endl;
		/* Mark the volume texture as up-to-date: */
		myDataItem->volumeTextureVersion=dataVersion;
		}
        
        
        /* Bind the noise and kernel textures: */
        /* Bind the volume texture: */
	glActiveTextureARB(GL_TEXTURE2_ARB);
	glBindTexture(GL_TEXTURE_3D,myDataItem->maskTextureID);
	glUniform1iARB(myDataItem->maskSamplerLoc,2);
	
	/* Check if the volume texture needs to be updated: */
	if(mask->getDataVersion()!=myDataItem->maskTextureVersion)
		{
		/* Upload the new mask data: */
		glTexSubImage3DEXT(GL_TEXTURE_3D,0,0,0,0,mask->getSize(0),mask->getSize(1),mask->getSize(2),GL_RGBA,GL_FLOAT,mask->getData());
		std::cout << "update mask data over." <<std::endl;
		/* Mark the mask texture as up-to-date: */
		myDataItem->maskTextureVersion=mask->getDataVersion();
		}
        
        /* Bind the noise and kernel textures: */
        
//      std::cout << "bind noise texture:  " << myDataItem->noiseTextureID << " at : " << myDataItem->noiseSamplerLoc << std::endl;
        glActiveTextureARB(GL_TEXTURE3_ARB);
	glBindTexture(GL_TEXTURE_3D,myDataItem->noiseTextureID);
	glUniform1iARB(myDataItem->noiseSamplerLoc,3);
        glActiveTextureARB(GL_TEXTURE4_ARB);
	glBindTexture(GL_TEXTURE_2D,myDataItem->kernalTextureID);
	glUniform1iARB(myDataItem->kernalSamplerLoc,4);
	
	/* Bind the color map textures: */
        glActiveTextureARB(GL_TEXTURE5_ARB);
        glBindTexture(GL_TEXTURE_1D,myDataItem->colorMapTextureID);

        /* Create the stepsize-adjusted colormap with pre-multiplied alpha: */
        GLColorMap adjustedColorMap(*colorMap);
        adjustedColorMap.changeTransparency(stepSize*transparencyGamma);
        adjustedColorMap.premultiplyAlpha();
        glTexImage1D(GL_TEXTURE_1D,0,myDataItem->haveFloatTextures?GL_RGBA32F_ARB:GL_RGBA,256,0,GL_RGBA,GL_FLOAT,adjustedColorMap.getColors());

	glUniform1iARB(myDataItem->colorMapSamplerLoc,5);
	}

void LICRaycaster::unbindShader(Raycaster::DataItem* dataItem) const
	{
	/* Unbind the color map textures: */

        glActiveTextureARB(GL_TEXTURE5_ARB);
        glBindTexture(GL_TEXTURE_1D,0);

        
	/* Unbind the noise and kernel texture: */
	glActiveTextureARB(GL_TEXTURE4_ARB);
	glBindTexture(GL_TEXTURE_2D,0);
        glActiveTextureARB(GL_TEXTURE3_ARB);
	glBindTexture(GL_TEXTURE_3D,0);
        glActiveTextureARB(GL_TEXTURE2_ARB);
	glBindTexture(GL_TEXTURE_3D,0);
        
        
	/* Unbind the volume texture: */
	glActiveTextureARB(GL_TEXTURE1_ARB);
	glBindTexture(GL_TEXTURE_3D,0);
	
	/* Call the base class method: */
	Raycaster::unbindShader(dataItem);
	}

bool LICRaycaster::loadNoiseTexture()
        {
        std::string noiseTextureName=VISUALIZER_SHAREDIR;
        noiseTextureName.append("/Noise/noise-256");
        FILE *src = NULL;
        src = fopen(noiseTextureName.c_str(), "rb");
        if(!src)
                {
                std::cerr<<"LICRaycaster::loadNoiseTexture: No such noise texture data "<<std::endl;
                return false;
                }
        if(fread((void*)noiseSize, 4, 3, src) != 3)
                {
                std::cerr<<"LICRaycaster::loadNoiseTexture: Coud not read noise header from "<< noiseTextureName <<std::endl;
                return false;
                }
        nData = new Voxel[noiseSize[0]*noiseSize[1]*noiseSize[2]];
        std::cout << "noise size: " << noiseSize[0] << " " << noiseSize[1] << " " << noiseSize[2] << std::endl;
        if(fread(nData, noiseSize[0]*noiseSize[1], noiseSize[2], src) != noiseSize[2])
                {
                std::cerr<<"LICRaycaster::loadNoiseTexture: Error reading noise header from "<< noiseTextureName <<std::endl;
                fclose(src);
                delete[] nData;
                nData = NULL;
                return false;
                }
        fclose(src);
        return true;
        }

void LICRaycaster::loadKernelFunction(DataItem* dataItem) const
        {
        /* Create the default topography file name: */
	std::string kernalFileName=VISUALIZER_SHAREDIR;
	#if IMAGES_CONFIG_HAVE_PNG
	kernalFileName.append("/Kernel/triangleKernel.png");
	#else
	kernalFileName.append("/Kernel/triangleKernel.ppm");
	#endif
	
	/* Load the Earth surface texture image from an image file: */
	Images::RGBImage kernelImage=Images::readImageFile(kernalFileName.c_str());
        std::cout<<"kernel image size: "<<kernelImage.getWidth() <<" "<<kernelImage.getHeight()<<std::endl;
        float functionArea = 0.0;
        for(int i = 0; i< int(kernelImage.getWidth()); i++)
            functionArea += kernelImage.getPixel(i,0)[0];
        dataItem->invKernelArea = GLfloat(0.5f*kernelImage.getWidth() * 255.0f / functionArea);
	std::cout<<"inv kernel area: "<<dataItem->invKernelArea<<std::endl;
	/* Select the Earth surface texture object: */
	glBindTexture(GL_TEXTURE_2D,dataItem->kernalTextureID);
	
	/* Upload the Earth surface texture image: */
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	kernelImage.glTexImage2D(GL_TEXTURE_2D,0,GL_LUMINANCE);
	
	/* Protect the Earth surface texture object: */
	glBindTexture(GL_TEXTURE_2D,0);
        }

float* LICRaycaster::paddingData(const unsigned int dataSize[3], Voxel* data) const
        {
        int size = dataSize[0]*dataSize[1]*dataSize[2];
        float* padded = new float[4*size];
        float maxLen = 0.0f;
        int adr;
        for(unsigned int z = 0; z < dataSize[2]; ++z)
            for(unsigned int y = 0; y < dataSize[1]; ++y)
                for(unsigned int x = 0; x < dataSize[0]; ++x)
                {
                    adr = (z*dataSize[1]+y)*dataSize[0]+x;
                    float len = Math::sqrt((float)Math::sqr(data[3 * adr]) + (float)Math::sqr(data[3 * adr + 1]) + (float)Math::sqr(data[3 * adr + 2]));
                    if(len < 1e-5f)
                    {
                        len = 0.0f;
                        padded[4*adr] = 0.5f;
                        padded[4*adr+1] = 0.5f;
                        padded[4*adr+2] = 0.5f;
                    }
                    else
                    {
                        padded[4 * adr] = 0.5f* data[3 * adr] / len + 0.5f;
                        padded[4 * adr + 1] = 0.5f* data[3 * adr + 1] / len + 0.5f;
			padded[4 * adr + 2] = 0.5f* data[3 * adr + 2] / len + 0.5f;
                    }
                    maxLen = std::max(len, maxLen);
                    padded[4 * adr + 3] = len;
                }
        for(unsigned int z = 0; z < dataSize[2]; ++z)
            for(unsigned int y = 0; y < dataSize[1]; ++y)
                for(unsigned int x = 0; x < dataSize[0]; ++x)
                {
                    adr = (z*dataSize[1]+y)*dataSize[0]+x;
                    float len = padded[4 * adr + 3]/maxLen;
                    padded[4 * adr + 3] = Math::clamp(len, 0.0f, 1.0f);
                }
        return padded;
        }

LICRaycaster::LICRaycaster(const unsigned int sDataSize[3],const Raycaster::Box& sDomain)
	:Raycaster(sDataSize,sDomain),
	 data(new Voxel[dataSize[0]*dataSize[1]*dataSize[2]*3]),dataVersion(0),
         nData(NULL)
	{
	/* Multiply the data stride values with the number of channels: */
	for(int dim=0;dim<3;++dim)
		dataStrides[dim]*=3;
        /* Initialize the mask */
//        mask = new LICBrushMask(dataSize);
//        mask->semiSphereX(); //test texture funtion
	
	/* Initialize the channels and color maps: */
        colorMap=0;
	transparencyGamma=1.0f;
        if(!loadNoiseTexture())
                {
                std::cerr<<"LICRaycaster::initContext: Caught exception when loading noise texture "<<std::endl;
                return;
                }   
	}

LICRaycaster::~LICRaycaster(void)
	{
	/* Delete the volume dataset: */
	delete[] data;
        delete[] nData;
	}

void LICRaycaster::initContext(GLContextData& contextData) const
	{
	/* Create a new data item: */
	DataItem* dataItem=new DataItem;
	contextData.addDataItem(this,dataItem);
                
        /* Load kernel function from png image */
        loadKernelFunction(dataItem);
        
	/* Initialize the data item: */
	initDataItem(dataItem);
	
	try
		{
		/* Load and compile the vertex program: */
		std::string vertexShaderName=VISUALIZER_SHADERDIR;
		vertexShaderName.append("/LICRaycaster.vs");
		dataItem->shader.compileVertexShader(vertexShaderName.c_str());
		std::string fragmentShaderName=VISUALIZER_SHADERDIR;
		fragmentShaderName.append("/LICRaycaster.fs");
		dataItem->shader.compileFragmentShader(fragmentShaderName.c_str());
		dataItem->shader.linkShader();
		
		/* Initialize the raycasting shader: */
		initShader(dataItem);
		}
	catch(std::runtime_error err)
		{
		/* Print an error message, but continue: */
		std::cerr<<"LICRaycaster::initContext: Caught exception "<<err.what()<<std::endl;
		}
	}

void LICRaycaster::setStepSize(Raycaster::Scalar newStepSize)
	{
	/* Call the base class method: */
	Raycaster::setStepSize(newStepSize);
	}

void LICRaycaster::updateData(void)
	{
	/* Bump up the data version number: */
	++dataVersion;
	}

void LICRaycaster::setColorMap(const GLColorMap* newColorMap)
	{
	colorMap=newColorMap;
	}

void LICRaycaster::setTransparencyGamma(GLfloat newTransparencyGamma)
	{
	transparencyGamma=newTransparencyGamma;
	}
void LICRaycaster::setLICMask(LICBrushMask* sMask)
        {
        mask = sMask;
        mask->resize(dataSize, domain);
//        mask->semiSphereX();
        }
