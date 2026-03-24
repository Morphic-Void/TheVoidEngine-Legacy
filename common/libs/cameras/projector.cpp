
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   projector.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	Projection geometry container and functions.
////
////    Comments:
////
////    	This library assumes row vectors with column major matricies implemented with
////    	row major multiplication order, as a result matrices are stored transposed.
////
////    	This only affects accessing the rows and components of matrices.
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Notes:
////
////    abbreviations:
////  
////        ss = ( stereo_seperation * 0.5f );
////        ip = infinite_projection;
////        ps = perspective_strength;
////        uw = unit_w_z;
////        fc = far_clip_z;
////        nc = near_clip_z;
////        vw = viewport_width;
////        vh = viewport_height;
////        vx = viewport_center.x;
////        vy = viewport_center.y;
////        vz = viewport_center.z;
////        ox = origin_position.x;
////        oy = origin_position.y;
////        oz = origin_position.z;
////  
////    standard projection:
////  
////        wz = ...........ps/(uw-oz)
////        ww = (uw(1-ps)-oz)/(uw-oz)
////  
////        zz = ...............(wz*ip)+((((fc+nc)*wz)+2ww).........*(1-ip)/(fc-nc))
////        zw = 0-((((nc*wz*2)+ww)*ip)+((((fc+nc)*ww)+(fc*nc*wz*2))*(1-ip)/(fc-nc)))
////  
////        yy = 2((vz*wz)+ww)/vh
////        yz = yy(oy-vy)/(vz-oz)
////        yw = yy((oz*vy)-(vz*oy))/(vz-oz)
////  
////        xx = 2((vz*wz)+ww)/vw
////        xz = xx((ox+/-ss)-vx)/(vz-oz)
////        xw = xx((oz*vx)-(vz*(ox+/-ss)))/(vz-oz)
////  
////    origin shift projection (effectively subtracts origin_position from all coordinates):
////  
////        wz = ps/(uw-oz)
////        ww = (1-ps)
////  
////        zz = ....................(wz*ip)+((((fc-oz+nc-oz)*wz)+2ww)...................*(1-ip)/(fc-nc))
////        zw = 0-(((((nc-oz)*wz*2)+ww)*ip)+((((fc-oz+nc-oz)*ww)+((fc-oz)*(nc-oz)*wz*2))*(1-ip)/(fc-nc)))
////  
////        yy = 2(((vz-oz)*wz)+ww)/vh
////        yz = yy(oy-vy)/(vz-oz)
////        yw = 0
////  
////        xx = 2(((vz-oz)*wz)+ww)/vw
////        xz = xx((ox+/-ss)-vx)/(vz-oz)
////        xw = 0
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

#include "projector.h"
#include "libs/maths/frect.h"
#include "libs/maths/mat44.h"
#include "libs/maths/consts.h"
#include "libs/system/base/types.h"
#include "libs/system/debug/asserts.h"
#include <math.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    begin maths namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace maths
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    projector
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
bool projector::IsNan( void ) const
{
	return( ::IsNan( stereo_seperation ) || ::IsNan( infinite_projection ) || ::IsNan( perspective_strength ) || ::IsNan( unit_w_z ) || ::IsNan( far_clip_z ) || ::IsNan( near_clip_z ) || ::IsNan( viewport_width ) || ::IsNan( viewport_height ) || viewport_center.IsNan() || origin_position.IsNan() );
}

bool projector::IsReal( void ) const
{
	return( ::IsReal( stereo_seperation ) && ::IsReal( infinite_projection ) && ::IsReal( perspective_strength ) && ::IsReal( unit_w_z ) && ::IsReal( far_clip_z ) && ::IsReal( near_clip_z ) && ::IsReal( viewport_width ) && ::IsReal( viewport_height ) && viewport_center.IsReal() && origin_position.IsReal() );
}

void projector::Jog( const float x, const float y )
{
	viewport_center.x += x;
	viewport_center.y += y;
	origin_position.x += x;
	origin_position.y += y;
}

void projector::Zoom( const float z )
{
	float scale = ( 1.0f / ( z + consts::FLOAT_MIN_NORM ) );
	viewport_width *= scale;
	viewport_height *= scale;
}

void projector::Pull( const float z )
{
	viewport_center.z -= z;
	origin_position.z -= z;
}

void projector::Lerp( const projector& trg, const float t )
{
	float s = ( 1.0f - t );
	stereo_seperation = ( ( stereo_seperation * s ) + ( trg.stereo_seperation * t ) );
	infinite_projection = ( ( infinite_projection * s ) + ( trg.infinite_projection * t ) );
	perspective_strength = ( ( perspective_strength * s ) + ( trg.perspective_strength * t ) );
	unit_w_z = ( ( unit_w_z * s ) + ( trg.unit_w_z * t ) );
	far_clip_z = ( ( far_clip_z * s ) + ( trg.far_clip_z * t ) );
	near_clip_z = ( ( near_clip_z * s ) + ( trg.near_clip_z * t ) );
	viewport_width = ( ( viewport_width * s ) + ( trg.viewport_width * t ) );
	viewport_height = ( ( viewport_height * s ) + ( trg.viewport_height * t ) );
	viewport_center.x = ( ( viewport_center.x * s ) + ( trg.viewport_center.x * t ) );
	viewport_center.y = ( ( viewport_center.y * s ) + ( trg.viewport_center.y * t ) );
	viewport_center.z = ( ( viewport_center.z * s ) + ( trg.viewport_center.z * t ) );
	origin_position.x = ( ( origin_position.x * s ) + ( trg.origin_position.x * t ) );
	origin_position.y = ( ( origin_position.y * s ) + ( trg.origin_position.y * t ) );
	origin_position.z = ( ( origin_position.z * s ) + ( trg.origin_position.z * t ) );
}

void projector::SetLerp( const projector& src, const projector& trg, const float t )
{
	float s = ( 1.0f - t );
	stereo_seperation = ( ( src.stereo_seperation * s ) + ( trg.stereo_seperation * t ) );
	infinite_projection = ( ( src.infinite_projection * s ) + ( trg.infinite_projection * t ) );
	perspective_strength = ( ( src.perspective_strength * s ) + ( trg.perspective_strength * t ) );
	unit_w_z = ( ( src.unit_w_z * s ) + ( trg.unit_w_z * t ) );
	far_clip_z = ( ( src.far_clip_z * s ) + ( trg.far_clip_z * t ) );
	near_clip_z = ( ( src.near_clip_z * s ) + ( trg.near_clip_z * t ) );
	viewport_width = ( ( src.viewport_width * s ) + ( trg.viewport_width * t ) );
	viewport_height = ( ( src.viewport_height * s ) + ( trg.viewport_height * t ) );
	viewport_center.x = ( ( src.viewport_center.x * s ) + ( trg.viewport_center.x * t ) );
	viewport_center.y = ( ( src.viewport_center.y * s ) + ( trg.viewport_center.y * t ) );
	viewport_center.z = ( ( src.viewport_center.z * s ) + ( trg.viewport_center.z * t ) );
	origin_position.x = ( ( src.origin_position.x * s ) + ( trg.origin_position.x * t ) );
	origin_position.y = ( ( src.origin_position.y * s ) + ( trg.origin_position.y * t ) );
	origin_position.z = ( ( src.origin_position.z * s ) + ( trg.origin_position.z * t ) );
}

void projector::SetDefaults( void )
{
	stereo_seperation = 0.0f;
	infinite_projection = 0.0f;
	perspective_strength = 1.0f;
	unit_w_z = 1.0f;
	far_clip_z = 4096.0f;
	near_clip_z = ( 1.0f / 256.0f );
	viewport_width = 2.0f;
	viewport_height = 2.0f;
	viewport_center.x = viewport_center.y = 0.0f;
	viewport_center.z = 1.0f;
	origin_position.x = origin_position.y = origin_position.z = 0.0f;
}

void projector::SetIsometric( const float radiansX, const float radiansY )
{
	float delta = ( viewport_center.z - origin_position.z );
	perspective_strength = 0.0f;
	origin_position.x = ( viewport_center.x - ( tanf( radiansX ) * delta ) );
	origin_position.y = ( viewport_center.y - ( tanf( radiansY ) * delta ) );
}

void projector::SetOrthogonal( void )
{
	perspective_strength = 0.0f;
	origin_position.x = viewport_center.x;
	origin_position.y = viewport_center.y;
}

void projector::SetCalibrated( const float aspect, const float size, const float unit )
{
	ASSERT( fabsf( aspect ) >= consts::FLOAT_MIN_RCP );
	viewport_width = ( ( size * unit * aspect ) / sqrtf( ( aspect * aspect ) + 1.0f ) );
	viewport_height = ( viewport_width / aspect );
}

void projector::SetWindow( const float left, const float right, const float bottom, const float top, const float depth )
{
	viewport_width = ( right - left );
	viewport_height = ( top - bottom );
	viewport_center.x = ( ( right + left ) * 0.5f );
	viewport_center.y = ( ( top + bottom ) * 0.5f );
	viewport_center.z = depth;
}

void projector::SetWindow( const frect& window, const float depth )
{
	viewport_width = ( window.r - window.l );
	viewport_height = ( window.t - window.b );
	viewport_center.x = ( ( window.r + window.l ) * 0.5f );
	viewport_center.y = ( ( window.t + window.b ) * 0.5f );
	viewport_center.z = depth;
}

void projector::SetLeft( const float left )
{
	float right = ( viewport_center.x + ( viewport_width * 0.5f ) );
	viewport_width = ( right - left );
	viewport_center.x = ( ( right + left ) * 0.5f );
}

void projector::SetRight( const float right )
{
	float left = ( viewport_center.x - ( viewport_width * 0.5f ) );
	viewport_width = ( right - left );
	viewport_center.x = ( ( right + left ) * 0.5f );
}

void projector::SetBottom( const float bottom )
{
	float top = ( viewport_center.y + ( viewport_height * 0.5f ) );
	viewport_height = ( top - bottom );
	viewport_center.y = ( ( top + bottom ) * 0.5f );
}

void projector::SetTop( const float top )
{
	float bottom = ( viewport_center.y - ( viewport_height * 0.5f ) );
	viewport_height = ( top - bottom );
	viewport_center.y = ( ( top + bottom ) * 0.5f );
}

void projector::SetEyeDepth( const float depth )
{
	float dx = ( viewport_center.x - origin_position.x );
	float dy = ( viewport_center.y - origin_position.y );
	float dz = ( viewport_center.z - origin_position.z );
	ASSERT( fabsf( dz ) >= consts::FLOAT_MIN_RCP );
	float ratio = ( ( viewport_center.z - depth ) / dz );
	viewport_width *= ratio;
	viewport_height *= ratio;
	origin_position.x = ( viewport_center.x - ( dx * ratio ) );
	origin_position.y = ( viewport_center.y - ( dy * ratio ) );
	origin_position.z = depth;
}

void projector::SetViewDepth( const float depth )
{
	float dx = ( viewport_center.x - origin_position.x );
	float dy = ( viewport_center.y - origin_position.y );
	float dz = ( viewport_center.z - origin_position.z );
	ASSERT( fabsf( dz ) >= consts::FLOAT_MIN_RCP );
	float ratio = ( ( depth - origin_position.z ) / dz );
	viewport_width *= ratio;
	viewport_height *= ratio;
	viewport_center.x = ( origin_position.x + ( dx * ratio ) );
	viewport_center.y = ( origin_position.y + ( dy * ratio ) );
	viewport_center.z = depth;
}

void projector::SetNVidia( const float convergence, const float separation )
{	//	set separation and view distance to match NVidia settings
	viewport_center.z = convergence;
	stereo_seperation = ( separation * viewport_width );
}

void projector::SetVerticalFoV( const float fov )
{
	ASSERT( fabsf( viewport_height ) >= consts::FLOAT_MIN_RCP );
	viewport_width /= viewport_height;
	viewport_height = ( tanf( fov * 0.5f ) * ( viewport_center.z - origin_position.z ) * 2.0f );
	viewport_width *= viewport_height;
}

void projector::SetVerticalFoV( const float fov, const float aspect )
{
	viewport_height = ( tanf( fov * 0.5f ) * ( viewport_center.z - origin_position.z ) * 2.0f );
	viewport_width = ( viewport_height * aspect );
}

void projector::SetVerticalFoV( const float fov, const float width, const float height )
{
	viewport_width = width;
	viewport_height = height;
	origin_position.z = ( viewport_center.z - ( viewport_height / ( tanf( fov * 0.5f ) * 2.0f ) ) );
}

void projector::SetHorizontalFoV( const float fov )
{
	ASSERT( fabsf( viewport_width ) >= consts::FLOAT_MIN_RCP );
	viewport_height /= viewport_width;
	viewport_width = ( tanf( fov * 0.5f ) * ( viewport_center.z - origin_position.z ) * 2.0f );
	viewport_height *= viewport_width;
}

void projector::SetHorizontalFoV( const float fov, const float aspect )
{
	ASSERT( fabsf( aspect ) >= consts::FLOAT_MIN_RCP );
	viewport_width = ( tanf( fov * 0.5f ) * ( viewport_center.z - origin_position.z ) * 2.0f );
	viewport_height = ( viewport_width / aspect );
}

void projector::SetHorizontalFoV( const float fov, const float width, const float height )
{
	viewport_width = width;
	viewport_height = height;
	origin_position.z = ( viewport_center.z - ( viewport_width / ( ( tanf( fov * 0.5f ) * 2.0f ) + consts::FLOAT_MIN_NORM ) ) );
}

void projector::GetWindow( frect& window, float& depth ) const
{
	float x = ( viewport_width * 0.5f );
	float y = ( viewport_height * 0.5f );
	window.l = ( viewport_center.x - x );
	window.r = ( viewport_center.x + x );
	window.b = ( viewport_center.y - y );
	window.t = ( viewport_center.y + y );
	depth = viewport_center.z;
}

void projector::GetWindow( float& left,float& right, float& bottom, float& top, float& depth ) const
{
	float x = ( viewport_width * 0.5f );
	float y = ( viewport_height * 0.5f );
	left   = ( viewport_center.x - x );
	right  = ( viewport_center.x + x );
	bottom = ( viewport_center.y - y );
	top    = ( viewport_center.y + y );
	depth  = viewport_center.z;
}

float projector::GetLeft( void ) const
{
	return( viewport_center.x - ( viewport_width * 0.5f ) );
}

float projector::GetRight( void ) const
{
	return( viewport_center.x + ( viewport_width * 0.5f ) );
}

float projector::GetBottom( void ) const
{
	return( viewport_center.y - ( viewport_height * 0.5f ) );
}

float projector::GetTop( void ) const
{
	return( viewport_center.y + ( viewport_height * 0.5f ) );
}

float projector::GetVerticalFoV( void ) const
{
	return( atan2f( ( ( viewport_center.z - origin_position.z ) * 2.0f ), viewport_height ) * 2.0f );
}

float projector::GetHorizontalFoV( void ) const
{
	return( atan2f( ( ( viewport_center.z - origin_position.z ) * 2.0f ), viewport_width ) * 2.0f );
}

float projector::GetAspectRatio( void ) const
{
	ASSERT( fabsf( viewport_height ) >= consts::FLOAT_MIN_RCP );
	return( viewport_width / viewport_height );
}

void projector::GetMatrix( mat44& m ) const
{
//  assert on any possible divide by zero conditions:

    ASSERT( fabsf( viewport_center.z - origin_position.z ) >= consts::FLOAT_MIN_RCP );
    ASSERT( fabsf( unit_w_z - origin_position.z ) >= consts::FLOAT_MIN_RCP );
    ASSERT( fabsf( far_clip_z - near_clip_z ) >= consts::FLOAT_MIN_RCP );
    ASSERT( fabsf( viewport_width ) >= consts::FLOAT_MIN_RCP );
    ASSERT( fabsf( viewport_height ) >= consts::FLOAT_MIN_RCP );

//  alias member data to make formulas more readable:
    //const float ss = ( stereo_seperation * 0.5f );  //  not used in the non-stereo calculation
    const float ip = infinite_projection;
    const float ps = perspective_strength;
    const float uw = unit_w_z;
    const float fc = far_clip_z;
    const float nc = near_clip_z;
    const float vw = viewport_width;
    const float vh = viewport_height;
    const float vx = viewport_center.x;
    const float vy = viewport_center.y;
    const float vz = viewport_center.z;
    const float ox = origin_position.x;
    const float oy = origin_position.y;
    const float oz = origin_position.z;

//  calculate w plane elements:
    float sw = ( 1.0f / ( uw - oz ) );
    float wz = ( ps * sw );
    float ww = ( ( ( uw * ( 1.0f - ps ) ) - oz ) * sw );

//  calculate z plane elements:
    float sz = ( ( 1.0f - ip ) / ( fc - nc ) );
    float nf = ( nc + fc );
    float nz = ( nc * wz * 2.0f );
    float zz = ( ( wz * ip ) + ( ( ( nf * wz ) + ww + ww ) * sz ) );
    float zw = ( 0.0f - ( ( ( nz + ww ) * ip ) + ( ( ( nf * ww ) + ( fc * nz ) ) * sz ) ) );

//  calculate common x and y plane factors:
    float xy = ( ( ( vz * wz ) + ww ) * 2.0f );
    float rv = ( 1.0f / ( vz - oz ) );

//  calculate y plane elements:
    float yy = ( xy / vh );
    float sy = ( yy * rv );
    float yz = ( ( oy - vy ) * sy );
    float yw = ( ( ( oz * vy ) - ( vz * oy ) ) * sy );

//  calculate x plane elements:
    float xx = ( xy / vw );
    float sx = ( xx * rv );
    float xz = ( ( ox - vx ) * sx );
    float xw = ( ( ( oz * vx ) - ( vz * ox ) ) * sx );

//  write calculated elements to the matrix:
    m.x.x = xx;
    m.x.y = 0.0f;
    m.x.z = xz;
    m.x.w = xw;
    m.y.x = 0.0f;
    m.y.y = yy;
    m.y.z = yz;
    m.y.w = yw;
    m.z.x = 0.0f;
    m.z.y = 0.0f;
    m.z.z = zz;
    m.z.w = zw;
    m.w.x = 0.0f;
    m.w.y = 0.0f;
    m.w.z = wz;
    m.w.w = ww;
}

void projector::GetLeftMatrix( mat44& ml ) const
{
//  assert on any possible divide by zero conditions:

    ASSERT( fabsf( viewport_center.z - origin_position.z ) >= consts::FLOAT_MIN_RCP );
    ASSERT( fabsf( unit_w_z - origin_position.z ) >= consts::FLOAT_MIN_RCP );
    ASSERT( fabsf( far_clip_z - near_clip_z ) >= consts::FLOAT_MIN_RCP );
    ASSERT( fabsf( viewport_width ) >= consts::FLOAT_MIN_RCP );
    ASSERT( fabsf( viewport_height ) >= consts::FLOAT_MIN_RCP );

//  alias member data to make formulas more readable:
    const float ss = ( stereo_seperation * 0.5f );
    const float ip = infinite_projection;
    const float ps = perspective_strength;
    const float uw = unit_w_z;
    const float fc = far_clip_z;
    const float nc = near_clip_z;
    const float vw = viewport_width;
    const float vh = viewport_height;
    const float vx = viewport_center.x;
    const float vy = viewport_center.y;
    const float vz = viewport_center.z;
    const float ox = origin_position.x;
    const float oy = origin_position.y;
    const float oz = origin_position.z;

//  calculate w plane elements:
    float sw = ( 1.0f / ( uw - oz ) );
    float wz = ( ps * sw );
    float ww = ( ( ( uw * ( 1.0f - ps ) ) - oz ) * sw );

//  calculate z plane elements:
    float sz = ( ( 1.0f - ip ) / ( fc - nc ) );
    float nf = ( nc + fc );
    float nz = ( nc * wz * 2.0f );
    float zz = ( ( wz * ip ) + ( ( ( nf * wz ) + ww + ww ) * sz ) );
    float zw = ( 0.0f - ( ( ( nz + ww ) * ip ) + ( ( ( nf * ww ) + ( fc * nz ) ) * sz ) ) );

//  calculate common x and y plane factors:
    float xy = ( ( ( vz * wz ) + ww ) * 2.0f );
    float rv = ( 1.0f / ( vz - oz ) );

//  calculate y plane elements:
    float yy = ( xy / vh );
    float sy = ( yy * rv );
    float yz = ( ( oy - vy ) * sy );
    float yw = ( ( ( oz * vy ) - ( vz * oy ) ) * sy );

//  calculate x plane elements:
    float lx = ( ox - ss );
    float xx = ( xy / vw );
    float sx = ( xx * rv );
    float lz = ( ( lx - vx ) * sx );
    float lw = ( ( ( oz * vx ) - ( vz * lx ) ) * sx );

//  write calculated elements to the matrix:
    ml.x.x = xx;
    ml.x.y = 0.0f;
    ml.x.z = lz;
    ml.x.w = lw;
    ml.y.x = 0.0f;
    ml.y.y = yy;
    ml.y.z = yz;
    ml.y.w = yw;
    ml.z.x = 0.0f;
    ml.z.y = 0.0f;
    ml.z.z = zz;
    ml.z.w = zw;
    ml.w.x = 0.0f;
    ml.w.y = 0.0f;
    ml.w.z = wz;
    ml.w.w = ww;
}

void projector::GetRightMatrix( mat44& mr ) const
{
//  assert on any possible divide by zero conditions:

    ASSERT( fabsf( viewport_center.z - origin_position.z ) >= consts::FLOAT_MIN_RCP );
    ASSERT( fabsf( unit_w_z - origin_position.z ) >= consts::FLOAT_MIN_RCP );
    ASSERT( fabsf( far_clip_z - near_clip_z ) >= consts::FLOAT_MIN_RCP );
    ASSERT( fabsf( viewport_width ) >= consts::FLOAT_MIN_RCP );
    ASSERT( fabsf( viewport_height ) >= consts::FLOAT_MIN_RCP );

//  alias member data to make formulas more readable:
    const float ss = ( stereo_seperation * 0.5f );
    const float ip = infinite_projection;
    const float ps = perspective_strength;
    const float uw = unit_w_z;
    const float fc = far_clip_z;
    const float nc = near_clip_z;
    const float vw = viewport_width;
    const float vh = viewport_height;
    const float vx = viewport_center.x;
    const float vy = viewport_center.y;
    const float vz = viewport_center.z;
    const float ox = origin_position.x;
    const float oy = origin_position.y;
    const float oz = origin_position.z;

//  calculate w plane elements:
    float sw = ( 1.0f / ( uw - oz ) );
    float wz = ( ps * sw );
    float ww = ( ( ( uw * ( 1.0f - ps ) ) - oz ) * sw );

//  calculate z plane elements:
    float sz = ( ( 1.0f - ip ) / ( fc - nc ) );
    float nf = ( nc + fc );
    float nz = ( nc * wz * 2.0f );
    float zz = ( ( wz * ip ) + ( ( ( nf * wz ) + ww + ww ) * sz ) );
    float zw = ( 0.0f - ( ( ( nz + ww ) * ip ) + ( ( ( nf * ww ) + ( fc * nz ) ) * sz ) ) );

//  calculate common x and y plane factors:
    float xy = ( ( ( vz * wz ) + ww ) * 2.0f );
    float rv = ( 1.0f / ( vz - oz ) );

//  calculate y plane elements:
    float yy = ( xy / vh );
    float sy = ( yy * rv );
    float yz = ( ( oy - vy ) * sy );
    float yw = ( ( ( oz * vy ) - ( vz * oy ) ) * sy );

//  calculate x plane elements:
    float rx = ( ox + ss );
    float xx = ( xy / vw );
    float sx = ( xx * rv );
    float rz = ( ( rx - vx ) * sx );
    float rw = ( ( ( oz * vx ) - ( vz * rx ) ) * sx );

//  write calculated elements to the matrix:
    mr.x.x = xx;
    mr.x.y = 0.0f;
    mr.x.z = rz;
    mr.x.w = rw;
    mr.y.x = 0.0f;
    mr.y.y = yy;
    mr.y.z = yz;
    mr.y.w = yw;
    mr.z.x = 0.0f;
    mr.z.y = 0.0f;
    mr.z.z = zz;
    mr.z.w = zw;
    mr.w.x = 0.0f;
    mr.w.y = 0.0f;
    mr.w.z = wz;
    mr.w.w = ww;
}

void projector::GetStereoMatrices( mat44& ml, mat44& mr ) const
{
//  assert on any possible divide by zero conditions:

    ASSERT( fabsf( viewport_center.z - origin_position.z ) >= consts::FLOAT_MIN_RCP );
    ASSERT( fabsf( unit_w_z - origin_position.z ) >= consts::FLOAT_MIN_RCP );
    ASSERT( fabsf( far_clip_z - near_clip_z ) >= consts::FLOAT_MIN_RCP );
    ASSERT( fabsf( viewport_width ) >= consts::FLOAT_MIN_RCP );
    ASSERT( fabsf( viewport_height ) >= consts::FLOAT_MIN_RCP );

//  alias member data to make formulas more readable:
    const float ss = ( stereo_seperation * 0.5f );
    const float ip = infinite_projection;
    const float ps = perspective_strength;
    const float uw = unit_w_z;
    const float fc = far_clip_z;
    const float nc = near_clip_z;
    const float vw = viewport_width;
    const float vh = viewport_height;
    const float vx = viewport_center.x;
    const float vy = viewport_center.y;
    const float vz = viewport_center.z;
    const float ox = origin_position.x;
    const float oy = origin_position.y;
    const float oz = origin_position.z;

//  calculate w plane elements:
    float sw = ( 1.0f / ( uw - oz ) );
    float wz = ( ps * sw );
    float ww = ( ( ( uw * ( 1.0f - ps ) ) - oz ) * sw );

//  calculate z plane elements:
    float sz = ( ( 1.0f - ip ) / ( fc - nc ) );
    float nf = ( nc + fc );
    float nz = ( nc * wz * 2.0f );
    float zz = ( ( wz * ip ) + ( ( ( nf * wz ) + ww + ww ) * sz ) );
    float zw = ( 0.0f - ( ( ( nz + ww ) * ip ) + ( ( ( nf * ww ) + ( fc * nz ) ) * sz ) ) );

//  calculate common x and y plane factors:
    float xy = ( ( ( vz * wz ) + ww ) * 2.0f );
    float rv = ( 1.0f / ( vz - oz ) );

//  calculate y plane elements:
    float yy = ( xy / vh );
    float sy = ( yy * rv );
    float yz = ( ( oy - vy ) * sy );
    float yw = ( ( ( oz * vy ) - ( vz * oy ) ) * sy );

//  calculate x plane elements:
    float lx = ( ox - ss );
    float rx = ( ox + ss );
    float xx = ( xy / vw );
    float sx = ( xx * rv );
    float lz = ( ( lx - vx ) * sx );
    float rz = ( ( rx - vx ) * sx );
    float lw = ( ( ( oz * vx ) - ( vz * lx ) ) * sx );
    float rw = ( ( ( oz * vx ) - ( vz * rx ) ) * sx );

//  write calculated elements to the matrix:
    ml.x.x = mr.x.x = xx;
    ml.x.y = mr.x.y = 0.0f;
    ml.x.z = lz;
    mr.x.z = rz;
    ml.x.w = lw;
    mr.x.w = rw;
    ml.y.x = mr.y.x = 0.0f;
    ml.y.y = mr.y.y = yy;
    ml.y.z = mr.y.z = yz;
    ml.y.w = mr.y.w = yw;
    ml.z.x = mr.z.x = 0.0f;
    ml.z.y = mr.z.y = 0.0f;
    ml.z.z = mr.z.z = zz;
    ml.z.w = mr.z.w = zw;
    ml.w.x = mr.w.x = 0.0f;
    ml.w.y = mr.w.y = 0.0f;
    ml.w.z = mr.w.z = wz;
    ml.w.w = mr.w.w = ww;
}

void projector::GetOriginMatrix( mat44& m ) const
{
//  assert on any possible divide by zero conditions:

    ASSERT( fabsf( viewport_center.z - origin_position.z ) >= consts::FLOAT_MIN_RCP );
    ASSERT( fabsf( unit_w_z - origin_position.z ) >= consts::FLOAT_MIN_RCP );
    ASSERT( fabsf( far_clip_z - near_clip_z ) >= consts::FLOAT_MIN_RCP );
    ASSERT( fabsf( viewport_width ) >= consts::FLOAT_MIN_RCP );
    ASSERT( fabsf( viewport_height ) >= consts::FLOAT_MIN_RCP );

//  alias member data to make formulas more readable:
    //const float ss = ( stereo_seperation * 0.5f );  //  not used in the non-stereo calculation
    const float ip = infinite_projection;
    const float ps = perspective_strength;
    const float uw = unit_w_z;
    const float fc = far_clip_z;
    const float nc = near_clip_z;
    const float vw = viewport_width;
    const float vh = viewport_height;
    const float vx = viewport_center.x;
    const float vy = viewport_center.y;
    const float vz = viewport_center.z;
    const float ox = origin_position.x;
    const float oy = origin_position.y;
    const float oz = origin_position.z;

//  calculate w plane elements:
    float sw = ( 1.0f / ( uw - oz ) );
    float wz = ( ps * sw );
    float ww = ( 1.0f - ps );

//  calculate z plane elements:
    float df = ( fc - oz );
    float dn = ( nc - oz );
    float sz = ( ( 1.0f - ip ) / ( df - dn ) );
    float nf = ( dn + df );
    float nz = ( dn * wz * 2.0f );
    float zz = ( ( wz * ip ) + ( ( ( nf * wz ) + ww + ww ) * sz ) );
    float zw = ( 0.0f - ( ( ( nz + ww ) * ip ) + ( ( ( nf * ww ) + ( df * nz ) ) * sz ) ) );

//  calculate common x and y plane factors:
    float xy = ( ( ( ( vz - oz ) * wz ) + ww ) * 2.0f );
    float rv = ( 1.0f / ( vz - oz ) );

//  calculate y plane elements:
    float yy = ( xy / vh );
    float yz = ( ( oy - vy ) * yy * rv );

//  calculate x plane elements:
    float xx = ( xy / vw );
    float xz = ( ( ox - vx ) * xx * rv );

//  write calculated elements to the matrix:
    m.x.x = xx;
    m.x.y = 0.0f;
    m.x.z = xz;
    m.x.w = 0.0f;
    m.y.x = 0.0f;
    m.y.y = yy;
    m.y.z = yz;
    m.y.w = 0.0f;
    m.z.x = 0.0f;
    m.z.y = 0.0f;
    m.z.z = zz;
    m.z.w = zw;
    m.w.x = 0.0f;
    m.w.y = 0.0f;
    m.w.z = wz;
    m.w.w = ww;
}

void projector::GetOriginLeftMatrix( mat44& ml ) const
{
//  assert on any possible divide by zero conditions:

    ASSERT( fabsf( viewport_center.z - origin_position.z ) >= consts::FLOAT_MIN_RCP );
    ASSERT( fabsf( unit_w_z - origin_position.z ) >= consts::FLOAT_MIN_RCP );
    ASSERT( fabsf( far_clip_z - near_clip_z ) >= consts::FLOAT_MIN_RCP );
    ASSERT( fabsf( viewport_width ) >= consts::FLOAT_MIN_RCP );
    ASSERT( fabsf( viewport_height ) >= consts::FLOAT_MIN_RCP );

//  alias member data to make formulas more readable:
    const float ss = ( stereo_seperation * 0.5f );
    const float ip = infinite_projection;
    const float ps = perspective_strength;
    const float uw = unit_w_z;
    const float fc = far_clip_z;
    const float nc = near_clip_z;
    const float vw = viewport_width;
    const float vh = viewport_height;
    const float vx = viewport_center.x;
    const float vy = viewport_center.y;
    const float vz = viewport_center.z;
    const float ox = origin_position.x;
    const float oy = origin_position.y;
    const float oz = origin_position.z;

//  calculate w plane elements:
    float sw = ( 1.0f / ( uw - oz ) );
    float wz = ( ps * sw );
    float ww = ( 1.0f - ps );

//  calculate z plane elements:
    float df = ( fc - oz );
    float dn = ( nc - oz );
    float sz = ( ( 1.0f - ip ) / ( df - dn ) );
    float nf = ( dn + df );
    float nz = ( dn * wz * 2.0f );
    float zz = ( ( wz * ip ) + ( ( ( nf * wz ) + ww + ww ) * sz ) );
    float zw = ( 0.0f - ( ( ( nz + ww ) * ip ) + ( ( ( nf * ww ) + ( df * nz ) ) * sz ) ) );

//  calculate common x and y plane factors:
    float xy = ( ( ( ( vz - oz ) * wz ) + ww ) * 2.0f );
    float rv = ( 1.0f / ( vz - oz ) );

//  calculate y plane elements:
    float yy = ( xy / vh );
    float yz = ( ( oy - vy ) * yy * rv );

//  calculate x plane elements:
    float lx = ( ox - ss );
    float xx = ( xy / vw );
    float lz = ( ( lx - vx ) * xx * rv );

//  write calculated elements to the matrix:
    ml.x.x = xx;
    ml.x.y = 0.0f;
    ml.x.z = lz;
    ml.x.w = 0.0f;
    ml.y.x = 0.0f;
    ml.y.y = yy;
    ml.y.z = yz;
    ml.y.w = 0.0f;
    ml.z.x = 0.0f;
    ml.z.y = 0.0f;
    ml.z.z = zz;
    ml.z.w = zw;
    ml.w.x = 0.0f;
    ml.w.y = 0.0f;
    ml.w.z = wz;
    ml.w.w = ww;
}

void projector::GetOriginRightMatrix( mat44& mr ) const
{
//  assert on any possible divide by zero conditions:

    ASSERT( fabsf( viewport_center.z - origin_position.z ) >= consts::FLOAT_MIN_RCP );
    ASSERT( fabsf( unit_w_z - origin_position.z ) >= consts::FLOAT_MIN_RCP );
    ASSERT( fabsf( far_clip_z - near_clip_z ) >= consts::FLOAT_MIN_RCP );
    ASSERT( fabsf( viewport_width ) >= consts::FLOAT_MIN_RCP );
    ASSERT( fabsf( viewport_height ) >= consts::FLOAT_MIN_RCP );

//  alias member data to make formulas more readable:
    const float ss = ( stereo_seperation * 0.5f );
    const float ip = infinite_projection;
    const float ps = perspective_strength;
    const float uw = unit_w_z;
    const float fc = far_clip_z;
    const float nc = near_clip_z;
    const float vw = viewport_width;
    const float vh = viewport_height;
    const float vx = viewport_center.x;
    const float vy = viewport_center.y;
    const float vz = viewport_center.z;
    const float ox = origin_position.x;
    const float oy = origin_position.y;
    const float oz = origin_position.z;

//  calculate w plane elements:
    float sw = ( 1.0f / ( uw - oz ) );
    float wz = ( ps * sw );
    float ww = ( 1.0f - ps );

//  calculate z plane elements:
    float df = ( fc - oz );
    float dn = ( nc - oz );
    float sz = ( ( 1.0f - ip ) / ( df - dn ) );
    float nf = ( dn + df );
    float nz = ( dn * wz * 2.0f );
    float zz = ( ( wz * ip ) + ( ( ( nf * wz ) + ww + ww ) * sz ) );
    float zw = ( 0.0f - ( ( ( nz + ww ) * ip ) + ( ( ( nf * ww ) + ( df * nz ) ) * sz ) ) );

//  calculate common x and y plane factors:
    float xy = ( ( ( ( vz - oz ) * wz ) + ww ) * 2.0f );
    float rv = ( 1.0f / ( vz - oz ) );

//  calculate y plane elements:
    float yy = ( xy / vh );
    float yz = ( ( oy - vy ) * yy * rv );

//  calculate x plane elements:
    float rx = ( ox + ss );
    float xx = ( xy / vw );
    float rz = ( ( rx - vx ) * xx * rv );

//  write calculated elements to the matrix:
    mr.x.x = xx;
    mr.x.y = 0.0f;
    mr.x.z = rz;
    mr.x.w = 0.0f;
    mr.y.x = 0.0f;
    mr.y.y = yy;
    mr.y.z = yz;
    mr.y.w = 0.0f;
    mr.z.x = 0.0f;
    mr.z.y = 0.0f;
    mr.z.z = zz;
    mr.z.w = zw;
    mr.w.x = 0.0f;
    mr.w.y = 0.0f;
    mr.w.z = wz;
    mr.w.w = ww;
}

void projector::GetOriginStereoMatrices( mat44& ml, mat44& mr ) const
{
//  assert on any possible divide by zero conditions:

    ASSERT( fabsf( viewport_center.z - origin_position.z ) >= consts::FLOAT_MIN_RCP );
    ASSERT( fabsf( unit_w_z - origin_position.z ) >= consts::FLOAT_MIN_RCP );
    ASSERT( fabsf( far_clip_z - near_clip_z ) >= consts::FLOAT_MIN_RCP );
    ASSERT( fabsf( viewport_width ) >= consts::FLOAT_MIN_RCP );
    ASSERT( fabsf( viewport_height ) >= consts::FLOAT_MIN_RCP );

//  alias member data to make formulas more readable:
    const float ss = ( stereo_seperation * 0.5f );
    const float ip = infinite_projection;
    const float ps = perspective_strength;
    const float uw = unit_w_z;
    const float fc = far_clip_z;
    const float nc = near_clip_z;
    const float vw = viewport_width;
    const float vh = viewport_height;
    const float vx = viewport_center.x;
    const float vy = viewport_center.y;
    const float vz = viewport_center.z;
    const float ox = origin_position.x;
    const float oy = origin_position.y;
    const float oz = origin_position.z;

//  calculate w plane elements:
    float sw = ( 1.0f / ( uw - oz ) );
    float wz = ( ps * sw );
    float ww = ( 1.0f - ps );

//  calculate z plane elements:
    float df = ( fc - oz );
    float dn = ( nc - oz );
    float sz = ( ( 1.0f - ip ) / ( df - dn ) );
    float nf = ( dn + df );
    float nz = ( dn * wz * 2.0f );
    float zz = ( ( wz * ip ) + ( ( ( nf * wz ) + ww + ww ) * sz ) );
    float zw = ( 0.0f - ( ( ( nz + ww ) * ip ) + ( ( ( nf * ww ) + ( df * nz ) ) * sz ) ) );

//  calculate common x and y plane factors:
    float xy = ( ( ( ( vz - oz ) * wz ) + ww ) * 2.0f );
    float rv = ( 1.0f / ( vz - oz ) );

//  calculate y plane elements:
    float yy = ( xy / vh );
    float yz = ( ( oy - vy ) * yy * rv );

//  calculate x plane elements:
    float lx = ( ox - ss );
    float rx = ( ox + ss );
    float xx = ( xy / vw );
    float sx = ( xx * rv );
    float lz = ( ( lx - vx ) * sx );
    float rz = ( ( rx - vx ) * sx );

//  write calculated elements to the matrix:
    ml.x.x = mr.x.x = xx;
    ml.x.y = mr.x.y = 0.0f;
    ml.x.z = lz;
    mr.x.z = rz;
    ml.x.w = mr.x.w = 0.0f;
    ml.y.x = mr.y.x = 0.0f;
    ml.y.y = mr.y.y = yy;
    ml.y.z = mr.y.z = yz;
    ml.y.w = mr.y.w = 0.0f;
    ml.z.x = mr.z.x = 0.0f;
    ml.z.y = mr.z.y = 0.0f;
    ml.z.z = mr.z.z = zz;
    ml.z.w = mr.z.w = zw;
    ml.w.x = mr.w.x = 0.0f;
    ml.w.y = mr.w.y = 0.0f;
    ml.w.z = mr.w.z = wz;
    ml.w.w = mr.w.w = ww;
}

void projector::GetOriginVector( vec3& v ) const
{
    v.x = origin_position.x;
    v.y = origin_position.y;
    v.z = origin_position.z;
}

void projector::GetOriginLeftVector( vec3& vl ) const
{
    const float ss = ( stereo_seperation * 0.5f );
    vl.x = ( origin_position.x - ss );
    vl.y = origin_position.y;
    vl.z = origin_position.z;
}

void projector::GetOriginRightVector( vec3& vr ) const
{
    const float ss = ( stereo_seperation * 0.5f );
    vr.x = ( origin_position.x + ss );
    vr.y = origin_position.y;
    vr.z = origin_position.z;
}

void projector::GetOriginStereoVectors( vec3& vl, vec3& vr ) const
{
    const float ss = ( stereo_seperation * 0.5f );
    vl.x = ( origin_position.x - ss );
    vr.x = ( origin_position.x + ss );
    vl.y = vr.y = origin_position.y;
    vl.z = vr.z = origin_position.z;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end maths namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

};	//	namespace maths

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
