/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   LICBrushMask.h
 * Author: Yangguang Liao
 *
 * Created on April 30, 2019, 2:22 PM
 */

#ifndef LICBRUSHMASK_H
#define LICBRUSHMASK_H

#include <GL/gl.h>
#include <GL/GLColor.h>
#include <GL/GLColorMap.h>
#include <Geometry/Box.h>

#include <Vrui/Geometry.h>

class LICBrushMask
{
private:
        typedef GLfloat Voxel; // Type for voxel data  
        typedef float Scalar;
	typedef Geometry::Box<Scalar,3> Box;
        Voxel* mdata;
        unsigned int mVersion;
        unsigned int mdataSize[3];
        Scalar cellSize;
        Box mDomain;

public:
        LICBrushMask();
        LICBrushMask(const unsigned int sDataSize[3]);
        LICBrushMask(const LICBrushMask& orig);
        virtual ~LICBrushMask();
        
        void resize(const unsigned int sDataSize[3], const Box& sDomain);
        
        Voxel* getData()
        {
                return mdata;
        }
        Voxel* getData(unsigned int index[3]);
        Voxel* getData(double pos[3]);
        unsigned int* getSize()
        {
                return mdataSize;
        }
        unsigned int getSize(int idx)
        {
                if(idx >=0 && idx <3)
                        return mdataSize[idx];
                else
                        return 0;
        }
        unsigned int getDataVersion()
        {
                return mVersion;
        }
        
        // Custom generate function
        void semiSphereX();
        
        void updateMaskByBrush(Vrui::Point brushPos, Vrui::Scalar brushSize, Voxel value);
        

};

#endif /* LICBRUSHMASK_H */

