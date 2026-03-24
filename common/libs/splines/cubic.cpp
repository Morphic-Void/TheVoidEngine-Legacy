
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   cubic.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	Generalised cubic spline classes.
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

#include "cubic.h"
#include "libs/maths/vec3.h"
#include "libs/maths/vec4.h"
#include "libs/maths/consts.h"
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
////    cubic vector spline structure function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int CubicVectorSpline::findSpanIndex( const float t_in, float& t_out ) const
{
    t_out = 0;
    if( span_count && span_array )
    {
        int index = 0;
        int limit = ( span_count - 1 );
        float lower = span_array[ 0 ]->lower;
        float upper = span_array[ limit ]->upper;
        float s = ( ( t_in * ( upper - lower ) ) + lower );
        while( index < limit )
        {
            int check = ( ( index + limit ) >> 1 );
            if( s >= span_array[ check ]->upper )
            {
                index = ( check + 1 );
            }
            else
            {
                limit = check;
            }
        }
        Span& span = *span_array[ index ];
        t_out = ( ( s - span.lower ) * span.scale );
        return( index );
    }
    return( -1 );
}

void CubicVectorSpline::updatePolynomial( Span& span ) const
{
	switch( form )
	{
	case( ECubicBezier ):
		{
			span.polynomial.SetMul( consts::MAT44::BEZIER, span.control );
			break;
		}
    case( ECubicBSpline ):
		{
			span.polynomial.SetMul( consts::MAT44::B_SPLINE, span.control );
			break;
		}
    case( ECubicCatmull ):
		{
			span.polynomial.SetMul( consts::MAT44::CATMULL_ROM, span.control );
			break;
		}
    case( ECubicHermite ):
		{
			span.polynomial.SetMul( consts::MAT44::HERMITE, span.control );
			break;
		}
    case( ECubicHBezier ):
		{
			span.polynomial.SetMul( consts::MAT44::HBEZIER, span.control );
			break;
		}
    case( ECubicNonUniformBSpline ):
		{
			mat44 basis;
			basis.SetBSpline( span.knots );
			span.polynomial.SetMul( basis, span.control );
			break;
		}
    case( ECubicNonUniformCatmull ):
		{
			mat44 basis;
			vec4 d1, d2, d3;
			d1.SetSub( span.control.y, span.control.x );
			d2.SetSub( span.control.z, span.control.y );
			d3.SetSub( span.control.w, span.control.z );
			basis.SetCatmull( d1.Length(), d2.Length(), d3.Length() );
			span.polynomial.SetMul( basis, span.control );
			break;
		}
	default:
		{
			break;
		}
	}
}

void CubicVectorSpline::draw( ISplineDrawLineStreamOut& stream_out, const uint divisions ) const
{
    if( span_count && span_array )
    {
        uint count = ( span_count * ( divisions + 1 ) );
        float dt = ( 1.0f / count );
        float t = dt;
		vec4 start, end;
        if( rational )
        {
			span_array[ 0 ]->polynomial.EvaluateRationalPolynomial( 0, end );
			end.t_vec3().Div( end.w );
            for( uint index = 0; index < count; ++index )
            {
                float s = 0;
                start = end;
				span_array[ findSpanIndex( t, s ) ]->polynomial.EvaluateRationalPolynomial( s, end );
				end.t_vec3().Div( end.w );
				stream_out.DrawLine( start.t_vec3(), end.t_vec3() );
                t += dt;
            }
        }
        else
        {
			span_array[ 0 ]->polynomial.EvaluatePolynomial( 0, end );
            for( uint index = 0; index < count; ++index )
            {
                float s = 0;
                start = end;
				span_array[ findSpanIndex( t, s ) ]->polynomial.EvaluatePolynomial( s, end );
				stream_out.DrawLine( start.t_vec3(), end.t_vec3() );
                t += dt;
            }
        }
    }
}

void CubicVectorSpline::drawTangents( ISplineDrawLineStreamOut& stream_out, const float scale ) const
{
    if( span_count && span_array )
    {
        vec4 start, end;
        if( rational )
        {
			for( uint index = 0; index < span_count; ++index )
			{
				span_array[ index ]->polynomial.EvaluateRationalPolynomial( 0.5f, start, end );
				start.t_vec3().Div( start.w );
				end.t_vec3().Div( end.w );
				end.t_vec3().Mul( scale );
				start.t_vec3().Sub( end.t_vec3() );
				end.t_vec3().Mul( 2 );
				end.t_vec3().Add( start.t_vec3() );
				stream_out.DrawLine( start.t_vec3(), end.t_vec3() );
			}
        }
        else
        {
			for( uint index = 0; index < span_count; ++index )
			{
				span_array[ index ]->polynomial.EvaluatePolynomial( 0.5f, start, end );
				end.t_vec3().Mul( scale );
				start.t_vec3().Sub( end.t_vec3() );
				end.t_vec3().Mul( 2 );
				end.t_vec3().Add( start.t_vec3() );
				stream_out.DrawLine( start.t_vec3(), end.t_vec3() );
			}
        }
    }
}

void CubicVectorSpline::drawControlHull( ISplineDrawLineStreamOut& stream_out ) const
{
	if( span_count && span_array )
    {
		if( rational )
		{
			vec3 start, end;
			switch( form )
			{
			case( ECubicBezier ):
				{
					{
						const mat44& control = span_array[ 0 ]->control;
						end.SetDiv( control.x.t_vec3(), control.x.w );
					}
					for( uint index = 0; index < span_count; ++index )
					{
						const mat44& control = span_array[ index ]->control;
						start = end;
						end.SetDiv( control.y.t_vec3(), control.y.w );
						stream_out.DrawLine( start, end );
						start = end;
						end.SetDiv( control.z.t_vec3(), control.z.w );
						stream_out.DrawLine( start, end );
						start = end;
						end.SetDiv( control.w.t_vec3(), control.w.w );
						stream_out.DrawLine( start, end );
					}
					break;
				}
			case( ECubicHermite ):
			case( ECubicHBezier ):
				{
					{
						const mat44& control = span_array[ 0 ]->control;
						end.SetDiv( control.y.t_vec3(), control.y.w );
					}
					for( uint index = 0; index < span_count; ++index )
					{
						const mat44& control = span_array[ index ]->control;
						start = end;
						end.SetDiv( control.z.t_vec3(), control.z.w );
						stream_out.DrawLine( start, end );
					}
					break;
				}
			default:
				{
					if( looping )
					{
						const mat44& control = span_array[ 0 ]->control;
						end.SetDiv( control.z.t_vec3(), control.z.w );
					}
					else
					{   //  only draw the first 2 hull segments if the spline is not a closed loop
						const mat44& control = span_array[ 0 ]->control;
						start.SetDiv( control.x.t_vec3(), control.x.w );
						end.SetDiv( control.y.t_vec3(), control.y.w );
						stream_out.DrawLine( start, end );
						start = end;
						end.SetDiv( control.z.t_vec3(), control.z.w );
						stream_out.DrawLine( start, end );
					}
					for( uint index = 0; index < span_count; ++index )
					{
						const mat44& control = span_array[ index ]->control;
						start = end;
						end.SetDiv( control.w.t_vec3(), control.w.w );
						stream_out.DrawLine( start, end );
					}
					break;
				}
			}
		}
		else
		{
			switch( form )
			{
				case( ECubicBezier ):
				{
					for( uint index = 0; index < span_count; ++index )
					{
						const mat44& control = span_array[ index ]->control;
						stream_out.DrawLine( control.x.t_vec3(), control.y.t_vec3() );
						stream_out.DrawLine( control.y.t_vec3(), control.z.t_vec3() );
						stream_out.DrawLine( control.z.t_vec3(), control.w.t_vec3() );
					}
					break;
				}
			case( ECubicHermite ):
			case( ECubicHBezier ):
				{
					for( uint index = 0; index < span_count; ++index )
					{
						const mat44& control = span_array[ index ]->control;
						stream_out.DrawLine( control.y.t_vec3(), control.z.t_vec3() );
					}
					break;
				}
			default:
				{
					if( !looping )
					{   //  only draw the first 2 hull segments if the spline is not a closed loop
						const mat44& control = span_array[ 0 ]->control;
						stream_out.DrawLine( control.x.t_vec3(), control.y.t_vec3() );
						stream_out.DrawLine( control.y.t_vec3(), control.z.t_vec3() );
					}
					for( uint index = 0; index < span_count; ++index )
					{
						const mat44& control = span_array[ index ]->control;
						stream_out.DrawLine( control.z.t_vec3(), control.w.t_vec3() );
					}
					break;
				}
			}
		}
	}
}

void CubicVectorSpline::drawControlPoints( ISplineDrawPointStreamOut& stream_out ) const
{
    if( span_count && span_array )
    {
		if( rational )
		{
			vec3 point;
			switch( form )
			{
			case( ECubicBezier ):
				{
					if( !looping )
					{   //  only draw the first control point if the spline is not a closed loop
						const mat44& control = span_array[ 0 ]->control;
						point.SetDiv( control.x.t_vec3(), control.x.w );
						stream_out.DrawPoint( point );
					}
					for( uint index = 0; index < span_count; ++index )
					{
						const mat44& control = span_array[ index ]->control;
						point.SetDiv( control.y.t_vec3(), control.y.w );
						stream_out.DrawPoint( point );
						point.SetDiv( control.z.t_vec3(), control.z.w );
						stream_out.DrawPoint( point );
						point.SetDiv( control.w.t_vec3(), control.w.w );
						stream_out.DrawPoint( point );
					}
					break;
				}
			case( ECubicHermite ):
			case( ECubicHBezier ):
				{
					if( !looping )
					{   //  only draw the first control point if the spline is not a closed loop
						const mat44& control = span_array[ 0 ]->control;
						point.SetDiv( control.y.t_vec3(), control.y.w );
						stream_out.DrawPoint( point );
					}
					for( uint index = 0; index < span_count; ++index )
					{
						const mat44& control = span_array[ index ]->control;
						point.SetDiv( control.z.t_vec3(), control.z.w );
						stream_out.DrawPoint( point );
					}
					break;
				}
			default:
				{
					if( !looping )
					{   //  only draw the first 3 control points if the spline is not a closed loop
						const mat44& control = span_array[ 0 ]->control;
						point.SetDiv( control.x.t_vec3(), control.x.w );
						stream_out.DrawPoint( point );
						point.SetDiv( control.y.t_vec3(), control.y.w );
						stream_out.DrawPoint( point );
						point.SetDiv( control.z.t_vec3(), control.z.w );
						stream_out.DrawPoint( point );
					}
					for( uint index = 0; index < span_count; ++index )
					{
						const mat44& control = span_array[ index ]->control;
						point.SetDiv( control.w.t_vec3(), control.w.w );
						stream_out.DrawPoint( point );
					}
					break;
				}
			}
		}
		else
		{
			switch( form )
			{
				case( ECubicBezier ):
				{
					if( !looping )
					{   //  only draw the first control point if the spline is not a closed loop
						const mat44& control = span_array[ 0 ]->control;
						stream_out.DrawPoint( control.x.t_vec3() );
					}
					for( uint index = 0; index < span_count; ++index )
					{
						const mat44& control = span_array[ index ]->control;
						stream_out.DrawPoint( control.y.t_vec3() );
						stream_out.DrawPoint( control.z.t_vec3() );
						stream_out.DrawPoint( control.w.t_vec3() );
					}
					break;
				}
			case( ECubicHermite ):
			case( ECubicHBezier ):
				{
					if( !looping )
					{   //  only draw the first control point if the spline is not a closed loop
						const mat44& control = span_array[ 0 ]->control;
						stream_out.DrawPoint( control.y.t_vec3() );
					}
					for( uint index = 0; index < span_count; ++index )
					{
						const mat44& control = span_array[ index ]->control;
						stream_out.DrawPoint( control.z.t_vec3() );
					}
					break;
				}
			default:
				{
					if( !looping )
					{   //  only draw the first 3 control points if the spline is not a closed loop
						const mat44& control = span_array[ 0 ]->control;
						stream_out.DrawPoint( control.x.t_vec3() );
						stream_out.DrawPoint( control.y.t_vec3() );
						stream_out.DrawPoint( control.z.t_vec3() );
					}
					for( uint index = 0; index < span_count; ++index )
					{
						const mat44& control = span_array[ index ]->control;
						stream_out.DrawPoint( control.w.t_vec3() );
					}
					break;
				}
			}
		}
	}
}

void CubicVectorSpline::drawControlTangents( ISplineDrawLineStreamOut& stream_out ) const
{
    if( span_count && span_array )
    {
        if( ( form == ECubicHermite ) || ( form == ECubicHBezier ) )
        {
			vec3 start, end;
            if( rational )
            {
				{
				    const mat44& control = span_array[ 0 ]->control;
					if( !looping )
					{   //  only draw the first control tangent if the spline is not a closed loop with the first and last control points the same
						start.SetDiv( control.y.t_vec3(), control.y.w );
						end.SetDiv( control.x.t_vec3(), control.x.w );
						end.Add( start );
						stream_out.DrawLine( start, end );
					}
					start.SetDiv( control.z.t_vec3(), control.z.w );
					end.SetDiv( control.w.t_vec3(), control.w.w );
					end.Add( start );
					stream_out.DrawLine( start, end );
				}
				for( uint index = 1; index < span_count; ++index )
				{
				    const mat44& control = span_array[ index ]->control;
					start.SetDiv( control.y.t_vec3(), control.y.w );
					end.SetDiv( control.x.t_vec3(), control.x.w );
					end.Add( start );
					stream_out.DrawLine( start, end );
					start.SetDiv( control.z.t_vec3(), control.z.w );
					end.SetDiv( control.w.t_vec3(), control.w.w );
					end.Add( start );
					stream_out.DrawLine( start, end );
				}
            }
            else
            {
				{
				    const mat44& control = span_array[ 0 ]->control;
					if( !looping )
					{   //  only draw the first control tangent if the spline is not a closed loop with the first and last control points the same
						start.Set( control.y.t_vec3() );
						end.SetAdd( control.x.t_vec3(), start );
						stream_out.DrawLine( start, end );
					}
					start.Set( control.z.t_vec3() );
					end.SetAdd( control.w.t_vec3(), start );
					stream_out.DrawLine( start, end );
				}
				for( uint index = 1; index < span_count; ++index )
				{
				    const mat44& control = span_array[ index ]->control;
					start.Set( control.y.t_vec3() );
					end.SetAdd( control.x.t_vec3(), start );
					stream_out.DrawLine( start, end );
					start.Set( control.z.t_vec3() );
					end.SetAdd( control.w.t_vec3(), start );
					stream_out.DrawLine( start, end );
				}
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    cubic scalar spline structure function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int CubicScalarSpline::findSpanIndex( const float t_in, float& t_out ) const
{
    t_out = 0;
    if( span_count && span_array )
    {
        int index = 0;
        int limit = ( span_count - 1 );
        float lower = span_array[ 0 ]->lower;
        float upper = span_array[ limit ]->upper;
        float s = ( ( t_in * ( upper - lower ) ) + lower );
        while( index < limit )
        {
            int check = ( ( index + limit ) >> 1 );
            if( s >= span_array[ check ]->upper )
            {
                index = ( check + 1 );
            }
            else
            {
                limit = check;
            }
        }
        Span& span = *span_array[ index ];
        t_out = ( ( s - span.lower ) * span.scale );
        return( index );
    }
    return( -1 );
}

void CubicScalarSpline::updatePolynomial( Span& span ) const
{
	switch( form )
	{
	case( ECubicBezier ):
		{
			span.polynomial.SetMul( span.control, consts::MAT44::BEZIER );
			break;
		}
    case( ECubicBSpline ):
		{
			span.polynomial.SetMul( span.control, consts::MAT44::B_SPLINE );
			break;
		}
    case( ECubicCatmull ):
		{
			span.polynomial.SetMul( span.control, consts::MAT44::CATMULL_ROM );
			break;
		}
    case( ECubicHermite ):
		{
			span.polynomial.SetMul( span.control, consts::MAT44::HERMITE );
			break;
		}
    case( ECubicHBezier ):
		{
			span.polynomial.SetMul( span.control, consts::MAT44::HBEZIER );
			break;
		}
    case( ECubicNonUniformBSpline ):
		{
			mat44 basis;
			basis.SetBSpline( span.knots );
			span.polynomial.SetMul( span.control, basis );
			break;
		}
    case( ECubicNonUniformCatmull ):
		{
			mat44 basis;
			float d1 = fabsf( span.control.y - span.control.x );
			float d2 = fabsf( span.control.z - span.control.y );
			float d3 = fabsf( span.control.w - span.control.z );
			basis.SetCatmull( d1, d2, d3 );
			span.polynomial.SetMul( span.control, basis );
			break;
		}
	default:
		{
			break;
		}
	}
}

void CubicScalarSpline::draw( ISplineDrawLineStreamOut& stream_out, const uint divisions ) const
{
    if( span_count && span_array )
    {
        uint count = ( span_count * ( divisions + 1 ) );
        float dt = ( 1.0f / count );
        float t = dt;
		vec3 start, end;
		end.x = 0;
		end.y = span_array[ 0 ]->polynomial.EvaluatePolynomial( 0 );
		end.z = 0;
        for( uint index = 0; index < count; ++index )
        {
            float s = 0;
            start = end;
			end.x = t;
			end.y = span_array[ findSpanIndex( t, s ) ]->polynomial.EvaluatePolynomial( s );
			stream_out.DrawLine( start, end );
            t += dt;
        }
    }
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

//
////-----------------------------------------------------------------------------
////                                EGO Libraries
//// (c) 2013 The Codemasters Software Company Limited. All rights reserved.
////-----------------------------------------------------------------------------
//#include "core/splines/ne_standard_splines.h"
//#include "core/splines/ne_spline_utils.h"
//
//namespace neon
//{
//
//namespace splines
//{
//
//namespace cubic
//{
//
////-----------------------------------------------------------------------------
////!  spline form text names
////-----------------------------------------------------------------------------
//
//const char* spline_type_names[ ESplineTypeCount ] = { " Bezier", " b-spline", "Catmull-Rom", " Hermite", " Hermite-Bezier", "Non-uniform cubic b-spline", "Non-uniform Catmull-Rom" };
//
////-----------------------------------------------------------------------------
////! PointSpan class function bodies
////-----------------------------------------------------------------------------
//
///// calculate the 't' value of the closest point on the infinite line segment between points localed at (t = a) and (t = b) to the given position
//float PointSpan::findClosestLinearT( const vec3& position, const float a, const float b ) const
//{
//    vec3 origin = getPoint( a );
//    vec3 direction = ( getPoint( b ) - origin );
//    float d = neDot( direction, direction );
//    return( ( d < ( FLT_EPSILON * FLT_EPSILON ) ) ? ( ( a + b ) * 0.5f ) : ( ( ( neDot( ( position - origin ), direction ) * ( b - a ) ) / d ) + a ) );
//}
//
///// calculate the 't' value of the closest point on the infinite line segment between points localed at (t = a) and (t = b) to the given position
//float PointSpan::findClosestLinearT( const vec4& position, const float a, const float b ) const
//{
//    vec4 origin = getPoint4( a );
//    vec4 direction = ( getPoint4( b ) - origin );
//    float d = neDot( direction, direction );
//    return( ( d < ( FLT_EPSILON * FLT_EPSILON ) ) ? ( ( a + b ) * 0.5f ) : ( ( ( neDot( ( position - origin ), direction ) * ( b - a ) ) / d ) + a ) );
//}
//
///// calculate the 't' value of the closest point on the infinite line segment between rational points localed at (t = a) and (t = b) to the given position
//float PointSpan::findProjectedClosestLinearT( const vec3& position, const float a, const float b ) const
//{
//    vec3 origin = getProjectedPoint( a );
//    vec3 direction = ( getProjectedPoint( b ) - origin );
//    float d = neDot( direction, direction );
//    return( ( d < ( FLT_EPSILON * FLT_EPSILON ) ) ? ( ( a + b ) * 0.5f ) : ( ( ( neDot( ( position - origin ), direction ) * ( b - a ) ) / d ) + a ) );
//}
//
///// calculate the 't' value of the closest point on the infinite line segment between rational points localed at (t = a) and (t = b) to the given rational position
//float PointSpan::findProjectedClosestLinearT( const vec4& position, const float a, const float b ) const
//{
//    vec3 origin = getProjectedPoint( a );
//    vec3 direction = ( getProjectedPoint( b ) - origin );
//    float d = neDot( direction, direction );
//    return( ( d < ( FLT_EPSILON * FLT_EPSILON ) ) ? ( ( a + b ) * 0.5f ) : ( ( ( neDot( ( projectRational( position ) - origin ), direction ) * ( b - a ) ) / d ) + a ) );
//}
//
////-----------------------------------------------------------------------------
////! KeyFrameSpan class function bodies
////-----------------------------------------------------------------------------
//
///// return the real root closest to 0.5 or returns 0.5 if there is no real root
//float KeyFrameSpan::getT( const float time ) const
//{
//    float r[ 3 ];
//    NEint count = ( static_cast< NEint >( getCubicRoots( polynomial, time, r[ 0 ], r[ 1 ], r[ 2 ] ) ) & EROOT_REAL_COUNT_MASK );
//    if( count )
//    {
//        float t = r[ 0 ];
//        for( NEint index = 1; index < count; ++index )
//        {
//            float s = r[ index ];
//            if( fabsf( s - 0.5f ) < fabsf( t - 0.5f ) )
//            {
//                t = s;
//            }
//        }
//        return( t );
//    }
//    return( 0.5f );
//}
//
///// calculate the time value at the specified 't' position relative to this span
//float KeyFrameSpan::getTime( const float t ) const
//{
//    return( getValue( t, polynomial ) );
//}
//
////-----------------------------------------------------------------------------
////! Templated iterator to simplify the setup functions of the spline structures
////-----------------------------------------------------------------------------
//
//template< typename T >
//struct SplineIter
//{
//    T*                  pd;
//    T* const*           pi;
//    NEbool              init( T* const direct, T* const* const indirect );
//    T&                  next( void );
//};
//
//template< typename T >
//NEbool SplineIter<T>::init( T* const direct, T* const* const indirect )
//{
//    pd = 0;
//    pi = &pd;
//    if( direct )
//    {
//        pd = direct;
//        return( true );
//    }
//    if( indirect )
//    {
//        pi = indirect;
//        return( true );
//    }
//    return( false );
//}
//
//template< typename T >
//T& SplineIter<T>::next( void )
//{
//    T& p = **pi;
//    if( pd )
//    {
//        ++pd;
//    }
//    else if( pi )
//    {
//        ++pi;
//    }
//    return( p );
//}
//
////-----------------------------------------------------------------------------
////! PointSpline class function bodies
////-----------------------------------------------------------------------------
//
///// !!!IMPORTANT!!! This function assumes that the control matrices have already been set
///// Calculates and sets the polynomial matrix, span_linear, span_length, spline_linear and spline_length
//void PointSpline::setup( const float* const k )
//{
//    SplineIter< PointSpan > iter;
//    if( iter.init( direct_spans, indirect_spans ) )
//    {
//        switch( spline_form )
//        {
//        case( ENonUniformBSpline ):
//            {   //  knot vector based spline
//                if( k )
//                {
//                    float spline_linear = 0;
//                    float spline_length = 0;
//                    for( NEuint span_index = 0; span_index < span_count; ++span_index )
//                    {
//                        PointSpan& span = iter.next();
//                        span.setupPolynomial( &k[ span_index ] );
//                        span.setupSpanCommon( rational, spline_linear, spline_length );
//                        span.setupSpanExtent( k[ span_index + 2 ], k[ span_index + 3 ] );
//                    }
//                }
//                break;
//            }
//        case( ENonUniformCatmull ):
//            {   //  linear length based spline
//                float spline_linear = 0;
//                float spline_length = 0;
//                for( NEuint span_index = 0; span_index < span_count; ++span_index )
//                {
//                    PointSpan& span = iter.next();
//                    span.setupPolynomial( rational );
//                    span.setupSpanCommon( rational, spline_linear, spline_length );
//                    span.setupSpanExtent( span.spline_linear, spline_linear );
//                }
//                break;
//            }
//        default:
//            {   //  integrated length based spline
//                const NeMatrix4& basis_matrix = *uniform_matrices::splines[ static_cast< NEint >( spline_form ) ];
//                float spline_linear = 0;
//                float spline_length = 0;
//                for( NEuint span_index = 0; span_index < span_count; ++span_index )
//                {
//                    PointSpan& span = iter.next();
//                    span.setupPolynomial( basis_matrix );
//                    span.setupSpanCommon( rational, spline_linear, spline_length );
//                    span.setupSpanExtent( span.spline_length, spline_length );
//
//                }
//                break;
//            }
//        }
//    }
//}
//
///// t_out will contain the t value relative to the returned span
//NEint PointSpline::findSpanIndex( const float t_in, float& t_out ) const
//{
//    t_out = 0;
//    if( span_count )
//    {
//        NEint index = 0;
//        NEint limit = ( span_count - 1 );
//        if( direct_spans )
//        {
//            float lower = direct_spans[ 0 ].span_lower;
//            float upper = direct_spans[ limit ].span_upper;
//            float s = ( ( t_in * ( upper - lower ) ) + lower );
//            while( index < limit )
//            {
//                NEint check = ( ( index + limit ) >> 1 );
//                if( s >= direct_spans[ check ].span_upper )
//                {
//                    index = ( check + 1 );
//                }
//                else
//                {
//                    limit = check;
//                }
//            }
//            PointSpan& span = direct_spans[ index ];
//            t_out = ( ( s - span.span_lower ) * span.span_scale );
//            return( index );
//        }
//        if( indirect_spans )
//        {
//            float lower = indirect_spans[ 0 ]->span_lower;
//            float upper = indirect_spans[ limit ]->span_upper;
//            float s = ( ( t_in * ( upper - lower ) ) + lower );
//            while( index < limit )
//            {
//                NEint check = ( ( index + limit ) >> 1 );
//                if( s >= indirect_spans[ check ]->span_upper )
//                {
//                    index = ( check + 1 );
//                }
//                else
//                {
//                    limit = check;
//                }
//            }
//            PointSpan& span = *indirect_spans[ index ];
//            t_out = ( ( s - span.span_lower ) * span.span_scale );
//            return( index );
//        }
//    }
//    return( -1 );
//}
//
//PointSpan* PointSpline::getSpanByIndex( const NEint index )
//{
//    if( index >= 0 )
//    {
//        if( static_cast< NEuint >( index ) < span_count )
//        {
//            if( direct_spans )
//            {
//                return( &direct_spans[ index ] );
//            }
//            if( indirect_spans )
//            {
//                return( indirect_spans[ index ] );
//            }
//        }
//    }
//    return( 0 );
//}
//
//const PointSpan* PointSpline::getSpanByIndex( const NEint index ) const
//{
//    if( index >= 0 )
//    {
//        if( static_cast< NEuint >( index ) < span_count )
//        {
//            if( direct_spans )
//            {
//                return( &direct_spans[ index ] );
//            }
//            if( indirect_spans )
//            {
//                return( indirect_spans[ index ] );
//            }
//        }
//    }
//    return( 0 );
//}
//
///// get the estimated closest span position based on the difference between the first and last control points
///// this is the fastest and most stable closest span function but also the most inaccurate
///// do not use this function if the spline loops as it in this case it will always return the loop point
///// t_out will contain the t value relative to the returned span
//NEint PointSpline::getEstimatedClosestSpan( const vec3& position, float& t_out ) const
//{
//    t_out = 0;
//    if( span_count && ( direct_spans || indirect_spans ) )
//    {
//        vec3 origin, direction;
//        if( rational )
//        {   //  rational control points
//            origin = getSpanByIndex( 0 )->getProjectedPoint( 0 );
//            direction = ( getSpanByIndex( span_count - 1 )->getProjectedPoint( 1 ) - origin );
//        }
//        else
//        {   //  non-rational control points
//            origin = getSpanByIndex( 0 )->getPoint( 0 );
//            direction = ( getSpanByIndex( span_count - 1 )->getPoint( 1 ) - origin );
//        }
//        float d = neDot( direction, direction );
//        float t = ( ( d < ( FLT_EPSILON * FLT_EPSILON ) ) ? 0.0f : ( neDot( ( position - origin ), direction ) / d ) );
//        if( t < 0 ) t = 0;
//        if( t > 1 ) t = 1;
//        return( findSpanIndex( t, t_out ) );
//    }
//    return( 0 );
//}
//
///// get the approximate closest span position based on a weighted binary search of 't'
///// this is a fairly fast function but can be very inaccurate particularly if the spline loops or has significant curvature
///// t_out will contain the t value relative to the returned span
//NEint PointSpline::getBinaryClosestSpan( const vec3& position, float& t_out ) const
//{
//    t_out = 0;
//    if( span_count && ( direct_spans || indirect_spans ) )
//    {
//        float b[ 2 ], d[ 2 ];
//        b[ 0 ] = 0;
//        b[ 1 ] = 1;
//        if( rational )
//        {   //  rational control points
//            d[ 0 ] = neLength( position - getSpanByIndex( 0 )->getProjectedPoint( 0 ) );
//            d[ 1 ] = neLength( position - getSpanByIndex( span_count - 1 )->getProjectedPoint( 1 ) );
//            for( NEuint n = 16; n; --n )
//            {
//                float t = ( d[ 0 ] + d[ 1 ] );
//                if( t < ( FLT_EPSILON * FLT_EPSILON ) ) break;                      //  exit if both distances are zero (avoid division by zero)
//                NEint i = ( ( d[ 0 ] < d[ 1 ] ) ? 1 : 0 );                          //  get the index of the furthest point so it can be replaced
//                b[ i ] = t = ( ( ( b[ 1 ] * d[ 0 ] ) + ( b[ 0 ] * d[ 1 ] ) ) / t ); //  calculate a weighted split point
//                d[ i ] = neLength( position - getSpanByIndex( findSpanIndex( t, t ) )->getProjectedPoint( t ) );
//            }
//        }
//        else
//        {   //  non-rational control points
//            d[ 0 ] = neLength( position - getSpanByIndex( 0 )->getPoint( 0 ) );
//            d[ 1 ] = neLength( position - getSpanByIndex( span_count - 1 )->getPoint( 1 ) );
//            for( NEuint n = 16; n; --n )
//            {
//                float t = ( d[ 0 ] + d[ 1 ] );
//                if( t < ( FLT_EPSILON * FLT_EPSILON ) ) break;                      //  exit if both distances are zero (avoid division by zero)
//                NEint i = ( ( d[ 0 ] < d[ 1 ] ) ? 1 : 0 );                          //  get the index of the furthest point so it can be replaced
//                b[ i ] = t = ( ( ( b[ 1 ] * d[ 0 ] ) + ( b[ 0 ] * d[ 1 ] ) ) / t ); //  calculate a weighted split point
//                d[ i ] = neLength( position - getSpanByIndex( findSpanIndex( t, t ) )->getPoint( t ) );
//            }
//        }
//        return( findSpanIndex( ( ( b[ 0 ] + b[ 1 ] ) * 0.5f ), t_out ) );
//    }
//    return( 0 );
//}
//
///// get the approximate closest span position based on a linear search of the spans
///// this is the slowest closest span function but also the most accurate
///// this is still not a fully accurate closest point as it treats the spline as a list of linear line segments
///// t_out will contain the t value relative to the returned span
//NEint PointSpline::getLinearClosestSpan( const vec3& position, float& t_out ) const
//{
//    t_out = 0;
//    if( span_count && ( direct_spans || indirect_spans ) )
//    {
//        if( rational )
//        {   //  rational control points
//            if( span_count == 1 )
//            {   //  special case for a single span
//                t_out = getSpanByIndex( 0 )->findProjectedClosestLinearT( position, 0, 1 );
//                return( 0 );
//            }
//            else
//            {   //  general case
//                NEint index = 1;
//                float best = neLengthSq( position - getSpanByIndex( 1 )->getProjectedPoint( 0 ) );
//                for( NEuint check = 2; check < span_count; ++check )
//                {
//                    float test = neLengthSq( position - getSpanByIndex( check )->getProjectedPoint( 0 ) );
//                    if( best > test )
//                    {
//                        index = check;
//                        best = test;
//                    }
//                }
//                const PointSpan& span1 = *getSpanByIndex( index - 1 );
//                const PointSpan& span2 = *getSpanByIndex( index );
//                float t1 = span1.findProjectedClosestLinearT( position, 0, 1 );
//                float t2 = span2.findProjectedClosestLinearT( position, 0, 1 );
//                if( t1 < 0 ) t1 = 0;
//                if( t2 < 0 ) t2 = 0;
//                if( t1 > 1 ) t1 = 1;
//                if( t2 > 1 ) t2 = 1;
//                t_out = t2;
//                if( neLengthSq( position - span1.getProjectedPoint( t1 ) ) < neLengthSq( position - span2.getProjectedPoint( t2 ) ) )
//                {
//                    t_out = t1;
//                    --index;
//                }
//                return( index );
//            }
//        }
//        else
//        {   //  non-rational control points
//            if( span_count == 1 )
//            {   //  special case for a single span
//                t_out = getSpanByIndex( 0 )->findClosestLinearT( position, 0, 1 );
//                return( 0 );
//            }
//            else
//            {   //  general case
//                NEint index = 1;
//                float best = neLengthSq( position - getSpanByIndex( 1 )->getPoint( 0 ) );
//                for( NEuint check = 2; check < span_count; ++check )
//                {
//                    float test = neLengthSq( position - getSpanByIndex( check )->getPoint( 0 ) );
//                    if( best > test )
//                    {
//                        index = check;
//                        best = test;
//                    }
//                }
//                const PointSpan& span1 = *getSpanByIndex( index - 1 );
//                const PointSpan& span2 = *getSpanByIndex( index );
//                float t1 = span1.findClosestLinearT( position, 0, 1 );
//                float t2 = span2.findClosestLinearT( position, 0, 1 );
//                if( t1 < 0 ) t1 = 0;
//                if( t2 < 0 ) t2 = 0;
//                if( t1 > 1 ) t1 = 1;
//                if( t2 > 1 ) t2 = 1;
//                t_out = t2;
//                if( neLengthSq( position - span1.getPoint( t1 ) ) < neLengthSq( position - span2.getPoint( t2 ) ) )
//                {
//                    t_out = t1;
//                    --index;
//                }
//                return( index );
//            }
//        }
//    }
//    return( 0 );
//}
//
///// helper function for setting up standard wrapping splines
//void PointSpline::propagateControlCol1( void )
//{
//    if( span_count )
//    {
//        if( direct_spans )
//        {
//            NEint a = ( span_count - 1 );
//            NEint b = 0;
//            NEint c = ( 1 % span_count );
//            NEint d = ( 2 % span_count );
//            if( ( spline_form == EHermite ) || ( spline_form == EHBezier ) || ( spline_form == EBezier ) )
//            {
//                float scale = ( 1.0f / ( ( spline_form == EHermite ) ? 2 : 6 ) );
//                for( NEuint count = span_count; count; --count )
//                {
//                    NeMatrix4& control = direct_spans[ b ].control;
//                    const vec4& va = direct_spans[ a ].control.getCol1();
//                    const vec4& vb = direct_spans[ b ].control.getCol1();
//                    const vec4& vc = direct_spans[ c ].control.getCol1();
//                    const vec4& vd = direct_spans[ d ].control.getCol1();
//                    control.setCol0( ( vc - va ) * scale );
//                    control.setCol2( vc );
//                    control.setCol3( ( vb - vd ) * scale );
//                    a = b;
//                    b = c;
//                    c = d;
//                    d = ( ( d + 1 ) % span_count );
//                }
//                if( spline_form == EBezier )
//                {
//                    for( NEuint index = 0; index < span_count; ++index )
//                    {
//                        NeMatrix4& control = direct_spans[ index ].control;
//                        const vec4& va = control.getCol0();
//                        const vec4& vb = control.getCol1();
//                        const vec4& vc = control.getCol2();
//                        const vec4& vd = control.getCol3();
//                        control = NeMatrix4( vb, ( vb + va ), ( vc + vd ), vc );
//                    }
//                }
//            }
//            else
//            {
//                for( NEuint count = span_count; count; --count )
//                {
//                    NeMatrix4& control = direct_spans[ b ].control;
//                    const vec4& va = direct_spans[ a ].control.getCol1();
//                    const vec4& vc = direct_spans[ c ].control.getCol1();
//                    const vec4& vd = direct_spans[ d ].control.getCol1();
//                    control.setCol0( va );
//                    control.setCol2( vc );
//                    control.setCol3( vd );
//                    a = b;
//                    b = c;
//                    c = d;
//                    d = ( ( d + 1 ) % span_count );
//                }
//            }
//        }
//        else if( indirect_spans )
//        {
//            NEint a = ( span_count - 1 );
//            NEint b = 0;
//            NEint c = ( 1 % span_count );
//            NEint d = ( 2 % span_count );
//            if( ( spline_form == EHermite ) || ( spline_form == EHBezier ) || ( spline_form == EBezier ) )
//            {
//                float scale = ( 1.0f / ( ( spline_form == EHermite ) ? 2 : 6 ) );
//                for( NEuint count = span_count; count; --count )
//                {
//                    NeMatrix4& control = indirect_spans[ b ]->control;
//                    const vec4& va = indirect_spans[ a ]->control.getCol1();
//                    const vec4& vb = indirect_spans[ b ]->control.getCol1();
//                    const vec4& vc = indirect_spans[ c ]->control.getCol1();
//                    const vec4& vd = indirect_spans[ d ]->control.getCol1();
//                    control.setCol0( ( vc - va ) * scale );
//                    control.setCol2( vc );
//                    control.setCol3( ( vb - vd ) * scale );
//                    a = b;
//                    b = c;
//                    c = d;
//                    d = ( ( d + 1 ) % span_count );
//                }
//                if( spline_form == EBezier )
//                {
//                    for( NEuint index = 0; index < span_count; ++index )
//                    {
//                        NeMatrix4& control = indirect_spans[ index ]->control;
//                        const vec4& va = control.getCol0();
//                        const vec4& vb = control.getCol1();
//                        const vec4& vc = control.getCol2();
//                        const vec4& vd = control.getCol3();
//                        control = NeMatrix4( vb, ( vb + va ), ( vc + vd ), vc );
//                    }
//                }
//            }
//            else
//            {
//                for( NEuint count = span_count; count; --count )
//                {
//                    NeMatrix4& control = indirect_spans[ b ]->control;
//                    const vec4& va = indirect_spans[ a ]->control.getCol1();
//                    const vec4& vc = indirect_spans[ c ]->control.getCol1();
//                    const vec4& vd = indirect_spans[ d ]->control.getCol1();
//                    control.setCol0( va );
//                    control.setCol2( vc );
//                    control.setCol3( vd );
//                    a = b;
//                    b = c;
//                    c = d;
//                    d = ( ( d + 1 ) % span_count );
//                }
//            }
//        }
//    }
//}
//
////-----------------------------------------------------------------------------
////! ValueSpline class function bodies
////-----------------------------------------------------------------------------
//
///// !!!IMPORTANT!!! This function assumes that the values vectors have already been set
///// Calculates and sets the polynomial vector, span_linear, span_length, spline_linear and spline_length
//void ValueSpline::setup( const float* const k )
//{
//    SplineIter< ValueSpan > iter;
//    if( iter.init( direct_spans, indirect_spans ) )
//    {
//        switch( spline_form )
//        {
//        case( ENonUniformBSpline ):
//            {   //  knot vector based spline
//                if( k )
//                {
//                    float spline_linear = 0;
//                    float spline_length = 0;
//                    for( NEuint span_index = 0; span_index < span_count; ++span_index )
//                    {
//                        ValueSpan& span = iter.next();
//                        span.setupPolynomial( &k[ span_index ] );
//                        span.setupSpanCommon( spline_linear, spline_length );
//                        span.setupSpanExtent( k[ span_index + 2 ], k[ span_index + 3 ] );
//                    }
//                }
//                break;
//            }
//        case( ENonUniformCatmull ):
//            {   //  linear length based spline
//                float spline_linear = 0;
//                float spline_length = 0;
//                for( NEuint span_index = 0; span_index < span_count; ++span_index )
//                {
//                    ValueSpan& span = iter.next();
//                    span.setupPolynomial();
//                    span.setupSpanCommon( spline_linear, spline_length );
//                    span.setupSpanExtent( span.spline_linear, spline_linear );
//                }
//                break;
//            }
//        default:
//            {   //  integrated length based spline
//                const NeMatrix4& basis_matrix = *uniform_matrices::splines[ static_cast< NEint >( spline_form ) ];
//                float spline_linear = 0;
//                float spline_length = 0;
//                for( NEuint span_index = 0; span_index < span_count; ++span_index )
//                {
//                    ValueSpan& span = iter.next();
//                    span.setupPolynomial( basis_matrix );
//                    span.setupSpanCommon( spline_linear, spline_length );
//                    span.setupSpanExtent( span.spline_length, spline_length );
//                }
//                break;
//            }
//        }
//    }
//}
//
///// t_out will contain the t value relative to the returned span
//NEint ValueSpline::findSpanIndex( const float t_in, float& t_out ) const
//{
//    t_out = 0;
//    if( span_count )
//    {
//        NEint index = 0;
//        NEint limit = ( span_count - 1 );
//        if( direct_spans )
//        {
//            float lower = direct_spans[ 0 ].span_lower;
//            float upper = direct_spans[ limit ].span_upper;
//            float s = ( ( t_in * ( upper - lower ) ) + lower );
//            while( index < limit )
//            {
//                NEint check = ( ( index + limit ) >> 1 );
//                if( s >= direct_spans[ check ].span_upper )
//                {
//                    index = ( check + 1 );
//                }
//                else
//                {
//                    limit = check;
//                }
//            }
//            ValueSpan& span = direct_spans[ index ];
//            t_out = ( ( s - span.span_lower ) * span.span_scale );
//            return( index );
//        }
//        if( indirect_spans )
//        {
//            float lower = indirect_spans[ 0 ]->span_lower;
//            float upper = indirect_spans[ limit ]->span_upper;
//            float s = ( ( t_in * ( upper - lower ) ) + lower );
//            while( index < limit )
//            {
//                NEint check = ( ( index + limit ) >> 1 );
//                if( s >= indirect_spans[ check ]->span_upper )
//                {
//                    index = ( check + 1 );
//                }
//                else
//                {
//                    limit = check;
//                }
//            }
//            ValueSpan& span = *indirect_spans[ index ];
//            t_out = ( ( s - span.span_lower ) * span.span_scale );
//            return( index );
//        }
//    }
//    return( -1 );
//}
//
//ValueSpan* ValueSpline::getSpanByIndex( const NEint index )
//{
//    if( index >= 0 )
//    {
//        if( static_cast< NEuint >( index ) < span_count )
//        {
//            if( direct_spans )
//            {
//                return( &direct_spans[ index ] );
//            }
//            if( indirect_spans )
//            {
//                return( indirect_spans[ index ] );
//            }
//        }
//    }
//    return( 0 );
//}
//
//const ValueSpan* ValueSpline::getSpanByIndex( const NEint index ) const
//{
//    if( index >= 0 )
//    {
//        if( static_cast< NEuint >( index ) < span_count )
//        {
//            if( direct_spans )
//            {
//                return( &direct_spans[ index ] );
//            }
//            if( indirect_spans )
//            {
//                return( indirect_spans[ index ] );
//            }
//        }
//    }
//    return( 0 );
//}
//
///// helper function for setting up standard wrapping splines
//void ValueSpline::propagateValuesY( void )
//{
//    if( span_count )
//    {
//        if( direct_spans )
//        {
//            NEint a = ( span_count - 1 );
//            NEint b = 0;
//            NEint c = ( 1 % span_count );
//            NEint d = ( 2 % span_count );
//            if( ( spline_form == EHermite ) || ( spline_form == EHBezier ) || ( spline_form == EBezier ) )
//            {   //  point tangent control point layout
//                float scale = ( 1.0f / ( ( spline_form == EHermite ) ? 2 : 6 ) );
//                for( NEuint count = span_count; count; --count )
//                {
//                    float fa = direct_spans[ a ].values.getYFloat();
//                    float fb = direct_spans[ b ].values.getYFloat();
//                    float fc = direct_spans[ c ].values.getYFloat();
//                    float fd = direct_spans[ d ].values.getYFloat();
//                    direct_spans[ b ].values = vec4( ( ( fc - fa ) * scale ), fb, fc, ( ( fb - fd ) * scale ) );
//                    a = b;
//                    b = c;
//                    c = d;
//                    d = ( ( d + 1 ) % span_count );
//                }
//                if( spline_form == EBezier )
//                {
//                    for( NEuint index = 0; index < span_count; ++index )
//                    {
//                        vec4& values = direct_spans[ index ].values;
//                        float fa = values.getXFloat();
//                        float fb = values.getYFloat();
//                        float fc = values.getZFloat();
//                        float fd = values.getWFloat();
//                        values = vec4( fb, ( fb + fa ), ( fc + fd ), fc );
//                    }
//                }
//            }
//            else
//            {   //  standard control point layout
//                for( NEuint count = span_count; count; --count )
//                {
//                    float fa = direct_spans[ a ].values.getYFloat();
//                    float fb = direct_spans[ b ].values.getYFloat();
//                    float fc = direct_spans[ c ].values.getYFloat();
//                    float fd = direct_spans[ d ].values.getYFloat();
//                    direct_spans[ b ].values = vec4( fa, fb, fc, fd );
//                    a = b;
//                    b = c;
//                    c = d;
//                    d = ( ( d + 1 ) % span_count );
//                }
//            }
//        }
//        else if( indirect_spans )
//        {
//            NEint a = ( span_count - 1 );
//            NEint b = 0;
//            NEint c = ( 1 % span_count );
//            NEint d = ( 2 % span_count );
//            if( ( spline_form == EHermite ) || ( spline_form == EHBezier ) || ( spline_form == EBezier ) )
//            {   //  point tangent control point layout
//                float scale = ( 1.0f / ( ( spline_form == EHermite ) ? 2 : 6 ) );
//                for( NEuint count = span_count; count; --count )
//                {
//                    float fa = indirect_spans[ a ]->values.getYFloat();
//                    float fb = indirect_spans[ b ]->values.getYFloat();
//                    float fc = indirect_spans[ c ]->values.getYFloat();
//                    float fd = indirect_spans[ d ]->values.getYFloat();
//                    indirect_spans[ b ]->values = vec4( ( ( fc - fa ) * scale ), fb, fc, ( ( fb - fd ) * scale ) );
//                    a = b;
//                    b = c;
//                    c = d;
//                    d = ( ( d + 1 ) % span_count );
//                }
//                if( spline_form == EBezier )
//                {
//                    for( NEuint index = 0; index < span_count; ++index )
//                    {
//                        vec4& values = indirect_spans[ index ]->values;
//                        float fa = values.getXFloat();
//                        float fb = values.getYFloat();
//                        float fc = values.getZFloat();
//                        float fd = values.getWFloat();
//                        values = vec4( fb, ( fb + fa ), ( fc + fd ), fc );
//                    }
//                }
//            }
//            else
//            {   //  standard control point layout
//                for( NEuint count = span_count; count; --count )
//                {
//                    float fa = indirect_spans[ a ]->values.getYFloat();
//                    float fb = indirect_spans[ b ]->values.getYFloat();
//                    float fc = indirect_spans[ c ]->values.getYFloat();
//                    float fd = indirect_spans[ d ]->values.getYFloat();
//                    indirect_spans[ b ]->values = vec4( fa, fb, fc, fd );
//                    a = b;
//                    b = c;
//                    c = d;
//                    d = ( ( d + 1 ) % span_count );
//                }
//            }
//        }
//    }
//}
//
////-----------------------------------------------------------------------------
////! KeyFrameSpline class function bodies
////-----------------------------------------------------------------------------
//
///// !!!IMPORTANT!!! This function assumes that the values vectors have already been set
///// Calculates and sets the polynomial vector
//void KeyFrameSpline::setup( void )
//{
//    if( direct_spans )
//    {
//        for( NEuint span_index = 0; span_index < span_count; ++span_index )
//        {
//            direct_spans[ span_index ].setupPolynomial();
//        }
//    }
//    if( indirect_spans )
//    {
//        for( NEuint span_index = 0; span_index < span_count; ++span_index )
//        {
//            indirect_spans[ span_index ]->setupPolynomial();
//        }
//    }
//}
//
//float KeyFrameSpline::getLinearT( const float time ) const
//{
//    if( span_count )
//    {
//        if( direct_spans )
//        {
//            float start = direct_spans[ 0 ].times.getYFloat();
//            float range = ( direct_spans[ span_count - 1 ].times.getZFloat() - start );
//            return( ( time - start ) / range );
//        }
//        if( indirect_spans )
//        {
//            float start = indirect_spans[ 0 ]->times.getYFloat();
//            float range = ( indirect_spans[ span_count - 1 ]->times.getZFloat() - start );
//            return( ( time - start ) / range );
//        }
//    }
//    return( 0 );
//}
//
//float KeyFrameSpline::getLinearTime( const float t ) const
//{
//    if( span_count )
//    {
//        if( direct_spans )
//        {
//            float start = direct_spans[ 0 ].times.getYFloat();
//            float range = ( direct_spans[ span_count - 1 ].times.getZFloat() - start );
//            return( ( t * range ) + start );
//        }
//        if( indirect_spans )
//        {
//            float start = indirect_spans[ 0 ]->times.getYFloat();
//            float range = ( indirect_spans[ span_count - 1 ]->times.getZFloat() - start );
//            return( ( t * range ) + start );
//        }
//    }
//    return( 0 );
//}
//
///// spline values are assumed to be increasing, though the tangents may vary
///// on exit, t_out contains the 't' value required to produce the given value
//NEint KeyFrameSpline::findSpanIndex( const float time ) const
//{
//    if( span_count )
//    {
//        NEint index = 0;
//        NEint limit = ( span_count - 1 );
//        if( direct_spans )
//        {
//            while( index < limit )
//            {
//                NEint check = ( ( index + limit ) >> 1 );
//                if( time >= direct_spans[ check ].times.getZFloat() )
//                {
//                    index = ( check + 1 );
//                }
//                else
//                {
//                    limit = check;
//                }
//            }
//            return( index );
//        }
//        if( indirect_spans )
//        {
//            while( index < limit )
//            {
//                NEint check = ( ( index + limit + 1 ) >> 1 );
//                if( time >= indirect_spans[ check ]->times.getZFloat() )
//                {
//                    index = ( check + 1 );
//                }
//                else
//                {
//                    limit = check;
//                }
//            }
//            return( index );
//        }
//    }
//    return( -1 );
//}
//
//KeyFrameSpan* KeyFrameSpline::getSpanByIndex( const NEint index )
//{
//    if( index >= 0 )
//    {
//        if( static_cast< NEuint >( index ) < span_count )
//        {
//            if( direct_spans )
//            {
//                return( &direct_spans[ index ] );
//            }
//            if( indirect_spans )
//            {
//                return( indirect_spans[ index ] );
//            }
//        }
//    }
//    return( 0 );
//}
//
//const KeyFrameSpan* KeyFrameSpline::getSpanByIndex( const NEint index ) const
//{
//    if( index >= 0 )
//    {
//        if( static_cast< NEuint >( index ) < span_count )
//        {
//            if( direct_spans )
//            {
//                return( &direct_spans[ index ] );
//            }
//            if( indirect_spans )
//            {
//                return( indirect_spans[ index ] );
//            }
//        }
//    }
//    return( 0 );
//}
//
///// convert a span index and time into a 't' value local to the specified span
//float KeyFrameSpline::getSpanT( const NEint span_index, const float time ) const
//{
//    if( span_index >= 0 )
//    {
//        if( static_cast< NEuint >( span_index ) < span_count )
//        {
//            if( direct_spans )
//            {
//                return( direct_spans[ span_index ].getT( time ) );
//            }
//            if( indirect_spans )
//            {
//                return( indirect_spans[ span_index ]->getT( time ) );
//            }
//        }
//    }
//    return( 0 );
//}
//
///// convert a span index and time into a 't' value that maps to the spline time-line
//float KeyFrameSpline::getT( const NEint span_index, const float time ) const
//{
//    if( span_index >= 0 )
//    {
//        if( static_cast< NEuint >( span_index ) < span_count )
//        {
//            if( direct_spans )
//            {
//                const KeyFrameSpan& span = direct_spans[ span_index ];
//                float startSpline = direct_spans[ 0 ].times.getYFloat();
//                float rangeSpline = ( direct_spans[ span_count - 1 ].times.getZFloat() - startSpline );
//                float startSpan = span.times.getYFloat();
//                float rangeSpan = ( span.times.getZFloat() - startSpan );
//                return( ( ( span.getT( time ) * rangeSpan ) + startSpan - startSpline ) / rangeSpline );
//            }
//            if( indirect_spans )
//            {
//                const KeyFrameSpan& span = *indirect_spans[ span_index ];
//                float startSpline = indirect_spans[ 0 ]->times.getYFloat();
//                float rangeSpline = ( indirect_spans[ span_count - 1 ]->times.getZFloat() - startSpline );
//                float startSpan = span.times.getYFloat();
//                float rangeSpan = ( span.times.getZFloat() - startSpan );
//                return( ( ( span.getT( time ) * rangeSpan ) + startSpan - startSpline ) / rangeSpline );
//            }
//        }
//    }
//    return( 0 );
//}
//
///// convert a span index and spline time-line 't' value to time
//float KeyFrameSpline::getTime( const NEint span_index, const float t ) const
//{
//    if( span_index >= 0 )
//    {
//        if( static_cast< NEuint >( span_index ) < span_count )
//        {
//            if( direct_spans )
//            {
//                const KeyFrameSpan& span = direct_spans[ span_index ];
//                float startSpline = direct_spans[ 0 ].times.getYFloat();
//                float rangeSpline = ( direct_spans[ span_count - 1 ].times.getZFloat() - startSpline );
//                float startSpan = span.times.getYFloat();
//                float rangeSpan = ( span.times.getZFloat() - startSpan );
//                return( span.getTime( ( ( t * rangeSpline ) + startSpline - startSpan ) / rangeSpan ) );
//            }
//            if( indirect_spans )
//            {
//                const KeyFrameSpan& span = *indirect_spans[ span_index ];
//                float startSpline = indirect_spans[ 0 ]->times.getYFloat();
//                float rangeSpline = ( indirect_spans[ span_count - 1 ]->times.getZFloat() - startSpline );
//                float startSpan = span.times.getYFloat();
//                float rangeSpan = ( span.times.getZFloat() - startSpan );
//                return( span.getTime( ( ( t * rangeSpline ) + startSpline - startSpan ) / rangeSpan ) );
//            }
//        }
//    }
//    return( 0 );
//}
//
///// convert a span index and time into a 't' value that maps to the spline frames
//float KeyFrameSpline::getFrameT( const NEint span_index, const float time ) const
//{
//    if( span_index >= 0 )
//    {
//        if( static_cast< NEuint >( span_index ) < span_count )
//        {
//            if( direct_spans )
//            {
//                return( ( direct_spans[ span_index ].getT( time ) + span_index ) / ( span_count + 1 ) );
//            }
//            if( indirect_spans )
//            {
//                return( ( indirect_spans[ span_index ]->getT( time ) + span_index ) / ( span_count + 1 ) );
//            }
//        }
//    }
//    return( 0 );
//}
//
///// convert a span index and spline frames 't' value to time
//float KeyFrameSpline::getFrameTime( const NEint span_index, const float t ) const
//{
//    if( span_index >= 0 )
//    {
//        if( static_cast< NEuint >( span_index ) < span_count )
//        {
//            if( direct_spans )
//            {
//                return( direct_spans[ span_index ].getTime( ( t * ( span_count + 1 ) ) - span_index ) );
//            }
//            if( indirect_spans )
//            {
//                return( indirect_spans[ span_index ]->getTime( ( t * ( span_count + 1 ) ) - span_index ) );
//            }
//        }
//    }
//    return( 0 );
//}
//
//}   //  namespace cubic
//
//}   //  namespace splines
//
//} // end of namespace neon
//
//
//
