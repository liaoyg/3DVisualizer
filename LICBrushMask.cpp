/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   LICBrushMask.cpp
 * Author: Yangguang Liao
 * 
 * Created on April 30, 2019, 2:22 PM
 */

#include "LICBrushMask.h"
#include <Math/Math.h>
#include <iostream>

LICBrushMask::LICBrushMask()
        {
        // default size texture is 64X64X64
        mdata = new Voxel[64*64*64*4];
        mVersion = 0;
        
        for(int dim=0;dim<3;++dim)
                {
                mdataSize[dim] =64;
                mDomain.min[dim] = 0.0;
                mDomain.max[dim] = 1.0;
		cellSize+=Math::sqr((mDomain.max[dim]-mDomain.min[dim])/Scalar(mdataSize[dim]));
                }
	cellSize=Math::sqrt(cellSize);
        std::cout << "LIC Brush cell size: "<< cellSize << std::endl;
        }

LICBrushMask::LICBrushMask(const unsigned int sDataSize[3])
        :mdata(new Voxel[sDataSize[0]*sDataSize[1]*sDataSize[2]*4]),
        mVersion(0)
        {
        for(int dim=0;dim<3;++dim)
                mdataSize[dim] =sDataSize[dim];
        
        }

LICBrushMask::LICBrushMask(const LICBrushMask& orig) 
        {
        delete[] mdata;
        }

LICBrushMask::~LICBrushMask() 
        {
        
        }

void LICBrushMask::resize(const unsigned int sDataSize[3], const Box& sDomain)
        {
        delete mdata;
        mdata = new Voxel[sDataSize[0]*sDataSize[1]*sDataSize[2]*4];
        mVersion++;
        mDomain = sDomain;
        for(int dim=0;dim<3;++dim)
                {
                mdataSize[dim] =sDataSize[dim];
		cellSize+=Math::sqr((mDomain.max[dim]-mDomain.min[dim])/Scalar(mdataSize[dim])); 
                }
	cellSize=Math::sqrt(cellSize);
        
        std::cout << "LIC Brush domain: " << std::endl;
        std::cout<<mDomain.min[0]<<" "<<mDomain.min[1]<<" "<<mDomain.min[2]<<std::endl;
        std::cout<<mDomain.max[0]<<" "<<mDomain.max[1]<<" "<<mDomain.max[2]<<std::endl;
        std::cout << "LIC Brush cell size: "<< cellSize << std::endl;
        }

void LICBrushMask::semiSphereX()
        {
        int size = mdataSize[0]*mdataSize[1]*mdataSize[2];
        std::cout << "mask size: " << size << std::endl;
        for(unsigned int i=0; i<mdataSize[0]/2; i++)
                for(unsigned int j=0; j<mdataSize[1]; j++)
                        for(unsigned int k=0; k<mdataSize[2]; k++)
                                {
                                long adr = (j+k*mdataSize[1])*mdataSize[0]+i;
                                mdata[4*adr] = 1.0f;
                                mdata[4*adr+1] = 1.0f;
                                mdata[4*adr+2] = 1.0f;
                                mdata[4*adr+3] = 1.0f;
                                }
        mVersion++;
        }

void LICBrushMask::updateMaskByBrush(Vrui::Point brushPos, Vrui::Scalar brushSize, Voxel value)
        {
        std::cout << "update with brush at: "<<brushPos[0]<<" "<<brushPos[1]<<" "<<brushPos[2]<<" with value: "<<value<< std::endl;
        Geometry::Box<int,3> ranges_v, ranges_n;
        Scalar nBrushSize = 0.4*brushSize;
        for(int dim = 0; dim < 3; dim++)
                {
                ranges_v.min[dim] = mdataSize[dim]*(brushPos[dim]-brushSize - mDomain.min[dim])/(mDomain.max[dim] - mDomain.min[dim]);
                ranges_v.min[dim] = Math::clamp<int>(ranges_v.min[dim], 0, mdataSize[dim]-1);
                ranges_n.min[dim] = mdataSize[dim]*(brushPos[dim]-nBrushSize - mDomain.min[dim])/(mDomain.max[dim] - mDomain.min[dim]);
                ranges_n.min[dim] = Math::clamp<int>(ranges_n.min[dim], 0, mdataSize[dim]-1);
                ranges_v.max[dim] = mdataSize[dim]*(brushPos[dim]+brushSize - mDomain.min[dim])/(mDomain.max[dim] - mDomain.min[dim]);
                ranges_v.max[dim] = Math::clamp<int>(ranges_v.max[dim], 0, mdataSize[dim]-1);
                ranges_n.max[dim] = mdataSize[dim]*(brushPos[dim]+nBrushSize - mDomain.min[dim])/(mDomain.max[dim] - mDomain.min[dim]);
                ranges_n.max[dim] = Math::clamp<int>(ranges_n.max[dim], 0, mdataSize[dim]-1);
//                std::cout <<dim<< "range: " << int(ranges_v.min[dim]) << " " << int(ranges_v.max[dim])<<std::endl; 
                }
        for(int x = ranges_v.min[0]; x <= ranges_v.max[0]; x++)
            for(int y = ranges_v.min[1]; y <= ranges_v.max[1]; y++)
                for(int z = ranges_v.min[2]; z <= ranges_v.max[2]; z++)
                        {
                        long adr = (y+z*mdataSize[1])*mdataSize[0]+x;
                        mdata[4*adr] = value; //r is stored vector_field_mask
                        if(x>=ranges_n.min[0] && x <= ranges_n.max[0] && 
                                y>=ranges_n.min[1] && y <= ranges_n.max[1] &&
                                    z>=ranges_n.min[2] && z <= ranges_n.max[2])
                                mdata[4*adr+1] = value;
                        
//                        mdata[4*adr+1] = value;
//                        mdata[4*adr+2] = value;
//                        mdata[4*adr+3] = value;
//                        std::cout <<"mdata: "<<mdata[4*adr]<<" "<<mdata[4*adr+1]<<" "<<mdata[4*adr+2]<<" "<<mdata[4*adr+3]<<std::endl;
                        }
        mVersion++;
        }
