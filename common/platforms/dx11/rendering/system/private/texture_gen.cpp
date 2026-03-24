
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   texture_gen.cpp
////    Author: Ritchie Brannan
////    Date:   16 Dec 14
////
////    Description:
////
////        Standard texture generation class.
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

#include "libs/system/debug/asserts.h"
#include "libs/maths/consts.h"
#include "libs/maths/prand.h"
#include "libs/maths/noise.h"
#include "libs/utils/tga.h"
#include "texture_gen.h"
#include <math.h>
#include <stdio.h>

#include "libs/system/debug/log.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    begin rendering namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace rendering
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Standard texture generation class static member data
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* CStdTexGen::s_names[ EStdTexCount ] =
{
    "invalid",
    "permute1D",
    "permute1Dx4",
    "permute2D",
    "permute2Dx4",
    "permuteQuad2D",
    "random1D",
    "random1Dx4",
    "random2D",
    "random2Dx4",
    "randomQuad2D",
    "median2D",
    "medianQuad2D",
    "perlin2Dx4a",
    "perlin2Dx4b"
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Standard texture generation class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CStdTexGen::init( const EStdTexID id )
{
    switch( id )
    {
        case( EStdTexPermute1D ):
        {
            m_id = id;
            m_width = 256;
            m_height = 1;
            m_stride = 1;
            if( m_buffer == NULL ) m_buffer = new uint8_t[ 256 * 256 * 4 ];
            uint index = 0;
            for( uint x = 0; x < 256; ++x )
            {
                uint8_t value = static_cast< uint8_t >( maths::consts::Permute[ x + maths::consts::Permute[ 0 ] ] );
                m_buffer[ index ] = value;
                ++index;
            }
            break;
        }
        case( EStdTexPermute1Dx4 ):
        {
            m_id = id;
            m_width = 256;
            m_height = 1;
            m_stride = 4;
            if( m_buffer == NULL ) m_buffer = new uint8_t[ 256 * 256 * 4 ];
            uint index = 0;
            for( uint x = 0; x < 256; ++x )
            {
                uint8_t value = static_cast< uint8_t >( maths::consts::Permute[ x + maths::consts::Permute[ 0 ] ] );
                for( uint z = 0; z < 4; ++z )
                {
                    m_buffer[ index ] = value;
                    value = static_cast< uint8_t >( maths::consts::Permute[ value ] );
                    ++index;
                }
            }
            break;
        }
        case( EStdTexPermute2D ):
        {
            m_id = id;
            m_width = 256;
            m_height = 256;
            m_stride = 1;
            if( m_buffer == NULL ) m_buffer = new uint8_t[ 256 * 256 * 4 ];
            uint index = 0;
            for( uint y = 0; y < 256; ++y )
            {
                for( uint x = 0; x < 256; ++x )
                {
                    uint8_t value = static_cast< uint8_t >( maths::consts::Permute[ x + maths::consts::Permute[ y ] ] );
                    m_buffer[ index ] = value;
                    ++index;
                }
            }
            break;
        }
        case( EStdTexPermute2Dx4 ):
        {
            m_id = id;
            m_width = 256;
            m_height = 256;
            m_stride = 4;
            if( m_buffer == NULL ) m_buffer = new uint8_t[ 256 * 256 * 4 ];
            uint index = 0;
            for( uint y = 0; y < 256; ++y )
            {
                for( uint x = 0; x < 256; ++x )
                {
                    uint8_t value = static_cast< uint8_t >( maths::consts::Permute[ x + maths::consts::Permute[ y ] ] );
                    for( uint z = 0; z < 4; ++z )
                    {
                        m_buffer[ index ] = value;
                        value = static_cast< uint8_t >( maths::consts::Permute[ value ] );
                        ++index;
                    }
                }
            }
            break;
        }
        case( EStdTexPermuteQuad2D ):
        {
            m_id = id;
            m_width = 256;
            m_height = 256;
            m_stride = 4;
            if( m_buffer == NULL ) m_buffer = new uint8_t[ 256 * 256 * 4 ];
            uint index = 0;
            for( uint y = 0; y < 256; ++y )
            {
                for( uint x = 0; x < 256; ++x )
                {
                    m_buffer[ index ] = static_cast< uint8_t >( maths::consts::Permute[ x + maths::consts::Permute[ y ] ] );
                    index += 4;
                }
            }
            index = 0;
            for( uint y = 0; y < 256; ++y )
            {
                uint r = ( y << 8 );
                uint s = ( ( ( y + 1 ) & 255 ) << 8 );
                for( uint x = 0; x < 256; ++x )
                {   //  copy neighbours
                    uint t = ( ( x + 1 ) & 255 );
                    m_buffer[ index + 1 ] = m_buffer[ t + r ];
                    m_buffer[ index + 2 ] = m_buffer[ x + s ];
                    m_buffer[ index + 3 ] = m_buffer[ t + s ];
                    index += 4;
                }
            }
            break;
        }
        case( EStdTexRandom1D ):
        {
            m_id = id;
            m_width = 256;
            m_height = 1;
            m_stride = 1;
            if( m_buffer == NULL ) m_buffer = new uint8_t[ 256 * 256 * 4 ];
            uint index = 0;
            maths::PLCprng prng;
            for( uint x = 0; x < 256; ++x )
            {
                uint32_t r = prng.Rand();
                m_buffer[ index ] = static_cast< uint8_t >( r >> 24 );
                ++index;
            }
            break;
        }
        case( EStdTexRandom1Dx4 ):
        {
            m_id = id;
            m_width = 256;
            m_height = 1;
            m_stride = 4;
            if( m_buffer == NULL ) m_buffer = new uint8_t[ 256 * 256 * 4 ];
            uint index = 0;
            maths::PLCprng prng;
            for( uint x = 0; x < 256; ++x )
            {
                uint32_t r = prng.Rand();
                for( uint z = 0; z < 4; ++z )
                {
                    m_buffer[ index ] = static_cast< uint8_t >( r >> ( ( 3 - z ) << 3 ) );
                    ++index;
                }
            }
            break;
        }
        case( EStdTexRandom2D ):
        {
            m_id = id;
            m_width = 256;
            m_height = 256;
            m_stride = 1;
            if( m_buffer == NULL ) m_buffer = new uint8_t[ 256 * 256 * 4 ];
            uint index = 0;
            maths::PLCprng prng;
            for( uint y = 0; y < 256; ++y )
            {
                for( uint x = 0; x < 256; ++x )
                {
                    uint32_t r = prng.Rand();
                    m_buffer[ index ] = static_cast< uint8_t >( r >> 24 );
                    ++index;
                }
            }
            break;
        }
        case( EStdTexRandom2Dx4 ):
        {
            m_id = id;
            m_width = 256;
            m_height = 256;
            m_stride = 4;
            if( m_buffer == NULL ) m_buffer = new uint8_t[ 256 * 256 * 4 ];
            uint index = 0;
            maths::PLCprng prng;
            for( uint y = 0; y < 256; ++y )
            {
                for( uint x = 0; x < 256; ++x )
                {
                    uint32_t r = prng.Rand();
                    for( uint z = 0; z < 4; ++z )
                    {
                        m_buffer[ index ] = static_cast< uint8_t >( r >> ( ( 3 - z ) << 3 ) );
                        ++index;
                    }
                }
            }
            break;
        }
        case( EStdTexRandomQuad2D ):
        {
            m_id = id;
            m_width = 256;
            m_height = 256;
            m_stride = 4;
            if( m_buffer == NULL ) m_buffer = new uint8_t[ 256 * 256 * 4 ];
            uint index = 0;
            maths::PLCprng prng;
            for( uint y = 0; y < 256; ++y )
            {
                for( uint x = 0; x < 256; ++x )
                {
                    uint32_t r = prng.Rand();
                    m_buffer[ index ] = static_cast< uint8_t >( r >> 24 );
                    index += 4;
                }
            }
            index = 0;
            for( uint y = 0; y < 256; ++y )
            {
                uint r = ( y << 8 );
                uint s = ( ( ( y + 1 ) & 255 ) << 8 );
                for( uint x = 0; x < 256; ++x )
                {   //  copy neighbours
                    uint t = ( ( x + 1 ) & 255 );
                    m_buffer[ index + 1 ] = m_buffer[ t + r ];
                    m_buffer[ index + 2 ] = m_buffer[ x + s ];
                    m_buffer[ index + 3 ] = m_buffer[ t + s ];
                    index += 4;
                }
            }
            break;
        }
        case( EStdTexMedian2D ):
        {
            m_id = id;
            m_width = 256;
            m_height = 256;
            m_stride = 1;
            if( m_buffer == NULL ) m_buffer = new uint8_t[ 256 * 256 * 4 ];
            uint index = 0;
            uint8_t s = 128;
            uint8_t a = 255;
            uint8_t b = 254;
            for( uint y = 0; y < 256; ++y )
            {
                for( uint x = 0; x < 256; ++x )
                {
                    m_buffer[ index ] = s;
                    if( x & 1 )
                    {
                        s += a;
                    }
                    else
                    {
                        s -= a;
                    }
                    a -= 2;
                    ++index;
                }
                if( ( y ^ ( y >> 7 ) ) & 1 )
                {
                    s -= b;
                }
                else
                {
                    s += b;
                }
                if( y == 127 )
                {
                    --s;
                }
                b -= 2;
            }
            break;
        }
        case( EStdTexMedianQuad2D ):
        {
            m_id = id;
            m_width = 256;
            m_height = 256;
            m_stride = 4;
            if( m_buffer == NULL ) m_buffer = new uint8_t[ 256 * 256 * 4 ];
            uint index = 0;
            uint8_t s = 128;
            uint8_t a = 255;
            uint8_t b = 254;
            for( uint y = 0; y < 256; ++y )
            {
                for( uint x = 0; x < 256; ++x )
                {
                    m_buffer[ index ] = s;
                    if( x & 1 )
                    {
                        s += a;
                    }
                    else
                    {
                        s -= a;
                    }
                    a -= 2;
                    index += 4;
                }
                if( ( y ^ ( y >> 7 ) ) & 1 )
                {
                    s -= b;
                }
                else
                {
                    s += b;
                }
                if( y == 127 )
                {
                    --s;
                }
                b -= 2;
            }
            index = 0;
            for( uint y = 0; y < 256; ++y )
            {
                uint r = ( y << 8 );
                uint q = ( ( ( y + 1 ) & 255 ) << 8 );
                for( uint x = 0; x < 256; ++x )
                {   //  copy neighbours
                    uint t = ( ( x + 1 ) & 255 );
                    m_buffer[ index + 1 ] = m_buffer[ t + r ];
                    m_buffer[ index + 2 ] = m_buffer[ x + q ];
                    m_buffer[ index + 3 ] = m_buffer[ t + q ];
                    index += 4;
                }
            }
            break;
        }
        case( EStdTexPerlin2Dx4a ):
        {
            m_id = id;
            m_width = 256;
            m_height = 256;
            m_stride = 4;
            if( m_buffer == NULL ) m_buffer = new uint8_t[ 256 * 256 * 4 ];
            uint index = 0;
            for( uint y = 0; y < 256; ++y )
            {
                float fy = ( y * ( 1.0f / 256.0f ) );
                for( uint x = 0; x < 256; ++x )
                {
                    float fx = ( x * ( 1.0f / 256.0f ) );
                    for( uint z = 0; z < 4; ++z )
                    {
                        float r = static_cast< float >( 128 >> z );
                        float s = ( fx * r );
                        float t = ( fy * r );
                        int n = static_cast< int >( floorf( ( maths::PerlinNoise( s, t, ( 127u >> z ) ) * 128.0f ) + 128.0f ) );
                        if( n < 0 ) n = 0;
                        if( n > 255 ) n = 255;
                        m_buffer[ index ] = static_cast< uint8_t >( n );
                        ++index;
                    }
                }
            }
            break;
        }
        case( EStdTexPerlin2Dx4b ):
        {
            m_id = id;
            m_width = 256;
            m_height = 256;
            m_stride = 4;
            if( m_buffer == NULL ) m_buffer = new uint8_t[ 256 * 256 * 4 ];
            uint index = 0;
            for( uint y = 0; y < 256; ++y )
            {
                float fy = ( y * ( 1.0f / 256.0f ) );
                for( uint x = 0; x < 256; ++x )
                {
                    float fx = ( x * ( 1.0f / 256.0f ) );
                    for( uint z = 0; z < 4; ++z )
                    {
                        float r = static_cast< float >( 8 >> z );
                        float s = ( fx * r );
                        float t = ( fy * r );
                        int n = static_cast< int >( floorf( ( maths::PerlinNoise( s, t, ( 7u >> z ) ) * 128.0f ) + 128.0f ) );
                        if( n < 0 ) n = 0;
                        if( n > 255 ) n = 255;
                        m_buffer[ index ] = static_cast< uint8_t >( n );
                        ++index;
                    }
                }
            }
            break;
        }
        default:
        {
            kill();
            break;
        }
    }
}

void CStdTexGen::kill()
{
    if( m_buffer != NULL ) delete[] m_buffer;
    zero();
}

void CStdTexGen::zero()
{
    m_id = EStdTexInvalid;
    m_width = 0;
    m_height = 0;
    m_stride = 0;
    m_buffer = NULL;
}

void CStdTexGen::writeTGA()
{
    char name[ 64 ];
    if( valid() )
    {
        sprintf_s( name, sizeof( name ), "output/%s.tga", s_names[ m_id ] );
        if( m_stride == 1 )
        {
            file::saveTGA_Grey( name, m_buffer, static_cast< uint16_t >( m_width ), static_cast< uint16_t >( m_height ) );
        }
        else
        {
            file::saveTGA_ARGB( name, reinterpret_cast< uint32_t* >( m_buffer ), static_cast< uint16_t >( m_width ), static_cast< uint16_t >( m_height ) );
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Debug class and instance to write out the textures
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//class CTest
//{
//public:
//    inline      CTest() { run(); };
//    inline      ~CTest() {};
//    void        run();
//};
//
//CTest aTest;
//
//void CTest::run()
//{
//    CStdTexGen texGen;
//    texGen.init( EStdTexPermute1D );
//    texGen.writeTGA();
//    texGen.init( EStdTexPermute1Dx4 );
//    texGen.writeTGA();
//    texGen.init( EStdTexPermute2D );
//    texGen.writeTGA();
//    texGen.init( EStdTexPermute2Dx4 );
//    texGen.writeTGA();
//    texGen.init( EStdTexPermuteQuad2D );
//    texGen.writeTGA();
//    texGen.init( EStdTexRandom1D );
//    texGen.writeTGA();
//    texGen.init( EStdTexRandom1Dx4 );
//    texGen.writeTGA();
//    texGen.init( EStdTexRandom2D );
//    texGen.writeTGA();
//    texGen.init( EStdTexRandom2Dx4 );
//    texGen.writeTGA();
//    texGen.init( EStdTexRandomQuad2D );
//    texGen.writeTGA();
//    texGen.init( EStdTexMedian2D );
//    texGen.writeTGA();
//    texGen.init( EStdTexMedianQuad2D );
//    texGen.writeTGA();
//    texGen.init( EStdTexPerlin2Dx4a );
//    texGen.writeTGA();
//    texGen.init( EStdTexPerlin2Dx4b );
//    texGen.writeTGA();
//}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end rendering namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

};	//	namespace rendering

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
