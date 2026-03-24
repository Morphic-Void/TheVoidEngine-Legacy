
////    File:   types.h
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	Standard types.

#pragma once

#ifndef	__SYSTEM_TYPES_INCLUDED__
#define	__SYSTEM_TYPES_INCLUDED__

#include <crtdefs.h>

////    platform configuration

#ifdef _WIN32
#define	RESTRICT	__restrict
#define	FRESTRICT	__declspec(restrict)
#define	NOVTABLE	__declspec(novtable)
#define	NOALIAS		__declspec(noalias)
#ifdef __cplusplus
#define EXPORT      extern "C" __declspec(dllexport)
#define IMPORT      extern "C" __declspec(dllimport)
#else
#define EXPORT      __declspec(dllexport)
#define IMPORT      __declspec(dllimport)
#endif
#else
#define	RESTRICT	__restrict__
#define	FRESTRICT	__restrict__
#define	NOVTABLE
#define	NOALIAS
#define	EXPORT
#define	IMPORT
#endif

////    common defines

#ifndef	NULL
#define	NULL				0
#endif

#ifndef	TRUE
#define	TRUE				1
#endif

#ifndef	FALSE
#define	FALSE				0
#endif

#ifndef	YES
#define	YES					1
#endif

#ifndef	NO
#define	NO					0
#endif

////    compatibility types

#ifdef _WIN32
typedef int					BOOL;
#else
typedef signed char			BOOL;
#endif
typedef unsigned char		BYTE;
typedef unsigned short		WORD;
typedef unsigned long		DWORD;
typedef char				CHAR;
typedef short				SHORT;
typedef long				LONG;
typedef int					INT;
typedef unsigned char		UCHAR;
typedef unsigned short		USHORT;
typedef unsigned long		ULONG;
typedef unsigned int		UINT;
typedef float				FLOAT;
typedef double				DOUBLE;

////    standard types

typedef	signed long long	longlong;
typedef	unsigned long long	ulonglong;
typedef	unsigned int		uint;
typedef	unsigned long		ulong;
typedef	unsigned short		ushort;
typedef	unsigned char		uchar;

////    storage size specific types

typedef signed char			int8_t;
typedef unsigned char		uint8_t;
typedef signed short		int16_t;
typedef unsigned short		uint16_t;
typedef signed int			int32_t;
typedef unsigned int		uint32_t;
typedef float				float32_t;
typedef signed long long	int64_t;
typedef unsigned long long	uint64_t;
typedef double				float64_t;
typedef ptrdiff_t		    intptr_t;
typedef size_t		        uintptr_t;

////    alternative storage size specific floating point types

typedef float				fp32;
typedef double				fp64;

////    aliased storage size specific types

union alias8_t				{ int8_t i; uint8_t u; };
union alias16_t				{ int16_t i; uint16_t u; };
union alias32_t				{ int32_t i; uint32_t u; float32_t f; };
union alias64_t				{ int64_t i; uint64_t u; float64_t f; };
union aliasptr_t			{ intptr_t i; uintptr_t u; void* ptr; };

////    inline type dependent floating point aliasing helper functions

inline bool			IsNan(const float32_t f)            {return(f!=f);};
inline bool			IsReal(const float32_t& f)          {return((reinterpret_cast<const alias32_t&>(f).u&0x7fffffff)<0x7f800000);};
inline float32_t	Uint32AsFloat(const uint32_t& u)    {return(reinterpret_cast<const alias32_t&>(u).f);};
inline uint32_t		FloatAsUint32(const float32_t& f)   {return(reinterpret_cast<const alias32_t&>(f).u);};
inline float64_t	Uint64AsDouble(const uint64_t& u)   {return(reinterpret_cast<const alias64_t&>(u).f);};
inline uint64_t		DoubleAsUint64(const float64_t& f)  {return(reinterpret_cast<const alias64_t&>(f).u);};

////    standard integer coordinate structures

struct SVEC2    { int x, y; };
struct SVEC3    { int x, y, z; };
struct SVEC4    { int x, y, z, w; };
struct SPOINT   { int x, y; };
struct SRECT    { int left, top, right, bottom; };
struct SBOX     { SVEC3 min, max; };
struct DIMS2D   { uint width, height; };

////    standard floating point coordinate structures

struct FVEC2    { float x, y; };
struct FVEC3    { float x, y, z; };
struct FVEC4    { float x, y, z, w; };
struct FPOINT   { float x, y; };
struct FRECT    { float left, top, right, bottom; };
struct FBOX     { FVEC3 min, max; };

////    interface type macros

#define INTERFACE_DECL( name )                      struct NOVTABLE name
#define INTERFACE_DECL_INHERIT( name, inherit )     struct NOVTABLE name : public inherit
#define INTERFACE_DECL_VIRTUAL( name, inherit )     struct NOVTABLE name : public virtual inherit

#define INTERFACE_BEGIN( name )                     struct NOVTABLE name { protected: inline name() {}; inline ~name() {}; public:
#define INTERFACE_BEGIN_INHERIT( name, inherit )    struct NOVTABLE name : public inherit { protected: inline name() {}; inline ~name() {}; public:
#define INTERFACE_BEGIN_VIRTUAL( name, inherit )    struct NOVTABLE name : public virtual inherit { protected: inline name() {}; inline ~name() {}; public:

#define INTERFACE_END()			                    }

////    inline type-safe array element count template

template<typename T,size_t N>
inline size_t INDEX_COUNT(const T(&)[N]) {return(N);};

////    common macros

////    unused parameter error suppression
#define UNUSED(x)           (void)(x)

////    struct and class member offset
#define	OFFSET_OF(C,M)      static_cast<size_t>(&(reinterpret_cast<const C* const>(0)->M))

////    array element count (not type-safe)
#define ARRAY_SIZE(A)		(sizeof(a)/sizeof(*a))

////    swap integer values:
#define	SWAP(a,b)           do{(a)^=(b);(b)^=(a);(a)^=(b);}while(true,false)

////    min/max/mid macros:
#define MIN2(a,b)           (((a)<(b))?(a):(b))
#define MAX2(a,b)           (((a)>(b))?(a):(b))
#define MIN3(a,b,c)         (((a)<(c))?(((a)<(b))?(a):(b)):(((b)<(c))?(b):(c)))
#define MAX3(a,b,c)         (((a)>(c))?(((a)>(b))?(a):(b)):(((b)>(c))?(b):(c)))
#define MID3(a,b,c)         (((a)<(c))?(((b)<(c))?(((b)<(a))?(a):(b)):(c)):(((b)<(c))?(c):(((b)<(a))?(b):(a))))

////    min/max/mid index macros:
#define IMIN2(a)            (((a)[0]<(a)[1])?0:1)
#define IMAX2(a)            (((a)[0]>(a)[1])?0:1)
#define IMIN3(a)            (((a)[0]<(a)[2])?(((a)[0]<(a)[1])?0:1):(((a)[1]<(a)[2])?1:2))
#define IMAX3(a)            (((a)[0]>(a)[2])?(((a)[0]>(a)[1])?0:1):(((a)[1]>(a)[2])?1:2))
#define IMID3(a)            (((a)[0]<(a)[2])?(((a)[1]<(a)[2])?(((a)[1]<(a)[0])?0:1):2):(((a)[1]<(a)[2])?2:(((a)[1]<(a)[0])?1:0)))

////    check value in range:
#define INRANGE(l,h,v)      (((v)>=(l))&&((v)<=(h)))

////    common maths functions:
#define SQR(n)              ((n)*(n))
#define CUBE(n)             ((n)*(n)*(n))
#define SUMN(n)             (((n)*((n)+1))/2)
#define SUMSQRS(n)          (((n)*((n)+1)*((n)+(n)+1))/6)
#define SUMCUBES(n)         (SQR(SUMN(n))/4)

////    common floating point only maths functions:
#define SUMPOWS(p)          (1/(1-(p)))
#define REINHARD(n)         ((n)/(1+(n)))
#define INVREINHARD(n)      ((n)/(1-(n)))

////    4 character code construction
#define MAKE4CC(a,b,c,d)    (((((((static_cast<uint32_t>(d)&255)<<8)+(static_cast<uint32_t>(c)&255))<<8)+(static_cast<uint32_t>(b) &255))<<8)+(static_cast<uint32_t>(a)&255))

#endif	//	#ifndef	__SYSTEM_TYPES_INCLUDED__
