
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   quadratic.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	Generalised quadratic spline classes.
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

#include "quadratic.h"
#include "libs/maths/vec2.h"
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
////    quadratic vector spline structure function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int QuadraticVectorSpline::findSpanIndex( const float t_in, float& t_out ) const
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

void QuadraticVectorSpline::updatePolynomial( Span& span ) const
{
	switch( form )
	{
	case( EQuadraticBezier ):
		{
			span.polynomial.SetMul( consts::MAT33::BEZIER, span.control );
			break;
		}
    case( EQuadraticBSpline ):
		{
			span.polynomial.SetMul( consts::MAT33::B_SPLINE, span.control );
			break;
		}
    case( EQuadraticNonUniformBSpline ):
		{
			mat33 basis;
			basis.SetBSpline( span.knots );
			span.polynomial.SetMul( basis, span.control );
			break;
		}
	default:
		{
			break;
		}
	}
}

void QuadraticVectorSpline::draw( ISplineDrawLineStreamOut& stream_out, const uint divisions ) const
{
    if( span_count && span_array )
    {
        uint count = ( span_count * ( divisions + 1 ) );
        float dt = ( 1.0f / count );
        float t = dt;
		vec3 start, end;
        if( rational )
        {
			span_array[ 0 ]->polynomial.EvaluateRationalPolynomial( 0, end );
			end.t_vec2().Div( end.z );
			end.z = 0;
            for( uint index = 0; index < count; ++index )
            {
                float s = 0;
                start = end;
				span_array[ findSpanIndex( t, s ) ]->polynomial.EvaluateRationalPolynomial( s, end );
				end.t_vec2().Div( end.z );
				end.z = 0;
				stream_out.DrawLine( start, end );
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
				stream_out.DrawLine( start, end );
                t += dt;
            }
        }
    }
}

void QuadraticVectorSpline::drawTangents( ISplineDrawLineStreamOut& stream_out, const float scale ) const
{
    if( span_count && span_array )
    {
        vec3 start, end;
        if( rational )
        {
			for( uint index = 0; index < span_count; ++index )
			{
				span_array[ index ]->polynomial.EvaluateRationalPolynomial( 0.5f, start, end );
				start.t_vec2().Div( start.z );
				end.t_vec2().Div( end.z );
				start.z = end.z = 0;
				end.t_vec2().Mul( scale );
				start.t_vec2().Sub( end.t_vec2() );
				end.t_vec2().Mul( 2 );
				end.t_vec2().Add( start.t_vec2() );
				stream_out.DrawLine( start, end );
			}
        }
        else
        {
			for( uint index = 0; index < span_count; ++index )
			{
				span_array[ index ]->polynomial.EvaluatePolynomial( 0.5f, start, end );
				end.Mul( scale );
				start.Sub( end );
				end.Mul( 2 );
				end.Add( start );
				stream_out.DrawLine( start, end );
			}
        }
    }
}

void QuadraticVectorSpline::drawControlHull( ISplineDrawLineStreamOut& stream_out ) const
{
    if( span_count && span_array )
    {
        if( rational )
        {
			vec3 start, end;
			start.z = end.z = 0;
			if( form == EQuadraticBezier )
			{
				{
			        const mat33& control = span_array[ 0 ]->control;
					end.t_vec2().SetDiv( control.x.t_vec2(), control.x.z );
				}
			    for( uint index = 0; index < span_count; ++index )
			    {
			        const mat33& control = span_array[ index ]->control;
			        start = end;
					end.t_vec2().SetDiv( control.y.t_vec2(), control.y.z );
			        stream_out.DrawLine( start, end );
			        start = end;
					end.t_vec2().SetDiv( control.z.t_vec2(), control.z.z );
			        stream_out.DrawLine( start, end );
			    }
			}
			else
			{
				{
			        const mat33& control = span_array[ 0 ]->control;
					end.t_vec2().SetDiv( control.y.t_vec2(), control.y.z );
					if( !looping )
					{   //  only draw the first hull segment if the spline is not a closed loop
						start.t_vec2().SetDiv( control.x.t_vec2(), control.x.z );
						stream_out.DrawLine( start, end );
					}
				}
			    for( uint index = 0; index < span_count; ++index )
			    {
			        const mat33& control = span_array[ index ]->control;
			        start = end;
					end.t_vec2().SetDiv( control.z.t_vec2(), control.z.z );
			        stream_out.DrawLine( start, end );
			    }
			}
        }
        else
        {
			if( form == EQuadraticBezier )
			{
			    for( uint index = 0; index < span_count; ++index )
			    {
			        const mat33& control = span_array[ index ]->control;
					stream_out.DrawLine( control.x, control.y );
			        stream_out.DrawLine( control.y, control.z );
			    }
			}
			else
			{
				if( !looping )
				{   //  only draw the first hull segment if the spline is not a closed loop
			        const mat33& control = span_array[ 0 ]->control;
			        stream_out.DrawLine( control.x, control.y );
				}
			    for( uint index = 0; index < span_count; ++index )
			    {
			        const mat33& control = span_array[ index ]->control;
			        stream_out.DrawLine( control.y, control.z );
			    }
			}
        }
    }
}

void QuadraticVectorSpline::drawControlPoints( ISplineDrawPointStreamOut& stream_out ) const
{
    if( span_count && span_array )
    {
		if( rational )
		{
			vec3 point;
			point.z = 0;
			if( form == EQuadraticBezier )
			{
				if( !looping )
				{   //  only draw the first control point if the spline is not a closed loop
					const mat33& control = span_array[ 0 ]->control;
					point.t_vec2().SetDiv( control.x.t_vec2(), control.x.z );
					stream_out.DrawPoint( point );
				}
				for( uint index = 0; index < span_count; ++index )
				{
					const mat33& control = span_array[ index ]->control;
					point.t_vec2().SetDiv( control.y.t_vec2(), control.y.z );
					stream_out.DrawPoint( point );
					point.t_vec2().SetDiv( control.z.t_vec2(), control.z.z );
					stream_out.DrawPoint( point );
				}
			}
			else
			{
				if( !looping )
				{   //  only draw the first 2 control points if the spline is not a closed loop
					const mat33& control = span_array[ 0 ]->control;
					point.t_vec2().SetDiv( control.x.t_vec2(), control.x.z );
					stream_out.DrawPoint( point );
					point.t_vec2().SetDiv( control.y.t_vec2(), control.y.z );
					stream_out.DrawPoint( point );
				}
				for( uint index = 0; index < span_count; ++index )
				{
					const mat33& control = span_array[ index ]->control;
					point.t_vec2().SetDiv( control.z.t_vec2(), control.z.z );
					stream_out.DrawPoint( point );
				}
			}
		}
		else
		{
			if( form == EQuadraticBezier )
			{
				if( !looping )
				{   //  only draw the first control point if the spline is not a closed loop
					const mat33& control = span_array[ 0 ]->control;
					stream_out.DrawPoint( control.x );
				}
				for( uint index = 0; index < span_count; ++index )
				{
					const mat33& control = span_array[ index ]->control;
					stream_out.DrawPoint( control.y );
					stream_out.DrawPoint( control.z );
				}
			}
			else
			{
				if( !looping )
				{   //  only draw the first 2 control points if the spline is not a closed loop
					const mat33& control = span_array[ 0 ]->control;
					stream_out.DrawPoint( control.x );
					stream_out.DrawPoint( control.y );
				}
				for( uint index = 0; index < span_count; ++index )
				{
					const mat33& control = span_array[ 0 ]->control;
					stream_out.DrawPoint( control.z );
				}
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    quadratic scalar spline structure function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int QuadraticScalarSpline::findSpanIndex( const float t_in, float& t_out ) const
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

void QuadraticScalarSpline::updatePolynomial( Span& span ) const
{
	switch( form )
	{
	case( EQuadraticBezier ):
		{
			span.polynomial.SetMul( span.control, consts::MAT33::BEZIER );
			break;
		}
    case( EQuadraticBSpline ):
		{
			span.polynomial.SetMul( span.control, consts::MAT33::B_SPLINE );
			break;
		}
    case( EQuadraticNonUniformBSpline ):
		{
			mat33 basis;
			basis.SetBSpline( span.knots );
			span.polynomial.SetMul( span.control, basis );
			break;
		}
	default:
		{
			break;
		}
	}
}

void QuadraticScalarSpline::draw( ISplineDrawLineStreamOut& stream_out, const uint divisions ) const
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
