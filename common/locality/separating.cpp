
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   separating.cpp
////    Author: Ritchie Brannan
////    Date:   16 Dec 14
////
////    Description:
////
////        Separating axis collision detection structures and functions
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    includes
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "separating.h"
#include "libs/system/debug/asserts.h"
#include "libs/maths/consts.h"
#include <math.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    begin detection namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace detection
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Oriented box collisions using separating axis theorem
////
////    needs to include collisions with verts, edges, tris and polys
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////  box->box
//bool collide( const Box& object, const Box& target, const maths::vec3& direction, float& t, maths::vec3& normal )
//{
//    maths::vec3 axes[ 15 ];
//    axes[  0 ].x = ( ( object.unit.y.y * object.unit.z.z ) - ( object.unit.z.y * object.unit.y.z ) );   //  object x face separating axis
//    axes[  0 ].y = ( ( object.unit.z.y * object.unit.x.z ) - ( object.unit.x.y * object.unit.z.z ) );
//    axes[  0 ].z = ( ( object.unit.x.y * object.unit.y.z ) - ( object.unit.y.y * object.unit.x.z ) );
//    axes[  1 ].x = ( ( object.unit.y.z * object.unit.z.x ) - ( object.unit.z.z * object.unit.y.x ) );   //  object y face separating axis
//    axes[  1 ].y = ( ( object.unit.z.z * object.unit.x.x ) - ( object.unit.x.z * object.unit.z.x ) );
//    axes[  1 ].z = ( ( object.unit.x.z * object.unit.y.x ) - ( object.unit.y.z * object.unit.x.x ) );
//    axes[  2 ].x = ( ( object.unit.y.x * object.unit.z.y ) - ( object.unit.z.x * object.unit.y.y ) );   //  object z face separating axis
//    axes[  2 ].y = ( ( object.unit.z.x * object.unit.x.y ) - ( object.unit.x.x * object.unit.z.y ) );
//    axes[  2 ].z = ( ( object.unit.x.x * object.unit.y.y ) - ( object.unit.y.x * object.unit.x.y ) );
//    axes[  3 ].x = ( ( target.unit.y.y * target.unit.z.z ) - ( target.unit.z.y * target.unit.y.z ) );   //  target x face separating axis
//    axes[  3 ].y = ( ( target.unit.z.y * target.unit.x.z ) - ( target.unit.x.y * target.unit.z.z ) );
//    axes[  3 ].z = ( ( target.unit.x.y * target.unit.y.z ) - ( target.unit.y.y * target.unit.x.z ) );
//    axes[  4 ].x = ( ( target.unit.y.z * target.unit.z.x ) - ( target.unit.z.z * target.unit.y.x ) );   //  target y face separating axis
//    axes[  4 ].y = ( ( target.unit.z.z * target.unit.x.x ) - ( target.unit.x.z * target.unit.z.x ) );
//    axes[  4 ].z = ( ( target.unit.x.z * target.unit.y.x ) - ( target.unit.y.z * target.unit.x.x ) );
//    axes[  5 ].x = ( ( target.unit.y.x * target.unit.z.y ) - ( target.unit.z.x * target.unit.y.y ) );   //  target z face separating axis
//    axes[  5 ].y = ( ( target.unit.z.x * target.unit.x.y ) - ( target.unit.x.x * target.unit.z.y ) );
//    axes[  5 ].z = ( ( target.unit.x.x * target.unit.y.y ) - ( target.unit.y.x * target.unit.x.y ) );
//    axes[  6 ].x = ( ( object.unit.y.x * target.unit.z.x ) - ( object.unit.z.x * target.unit.y.x ) );   //  object x edge and target x edge separating axis
//    axes[  6 ].y = ( ( object.unit.z.x * target.unit.x.x ) - ( object.unit.x.x * target.unit.z.x ) );
//    axes[  6 ].z = ( ( object.unit.x.x * target.unit.y.x ) - ( object.unit.y.x * target.unit.x.x ) );
//    axes[  7 ].x = ( ( object.unit.y.x * target.unit.z.y ) - ( object.unit.z.x * target.unit.y.y ) );   //  object x edge and target y edge separating axis
//    axes[  7 ].y = ( ( object.unit.z.x * target.unit.x.y ) - ( object.unit.x.x * target.unit.z.y ) );
//    axes[  7 ].z = ( ( object.unit.x.x * target.unit.y.y ) - ( object.unit.y.x * target.unit.x.y ) );
//    axes[  8 ].x = ( ( object.unit.y.x * target.unit.z.z ) - ( object.unit.z.x * target.unit.y.z ) );   //  object x edge and target z edge separating axis
//    axes[  8 ].y = ( ( object.unit.z.x * target.unit.x.z ) - ( object.unit.x.x * target.unit.z.z ) );
//    axes[  8 ].z = ( ( object.unit.x.x * target.unit.y.z ) - ( object.unit.y.x * target.unit.x.z ) );
//    axes[  9 ].x = ( ( object.unit.y.y * target.unit.z.x ) - ( object.unit.z.y * target.unit.y.x ) );   //  object y edge and target x edge separating axis
//    axes[  9 ].y = ( ( object.unit.z.y * target.unit.x.x ) - ( object.unit.x.y * target.unit.z.x ) );
//    axes[  9 ].z = ( ( object.unit.x.y * target.unit.y.x ) - ( object.unit.y.y * target.unit.x.x ) );
//    axes[ 10 ].x = ( ( object.unit.y.y * target.unit.z.y ) - ( object.unit.z.y * target.unit.y.y ) );   //  object y edge and target y edge separating axis
//    axes[ 10 ].y = ( ( object.unit.z.y * target.unit.x.y ) - ( object.unit.x.y * target.unit.z.y ) );
//    axes[ 10 ].z = ( ( object.unit.x.y * target.unit.y.y ) - ( object.unit.y.y * target.unit.x.y ) );
//    axes[ 11 ].x = ( ( object.unit.y.y * target.unit.z.z ) - ( object.unit.z.y * target.unit.y.z ) );   //  object y edge and target z edge separating axis
//    axes[ 11 ].y = ( ( object.unit.z.y * target.unit.x.z ) - ( object.unit.x.y * target.unit.z.z ) );
//    axes[ 11 ].z = ( ( object.unit.x.y * target.unit.y.z ) - ( object.unit.y.y * target.unit.x.z ) );
//    axes[ 12 ].x = ( ( object.unit.y.z * target.unit.z.x ) - ( object.unit.z.z * target.unit.y.x ) );   //  object z edge and target x edge separating axis
//    axes[ 12 ].y = ( ( object.unit.z.z * target.unit.x.x ) - ( object.unit.x.z * target.unit.z.x ) );
//    axes[ 12 ].z = ( ( object.unit.x.z * target.unit.y.x ) - ( object.unit.y.z * target.unit.x.x ) );
//    axes[ 13 ].x = ( ( object.unit.y.z * target.unit.z.y ) - ( object.unit.z.z * target.unit.y.y ) );   //  object z edge and target y edge separating axis
//    axes[ 13 ].y = ( ( object.unit.z.z * target.unit.x.y ) - ( object.unit.x.z * target.unit.z.y ) );
//    axes[ 13 ].z = ( ( object.unit.x.z * target.unit.y.y ) - ( object.unit.y.z * target.unit.x.y ) );
//    axes[ 14 ].x = ( ( object.unit.y.z * target.unit.z.z ) - ( object.unit.z.z * target.unit.y.z ) );   //  object z edge and target z edge separating axis
//    axes[ 14 ].y = ( ( object.unit.z.z * target.unit.x.z ) - ( object.unit.x.z * target.unit.z.z ) );
//    axes[ 14 ].z = ( ( object.unit.x.z * target.unit.y.z ) - ( object.unit.y.z * target.unit.x.z ) );
//
////  some of the above axes may be invalid (zero vectors) or perpendicular to the direction of travel (also invalid)
//
//}
//
////  obb->obb
//bool collide( const OBB& object, const OBB& target, const maths::vec3& direction, float& t, maths::vec3& normal )
//{
//    maths::mat33 m[ 2 ];
//    maths::vec3 axes[ 15 ];
//    object.location.q.Get( m[ 0 ] );
//    target.location.q.Get( m[ 1 ] );
//    axes[  0 ].x = m[ 0 ].x.x;      //  object x face separating axis
//    axes[  0 ].y = m[ 0 ].y.x;
//    axes[  0 ].z = m[ 0 ].z.x;
//    axes[  1 ].x = m[ 0 ].x.y;      //  object y face separating axis
//    axes[  1 ].y = m[ 0 ].y.y;
//    axes[  1 ].z = m[ 0 ].z.y;
//    axes[  2 ].x = m[ 0 ].x.z;      //  object z face separating axis
//    axes[  2 ].y = m[ 0 ].y.z;
//    axes[  2 ].z = m[ 0 ].z.z;
//    axes[  3 ].x = m[ 1 ].x.x;      //  target x face separating axis
//    axes[  3 ].y = m[ 1 ].y.x;
//    axes[  3 ].z = m[ 1 ].z.x;
//    axes[  4 ].x = m[ 1 ].x.y;      //  target y face separating axis
//    axes[  4 ].y = m[ 1 ].y.y;
//    axes[  4 ].z = m[ 1 ].z.y;
//    axes[  5 ].x = m[ 1 ].x.z;      //  target z face separating axis
//    axes[  5 ].y = m[ 1 ].y.z;
//    axes[  5 ].z = m[ 1 ].z.z;
//    axes[  6 ].x = ( ( m[ 0 ].y.x * m[ 1 ].z.x ) - ( m[ 0 ].z.x * m[ 1 ].y.x ) );   //  object x edge and target x edge separating axis
//    axes[  6 ].y = ( ( m[ 0 ].z.x * m[ 1 ].x.x ) - ( m[ 0 ].x.x * m[ 1 ].z.x ) );
//    axes[  6 ].z = ( ( m[ 0 ].x.x * m[ 1 ].y.x ) - ( m[ 0 ].y.x * m[ 1 ].x.x ) );
//    axes[  7 ].x = ( ( m[ 0 ].y.x * m[ 1 ].z.y ) - ( m[ 0 ].z.x * m[ 1 ].y.y ) );   //  object x edge and target y edge separating axis
//    axes[  7 ].y = ( ( m[ 0 ].z.x * m[ 1 ].x.y ) - ( m[ 0 ].x.x * m[ 1 ].z.y ) );
//    axes[  7 ].z = ( ( m[ 0 ].x.x * m[ 1 ].y.y ) - ( m[ 0 ].y.x * m[ 1 ].x.y ) );
//    axes[  8 ].x = ( ( m[ 0 ].y.x * m[ 1 ].z.z ) - ( m[ 0 ].z.x * m[ 1 ].y.z ) );   //  object x edge and target z edge separating axis
//    axes[  8 ].y = ( ( m[ 0 ].z.x * m[ 1 ].x.z ) - ( m[ 0 ].x.x * m[ 1 ].z.z ) );
//    axes[  8 ].z = ( ( m[ 0 ].x.x * m[ 1 ].y.z ) - ( m[ 0 ].y.x * m[ 1 ].x.z ) );
//    axes[  9 ].x = ( ( m[ 0 ].y.y * m[ 1 ].z.x ) - ( m[ 0 ].z.y * m[ 1 ].y.x ) );   //  object y edge and target x edge separating axis
//    axes[  9 ].y = ( ( m[ 0 ].z.y * m[ 1 ].x.x ) - ( m[ 0 ].x.y * m[ 1 ].z.x ) );
//    axes[  9 ].z = ( ( m[ 0 ].x.y * m[ 1 ].y.x ) - ( m[ 0 ].y.y * m[ 1 ].x.x ) );
//    axes[ 10 ].x = ( ( m[ 0 ].y.y * m[ 1 ].z.y ) - ( m[ 0 ].z.y * m[ 1 ].y.y ) );   //  object y edge and target y edge separating axis
//    axes[ 10 ].y = ( ( m[ 0 ].z.y * m[ 1 ].x.y ) - ( m[ 0 ].x.y * m[ 1 ].z.y ) );
//    axes[ 10 ].z = ( ( m[ 0 ].x.y * m[ 1 ].y.y ) - ( m[ 0 ].y.y * m[ 1 ].x.y ) );
//    axes[ 11 ].x = ( ( m[ 0 ].y.y * m[ 1 ].z.z ) - ( m[ 0 ].z.y * m[ 1 ].y.z ) );   //  object y edge and target z edge separating axis
//    axes[ 11 ].y = ( ( m[ 0 ].z.y * m[ 1 ].x.z ) - ( m[ 0 ].x.y * m[ 1 ].z.z ) );
//    axes[ 11 ].z = ( ( m[ 0 ].x.y * m[ 1 ].y.z ) - ( m[ 0 ].y.y * m[ 1 ].x.z ) );
//    axes[ 12 ].x = ( ( m[ 0 ].y.z * m[ 1 ].z.x ) - ( m[ 0 ].z.z * m[ 1 ].y.x ) );   //  object z edge and target x edge separating axis
//    axes[ 12 ].y = ( ( m[ 0 ].z.z * m[ 1 ].x.x ) - ( m[ 0 ].x.z * m[ 1 ].z.x ) );
//    axes[ 12 ].z = ( ( m[ 0 ].x.z * m[ 1 ].y.x ) - ( m[ 0 ].y.z * m[ 1 ].x.x ) );
//    axes[ 13 ].x = ( ( m[ 0 ].y.z * m[ 1 ].z.y ) - ( m[ 0 ].z.z * m[ 1 ].y.y ) );   //  object z edge and target y edge separating axis
//    axes[ 13 ].y = ( ( m[ 0 ].z.z * m[ 1 ].x.y ) - ( m[ 0 ].x.z * m[ 1 ].z.y ) );
//    axes[ 13 ].z = ( ( m[ 0 ].x.z * m[ 1 ].y.y ) - ( m[ 0 ].y.z * m[ 1 ].x.y ) );
//    axes[ 14 ].x = ( ( m[ 0 ].y.z * m[ 1 ].z.z ) - ( m[ 0 ].z.z * m[ 1 ].y.z ) );   //  object z edge and target z edge separating axis
//    axes[ 14 ].y = ( ( m[ 0 ].z.z * m[ 1 ].x.z ) - ( m[ 0 ].x.z * m[ 1 ].z.z ) );
//    axes[ 14 ].z = ( ( m[ 0 ].x.z * m[ 1 ].y.z ) - ( m[ 0 ].y.z * m[ 1 ].x.z ) );
//
//}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end detection namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

};	//	namespace detection

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

