#include "stdafx.h"
#include "RotationSpline.h"
#include "..\Util\DebugMisc.h"


namespace Cool3D {

    //---------------------------------------------------------------------
    RotationalSpline::RotationalSpline()
        : mAutoCalc(true)
    {
    }
    //---------------------------------------------------------------------
    RotationalSpline::~RotationalSpline()
    {
    }
    //---------------------------------------------------------------------
    void RotationalSpline::addPoint(const Quaternion& p)
    {
        mPoints.push_back(p);
        if (mAutoCalc)
        {
            recalcTangents();
        }
    }
    //---------------------------------------------------------------------
    Quaternion RotationalSpline::interpolate(float t, bool useShortestPath)
    {
        // Work out which segment this is in
        float fSeg = t * (mPoints.size() - 1);
        unsigned int segIdx = (unsigned int)fSeg;
        // Apportion t 
        t = fSeg - segIdx;

        return interpolate(segIdx, t, useShortestPath);

    }
    //---------------------------------------------------------------------
    Quaternion RotationalSpline::interpolate(unsigned int fromIndex, float t,
		bool useShortestPath)
    {
        // Bounds check
        ASSERT (fromIndex >= 0 && fromIndex < mPoints.size() &&
            "fromIndex out of bounds");

        if ((fromIndex + 1) == mPoints.size())
        {
            // Duff request, cannot blend to nothing
            // Just return source
            return mPoints[fromIndex];

        }
        // Fast special cases
        if (t == 0.0f)
        {
            return mPoints[fromIndex];
        }
        else if(t == 1.0f)
        {
            return mPoints[fromIndex + 1];
        }

        // Real interpolation
        // Use squad using tangents we've already set up
        Quaternion &p = mPoints[fromIndex];
        Quaternion &q = mPoints[fromIndex+1];
        Quaternion &a = mTangents[fromIndex];
        Quaternion &b = mTangents[fromIndex+1];

        // NB interpolate to nearest rotation
		Quaternion result;
		D3DXQuaternionSquad(&result,&p, &a, &b, &q,t);
        return result;

    }
    //---------------------------------------------------------------------
    void RotationalSpline::recalcTangents(void)
    {
        // ShoeMake (1987) approach
        // Just like Catmull-Rom really, just more gnarly
        // And no, I don't understand how to derive this!
        //
        // let p = point[i], pInv = p.Inverse
        // tangent[i] = p * exp( -0.25 * ( log(pInv * point[i+1]) + log(pInv * point[i-1]) ) )
        //
        // Assume endpoint tangents are parallel with line with neighbour

        unsigned int i, numPoints;
        bool isClosed;

        numPoints = (unsigned int)mPoints.size();

        if (numPoints < 2)
        {
            // Can't do anything yet
            return;
        }

        mTangents.resize(numPoints);

        if (mPoints[0] == mPoints[numPoints-1])
        {
            isClosed = true;
        }
        else
        {
            isClosed = false;
        }

        Quaternion invp, part1, part2, preExp,preExp2;
        for(i = 0; i < numPoints; ++i)
        {
            Quaternion &p = mPoints[i];
			D3DXQuaternionInverse(&invp,&p);
 
            if (i ==0)
            {
                // special case start
				D3DXQuaternionLn(&part1,&(invp * mPoints[i+1]));
                if (isClosed)
                {
                    // Use numPoints-2 since numPoints-1 == end == start == this one
					D3DXQuaternionLn(&part2,&(invp * mPoints[numPoints-2]));
                }
                else
                {
					D3DXQuaternionLn(&part2,&(invp * p));
                }
            }
            else if (i == numPoints-1)
            {
                // special case end
                if (isClosed)
                {
                    // Wrap to [1] (not [0], this is the same as end == this one)
					D3DXQuaternionLn(&part1,&(invp * mPoints[1]));
                }
                else
                {
					D3DXQuaternionLn(&part1,&(invp * p));

                }
				D3DXQuaternionLn(&part2,&(invp * mPoints[i-1]));
            }
            else
            {
				D3DXQuaternionLn(&part1,&(invp * mPoints[i+1]));
				D3DXQuaternionLn(&part2,&(invp * mPoints[i-1]));
            }

            preExp = -0.25 * (part1 + part2);
			D3DXQuaternionExp(&preExp2,&preExp);
            mTangents[i] = p * preExp2;
            
        }



    }
    //---------------------------------------------------------------------
    const Quaternion& RotationalSpline::getPoint(unsigned short index) const
    {
        ASSERT (index < mPoints.size() && "Point index is out of bounds!!");

        return mPoints[index];
    }
    //---------------------------------------------------------------------
    unsigned short RotationalSpline::getNumPoints(void) const
    {
        return (unsigned short)mPoints.size();
    }
    //---------------------------------------------------------------------
    void RotationalSpline::clear(void)
    {
        mPoints.clear();
        mTangents.clear();
    }
    //---------------------------------------------------------------------
    void RotationalSpline::updatePoint(unsigned short index, const Quaternion& value)
    {
        ASSERT (index < mPoints.size() && "Point index is out of bounds!!");

        mPoints[index] = value;
        if (mAutoCalc)
        {
            recalcTangents();
        }
    }
    //---------------------------------------------------------------------
    void RotationalSpline::setAutoCalculate(bool autoCalc)
    {
        mAutoCalc = autoCalc;
    }
    //---------------------------------------------------------------------



}




