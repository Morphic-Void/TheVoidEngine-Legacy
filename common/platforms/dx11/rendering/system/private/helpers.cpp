
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   helpers.cpp
////    Author: Ritchie Brannan
////    Date:   16 Dec 14
////
////    Description:
////
////        Helper structures and functions for the public user rendering system.
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

#include "../public/helpers.h"
#include "libs/system/io/file.h"

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
////    Resource type specific paired user resource and user binding structures function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CompoundStream::create( const CoreSystem& system, const StringItem& name )
{
    if( system.factory.valid() && system.shaders.valid() )
    {
        if( system.factory.create( resource ) )
        {
            if( system.shaders.create( name, binding ) )
            {
                if( binding.bind( resource ) )
                {
                    return( true );
                }
            }
        }
    }
    return( false );
}

bool CompoundOutput::create( const CoreSystem& system, const StringItem& name )
{
    if( system.factory.valid() && system.shaders.valid() )
    {
        if( system.factory.create( resource ) )
        {
            if( system.shaders.create( name, binding ) )
            {
                if( binding.bind( resource ) )
                {
                    return( true );
                }
            }
        }
    }
    return( false );
}

bool CompoundBuffer::create( const CoreSystem& system, const StringItem& name )
{
    if( system.factory.valid() && system.shaders.valid() )
    {
        if( system.factory.create( resource ) )
        {
            if( system.shaders.create( name, binding ) )
            {
                if( binding.bind( resource ) )
                {
                    return( true );
                }
            }
        }
    }
    return( false );
}

bool CompoundSRV::create( const CoreSystem& system, const StringItem& name )
{
    if( system.factory.valid() && system.shaders.valid() )
    {
        if( system.factory.create( resource ) )
        {
            if( system.shaders.create( name, binding ) )
            {
                if( binding.bind( resource ) )
                {
                    return( true );
                }
            }
        }
    }
    return( false );
}

bool CompoundUAV::create( const CoreSystem& system, const StringItem& name )
{
    if( system.factory.valid() && system.shaders.valid() )
    {
        if( system.factory.create( resource ) )
        {
            if( system.shaders.create( name, binding ) )
            {
                if( binding.bind( resource ) )
                {
                    return( true );
                }
            }
        }
    }
    return( false );
}

bool CompoundRTV::create( const CoreSystem& system, const StringItem& name )
{
    if( system.factory.valid() && system.shaders.valid() )
    {
        if( system.factory.create( resource ) )
        {
            if( system.shaders.create( name, binding ) )
            {
                if( binding.bind( resource ) )
                {
                    return( true );
                }
            }
        }
    }
    return( false );
}

bool CompoundDSV::create( const CoreSystem& system, const StringItem& name )
{
    if( system.factory.valid() && system.shaders.valid() )
    {
        if( system.factory.create( resource ) )
        {
            if( system.shaders.create( name, binding ) )
            {
                if( binding.bind( resource ) )
                {
                    return( true );
                }
            }
        }
    }
    return( false );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Standard compound structures for renderable user shader function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CompoundDrawing::create( const CoreSystem& system, const StringItem& name )
{
    if( system.factory.valid() && system.shaders.valid() )
    {
        if( system.shaders.create( name, shader ) )
        {
            if( system.shaders.create( name, bindingStream ) )
            {
                if( system.factory.create( resourceStream ) )
                {
                    if( system.factory.create( resourceCommand ) )
                    {
                        if( bindingStream.bind( resourceStream ) )
                        {
                            shader.bind( system.defaults );
                            if( shader.bind( bindingStream ) )
                            {
                                return( true );
                            }
                        }
                        resourceCommand.release();
                    }
                    resourceStream.release();
                }
                bindingStream.release();
            }
            shader.release();
        }
    }
    return( false );
}

bool CompoundIndexed::create( const CoreSystem& system, const StringItem& name )
{
    if( system.factory.valid() && system.shaders.valid() )
    {
        if( system.shaders.create( name, shader ) )
        {
            if( system.shaders.create( name, bindingStream ) )
            {
                if( system.factory.create( resourceStream ) )
                {
                    if( system.factory.create( resourceCommand ) )
                    {
                        if( system.factory.create( resourceIndices ) )
                        {
                            if( bindingStream.bind( resourceStream ) )
                            {
                                shader.bind( system.defaults );
                                if( shader.bind( bindingStream ) )
                                {
                                    return( true );
                                }
                            }
                            resourceIndices.release();
                        }
                        resourceCommand.release();
                    }
                    resourceStream.release();
                }
                bindingStream.release();
            }
            shader.release();
        }
    }
    return( false );
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
