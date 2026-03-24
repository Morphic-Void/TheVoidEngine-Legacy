
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   plugin.cpp
////    Author: Ritchie Brannan
////    Date:   11 Nov 10
////
////    Description:
////
////    	DLL plug-in class.
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

#include "stdafx.h"
#include "plugin.h"
#include "libs/system/debug/asserts.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    begin platform_windows namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace platform_windows
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    NULL_INTERFACE_QUERY : default interface querying class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint NULL_INTERFACE_QUERY::getInterfaceCount()
{
    return( 0 );
}

const char* NULL_INTERFACE_QUERY::getInterfaceName( const uint index )
{
    UNUSED( index );
    return( NULL );
}

void* NULL_INTERFACE_QUERY::getInterface( const uint index )
{
    UNUSED( index );
    return( NULL );
}

void* NULL_INTERFACE_QUERY::findInterface( const char* const name )
{
    UNUSED( name );
    return( NULL );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    NULL_PLUGIN : default plug-in class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const char* NULL_PLUGIN::id() const
{
    return( "NULL_PLUGIN" );
}

bool NULL_PLUGIN::update()
{
    return( true );
}

bool NULL_PLUGIN::notify( const EPLUGIN_ACTION action )
{
    UNUSED( action );
    return( true );
}

bool NULL_PLUGIN::setAppQuery( INTERFACE_QUERY* query )
{
    m_appQuery = ( ( query != NULL ) ? query : &m_nullQuery );
    return( true );
}

INTERFACE_QUERY* NULL_PLUGIN::getDllQuery()
{
    return( &m_nullQuery );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CPlugin : host class for the dll plug-in : class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CPlugin::setAppQuery( INTERFACE_QUERY* query )
{
    if( m_hinst != NULL )
    {
        ASSERT_FAIL( "Error: Attempted to change the application INTERFACE_QUERY for an active plug-in" );
        return( false );
    }
    m_appQuery = ( ( query != NULL ) ? query : &m_nullQuery );
    return( true );
}

bool CPlugin::load( const char* const name, const bool dynamic )
{
    unload();
    m_dynamic = dynamic;
    m_dllName = name;
    return( load() );
}

bool CPlugin::reload()
{
    if( m_hinst != NULL )
    {
        return( load() );
    }
    return( false );
}

void CPlugin::unload()
{
    if( m_hinst != NULL )
    {
        m_plugin->notify( EPLUGIN_EXIT );
        FreeLibrary( m_hinst );
        if( m_dllTemp[ 0 ] )
        {   //  delete the temporary dll
            DeleteFile( m_dllTemp );
        }
        m_dynamic = false;
        m_created.dwLowDateTime = 0;
        m_created.dwHighDateTime = 0;
        memset( m_dllTemp, 0, sizeof( m_dllTemp ) );
        m_dllName = NULL;
        m_hinst = NULL;
        m_plugin = NULL;
        m_dllQuery = &m_nullQuery;;
    }
}

bool CPlugin::refresh()
{
    if( m_hinst != NULL )
    {
        if( m_dynamic )
        {
            WIN32_FILE_ATTRIBUTE_DATA attributes;
            if( GetFileAttributesEx( m_dllName, GetFileExInfoStandard, &attributes ) )
            {
                if( ( m_created.dwLowDateTime != attributes.ftLastWriteTime.dwLowDateTime ) || ( m_created.dwHighDateTime != attributes.ftLastWriteTime.dwHighDateTime ) )
                {
                    return( load() );
                }
            }
        }
    }
    return( true );
}

bool CPlugin::update()
{
    if( m_hinst != NULL )
    {
        if( !m_plugin->update() )
        {
            unload();
            return( false );
        }
    }
    return( true );
}

bool CPlugin::load()
{
    EPLUGIN_ACTION action = EPLUGIN_LOAD;
    if( m_hinst != NULL )
    {
        action = EPLUGIN_RELOAD;
        if( !m_plugin->notify( EPLUGIN_UNLOAD ) )
        {
            unload();
            return( false );
        }
        FreeLibrary( m_hinst );
        m_hinst = NULL;
        m_plugin = NULL;
        m_dllQuery = &m_nullQuery;;
    }
    if( m_dynamic )
    {
        if( ( m_dllTemp[ 0 ] != 0 ) || ( GetTempFileName( "temp\\", "xxx", 0, m_dllTemp ) != 0 ) )
        {
            WIN32_FILE_ATTRIBUTE_DATA attributes;
            if( GetFileAttributesEx( m_dllName, GetFileExInfoStandard, &attributes ) )
            {
                m_created.dwLowDateTime = attributes.ftLastWriteTime.dwLowDateTime;
                m_created.dwHighDateTime = attributes.ftLastWriteTime.dwHighDateTime;
                CopyFile( m_dllName, m_dllTemp, FALSE );
                m_hinst = LoadLibrary( m_dllTemp );
            }
        }
    }
    else
    {
        m_hinst = LoadLibrary( m_dllName );
    }
    if( m_hinst != NULL )
    {
        PLUGIN_FUNC_PTR func = reinterpret_cast< PLUGIN_FUNC_PTR >( GetProcAddress( m_hinst, PLUGIN_STRING( PLUGIN_FUNC_NAME ) ) );
        if( func != NULL )
        {
            m_plugin = func();
            if( m_plugin != NULL )
            {
                if( m_plugin->notify( action ) )
                {
                    if( m_plugin->setAppQuery( m_appQuery ) )
                    {
                        m_dllQuery = m_plugin->getDllQuery();
                        if( m_plugin->notify( EPLUGIN_VERIFY ) )
                        {
                            return( true );
                        }
                        m_dllQuery = &m_nullQuery;
                    }
                }
                m_plugin = NULL;
            }
        }
        FreeLibrary( m_hinst );
        m_hinst = NULL;
    }
    m_dynamic = false;
    m_dllName = NULL;
    m_created.dwLowDateTime = 0;
    m_created.dwHighDateTime = 0;
    memset( m_dllTemp, 0, sizeof( m_dllTemp ) );
    return( false );
}

void CPlugin::zero()
{
    m_dynamic = false;
    m_created.dwLowDateTime = 0;
    m_created.dwHighDateTime = 0;
    memset( m_dllTemp, 0, sizeof( m_dllTemp ) );
    m_dllName = NULL;
    m_hinst = NULL;
    m_plugin = NULL;
    m_appQuery = &m_nullQuery;
    m_dllQuery = &m_nullQuery;
}

const char* CPlugin::getID()
{
    return( ( m_plugin != NULL ) ? m_plugin->id() : NULL );
}

uint CPlugin::getInterfaceCount()
{
    return( ( m_dllQuery != NULL ) ? m_dllQuery->getInterfaceCount() : 0 );
}

const char* CPlugin::getInterfaceName( const uint index )
{
    return( ( m_dllQuery != NULL ) ? m_dllQuery->getInterfaceName( index ) : NULL );
}

void* CPlugin::getInterface( const uint index )
{
    return( ( m_dllQuery != NULL ) ? m_dllQuery->getInterface( index ) : NULL );
}

void* CPlugin::findInterface( const char* const name )
{
    return( ( m_dllQuery != NULL ) ? m_dllQuery->findInterface( name ) : NULL );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end platform_windows namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

};	//	namespace platform_windows

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    dll entry point
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL WINAPI DllMain( _In_ HINSTANCE hinstDLL, _In_ DWORD fdwReason, _In_ LPVOID lpvReserved )
{
    UNUSED( hinstDLL );
    UNUSED( fdwReason );
    UNUSED( lpvReserved );
    switch( fdwReason ) 
    {   //  Perform actions based on the reason for calling.
        case( DLL_PROCESS_ATTACH ):
        {   //  Initialize once for each new process. Return FALSE to fail DLL load.
            break;
        }
        case( DLL_THREAD_ATTACH ):
        {   //  Do thread-specific initialization.
            break;
        }
        case( DLL_THREAD_DETACH ):
        {   //  Do thread-specific cleanup.
            break;
        }
        case( DLL_PROCESS_DETACH ):
        {   //  Perform any necessary cleanup.
            break;
        }
        default:
        {
            break;
        }
    }
    return( TRUE ); //  Successful DLL_PROCESS_ATTACH.
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

