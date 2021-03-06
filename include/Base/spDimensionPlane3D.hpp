/*
 * Plane 3D header
 * 
 * This file is part of the "SoftPixel Engine" (Copyright (c) 2008 by Lukas Hermanns)
 * See "SoftPixelEngine.hpp" for license information.
 */

#ifndef __SP_DIMENSION_PLANE3D_H__
#define __SP_DIMENSION_PLANE3D_H__


#include "Base/spStandard.hpp"
#include "Base/spDimensionVector3D.hpp"
#include "Base/spDimensionTriangle3D.hpp"
#include "Base/spDimensionAABB.hpp"
#include "Base/spDimensionOBB.hpp"

#include <math.h>


namespace sp
{
namespace dim
{


enum EPlaneAABBRelations
{
    RELATION_FRONT,
    RELATION_BACK,
    RELATION_CLIPPED,
};


template <typename T> class plane3d
{
    
    public:
        
        plane3d() : Distance(0)
        {
        }
        plane3d(const vector3d<T> &PlaneNormal, const T Dist) : Normal(PlaneNormal), Distance(Dist)
        {
        }
        plane3d(const vector3d<T> &PointA, const vector3d<T> &PointB, const vector3d<T> &PointC)
        {
            computePlane(PointA, PointB, PointC);
        }
        plane3d(const triangle3d<T> &Triangle)
        {
            computePlane(Triangle.PointA, Triangle.PointB, Triangle.PointC);
        }
        plane3d(const plane3d<T> &other) : Normal(other.Normal), Distance(other.Distance)
        {
        }
        ~plane3d()
        {
        }
        
        /* === Extra functions === */
        
        inline void computePlane(
            const vector3d<T> &PointA, const vector3d<T> &PointB, const vector3d<T> &PointC)
        {
            Normal = vector3d<T>(PointB - PointA).cross(PointC - PointA).normalize();
            Distance = Normal.dot(PointA);
        }
        
        inline bool checkLineIntersection(
            const vector3d<T> &LineStart, const vector3d<T> &LineEnd, vector3d<T> &Intersection) const
        {
            vector3d<T> ab(LineEnd - LineStart);
            T t = (Distance - Normal.dot(LineStart)) / Normal.dot(ab);
            
            if (t >= 0.0 && t <= 1.0)
            {
                Intersection = LineStart + ab * t;
                return true;
            }
            
            return false;
        }
        
        inline bool checkPlaneIntersection(
            const plane3d<T> &other, vector3d<T> &Intersection, vector3d<T> &Direction) const
        {
            /* Compute the direction of the intersection line */
            Direction = Normal.cross(other.Normal);
            
            /*
             * If d is 0 (zero), the planes are parallel (ans separated)
             * or coincident, so they are not considered intersecting
             */
            if (Direction.dot(Direction) < 1.0e-8)
                return false;
            
            /* Compute the intersection */
            f32 d11 = Normal.dot(Normal);
            f32 d12 = Normal.dot(other.Normal);
            f32 d22 = other.Normal.dot(other.Normal);
            
            //f32 det = d11*d22 - d12*d12;
            f32 k1 = ( Distance*d22 - other.Distance*d12 );
            f32 k2 = ( other.Distance*d11 - Distance*d12 );
            
            Intersection = Normal*k1 + other.Normal*k2;
            
            return true;
        }
        
        inline bool checkMultiplePlaneIntersection(
            const plane3d<T> &Plane1, const plane3d<T> &Plane2, vector3d<T> &Intersection) const
        {
            vector3d<T> Point, Direction;
            
            if (checkPlaneIntersection(Plane1, Point, Direction))
                return Plane2.checkLineIntersection(Point, Direction, Intersection);
            
            return false;
        }
        
        inline bool checkAABBoxIntersection(const aabbox3d<T> &Box) const
        {
            /* These two lines not necessary with a (center, extents) AABB representation */
            vector3d<T> c(Box.getCenter()); // Compute AABB center
            vector3d<T> e(Box.Max - c);     // Compute positive extents
            
            /* Compute the projection interval readius of b onto L(t) = b.c + t * p.n */
            f32 r = e.X * fabs(Normal.X) + e.Y * fabs(Normal.Y) + e.Z * fabs(Normal.Z);
            
            /* Compute distance of box center from plane */
            f32 s = Normal.dot(c) - Distance;
            
            /* Intersection occurs when distance s falls within [-r, +r] interval */
            return fabs(s) <= r;
        }
        
        inline bool checkOBBoxIntersection(const obbox3d<T> &Box) const
        {
            /* Compute the projection interval radius of box */
            f32 r =
                Box.HalfSize.X * fabs(Normal.dot(Box.Axis.X)) +
                Box.HalfSize.Y * fabs(Normal.dot(Box.Axis.Y)) +
                Box.HalfSize.Z * fabs(Normal.dot(Box.Axis.Z));
            
            /* Compute distance of box center from plane */
            f32 s = Normal.dot(Box.Center) - Distance;
            
            /* Intersection occurs when distance s falls within [-r, +r] interval */
            return fabs(s) <= r;
        }
        
        inline EPlaneAABBRelations getAABBoxRelation(const aabbox3d<T> &Box) const
        {
            vector3d<T> NearPoint(Box.Max);
            vector3d<T> FarPoint(Box.Min);
            
            if (Normal.X > (T)0)
            {
                NearPoint.X = Box.Min.X;
                FarPoint.X  = Box.Max.X;
            }
            if (Normal.Y > (T)0)
            {
                NearPoint.Y = Box.Min.Y;
                FarPoint.Y  = Box.Max.Y;
            }
            if (Normal.Z > (T)0)
            {
                NearPoint.Z = Box.Min.Z;
                FarPoint.Z  = Box.Max.Z;
            }
            
            if (isPointFrontSide(NearPoint))
                return RELATION_FRONT;
            if (isPointFrontSide(FarPoint))
                return RELATION_CLIPPED;
            
            return RELATION_BACK;
        }
        
        inline vector3d<T> getClosestPoint(const vector3d<T> &Point) const
        {
            return Point - Normal * ( (Normal.dot(Point) - Distance) / Normal.dot(Normal) );
        }
        
        inline vector3d<T> getClosestPointNormalized(const vector3d<T> &Point) const
        {
            return Point - Normal * ( Normal.dot(Point) - Distance );
        }
        
        inline vector3d<T> getMemberPoint() const
        {
            return Normal * Distance;
        }
        
        inline T getPointDistance(const vector3d<T> &Point) const
        {
            return (Normal.dot(Point) - Distance) / Normal.dot(Normal);
        }
        
        inline bool isPointFrontSide(const vector3d<T> &Point) const
        {
            return getPointDistance(Point) >= 0;
        }
        
        inline bool equal(const plane3d<T> &other, f32 Precision = 1.0e-6) const
        {
            return
                Normal.equal(other.Normal, Precision) &&
                (Distance + Precision > other.Distance) &&
                (Distance - Precision < other.Distance);
        }
        
        template <typename B> inline vector3d<B> cast() const
        {
            return plane3d<B>(Normal.cast<B>(), static_cast<B>(Distance));
        }
        
        /* Members */
        
        vector3d<T> Normal;
        T Distance;
        
};

typedef plane3d<s32> plane3di;
typedef plane3d<f32> plane3df;


} // /namespace dim

} // /namespace sp


#endif



// ================================================================================
