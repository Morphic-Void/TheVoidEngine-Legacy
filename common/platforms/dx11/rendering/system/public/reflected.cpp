
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   reflected.cpp
////    Author: Ritchie Brannan
////    Date:   16 Dec 14
////
////    Description:
////
////        Reflected shader structures and class wrappers
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

#include "reflected.h"
#include "platforms/dx11/rendering/utils/standard_palette.h"
#include <memory.h>
#include <math.h>

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
////    helper function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setTargetConsts( hlsl::Target& targetConsts, const float width, const float height )
{
    memset( &targetConsts, 0, sizeof( targetConsts ) );
    targetConsts.dimensions.x = width;
    targetConsts.dimensions.y = height;
    targetConsts.dimensions.z = ( 1.0f / width );
    targetConsts.dimensions.w = ( 1.0f / height );
    targetConsts.clip_to_uv.x = ( ( width - 1.0f ) / ( width * 2.0f ) );
    targetConsts.clip_to_uv.y = ( ( 1.0f - height ) / ( height * 2.0f ) );
    targetConsts.clip_to_uv.z = 0.5f;
    targetConsts.clip_to_uv.w = 0.5f;
    targetConsts.uv_to_clip.x = ( ( width * 2.0f ) / ( width - 1.0f ) );
    targetConsts.uv_to_clip.y = ( ( height * 2.0f ) / ( 1.0f - height ) );
    targetConsts.uv_to_clip.z = ( ( 0.0f - width ) / ( width - 1.0f ) );
    targetConsts.uv_to_clip.w = ( ( 0.0f - height ) / ( 1.0f - height ) );
}

void setCameraConsts( hlsl::Camera& cameraConsts, const maths::camera& camera )
{
    memset( &cameraConsts, 0, sizeof( cameraConsts ) );
    maths::mat44 project, unproject;
    project.SetTranspose( camera.project );
    unproject.SetTranspose( camera.unproject );
    cameraConsts.rotate.x = camera.rotate.x;
    cameraConsts.rotate.y = camera.rotate.y;
    cameraConsts.rotate.z = camera.rotate.z;
    cameraConsts.rotate.w = camera.rotate.w;
    cameraConsts.locate.x = camera.translate.x;
    cameraConsts.locate.y = camera.translate.y;
    cameraConsts.locate.z = camera.translate.z;
    cameraConsts.locate.w = 1.0f;
    memcpy_s( &cameraConsts.project, sizeof( cameraConsts.project ), &project, sizeof( project ) );
    memcpy_s( &cameraConsts.unproject, sizeof( cameraConsts.unproject ), &unproject, sizeof( unproject ) );
}

void setCameraConsts( hlsl::Camera& cameraConsts, const maths::viewer& viewer )
{
    maths::camera camera;
    camera.Set( viewer );
    setCameraConsts( cameraConsts, camera );
}

void setEntityConsts( hlsl::Entity& entityConsts, const maths::joint& entity, const maths::vec3& scaling, const float bias )
{
    memset( &entityConsts, 0, sizeof( entityConsts ) );
    maths::vec3 scales;
    scales.SetMul( scaling, entity.q.GetScaling() );
    maths::quat rotate;
    rotate.SetNormalized( entity.q );
    entityConsts.scale.x = scales.x;
    entityConsts.scale.y = scales.y;
    entityConsts.scale.z = scales.z;
    entityConsts.scale.w = 1.0f;
    entityConsts.rotate.x = rotate.x;
    entityConsts.rotate.y = rotate.y;
    entityConsts.rotate.z = rotate.z;
    entityConsts.rotate.w = rotate.w;
    entityConsts.locate.x = entity.v.x;
    entityConsts.locate.y = entity.v.y;
    entityConsts.locate.z = entity.v.z;
    entityConsts.locate.w = bias;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    object consts helper class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ObjectConsts::zero()
{
    memset( &m_consts, 0, sizeof( m_consts ) );
    m_consts.pscale.w = 1.0f;
    m_consts.nscale.w = 1.0f;
    m_consts.offset.w = 0.0f;
}

void ObjectConsts::setScalingAndOffset( const maths::vec3& scaling, const maths::vec3& offset )
{
    m_consts.pscale.x = scaling.x;
    m_consts.pscale.y = scaling.y;
    m_consts.pscale.z = scaling.z;
    m_consts.nscale.x = ( 1.0f / scaling.x );
    m_consts.nscale.y = ( 1.0f / scaling.y );
    m_consts.nscale.z = ( 1.0f / scaling.z );
    m_consts.offset.x = offset.x;
    m_consts.offset.y = offset.y;
    m_consts.offset.z = offset.z;
}

void ObjectConsts::setRotateAndLocate( const maths::quat& rotate, const maths::vec4& locate )
{
    m_consts.rotate.x = rotate.x;
    m_consts.rotate.y = rotate.y;
    m_consts.rotate.z = rotate.z;
    m_consts.rotate.w = rotate.w;
    m_consts.locate.x = locate.x;
    m_consts.locate.y = locate.y;
    m_consts.locate.z = locate.z;
    m_consts.locate.w = locate.w;
}

void ObjectConsts::setLocalToLight( const maths::joint& localToLight )
{
    float s = localToLight.q.GetScaling();
    float n = ( 1.0f / ( sqrtf( s ) + maths::consts::FLOAT_MIN_NORM ) );
    m_consts.rotate.x = ( localToLight.q.x * n );
    m_consts.rotate.y = ( localToLight.q.y * n );
    m_consts.rotate.z = ( localToLight.q.z * n );
    m_consts.rotate.w = ( localToLight.q.w * n );
    m_consts.locate.x = localToLight.v.x;
    m_consts.locate.y = localToLight.v.y;
    m_consts.locate.z = localToLight.v.z;
    m_consts.locate.w = s;
}

void ObjectConsts::setEyePosition( const maths::vec3& eyepos, const float scale )
{
    m_consts.eyepos.x = eyepos.x;
    m_consts.eyepos.y = eyepos.y;
    m_consts.eyepos.z = eyepos.z;
    m_consts.eyepos.w = scale;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    viewer consts helper class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ViewerConsts::zero()
{
    memset( &m_consts, 0, sizeof( m_consts ) );
}

void ViewerConsts::setRotateAndLocate( const maths::quat& rotate, const maths::vec4& locate )
{
    m_consts.rotate.x = rotate.x;
    m_consts.rotate.y = rotate.y;
    m_consts.rotate.z = rotate.z;
    m_consts.rotate.w = rotate.w;
    m_consts.locate.x = locate.x;
    m_consts.locate.y = locate.y;
    m_consts.locate.z = locate.z;
    m_consts.locate.w = locate.w;
}

void ViewerConsts::setLocalToView( const maths::joint& localToView )
{
    float s = localToView.q.GetScaling();
    float n = ( 1.0f / ( sqrtf( s ) + maths::consts::FLOAT_MIN_NORM ) );
    m_consts.rotate.x = ( localToView.q.x * n );
    m_consts.rotate.y = ( localToView.q.y * n );
    m_consts.rotate.z = ( localToView.q.z * n );
    m_consts.rotate.w = ( localToView.q.w * n );
    m_consts.locate.x = localToView.v.x;
    m_consts.locate.y = localToView.v.y;
    m_consts.locate.z = localToView.v.z;
    m_consts.locate.w = s;
}

void ViewerConsts::setProjection( const maths::mat44& projection )
{
    m_consts.mhclip.x.x = projection.x.x;
    m_consts.mhclip.x.y = projection.y.x;
    m_consts.mhclip.x.z = projection.z.x;
    m_consts.mhclip.x.w = projection.w.x;
    m_consts.mhclip.y.x = projection.x.y;
    m_consts.mhclip.y.y = projection.y.y;
    m_consts.mhclip.y.z = projection.z.y;
    m_consts.mhclip.y.w = projection.w.y;
    m_consts.mhclip.z.x = projection.x.z;
    m_consts.mhclip.z.y = projection.y.z;
    m_consts.mhclip.z.z = projection.z.z;
    m_consts.mhclip.z.w = projection.w.z;
    m_consts.mhclip.w.x = projection.x.w;
    m_consts.mhclip.w.y = projection.y.w;
    m_consts.mhclip.w.z = projection.z.w;
    m_consts.mhclip.w.w = projection.w.w;
}

void ViewerConsts::setNearPlane( const maths::plane& nearPlane )
{
    m_consts.plane1.x = nearPlane.x;
    m_consts.plane1.y = nearPlane.y;
    m_consts.plane1.z = nearPlane.z;
    m_consts.plane1.w = nearPlane.w;
}

void ViewerConsts::setDoorPlane( const maths::plane& doorPlane )
{
    m_consts.plane2.x = doorPlane.x;
    m_consts.plane2.y = doorPlane.y;
    m_consts.plane2.z = doorPlane.z;
    m_consts.plane2.w = doorPlane.w;
}

void ViewerConsts::clearNearPlane()
{
    m_consts.plane1.x = m_consts.plane1.y = m_consts.plane1.z = 0.0f;
    m_consts.plane1.w = 1.0f;
}

void ViewerConsts::clearDoorPlane()
{
    m_consts.plane2.x = m_consts.plane2.y = m_consts.plane2.z = 0.0f;
    m_consts.plane2.w = 1.0f;
}

void ViewerConsts::clearPlanes()
{
    m_consts.plane1.x = m_consts.plane1.y = m_consts.plane1.z = 0.0f;
    m_consts.plane2.x = m_consts.plane2.y = m_consts.plane2.z = 0.0f;
    m_consts.plane1.w = m_consts.plane2.w = 1.0f;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    lintel effect consts helper class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void LintelEffect::zero()
{
    memset( &m_consts, 0, sizeof( m_consts ) );
}

void LintelEffect::setColour( const uint32_t colour )
{
    m_consts.colour.x = ( ( colour & 255 ) / 255.0f );
    m_consts.colour.y = ( ( ( colour >> 8 ) & 255 ) / 255.0f );
    m_consts.colour.z = ( ( ( colour >> 16 ) & 255 ) / 255.0f );
    m_consts.colour.w = ( ( ( colour >> 24 ) & 255 ) / 255.0f );
}

void LintelEffect::setParams( const float side, const float face, const float back, const float offset )
{
    m_consts.param1.x = m_consts.param2.x = side;
    m_consts.param1.y = m_consts.param2.y = face;
    m_consts.param1.z = m_consts.param2.z = back;
    m_consts.param1.w = m_consts.param2.w = offset;
}

void LintelEffect::setParamA( const float side, const float face, const float back, const float offset )
{
    m_consts.param1.x = side;
    m_consts.param1.y = face;
    m_consts.param1.z = back;
    m_consts.param1.w = offset;
}

void LintelEffect::setParamB( const float side, const float face, const float back, const float offset )
{
    m_consts.param2.x = side;
    m_consts.param2.y = face;
    m_consts.param2.z = back;
    m_consts.param2.w = offset;
}

void LintelEffect::setMixer( const ELintelFunc func1, const ELintelFunc func2 )
{
    setMixer( func1, m_consts.mixer1 );
    setMixer( func2, m_consts.mixer2 );
}

void LintelEffect::setMixer( const ELintelFunc func, hlsl::fvec4& mixer )
{
    mixer.x = mixer.y = mixer.z = mixer.w = 0.0f;
    switch( func )
    {
        case( ELintelFuncA ):
        {
            mixer.x = 1.0f;
            break;
        }
        case( ELintelFuncB ):
        {
            mixer.y = 1.0f;
            break;
        }
        case( ELintelFuncAB ):
        {
            mixer.z = 1.0f;
            break;
        }
        case( ELintelFuncInvA ):
        {
            mixer.x = -1.0f;
            mixer.w = 1.0f;
            break;
        }
        case( ELintelFuncInvB ):
        {
            mixer.y = -1.0f;
            mixer.w = 1.0f;
            break;
        }
        case( ELintelFuncInvAB ):
        {
            mixer.z = -1.0f;
            mixer.w = 1.0f;
            break;
        }
        case( ELintelFuncAvg ):
        {
            mixer.x = 0.5f;
            mixer.y = 0.5f;
            break;
        }
        default:
        {   //  ELintelFuncOne
            mixer.w = 1.0f;
            break;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    lintel consts helper class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void LintelConsts::zero()
{
    memset( &m_consts, 0, sizeof( m_consts ) );
}

void LintelConsts::setEffect( const ELintelEffect effect, const float t )
{
    switch( effect )
    {
        case( ELintelEffectDefault ):
        {
            zero();
            break;
        }
        case( ELintelEffectPlasma ):
        {
            zero();
            float time = ( t * 1024.0f );
            time -= floorf( time );
            getEffect1().setColour( getStandardColour( EColourViolet, 4 ) );
            getEffect1().setParamA( -8.0f,  0.25f,  0.0f,  time );
            getEffect1().setParamB(  8.0f,  0.0f,  -0.25f, time );
            getEffect1().setMixer( ELintelFuncAvg, ELintelFuncOne );
            getEffect2().setColour( getStandardColour( EColourViolet, 4 ) );
            getEffect2().setParamA(  8.0f, -0.25f,  0.0f,  time );
            getEffect2().setParamB( -8.0f,  0.0f,   0.25f, time );
            getEffect2().setMixer( ELintelFuncAvg, ELintelFuncOne );
            enableFaceEffect1();
            enableBackEffect2();
            break;
        }
        default:
        {   //  ELintelEffectError
            zero();
            float time = ( t * 1024.0f );
            time -= floorf( time );
            getEffect1().setColour( getStandardColour( EColourDarkRed, 0 ) );
            getEffect1().setParamA( 16, 0, 0, time );
            getEffect1().setParamB( 16, 0, 0, time );
            getEffect1().setMixer( ELintelFuncAB, ELintelFuncAB );
            getEffect2().setColour( getStandardColour( EColourDarkBlue, 0 ) );
            getEffect2().setParamA( 16, 0, 0, ( time + 0.5f ) );
            getEffect2().setParamB( 16, 0, 0, ( time + 0.5f ) );
            getEffect2().setMixer( ELintelFuncAB, ELintelFuncAB );
            enableEffects();
            break;
        }
    }
}

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

