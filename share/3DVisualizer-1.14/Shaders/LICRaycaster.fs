/***********************************************************************
Fragment shader for GPU-based 3D LIC raycasting
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

uniform vec3 mcScale;
uniform sampler2D depthSampler;
uniform mat4 depthMatrix;
uniform vec2 depthSize;
uniform vec3 eyePosition;
uniform float stepSize;

uniform vec3 gradient; // scale, illum scale, frequency scale
uniform vec3 licKernel;  // kernel step width forward (0.5/licParams.x),
                         // kernel step width backward (0.5/licParams.y),
                         // inverse filter area
uniform vec3 licParams;  // lics steps forward, backward, lic step width

uniform sampler3D volumeSampler;
uniform sampler3D noiseSampler;
uniform sampler1D licKernelSampler;

uniform bool channelEnableds[3];
uniform sampler1D colorMapSamplers[3];

varying vec3 mcPosition;
varying vec3 dcPosition;

// 3D LIC functions

float noiseLookup(in vec3 objPos, in float freqScale)
{
    vec3 freqTexCoord = objPos * freqScale;
    return texture3D(noiseSampler, freqTexCoord).a;
}

float freqSampling(in vec3 pos, in sampler3D vectorVolume = volumeSampler)
	{
	//if (scalarData > minScalarRange && scalarData < maxScalarRange)
	//if (vectorData.z > minScalarRange && vectorData.z < maxScalarRange)
	if (true)
	{

		return noiseLookup(pos, gradient.z);
	}
	else
		return 0;
}

float singleLICstep(in out vec3 newPos, in out vec4 step, in float kernelOffset,
                   in float dir, in sampler3D vectorVolume  = volumeSampler)
	{
    	float noise;

	vec3 licdir = dir*(2.0*step.rgb - 1.0);
    	// consider speed of flow
	#ifdef SPEED_OF_FLOW
    	licdir *= step.a;
	#endif

    	// scale with LIC step size
    	// also correct length according to camera distance
    	licdir *= licParams.z * 0.3;
    	vec3 Pos2 = newPos + licdir;
	vec4 step2 = texture3D(vectorVolume, Pos2);
	vec3 licdir2 = dir*(2.0*step2.rgb - 1.0);
	//licdir2 = step2.rgb;
	#ifdef SPEED_OF_FLOW
    	licdir2 *= step.a;
	#endif
	licdir2 *= licParams.z * 0.3;
	newPos += 0.5 * (licdir + licdir2);

    	step = texture3D(vectorVolume, newPos);
	noise = freqSampling(newPos, vectorVolume);
    	// determine weighting
    	noise *= texture1D(licKernelSampler, kernelOffset).r;

    	return noise;
	}

vec4 computeLIC(in vec3 pos, in vec4 vec, in sampler3D vectorVolume)
	{
	vec3 licdir;
    	float logEyeDist;
    	float kernelOffset = 0.5;
	float ao = 1.0;
	
	float noise;
    	float illum = freqSampling(pos, vectorVolume);
	float illum_front = 0.0;
	float illum_back = 0.0;

	// weight sample with lic kernel at position 0
    	illum *= texture1D(licKernelSampler, 0.5).r;

	float dir = -1;
    	// backward LIC
    	vec3 newPos = pos;
    	vec4 step = vec;
    	for (int i=0; i<int(licParams.y); ++i)
    		{
        	kernelOffset -= licKernel.y;
        	illum_front += singleLICstep(newPos, level, step, kernelOffset, 
logEyeDist, dir, ao, vectorVolume);
    		}
    	// forward LIC
	dir = 1;
    	newPos = pos;
    	step = vec;
    	kernelOffset = 0.5;
    	for (int i=0; i<int(licParams.x); ++i)
    		{
        	kernelOffset += licKernel.x;
        	illum_back += singleLICstep(newPos, level, step,kernelOffset, 
logEyeDist, dir, ao, vectorVolume);
    		}
    	return vec4(illum+illum_front+illum_back);
	}

// Ray casting function

void main()
	{
	/* Calculate the ray direction in model coordinates: */
	vec3 mcDir=mcPosition-eyePosition;
	
	/* Get the distance from the eye to the ray starting point: */
	float eyeDist=length(mcDir);
	
	/* Normalize and multiply the ray direction with the current step size: */
	mcDir=normalize(mcDir);
	mcDir*=stepSize;
	eyeDist/=stepSize;
	
	/* Get the fragment's ray termination depth from the depth texture: */
	float termDepth=2.0*texture2D(depthSampler,gl_FragCoord.xy/depthSize).x-1.0;
	
	/* Calculate the maximum number of steps based on the termination depth: */
	vec4 cc1=depthMatrix*vec4(mcPosition,1.0);
	vec4 cc2=depthMatrix*vec4(mcDir,0.0);
	float lambdaMax=-(termDepth*cc1.w-cc1.z)/(termDepth*cc2.w-cc2.z);
	
	/* Convert the ray direction to data coordinates: */
	vec3 dcDir=mcDir*mcScale;
	
	/* Cast the ray and accumulate opacities and colors: */
	vec4 accum=vec4(0.0,0.0,0.0,0.0);
	
	/* Move the ray starting position forward to an integer multiple of the step size: */
	vec3 samplePos=dcPosition;
	float lambda=ceil(eyeDist)-eyeDist;
	if(lambda<lambdaMax)
		{
		samplePos+=dcDir*lambda;
		for(int i=0;i<1500;++i)
			{
			/* Get the volume data value at the current sample position: */
			vec3 data=texture3D(volumeSampler,samplePos);
			
			/* Apply the three transfer functions: */
			vec4 vol=vec4(0.0,0.0,0.0,0.0);
			if(channelEnableds[0])
				vol+=texture1D(colorMapSamplers[0],data.r);
			if(channelEnableds[1])
				vol+=texture1D(colorMapSamplers[1],data.g);
			if(channelEnableds[2])
				vol+=texture1D(colorMapSamplers[2],data.b);
			
			/* Limit sample's opacity to [0.0, 1.0] range: */
			vol.a=clamp(vol.a,0.0,1.0);
			
			/* Accumulate color and opacity: */
			accum+=vol*(1.0-accum.a);
			
			/* Bail out when opacity hits 1.0: */
			if(accum.a>=1.0-1.0/256.0||lambda>=lambdaMax)
				break;
			
			/* Advance the sample position: */
			samplePos+=dcDir;
			lambda+=1.0;
			}
		}
	
	/* Assign the final color value: */
	gl_FragColor=accum;
	}
