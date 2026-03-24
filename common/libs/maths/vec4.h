
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   vec4.h
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	4 element vector container and functions.
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    include guard begin
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef	__MATHS_VEC4_INCLUDED__
#define	__MATHS_VEC4_INCLUDED__

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
////    forward declarations
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct vec4;
struct vec3;
struct vec2;
struct quat;
struct dquat;
struct plane;
struct joint;
struct mat33;
struct mat43;
struct mat44;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    consts
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace consts
{
	namespace VEC4
	{
		extern const vec4		ORIGIN;
		extern const vec4		UNIT_X;
		extern const vec4		UNIT_Y;
		extern const vec4		UNIT_Z;
		extern const vec4		UNIT_W;
		extern const vec4		ZERO;
		extern const vec4		ONE;
	};
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    vec4
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct vec4
{
	float					x, y, z, w;
	inline vec2&			t_vec2( void ) { return( *reinterpret_cast< vec2* >( this ) ); };
	inline const vec2&		t_vec2( void ) const { return( *reinterpret_cast< const vec2* >( this ) ); };
	inline vec3&			t_vec3( void ) { return( *reinterpret_cast< vec3* >( this ) ); };
	inline const vec3&		t_vec3( void ) const { return( *reinterpret_cast< const vec3* >( this ) ); };
	inline quat&			t_quat( void ) { return( *reinterpret_cast< quat* >( this ) ); };
	inline const quat&		t_quat( void ) const { return( *reinterpret_cast< const quat* >( this ) ); };
	inline plane&			t_plane( void ) { return( *reinterpret_cast< plane* >( this ) ); };
	inline const plane&		t_plane( void ) const { return( *reinterpret_cast< const plane* >( this ) ); };
	bool					IsNan( void ) const;
	bool					IsReal( void ) const;
	float					Manhattan( void ) const;
	float					Chebyshev( void ) const;
	float					Length( void ) const;
	float					SqrLen( void ) const;
	void					Abs( void );
	void					Sat( void );
	void					Frac( void );
	void					Floor( void );
	void					Ceil( void );
	float					Dot( const vec3& dot ) const;
	float					Dot( const vec4& dot ) const;
	float					Dot( const plane& dot ) const;
	void					Add( const vec4& add );
	void					Sub( const vec4& sub );
	void					Mul( const vec4& mul );
	void					Mul( const float mul );
	void					Mul( const quat& mul );
	void					Mul( const dquat& mul );
	void					Mul( const joint& mul );
	void					Mul( const mat33& mul );
	void					Mul( const mat43& mul );
	void					Mul( const mat44& mul );
	void					Div( const vec4& div );
	void					Div( const float div );
	void					Lerp( const vec4& trg, const float t );
	void					Negate( void );
	void					Normalize( void );
	void					SetAbs( const vec4& src );
	void					SetSat( const vec4& src );
	void					SetFrac( const vec4& src );
	void					SetFloor( const vec4& src );
	void					SetCeil( const vec4& src );
	void					SetAdd( const vec4& src, const vec4& add );
	void					SetSub( const vec4& src, const vec4& sub );
	void					SetMul( const vec4& src, const vec4& mul );
	void					SetMul( const vec4& src, const float mul );
	void					SetMul( const vec4& src, const quat& mul );
	void					SetMul( const vec4& src, const dquat& mul );
	void					SetMul( const vec4& src, const joint& mul );
	void					SetMul( const vec4& src, const mat33& mul );
	void					SetMul( const vec4& src, const mat43& mul );
	void					SetMul( const vec4& src, const mat44& mul );
	void					SetMul( const vec3& src, const mat44& mul );
	void					SetDiv( const vec4& src, const vec4& div );
	void					SetDiv( const vec4& src, const float div );
	void					SetLerp( const vec4& src, const vec4& trg, const float t );
	void					SetNegate( const vec4& src );
	void					SetNormalized( const vec4& src );
    inline void             SetUnit( const vec4& src ) { SetNormalized( src ); };
	void					Set( const float vx, const float vy, const float vz, const float vw = 1.0f );
	void					Set( const vec4& v );
	void					Set( const vec3& v );
	void					Get( float& vx, float& vy, float& vz, float& vw ) const;
	void					Get( vec4& v ) const;
	void					Get( vec3& v ) const;
	float					EvaluatePolynomial( const float t ) const;
	float					EvaluatePolynomial( const float t, float& first ) const;
	float					EvaluatePolynomial( const float t, float& first, float& second ) const;
	float					EvaluateDerivative( const float t ) const;
	void					EvaluateDerivatives( const float t, float& first, float& second ) const;
	float					IntegratePolynomialLength( const float lower_t, const float upper_t ) const;
	float					IntegratePolynomialLength( const unsigned int steps, const float lower_t, const float upper_t ) const;
	inline vec4&			operator=( const vec4& v ) { Set( v ); return( *this ); };
	inline vec4&			operator+=( const vec4& v ) { Add( v ); return( *this ); };
	inline vec4&			operator-=( const vec4& v ) { Sub( v ); return( *this ); };
	inline vec4&			operator*=( const vec4& v ) { Mul( v ); return( *this ); };
	inline vec4&			operator*=( const float f ) { Mul( f ); return( *this ); };
	inline vec4&			operator/=( const vec4& v ) { Div( v ); return( *this ); };
	inline vec4&			operator/=( const float f ) { Div( f ); return( *this ); };
	inline vec4&			operator*=( const quat& q ) { Mul( q ); return( *this ); };
	inline vec4&			operator*=( const joint& j ) { Mul( j ); return( *this ); };
	inline vec4&			operator*=( const mat33& m ) { Mul( m ); return( *this ); };
	inline vec4&			operator*=( const mat43& m ) { Mul( m ); return( *this ); };
	inline vec4&			operator*=( const mat44& m ) { Mul( m ); return( *this ); };
};

inline vec4 operator+( const vec4& src, const vec4& add ) { vec4 v; v.SetAdd( src, add ); return( v ); };
inline vec4 operator-( const vec4& src, const vec4& sub ) { vec4 v; v.SetSub( src, sub ); return( v ); };
inline vec4 operator*( const vec4& src, const vec4& mul ) { vec4 v; v.SetMul( src, mul ); return( v ); };
inline vec4 operator*( const vec4& src, const float mul ) { vec4 v; v.SetMul( src, mul ); return( v ); };
inline vec4 operator*( const vec4& src, const quat& mul ) { vec4 v; v.SetMul( src, mul ); return( v ); };
inline vec4 operator*( const vec4& src, const dquat& mul ) { vec4 v; v.SetMul( src, mul ); return( v ); };
inline vec4 operator*( const vec4& src, const joint& mul ) { vec4 v; v.SetMul( src, mul ); return( v ); };
inline vec4 operator*( const vec4& src, const mat33& mul ) { vec4 v; v.SetMul( src, mul ); return( v ); };
inline vec4 operator*( const vec4& src, const mat43& mul ) { vec4 v; v.SetMul( src, mul ); return( v ); };
inline vec4 operator*( const vec4& src, const mat44& mul ) { vec4 v; v.SetMul( src, mul ); return( v ); };
inline vec4 operator*( const vec3& src, const mat44& mul ) { vec4 v; v.SetMul( src, mul ); return( v ); };
inline vec4 operator/( const vec4& src, const vec4& div ) { vec4 v; v.SetDiv( src, div ); return( v ); };
inline vec4 operator/( const vec4& src, const float div ) { vec4 v; v.SetDiv( src, div ); return( v ); };
inline vec4 operator-( const vec4& src ) { vec4 v; v.SetNegate( src ); return( v ); };

inline vec4 normalize( const vec4& src ) { vec4 v; v.SetNormalized( src ); return( v ); };
inline vec4 abs( const vec4& src ) { vec4 v; v.SetAbs( src ); return( v ); };
inline vec4 sat( const vec4& src ) { vec4 v; v.SetSat( src ); return( v ); };
inline vec4 frac( const vec4& src ) { vec4 v; v.SetFrac( src ); return( v ); };
inline vec4 floor( const vec4& src ) { vec4 v; v.SetFloor( src ); return( v ); };
inline vec4 ceil( const vec4& src ) { vec4 v; v.SetCeil( src ); return( v ); };
inline vec4 lerp( const vec4& a, const vec4& b, const float t ) { vec4 v; v.SetLerp( a, b, t ); return( v ); };
inline float dot( const vec4& a, const vec3& b ) { return( a.Dot( b ) ); };
inline float dot( const vec4& a, const vec4& b ) { return( a.Dot( b ) ); };
inline float dot( const vec4& a, const plane& b ) { return( a.Dot( b ) ); };

class cvec4 : public vec4
{
public:
	inline					cvec4() {};
	inline					cvec4( const float vx, const float vy, const float vz, const float vw ) { Set( vx, vy, vz, vw ); };
	inline					~cvec4() {};
	inline					operator vec4() { return( *reinterpret_cast< vec4* >( this ) ); };
	inline					operator vec4() const { return( *reinterpret_cast< const vec4* >( this ) ); };
};

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
////    include guard end
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif	//	#ifndef	__MATHS_VEC4_INCLUDED__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

