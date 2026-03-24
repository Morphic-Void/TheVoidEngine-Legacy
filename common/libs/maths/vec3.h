
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   vec3.h
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	3 element vector container and functions.
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

#ifndef	__MATHS_VEC3_INCLUDED__
#define	__MATHS_VEC3_INCLUDED__

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
	namespace VEC3
	{
		extern const vec3		ORIGIN;
		extern const vec3		UNIT_X;
		extern const vec3		UNIT_Y;
		extern const vec3		UNIT_Z;
		extern const vec3		ZERO;
		extern const vec3		ONE;
	};
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    vec3
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct vec3
{
	float					x, y, z, w;
	inline vec2&			t_vec2( void ) { return( *reinterpret_cast< vec2* >( this ) ); };
	inline const vec2&		t_vec2( void ) const { return( *reinterpret_cast< const vec2* >( this ) ); };
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
	float					Dot( const plane& dot ) const;
	float					Triple( const vec3& u, const vec3& v ) const;
	void					Add( const vec3& add );
	void					Sub( const vec3& sub );
	void					Mul( const vec3& mul );
	void					Mul( const float mul );
	void					Mul( const quat& mul );
	void					Mul( const dquat& mul );
	void					Mul( const joint& mul );
	void					Mul( const mat33& mul );
	void					Mul( const mat43& mul );
	bool					Mul( const mat44& mul );
	void					Div( const vec3& div );
	void					Div( const float div );
	void					Lerp( const vec3& trg, const float t );
	void					Cross( const vec3& cross, vec3& trg ) const;
	void					Negate( void );
	void					Normalize( void );
	void					SetAbs( const vec3& src );
	void					SetSat( const vec3& src );
	void					SetFrac( const vec3& src );
	void					SetFloor( const vec3& src );
	void					SetCeil( const vec3& src );
	void					SetAdd( const vec3& src, const vec3& add );
	void					SetSub( const vec3& src, const vec3& sub );
	void					SetMul( const vec3& src, const vec3& mul );
	void					SetMul( const vec3& src, const float mul );
	void					SetMul( const vec3& src, const quat& mul );
	void					SetMul( const vec3& src, const dquat& mul );
	void					SetMul( const vec3& src, const joint& mul );
	void					SetMul( const vec3& src, const mat33& mul );
	void					SetMul( const vec3& src, const mat43& mul );
	bool					SetMul( const vec3& src, const mat44& mul );
	bool					SetMul( const vec4& src, const mat44& mul );
	void					SetDiv( const vec3& src, const vec3& div );
	void					SetDiv( const vec3& src, const float div );
	void					SetLerp( const vec3& src, const vec3& trg, const float t );
	void					SetCross( const vec3& src, const vec3& cross );
	void					SetTriple( const vec3& b, const vec3& c );
	void					SetTriple( const vec3& a, const vec3& b, const vec3& c );
	void					SetNegate( const vec3& src );
	void					SetNormalized( const vec3& src );
	bool					SetProjected( const vec4& src );
    inline void             SetUnit( const vec3& src ) { SetNormalized( src ); };
	void					Set( const float vx, const float vy, const float vz );
	void					Set( const vec3& v );
	void					Get( float& vx, float& vy, float& vz ) const;
	void					Get( vec3& v ) const;
	bool					PointIsInTriangle( const vec3& vtx1, const vec3& vtx2, const vec3& vtx3 ) const;
	float					EvaluatePolynomial( const float t ) const;
	float					EvaluatePolynomial( const float t, float& first ) const;
	float					EvaluateDerivative( const float t ) const;
	float					IntegratePolynomialLength( const float lower_t, const float upper_t ) const;
	float					IntegratePolynomialLength( const unsigned int steps, const float lower_t, const float upper_t ) const;
	inline vec3&			operator=( const vec3& v ) { Set( v ); return( *this ); };
	inline vec3&			operator+=( const vec3& v ) { Add( v ); return( *this ); };
	inline vec3&			operator-=( const vec3& v ) { Sub( v ); return( *this ); };
	inline vec3&			operator*=( const vec3& v ) { Mul( v ); return( *this ); };
	inline vec3&			operator*=( const float f ) { Mul( f ); return( *this ); };
	inline vec3&			operator/=( const vec3& v ) { Div( v ); return( *this ); };
	inline vec3&			operator/=( const float f ) { Div( f ); return( *this ); };
	inline vec3&			operator*=( const quat& q ) { Mul( q ); return( *this ); };
	inline vec3&			operator*=( const joint& j ) { Mul( j ); return( *this ); };
	inline vec3&			operator*=( const mat33& m ) { Mul( m ); return( *this ); };
	inline vec3&			operator*=( const mat43& m ) { Mul( m ); return( *this ); };
	inline vec3&			operator*=( const mat44& m ) { Mul( m ); return( *this ); };
};

inline vec3 operator+( const vec3& src, const vec3& add ) { vec3 v; v.SetAdd( src, add ); return( v ); };
inline vec3 operator-( const vec3& src, const vec3& sub ) { vec3 v; v.SetSub( src, sub ); return( v ); };
inline vec3 operator*( const vec3& src, const vec3& mul ) { vec3 v; v.SetMul( src, mul ); return( v ); };
inline vec3 operator*( const vec3& src, const float mul ) { vec3 v; v.SetMul( src, mul ); return( v ); };
inline vec3 operator*( const vec3& src, const quat& mul ) { vec3 v; v.SetMul( src, mul ); return( v ); };
inline vec3 operator*( const vec3& src, const dquat& mul ) { vec3 v; v.SetMul( src, mul ); return( v ); };
inline vec3 operator*( const vec3& src, const joint& mul ) { vec3 v; v.SetMul( src, mul ); return( v ); };
inline vec3 operator*( const vec3& src, const mat33& mul ) { vec3 v; v.SetMul( src, mul ); return( v ); };
inline vec3 operator*( const vec3& src, const mat43& mul ) { vec3 v; v.SetMul( src, mul ); return( v ); };
inline vec3 operator/( const vec3& src, const vec3& div ) { vec3 v; v.SetDiv( src, div ); return( v ); };
inline vec3 operator/( const vec3& src, const float div ) { vec3 v; v.SetDiv( src, div ); return( v ); };
inline vec3 operator-( const vec3& src ) { vec3 v; v.SetNegate( src ); return( v ); };

inline vec3 normalize( const vec3& src ) { vec3 v; v.SetNormalized( src ); return( v ); };
inline vec3 abs( const vec3& src ) { vec3 v; v.SetAbs( src ); return( v ); };
inline vec3 sat( const vec3& src ) { vec3 v; v.SetSat( src ); return( v ); };
inline vec3 frac( const vec3& src ) { vec3 v; v.SetFrac( src ); return( v ); };
inline vec3 floor( const vec3& src ) { vec3 v; v.SetFloor( src ); return( v ); };
inline vec3 ceil( const vec3& src ) { vec3 v; v.SetCeil( src ); return( v ); };
inline vec3 cross( const vec3& a, const vec3& b ) { vec3 v; v.SetCross( a, b ); return( v ); };
inline vec3 lerp( const vec3& a, const vec3& b, const float t ) { vec3 v; v.SetLerp( a, b, t ); return( v ); };
inline float dot( const vec3& a, const vec3& b ) { return( a.Dot( b ) ); };
inline float dot( const vec3& a, const plane& b ) { return( a.Dot( b ) ); };
inline float triple( const vec3& a, const vec3& b, const vec3& c ) { return( a.Triple( b, c ) ); };

class cvec3 : public vec3
{
public:
	inline					cvec3() {};
	inline					cvec3( const float vx, const float vy, const float vz ) { Set( vx, vy, vz ); };
	inline					~cvec3() {};
	inline					operator vec3() { return( *reinterpret_cast< vec3* >( this ) ); };
	inline					operator vec3() const { return( *reinterpret_cast< const vec3* >( this ) ); };
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

#endif	//	#ifndef	__MATHS_VEC3_INCLUDED__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

