
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   quat.h
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	Quaternion container and functions.
////
////    	Notes:
////
////    		Twist, turn and tilt are post-multipliers (rotate relative to the world axes).
////    		Roll, pitch and yaw are pre-multipliers (rotate relative to the local axes).
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

#ifndef	__MATHS_QUAT_INCLUDED__
#define	__MATHS_QUAT_INCLUDED__

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

struct quat;
struct vec2;
struct vec3;
struct vec4;
struct plane;
struct mat33;
struct qtangent;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    consts
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace consts
{
	namespace QUAT
	{
		extern const quat		IDENTITY;
	};
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    quat
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct quat
{
	float					x, y, z, w;
	inline vec3&			t_vec3( void ) { return( *reinterpret_cast< vec3* >( this ) ); };
	inline const vec3&		t_vec3( void ) const { return( *reinterpret_cast< const vec3* >( this ) ); };
	inline vec4&			t_vec4( void ) { return( *reinterpret_cast< vec4* >( this ) ); };
	inline const vec4&		t_vec4( void ) const { return( *reinterpret_cast< const vec4* >( this ) ); };
	inline plane&			t_plane( void ) { return( *reinterpret_cast< plane* >( this ) ); };
	inline const plane&		t_plane( void ) const { return( *reinterpret_cast< const plane* >( this ) ); };
	inline qtangent&		t_qtangent( void ) { return( *reinterpret_cast< qtangent* >( this ) ); };
	inline const qtangent&	t_qtangent( void ) const { return( *reinterpret_cast< const qtangent* >( this ) ); };
	bool					IsNan( void ) const;
	bool					IsReal( void ) const;
	void					Transform( const vec3& src, vec3& trg ) const;
	void					InverseTransform( const vec3& src, vec3& trg ) const;
	void					UnscaledTransform( const vec3& src, vec3& trg ) const;
	void					UnscaledInverseTransform( const vec3& src, vec3& trg ) const;
	void					ConjugateTransform( const vec3& src, vec3& trg ) const;
	float					Length( void ) const;
	float					SqrLen( void ) const;
	float					Dot( const quat& dot ) const;
	void					PointTo( const vec3& to, const vec3& up );
	void					LookTo( const vec3& to, const vec3& up );
	void					Scale( const float scale );
	void					Twist( const float radians );
	void					Tilt( const float radians );
	void					Turn( const float radians );
	void					Roll( const float radians );
	void					Pitch( const float radians );
	void					Yaw( const float radians );
	void					Add( const quat& add );
	void					Sub( const quat& sub );
	void					Mul( const quat& mul );
	void					MulByConjugate( const quat& conj );
	void					Mul( const float mul );
	void					Div( const quat& div );
	void					Div( const float div );
	void					Pow( const quat& pow );
	void					Pow( const float pow );
	void					Log( void );
	void					Exp( void );
	void					Negate( void );
	void					Invert( void );
	void					Conjugate( void );
	void					Normalize( void );
	void					Lerp( const quat& trg, const float t );
	void					Slerp( const quat& trg, const float t );
	void					Blend( const quat& trg, const float t );
	void					SetAdd( const quat& src, const quat& add );
	void					SetSub( const quat& src, const quat& sub );
	void					SetMul( const quat& src, const quat& mul );
	void					SetMulByConjugate( const quat& src, const quat& conj );
	void					SetMul( const quat& src, const float mul );
	void					SetDiv( const quat& src, const quat& div );
	void					SetDiv( const quat& src, const float div );
	void					SetPow( const quat& src, const quat& pow );
	void					SetPow( const quat& src, const float pow );
	void					SetLog( const quat& src );
	void					SetExp( const quat& src );
	void					SetNegate( const quat& src );
	void					SetInverse( const quat& src );
	void					SetConjugate( const quat& src );
	void					SetNormalized( const quat& src );
    inline void             SetUnit( const quat& src ) { SetNormalized( src ); };
	void					SetLerp( const quat& src, const quat& trg, const float t );
	void					SetSlerp( const quat& src, const quat& trg, const float t );
	void					SetBlend( const quat& src, const quat& trg, const float t );
	void					SetDelta( const quat& src, const quat& trg );
	void					SetPoint( const float px, const float py, const float pz );
	void					SetPoint( const vec3& point );
	void					SetRemap( const vec3& src, const vec3& trg );
	void					SetNormalized( const float ni, const float nj, const float nk, const float nr );
	void					SetUnit( const float ni, const float nj, const float nk, const float nr ) { SetNormalized( ni, nj, nk, nr ); };
	inline void				SetTwistTiltTurn( const float twist, const float tilt, const float turn ) { SetYawPitchRoll( turn, tilt, twist ); };
	inline void				SetTurnTiltTwist( const float turn, const float tilt, const float twist ) { SetRollPitchYaw( twist, tilt, turn ); };
	void					SetRollPitchYaw( const float roll, const float pitch, const float yaw );
	void					SetYawPitchRoll( const float yaw, const float pitch, const float roll );
	inline void				SetTiltTurn( const float tilt, const float turn ) { SetYawPitch( turn, tilt ); };
	inline void				SetTurnTilt( const float turn, const float tilt ) { SetPitchYaw( tilt, turn ); };
	void					SetPitchYaw( const float pitch, const float yaw );
	void					SetYawPitch( const float yaw, const float pitch );
	void					SetAxisAngle( const float axis_x, const float axis_y, const float axis_z, const float radians );
	void					SetAxisAngle( const vec3& axis, const float radians );
	void					SetScaling( const float scale );
	inline void				SetIdentity( void ) { Set( consts::QUAT::IDENTITY ); };
	void					Set( const quat& q );
	void					Set( const mat33& m );
	void					Set( const float i, const float j, const float k, const float r );
	void					GetNormalized( float& ni, float& nj, float& nk, float& nr ) const;
	inline void				GetTwistTiltTurn( float& twist, float& tilt, float& turn ) const { GetYawPitchRoll( turn, tilt, twist ); };
	inline void				GetTurnTiltTwist( float& turn, float& tilt, float& twist ) const { GetRollPitchYaw( twist, tilt, turn ); };
	void					GetRollPitchYaw( float& roll, float& pitch, float& yaw ) const;
	void					GetYawPitchRoll( float& yaw, float& pitch, float& roll ) const;
	void					GetAxisAngle( float& axis_x, float& axis_y, float& axis_z, float& radians ) const;
	void					GetAxisAngle( vec3& axis, float& radians ) const;
	inline float			GetScaling( void ) const { return( SqrLen() ); };
	void					Get( quat& q ) const;
	void					Get( mat33& m ) const;
	void					Get( float& i, float& j, float& k, float& r ) const;
	void					GetInverse( mat33& m ) const;
	void					GetInverseTranspose( mat33& m ) const;
	inline quat&			operator=( const quat& q ) { Set( q ); return( *this ); };
	inline quat&			operator=( const mat33& m ) { Set( m ); return( *this ); };
	inline quat&			operator+=( const quat& add ) { Add( add ); return( *this ); };
	inline quat&			operator-=( const quat& sub ) { Sub( sub ); return( *this ); };
	inline quat&			operator*=( const quat& mul ) { Mul( mul ); return( *this ); };
	inline quat&			operator*=( const float mul ) { Mul( mul ); return( *this ); };
	inline quat&			operator/=( const quat& div ) { Div( div ); return( *this ); };
	inline quat&			operator/=( const float div ) { Div( div ); return( *this ); };
};

inline quat operator+( const quat& src, const quat& add ) { quat q; q.SetAdd( src, add ); return( q ); };
inline quat operator-( const quat& src, const quat& sub ) { quat q; q.SetSub( src, sub ); return( q ); };
inline quat operator*( const quat& src, const quat& mul ) { quat q; q.SetMul( src, mul ); return( q ); };
inline quat operator*( const quat& src, const float mul ) { quat q; q.SetMul( src, mul ); return( q ); };
inline quat operator/( const quat& src, const quat& div ) { quat q; q.SetDiv( src, div ); return( q ); };
inline quat operator/( const quat& src, const float div ) { quat q; q.SetDiv( src, div ); return( q ); };
inline quat operator-( const quat& src ) { quat q; q.SetNegate( src ); return( q ); };

inline quat normalize( const quat& src ) { quat q; q.SetNormalized( src ); return( q ); };
inline quat invert( const quat& src ) { quat q; q.SetInverse( src ); return( q ); };
inline quat conjugate( const quat& src ) { quat q; q.SetConjugate( src ); return( q ); };
inline quat pow( const quat& src, const quat& pow ) { quat q; q.SetPow( src, pow ); return( q ); };
inline quat pow( const quat& src, const float pow ) { quat q; q.SetPow( src, pow ); return( q ); };
inline quat log( const quat& src ) { quat q; q.SetLog( src ); return( q ); };
inline quat exp( const quat& src ) { quat q; q.SetExp( src ); return( q ); };
inline quat lerp( const quat& a, const quat& b, const float t ) { quat q; q.SetLerp( a, b, t ); return( q ); };
inline quat slerp( const quat& a, const quat& b, const float t ) { quat q; q.SetSlerp( a, b, t ); return( q ); };
inline quat blend( const quat& a, const quat& b, const float t ) { quat q; q.SetBlend( a, b, t ); return( q ); };

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

#endif	//	#ifndef	__MATHS_QUAT_INCLUDED__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

