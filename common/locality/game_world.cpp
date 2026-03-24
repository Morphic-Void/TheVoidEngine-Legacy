
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   game_world.cpp
////    Author: Ritchie Brannan
////    Date:   16 Dec 14
////
////    Description:
////
////        "Locality" game world classes and structures
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

#include "game_world.h"
#include "generators.h"
#include "platforms/dx11/rendering/utils/standard_palette.h"
#include "libs/system/debug/asserts.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    begin locality namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace locality
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    GameWorld class function bodies
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool GameWorld::create( const CoreSystem& system, const PrimaryViews& primary )
{
    kill();
    if( init( system, primary ) )
    {
        return( true );
    }
    kill();
    return( false );
}

void GameWorld::destroy()
{
    if( valid() )
    {
        kill();
    }
}

void GameWorld::update( const float time, const input::CGamePadState& gamePadState, const input::CKeyMouseState& keyMouseState )
{
    static const uint frame_passes[] = { EDoorNullPass, EDoorEdgeFace, EDoorEdgeBack, EDoorEdgeBoth, EDoorTrimFace, EDoorTrimBack, EDoorTrimBoth };
    if( valid() )
    {

//  update positions
        updateViewer( time, gamePadState, keyMouseState );

//  update hierarchy
        uint rootCount = m_nodeLocalities.getUsed();
        for( uint rootIndex = 0; rootIndex < rootCount; ++rootIndex )
        {
            m_nodeLocalities.getItem( rootIndex ).updateNode();
        }

//  check if viewers have passed through doors and update accordingly
        relocateViews();

//  collide the camera
        uint viewCount = m_viewLocalities.getUsed();
        for( uint viewIndex = 0; viewIndex < viewCount; ++viewIndex )
        {
            ViewLocality& view = m_viewLocalities.getItem( viewIndex );
            if( !view.isFreeCam() )
            {
                RoomLocality* room = view.getRoom();
                if( room != NULL )
                {
                    maths::sphere asphere;
                    asphere.r = ( view.getRadius() * view.getLocation().global.q.GetScaling() );
                    asphere.t_vec3() = view.getLocation().global.v;
                    if( room->sphereExclude( asphere, view.getLocation().global.v ) )
                    {
                        view.getParent()->getLocation().global.InverseTransform( view.getLocation().global.v, view.getLocation().local.v);
                    }
                }
            }
        }

//  check if objects have passed through doors and update accordingly (TBD)

//  reset the memory pools used by the rendering
        resetRenderPools();

//  create the scene graph
        preloadRooms();
        for( uint roomIndex = 0; roomIndex < m_roomInstances.getUsed(); ++roomIndex )
        {   //  trace through doors to create room and door lists
            RoomInstance& roomInstance = m_roomInstances.getItem( roomIndex );
            ViewLocality* observer = roomInstance.observer;
            RoomLocality* room = roomInstance.locality;
            ItemLocality* itemLocality = room->getFirstItem();
            for( uint itemCount = room->getItemCount(); itemCount; --itemCount )
            {   //  process items not attached to doors
                if( itemLocality->getDoor() == NULL )
                {
                    int nextItem = roomInstance.firstItem[ 0 ];
                    roomInstance.firstItem[ 0 ] = m_itemInstances.getUsed();
                    ++roomInstance.itemCount[ 0 ];
                    ItemInstance& newItemInstance = m_itemInstances.fetch();
                    newItemInstance.zero();
                    newItemInstance.locality = itemLocality;
                    newItemInstance.nextItem = nextItem;
                    newItemInstance.roomIndex = roomIndex;
                    newItemInstance.stencilRef = roomInstance.stencilRef;
                    newItemInstance.localToLight = itemLocality->getObject();
                    getLocalToView( newItemInstance.localToLight, roomInstance.viewToWorld, newItemInstance.localToView );
                }
                itemLocality = itemLocality->getNextByRoom();
            }
            DoorLocality* doorLocality = room->getFirstDoor();
            for( uint doorCount = room->getDoorCount(); doorCount; --doorCount )
            {   //  process doors and items attached to doors
                if( doorLocality != roomInstance.entrance )
                {   //  process a door and items attached to the door
                    DoorGeometry* doorGeometry = doorLocality->getGeometry();
                    DoorCollider& doorCollider = doorGeometry->getCollider();
                    SRECT window = roomInstance.scissoring;
                    maths::joint localToView;
                    getLocalToView( doorLocality->getObject(), roomInstance.viewToWorld, localToView );
                    maths::plane faceClip;
                    getClipPlane( localToView, faceClip );
                    EViewState viewState = doorCollider.identifyView( localToView );
                    EDoorState doorState = ( ( observer->getStencilRef() < 256 ) ? doorLocality->getState() : EDoorClosed );
                    bool visible = doorCollider.isVisible( localToView, roomInstance.nearClip );
                    if( visible )
                    {
                        if( viewState & EViewFrom )
                        {
                            switch( doorState )
                            {
                                case( EDoorBothOpen ):
                                {   //  both faces of the door are traversable
                                    viewState = static_cast< EViewState >( viewState & ( EViewFrom | EViewFace | EViewBack ) );
                                    break;
                                }
                                case( EDoorFaceOpen ):
                                {   //  only the front face of the door is traversable
                                    viewState = ( ( viewState & EViewFace ) ? static_cast< EViewState >( EViewFrom | EViewFace ) : EViewNone );
                                    break;
                                }
                                case( EDoorBackOpen ):
                                {   //  only the back face of the door is traversable
                                    viewState = ( ( viewState & EViewBack ) ? static_cast< EViewState >( EViewFrom | EViewBack ) : EViewNone );
                                    break;
                                }
                                default:
                                {   //  the door is closed
                                    viewState = EViewNone;
                                    break;
                                }
                            }
                        }
                        else
                        {
                            maths::frect extents;
                            visible = doorCollider.getViewExtents( localToView, observer->getProjection(), extents );
                            if( visible )
                            {
                                visible = observer->adjustWindow( extents, window );
                                if( visible )
                                {
                                    switch( doorState )
                                    {
                                        case( EDoorBothOpen ):
                                        {   //  both faces of the door are traversable
                                            viewState = static_cast< EViewState >( viewState & ( EViewFace | EViewBack ) );
                                            break;
                                        }
                                        case( EDoorFaceOpen ):
                                        {   //  only the front face of the door is traversable
                                            viewState = ( ( viewState & EViewFaceNear ) ? EViewFace : EViewNone );
                                            break;
                                        }
                                        case( EDoorBackOpen ):
                                        {   //  only the back face of the door is traversable
                                            viewState = ( ( viewState & EViewBackNear ) ? EViewBack : EViewNone );
                                            break;
                                        }
                                        default:
                                        {   //  the door is closed
                                            viewState = EViewNone;
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    if( doorLocality->isLooped() || ( ( doorState == EDoorClosed ) && doorLocality->isEthereal() ) )
                    {   //  non-blocking/ethereal doors are invisible when closed and self referencing (looped) doors can be treated as invisible
                        visible = false;
                    }
                    uint stencilRef = roomInstance.stencilRef;
                    uint stencilNew = observer->getStencilRef();
                    maths::joint doorRemap = maths::consts::JOINT::IDENTITY;
                    if( !visible )
                    {
                        viewState = EViewNone;
                    }
                    EDoorFrame frame_type = doorLocality->getFrameType();
                    if( visible || ( frame_type != EDoorUnframed ) )
                    {
                        int nextDoor = roomInstance.firstDoor;
                        roomInstance.firstDoor = m_doorInstances.getUsed();
                        ++roomInstance.doorCount;
                        DoorInstance& newDoorInstance = m_doorInstances.fetch();
                        newDoorInstance.zero();
                        newDoorInstance.locality = doorLocality;
                        newDoorInstance.nextDoor = nextDoor;
                        newDoorInstance.roomIndex = roomIndex;
                        newDoorInstance.localToLight = doorLocality->getObject();
                        newDoorInstance.localToView = localToView;
                        newDoorInstance.stencilRef = stencilRef;
                        newDoorInstance.stencilNew = stencilRef;
                        newDoorInstance.passes[ 0 ] = static_cast< EDoorRenderPass >( frame_passes[ static_cast< uint >( frame_type ) ] );
                        newDoorInstance.passes[ 1 ] = EDoorNullPass;
                        newDoorInstance.passes[ 2 ] = EDoorNullPass;
                        newDoorInstance.passes[ 3 ] = EDoorNullPass;
                        newDoorInstance.passes[ 4 ] = EDoorNullPass;
                        newDoorInstance.faceClip = faceClip;
                        if( visible )
                        {
                            newDoorInstance.passes[ 1 ] = ( doorLocality->isEmbedded() ? EDoorBiasBoth : EDoorNullPass );
                            newDoorInstance.passes[ 2 ] = static_cast< EDoorRenderPass >( ( doorLocality->isEthereal() && ( doorState != EDoorBothOpen ) ) ? ( EDoorSortNone - static_cast< uint >( doorState ) ) : ( static_cast< uint >( doorState ) + EDoorSortNone ) );
                            newDoorInstance.faceClip = faceClip;
                            if( viewState != EViewNone )
                            {   //  we can see through the door
                                newDoorInstance.stencilNew = stencilNew;
                                observer->incStencilRef();
                                if( viewState & EViewFrom )
                                {
                                    if( doorLocality->isFastPass( ( viewState & EViewFace ) ? true : false ) )
                                    {
                                        newDoorInstance.passes[ 1 ] = EDoorNullPass;
                                        newDoorInstance.passes[ 2 ] = EDoorNullPass;
                                        newDoorInstance.passes[ 3 ] = static_cast< EDoorRenderPass >( ( viewState & ( EViewFace | EViewBack ) ) + EDoorFastFace - 1 );
                                    }
                                    else
                                    {
                                        newDoorInstance.passes[ 2 ] = EDoorSortFrom;
                                        newDoorInstance.passes[ 3 ] = static_cast< EDoorRenderPass >( ( viewState & ( EViewFace | EViewBack ) ) + EDoorFromFace - 1 );
                                    }
                                }
                                else
                                {
                                    newDoorInstance.passes[ 3 ] = static_cast< EDoorRenderPass >( doorState + EDoorMaskFace - 1 );
                                }
                                RoomInstance& newRoomInstance = m_roomInstances.fetch();
                                newRoomInstance.zero();
                                newRoomInstance.topLevel = false;
                                newRoomInstance.observer = observer;
                                if( viewState & EViewFace )
                                {   //  viewing through face
                                    newRoomInstance.entrance = doorLocality->getFaceTarget();
                                    doorLocality->getFaceRemap( doorRemap );
                                    newRoomInstance.nearClip.Set( faceClip );
                                }
                                else
                                {   //  viewing through back
                                    newRoomInstance.entrance = doorLocality->getBackTarget();
                                    doorLocality->getBackRemap( doorRemap );
                                    newRoomInstance.nearClip.SetNegate( faceClip );
                                }
                                newDoorInstance.passes[ 4 ] = static_cast< EDoorRenderPass >( frame_passes[ static_cast< uint >( newRoomInstance.entrance->getFrameType() ) ] );
                                newRoomInstance.nearClip.w += ( 1.0f / 16384.0f );
                                newRoomInstance.viewToWorld.SetMul( roomInstance.viewToWorld, doorRemap );
                                newRoomInstance.locality = newRoomInstance.entrance->getRoom();
                                newRoomInstance.stencilRef = stencilNew;
                                observer->getWindowed( window, newRoomInstance.projection );
                                newRoomInstance.scissoring = window;
                                newRoomInstance.viewport.min.x = static_cast< float >( window.left );
                                newRoomInstance.viewport.max.x = static_cast< float >( window.right );
                                newRoomInstance.viewport.min.y = static_cast< float >( window.top );
                                newRoomInstance.viewport.max.y = static_cast< float >( window.bottom );
                                newRoomInstance.viewport.min.z = roomInstance.viewport.min.z;
                                newRoomInstance.viewport.max.z = roomInstance.viewport.max.z;
                                newRoomInstance.localToLight = newRoomInstance.locality->getObject();
                                getLocalToView( newRoomInstance.locality->getObject(), newRoomInstance.viewToWorld, newRoomInstance.localToView );
                            }
                        }
                    }
//  item pass 0: items attached to door face
//  item pass 1: items attached to door back
//  item pass 2: items attached to back of face target door
//  item pass 3: items attached to face of back target door
                    for( uint itemPass = 0; itemPass < 4; ++itemPass )
                    {
                        bool viewOfFace = ( ( itemPass & 1 ) ? false : true );
                        bool inThisRoom = ( ( itemPass & 2 ) ? false : true );
                        DoorLocality* passLocality = ( inThisRoom ? doorLocality : ( viewOfFace ? doorLocality->getFaceTarget() : doorLocality->getBackTarget() ) );
                        if( passLocality != NULL )
                        {
                            bool canSeeDoor = false;
                            float newDoorScale, altDoorScale;
                            if( viewOfFace )
                            {   //  passes 0 and 2
                                canSeeDoor = ( ( viewState & EViewFace ) ? true : false );
                                newDoorScale = -1.0f;
                                altDoorScale = 1.0f;
                            }
                            else
                            {   //  passes 1 and 3
                                canSeeDoor = ( ( viewState & EViewBack ) ? true : false );
                                newDoorScale = 1.0f;
                                altDoorScale = -1.0f;
                            }
                            itemLocality = NULL;
                            uint itemCount = 0;
                            if( ( itemPass + 1 ) & 2 )
                            {   //  passes 1 and 2
                                passLocality->getBackRemap( doorRemap );
                                itemLocality = passLocality->getBackFirstItem();
                                itemCount = passLocality->getBackItemCount();
                            }
                            else
                            {   //  passes 0 and 3
                                passLocality->getFaceRemap( doorRemap );
                                itemLocality = passLocality->getFaceFirstItem();
                                itemCount = passLocality->getFaceItemCount();
                            }
                            while( itemCount )
                            {
                                int nextItem = roomInstance.firstItem[ 0 ];
                                roomInstance.firstItem[ 0 ] = m_itemInstances.getUsed();
                                ++roomInstance.itemCount[ 0 ];
                                ItemInstance& newItemInstance = m_itemInstances.fetch();
                                newItemInstance.zero();
                                newItemInstance.locality = itemLocality;
                                newItemInstance.nextItem = nextItem;
                                newItemInstance.roomIndex = roomIndex;
                                newItemInstance.stencilRef = stencilRef;
                                if( inThisRoom )
                                {
                                    newItemInstance.localToLight = itemLocality->getObject();
                                }
                                else
                                {
                                    newItemInstance.localToLight.SetMul( itemLocality->getObject(), doorRemap );
                                }
                                getLocalToView( newItemInstance.localToLight, roomInstance.viewToWorld, newItemInstance.localToView );
                                newItemInstance.doorClip = faceClip;
                                newItemInstance.doorScale = newDoorScale;
                                if( canSeeDoor )
                                {   //  we can see through the door so render the item in the target room instance re-using the view space transform
                                    nextItem = roomInstance.firstItem[ 1 ];
                                    roomInstance.firstItem[ 1 ] = m_itemInstances.getUsed();
                                    ++roomInstance.itemCount[ 1 ];
                                    ItemInstance& altItemInstance = m_itemInstances.fetch();
                                    altItemInstance.zero();
                                    altItemInstance.locality = itemLocality;
                                    altItemInstance.nextItem = nextItem;
                                    altItemInstance.roomIndex = roomIndex;
                                    altItemInstance.itemClone = roomInstance.firstItem[ 0 ];
                                    altItemInstance.stencilRef = stencilNew;
                                    if( inThisRoom )
                                    {
                                        altItemInstance.localToLight.SetMul( itemLocality->getObject(), doorRemap );
                                    }
                                    else
                                    {
                                        altItemInstance.localToLight = itemLocality->getObject();
                                    }
                                    altItemInstance.localToView = newItemInstance.localToView;
                                    altItemInstance.doorClip = faceClip;
                                    altItemInstance.doorScale = altDoorScale;
                                }
                                itemLocality = itemLocality->getNextByDoor();
                                --itemCount;
                            }
                        }
                    }
                }
                doorLocality = doorLocality->getNextByRoom();
            }
        }

//  create the constant buffer resources
        {   //  create the door lintel animation constant buffer resources
            static float offset = 0.0f;
            LintelConsts lintelConsts( ELintelEffectPlasma, offset );
            m_lintelBuffer.create( static_cast< UINT >( sizeof( lintelConsts ) ), &lintelConsts );
            m_lintelResource.setBuffer( m_lintelBuffer.getBuffer() );
            m_lintelResource.setOffset( 0 );
            m_lintelResource.setStride( m_lintelBuffer.getBytes() );
            offset += ( 1.0f / 65536.0f );
            offset -= floorf( offset );
        }
        {   //  create the room instance constant buffer resources
            uint roomCount = m_roomInstances.getUsed();
            for( uint roomIndex = 0; roomIndex < roomCount; ++roomIndex )
            {
                RoomInstance& roomInstance = m_roomInstances.getItem( roomIndex );
                updateRoomConsts( roomInstance );
            }
        }
        {   //  create the door instance constant buffer resources
            uint doorCount = m_doorInstances.getUsed();
            for( uint doorIndex = 0; doorIndex < doorCount; ++doorIndex )
            {
                DoorInstance& doorInstance = m_doorInstances.getItem( doorIndex );
                RoomInstance& roomInstance = m_roomInstances.getItem( doorInstance.roomIndex );
                updateDoorConsts( doorInstance, roomInstance );
            }
        }
        {   //  create the item instance constant buffer resources
            uint itemCount = m_itemInstances.getUsed();
            for( uint itemIndex = 0; itemIndex < itemCount; ++itemIndex )
            {
                ItemInstance& itemInstance = m_itemInstances.getItem( itemIndex );
                RoomInstance& roomInstance = m_roomInstances.getItem( itemInstance.roomIndex );
                updateItemConsts( itemInstance, roomInstance );
            }
        }

//  add items to the renderable list
        uint roomCount = m_roomInstances.getUsed();
        for( uint roomIndex = 0; roomIndex < roomCount; ++roomIndex )
        {
            const RoomInstance& roomInstance = m_roomInstances.getItem( roomIndex );
            if( roomInstance.topLevel )
            {
                RenderableRoom& renderableRoom = m_roomRenderables.fetch();
                renderableRoom.setRenderer( roomInstance.locality->getGeometry()->getRenderer() );
                renderableRoom.setResources( roomInstance.observer->getTargetRTV(), roomInstance.observer->getTargetDSV(), roomInstance.object, roomInstance.viewer );
                renderableRoom.setConfig( roomInstance.viewport, roomInstance.scissoring, roomInstance.stencilRef, false, true );
                m_renderables.fetch() = &renderableRoom;
            }
        }
        for( uint roomIndex = 0; roomIndex < roomCount; ++roomIndex )
        {
            const RoomInstance& roomInstance = m_roomInstances.getItem( roomIndex );
            if( !roomInstance.topLevel )
            {
                RenderableRoom& renderableRoom = m_roomRenderables.fetch();
                renderableRoom.setRenderer( roomInstance.locality->getGeometry()->getRenderer() );
                renderableRoom.setResources( roomInstance.observer->getTargetRTV(), roomInstance.observer->getTargetDSV(), roomInstance.object, roomInstance.viewer );
                renderableRoom.setConfig( roomInstance.viewport, roomInstance.scissoring, roomInstance.stencilRef, false, false );
                m_renderables.fetch() = &renderableRoom;
            }
            int itemIndexPass1 = roomInstance.firstItem[ 0 ];
            for( int itemCountPass1 = roomInstance.itemCount[ 0 ]; itemCountPass1; --itemCountPass1 )
            {
                const ItemInstance& itemInstance = m_itemInstances.getItem( itemIndexPass1 );
                RenderableItem& renderableItem = m_itemRenderables.fetch();
                renderableItem.setRenderer( itemInstance.locality->getGeometry()->getRenderer() );
                renderableItem.setResources( itemInstance.object, itemInstance.viewer );
                renderableItem.setConfig( itemInstance.stencilRef, false );
                m_renderables.fetch() = &renderableItem;
                itemIndexPass1 = itemInstance.nextItem;
            }
            for( uint doorPass = 0; doorPass < 5; ++doorPass )
            {
                int doorIndex = roomInstance.firstDoor;
                for( int doorCount = roomInstance.doorCount; doorCount; --doorCount )
                {
                    const DoorInstance& doorInstance = m_doorInstances.getItem( doorIndex );
                    EDoorRenderPass renderPass = doorInstance.passes[ doorPass ];
                    if( renderPass != EDoorNullPass )
                    {
                        RenderableDoor& renderableDoor = m_doorRenderables.fetch();
                        renderableDoor.setRenderer( doorInstance.locality->getGeometry()->getRenderer() );
                        renderableDoor.setResources( doorInstance.object, doorInstance.viewer, doorInstance.lintel );
                        renderableDoor.setConfig( ( ( doorPass == 4 ) ? doorInstance.stencilNew : doorInstance.stencilRef ), doorInstance.stencilNew, renderPass );
                        m_renderables.fetch() = &renderableDoor;
                    }
                    doorIndex = doorInstance.nextDoor;
                }
            }
            int itemIndexPass2 = roomInstance.firstItem[ 1 ];
            for( int itemCountPass2 = roomInstance.itemCount[ 1 ]; itemCountPass2; --itemCountPass2 )
            {
                const ItemInstance& itemInstance = m_itemInstances.getItem( itemIndexPass2 );
                RenderableItem& renderableItem = m_itemRenderables.fetch();
                renderableItem.setRenderer( itemInstance.locality->getGeometry()->getRenderer() );
                renderableItem.setResources( itemInstance.object, itemInstance.viewer );
                renderableItem.setConfig( itemInstance.stencilRef, false );
                m_renderables.fetch() = &renderableItem;
                itemIndexPass2 = itemInstance.nextItem;
            }
        }
    }
}

void GameWorld::render()
{
    if( valid() )
    {
        uint count = m_renderables.getUsed();
        for( uint index = 0; index < count; ++index )
        {
            m_renderables.getItem( index )->render();
        }
    }
}

void GameWorld::resize( const PrimaryViews& primary )
{
    if( valid() )
    {
        m_primary = primary;
        uint viewCount = m_viewLocalities.getUsed();
        for( uint viewIndex = 0; viewIndex < viewCount; ++viewIndex )
        {
            ViewLocality& viewLocality = m_viewLocalities.getItem( viewIndex );
            viewLocality.setWindow( m_primary.dimensions );
            viewLocality.getProjector().SetVerticalFoV( ( 50.0f * maths::consts::DEGTORAD ), ( static_cast< float >( m_primary.dimensions.width ) / static_cast< float >( m_primary.dimensions.height ) ) );
            viewLocality.setTargets( m_primary.primaryRTV.resource, m_primary.primaryDSV.resource );
        }
    }
}

bool GameWorld::init( const CoreSystem& system, const PrimaryViews& primary )
{
    m_system = system;
    m_primary = primary;
    m_renderables.init( 1024 );
    m_roomRenderables.init( 128 );
    m_itemRenderables.init( 256 );
    m_doorRenderables.init( 256 );
    m_objectBuffers.init( 512 );
    m_viewerBuffers.init( 512 );
    m_roomInstances.init( 128 );
    m_itemInstances.init( 256 );
    m_doorInstances.init( 256 );
    m_nodeLocalities.init( 2 );
    m_roomLocalities.init( 16 );
    m_itemLocalities.init( 32 );
    m_doorLocalities.init( 64 );
    m_viewLocalities.init( 2 );
    m_roomGeometries.init( 16 );
    m_itemGeometries.init( 16 );
    m_doorGeometries.init( 8 );
    m_system.factory.create( m_lintelResource );
    if( !buildGeometry() )
    {
        return( false );
    }
    buildScene();
    m_valid = true;
    return( true );
}

void GameWorld::kill()
{
    m_valid = false;
    m_system.release();
    m_renderables.kill();
    m_roomRenderables.kill();
    m_itemRenderables.kill();
    m_doorRenderables.kill();
    m_objectBuffers.kill();
    m_viewerBuffers.kill();
    m_roomInstances.kill();
    m_itemInstances.kill();
    m_doorInstances.kill();
    m_nodeLocalities.kill();
    m_roomLocalities.kill();
    m_itemLocalities.kill();
    m_doorLocalities.kill();
    m_viewLocalities.kill();
    m_roomGeometries.kill();
    m_itemGeometries.kill();
    m_doorGeometries.kill();
    m_lintelResource.release();
    m_lintelBuffer.destroy();
}

bool GameWorld::buildGeometry()
{
    m_roomGeometries.reset();
    m_itemGeometries.reset();
    m_doorGeometries.reset();
    constructs::GeometryConstruct collidable;
    constructs::GeometryConstruct renderable;
    constructs::LayeringConstruct layering;
    collidable.init();
    renderable.init();
    layering.init();
    layering.setConstruct( ECollidableLayer1, collidable );
    layering.setConstruct( ERenderableLayer1, renderable );
    layering.reset();
    constructRoom( EBuildRoomType1, layering );
    layering.finalise();
    if( !m_roomGeometries.fetch().create( m_system, layering ) )
    {
        return( false );
    }
    layering.reset();
    constructRoom( EBuildRoomType3, layering );
    layering.finalise();
    if( !m_roomGeometries.fetch().create( m_system, layering ) )
    {
        return( false );
    }
    layering.reset();
    constructRoom( EBuildRoomType2, layering );
    layering.finalise();
    if( !m_roomGeometries.fetch().create( m_system, layering ) )
    {
        return( false );
    }
    layering.reset();
    constructRoom( EBuildRoomType4, layering );
    layering.finalise();
    if( !m_roomGeometries.fetch().create( m_system, layering ) )
    {
        return( false );
    }
    layering.reset();
    constructRoom( EBuildRoomType8, layering );
    layering.finalise();
    if( !m_roomGeometries.fetch().create( m_system, layering ) )
    {
        return( false );
    }
    layering.reset();
    constructRoom( EBuildRoomType6, layering );
    layering.finalise();
    if( !m_roomGeometries.fetch().create( m_system, layering ) )
    {
        return( false );
    }
    layering.reset();
    constructRoom( EBuildRoomType7, layering );
    layering.finalise();
    if( !m_roomGeometries.fetch().create( m_system, layering ) )
    {
        return( false );
    }
    layering.reset();
    constructRoom( EBuildRoomType5, layering );
    layering.finalise();
    if( !m_roomGeometries.fetch().create( m_system, layering ) )
    {
        return( false );
    }
    layering.reset();
    constructItem( EBuildItemPole, layering );
    layering.finalise();
    if( !m_itemGeometries.fetch().create( m_system, layering ) )
    {
        return( false );
    }
    maths::vec2 door1Outline[] = { { -0.5f, 2.5f }, { 0.5f, 2.5f }, { 1.5f, 0.5f }, { 1.0f, 0.0f }, { -1.0f, 0.0f }, { -1.5f, 0.5f } };
    if( !m_doorGeometries.fetch().create( m_system, door1Outline, 6 ) )
    {
        return( false );
    }
    maths::vec2 door2Outline[] = { { -0.75f, 2.5f }, { 0.75f, 2.5f }, { 0.75f, 0.0f }, { -0.75f, 0.0f } };
    if( !m_doorGeometries.fetch().create( m_system, door2Outline, 4 ) )
    {
        return( false );
    }
    maths::vec2 door3Outline[] = { { 1.0f, 2.5f }, { 4.0f, 2.5f }, { 4.0f, 0.0f }, { 1.0f, 0.0f } };
    if( !m_doorGeometries.fetch().create( m_system, door3Outline, 4 ) )
    {
        return( false );
    }
    maths::vec2 door4Outline[] = { { -1.0f, 3.0f }, { 1.0f, 3.0f }, { 1.0f, 0.0f }, { -1.0f, 0.0f } };
    if( !m_doorGeometries.fetch().create( m_system, door4Outline, 4 ) )
    {
        return( false );
    }
    maths::vec2 door5Outline[] = { { -4.0f, 5.0f }, { 4.0f, 5.0f }, { 4.0f, 0.25f }, { -4.0f, 0.25f } };
    if( !m_doorGeometries.fetch().create( m_system, door5Outline, 4 ) )
    {
        return( false );
    }
    maths::vec2 door6Outline[ 64 ];
    for( int i = 0; i < 64; ++i )
    {
        float arc = ( ( i * ( 1.0f / 64.0f ) ) * maths::consts::PI2 );
        float x = ( sinf( arc ) * maths::consts::ROOT2 );
        float y = ( cosf( arc ) * maths::consts::ROOT2 );
        door6Outline[ i ] = { x, y };
    }
    if( !m_doorGeometries.fetch().create( m_system, door6Outline, 64 ) )
    {
        return( false );
    }
    return( true );
}

void GameWorld::buildScene()
{
//  reset locality arrays:
    m_nodeLocalities.reset();
    m_roomLocalities.reset();
    m_itemLocalities.reset();
    m_doorLocalities.reset();
    m_viewLocalities.reset();

//  prefetch locality arrays:
    m_nodeLocalities.prefetch( 2 );
    m_roomLocalities.prefetch( 8 );
    m_itemLocalities.prefetch( 1 );
    m_doorLocalities.prefetch( 29 );
    m_viewLocalities.prefetch( 1 );

//  initialise the primary viewer:
    m_viewLocalities.getItem( 0 ).getLocation().local.Move( 0.0f, 1.0f, -2.0f );
    m_viewLocalities.getItem( 0 ).setRoom( m_roomLocalities.getItem( 0 ) );
    m_viewLocalities.getItem( 0 ).reparentNode( m_nodeLocalities.getItem( 1 ) );

//  assign room geometry:
    m_roomLocalities.getItem( 0 ).setGeometry( m_roomGeometries.getItem( 0 ) );
    m_roomLocalities.getItem( 1 ).setGeometry( m_roomGeometries.getItem( 1 ) );
    m_roomLocalities.getItem( 2 ).setGeometry( m_roomGeometries.getItem( 2 ) );
    m_roomLocalities.getItem( 3 ).setGeometry( m_roomGeometries.getItem( 3 ) );
    m_roomLocalities.getItem( 4 ).setGeometry( m_roomGeometries.getItem( 4 ) );
    m_roomLocalities.getItem( 5 ).setGeometry( m_roomGeometries.getItem( 5 ) );
    m_roomLocalities.getItem( 6 ).setGeometry( m_roomGeometries.getItem( 6 ) );
    m_roomLocalities.getItem( 7 ).setGeometry( m_roomGeometries.getItem( 7 ) );

//  assign item geometry:
    m_itemLocalities.getItem( 0 ).setGeometry( m_itemGeometries.getItem( 0 ) );

//  assign door geometry:
    m_doorLocalities.getItem( 0 ).setGeometry( m_doorGeometries.getItem( 0 ) );
    m_doorLocalities.getItem( 1 ).setGeometry( m_doorGeometries.getItem( 0 ) );
    m_doorLocalities.getItem( 2 ).setGeometry( m_doorGeometries.getItem( 0 ) );
    m_doorLocalities.getItem( 3 ).setGeometry( m_doorGeometries.getItem( 1 ) );
    m_doorLocalities.getItem( 4 ).setGeometry( m_doorGeometries.getItem( 1 ) );
    m_doorLocalities.getItem( 5 ).setGeometry( m_doorGeometries.getItem( 1 ) );
    m_doorLocalities.getItem( 6 ).setGeometry( m_doorGeometries.getItem( 1 ) );
    m_doorLocalities.getItem( 7 ).setGeometry( m_doorGeometries.getItem( 2 ) );
    m_doorLocalities.getItem( 8 ).setGeometry( m_doorGeometries.getItem( 2 ) );
    m_doorLocalities.getItem( 9 ).setGeometry( m_doorGeometries.getItem( 2 ) );
    m_doorLocalities.getItem( 10 ).setGeometry( m_doorGeometries.getItem( 1 ) );
    m_doorLocalities.getItem( 11 ).setGeometry( m_doorGeometries.getItem( 1 ) );
    m_doorLocalities.getItem( 12 ).setGeometry( m_doorGeometries.getItem( 1 ) );
    m_doorLocalities.getItem( 13 ).setGeometry( m_doorGeometries.getItem( 1 ) );
    m_doorLocalities.getItem( 14 ).setGeometry( m_doorGeometries.getItem( 3 ) );
    m_doorLocalities.getItem( 15 ).setGeometry( m_doorGeometries.getItem( 3 ) );
    m_doorLocalities.getItem( 16 ).setGeometry( m_doorGeometries.getItem( 3 ) );
    m_doorLocalities.getItem( 17 ).setGeometry( m_doorGeometries.getItem( 4 ) );
    m_doorLocalities.getItem( 18 ).setGeometry( m_doorGeometries.getItem( 4 ) );
    m_doorLocalities.getItem( 19 ).setGeometry( m_doorGeometries.getItem( 4 ) );
    m_doorLocalities.getItem( 20 ).setGeometry( m_doorGeometries.getItem( 4 ) );
    m_doorLocalities.getItem( 21 ).setGeometry( m_doorGeometries.getItem( 5 ) );
    m_doorLocalities.getItem( 22 ).setGeometry( m_doorGeometries.getItem( 5 ) );
    m_doorLocalities.getItem( 23 ).setGeometry( m_doorGeometries.getItem( 1 ) );
    m_doorLocalities.getItem( 24 ).setGeometry( m_doorGeometries.getItem( 1 ) );
    m_doorLocalities.getItem( 25 ).setGeometry( m_doorGeometries.getItem( 0 ) );
    m_doorLocalities.getItem( 26 ).setGeometry( m_doorGeometries.getItem( 0 ) );
    m_doorLocalities.getItem( 27 ).setGeometry( m_doorGeometries.getItem( 1 ) );
    m_doorLocalities.getItem( 28 ).setGeometry( m_doorGeometries.getItem( 1 ) );

//  link the doors:
    m_doorLocalities.getItem( 0 ).setTargetLink( m_doorLocalities.getItem( 1 ) );
    m_doorLocalities.getItem( 2 ).setTargetLink( m_doorLocalities.getItem( 0 ) );
    m_doorLocalities.getItem( 3 ).setTargetLink( m_doorLocalities.getItem( 4 ) );
    m_doorLocalities.getItem( 5 ).setTargetLink( m_doorLocalities.getItem( 6 ) );
    m_doorLocalities.getItem( 7 ).setTargetLink( m_doorLocalities.getItem( 8 ) );
    m_doorLocalities.getItem( 8 ).setTargetLink( m_doorLocalities.getItem( 9 ) );
    m_doorLocalities.getItem( 9 ).setTargetLink( m_doorLocalities.getItem( 7 ) );
    m_doorLocalities.getItem( 10 ).setTargetLink( m_doorLocalities.getItem( 11 ) );
    m_doorLocalities.getItem( 12 ).setTargetLink( m_doorLocalities.getItem( 13 ) );
    m_doorLocalities.getItem( 14 ).setTargetLink( m_doorLocalities.getItem( 15 ) );
    m_doorLocalities.getItem( 15 ).setTargetLink( m_doorLocalities.getItem( 16 ) );
    m_doorLocalities.getItem( 17 ).setTargetLink( m_doorLocalities.getItem( 18 ) );
    m_doorLocalities.getItem( 19 ).setTargetLink( m_doorLocalities.getItem( 20 ) );
    m_doorLocalities.getItem( 21 ).setTargetLink( m_doorLocalities.getItem( 22 ) );
    m_doorLocalities.getItem( 23 ).setTargetLink( m_doorLocalities.getItem( 24 ) );
    m_doorLocalities.getItem( 25 ).setTargetLink( m_doorLocalities.getItem( 26 ) );
    m_doorLocalities.getItem( 27 ).setTargetLink( m_doorLocalities.getItem( 28 ) );

//  connect locations to the world:
    m_roomLocalities.getItem( 0 ).reparentNode( m_nodeLocalities.getItem( 0 ) );
    m_roomLocalities.getItem( 1 ).reparentNode( m_nodeLocalities.getItem( 0 ) );
    m_roomLocalities.getItem( 2 ).reparentNode( m_nodeLocalities.getItem( 0 ) );
    m_roomLocalities.getItem( 3 ).reparentNode( m_nodeLocalities.getItem( 0 ) );
    m_roomLocalities.getItem( 4 ).reparentNode( m_nodeLocalities.getItem( 0 ) );
    m_roomLocalities.getItem( 5 ).reparentNode( m_nodeLocalities.getItem( 0 ) );
    m_roomLocalities.getItem( 6 ).reparentNode( m_nodeLocalities.getItem( 0 ) );
    m_roomLocalities.getItem( 7 ).reparentNode( m_nodeLocalities.getItem( 0 ) );

//  attach doors to rooms:
    m_doorLocalities.getItem( 0 ).setRoom( m_roomLocalities.getItem( 0 ), true );
    m_doorLocalities.getItem( 1 ).setRoom( m_roomLocalities.getItem( 1 ), true );
    m_doorLocalities.getItem( 2 ).setRoom( m_roomLocalities.getItem( 2 ), true );
    m_doorLocalities.getItem( 3 ).setRoom( m_roomLocalities.getItem( 1 ), true );
    m_doorLocalities.getItem( 4 ).setRoom( m_roomLocalities.getItem( 1 ), true );
    m_doorLocalities.getItem( 5 ).setRoom( m_roomLocalities.getItem( 2 ), true );
    m_doorLocalities.getItem( 6 ).setRoom( m_roomLocalities.getItem( 3 ), true );
    m_doorLocalities.getItem( 7 ).setRoom( m_roomLocalities.getItem( 3 ), true );
    m_doorLocalities.getItem( 8 ).setRoom( m_roomLocalities.getItem( 4 ), true );
    m_doorLocalities.getItem( 9 ).setRoom( m_roomLocalities.getItem( 5 ), true );
    m_doorLocalities.getItem( 10 ).setRoom( m_roomLocalities.getItem( 0 ), true );
    m_doorLocalities.getItem( 11 ).setRoom( m_roomLocalities.getItem( 5 ), true );
    m_doorLocalities.getItem( 12 ).setRoom( m_roomLocalities.getItem( 5 ), true );
    m_doorLocalities.getItem( 13 ).setRoom( m_roomLocalities.getItem( 2 ), true );
    m_doorLocalities.getItem( 14 ).setRoom( m_roomLocalities.getItem( 6 ), true );
    m_doorLocalities.getItem( 15 ).setRoom( m_roomLocalities.getItem( 5 ), true );
    m_doorLocalities.getItem( 16 ).setRoom( m_roomLocalities.getItem( 6 ), true );
    m_doorLocalities.getItem( 17 ).setRoom( m_roomLocalities.getItem( 4 ), true );
    m_doorLocalities.getItem( 18 ).setRoom( m_roomLocalities.getItem( 4 ), true );
    m_doorLocalities.getItem( 19 ).setRoom( m_roomLocalities.getItem( 4 ), true );
    m_doorLocalities.getItem( 20 ).setRoom( m_roomLocalities.getItem( 4 ), true );
    m_doorLocalities.getItem( 21 ).setRoom( m_roomLocalities.getItem( 4 ), true );
    m_doorLocalities.getItem( 22 ).setRoom( m_roomLocalities.getItem( 0 ), true );
    m_doorLocalities.getItem( 23 ).setRoom( m_roomLocalities.getItem( 1 ), true );
    m_doorLocalities.getItem( 24 ).setRoom( m_roomLocalities.getItem( 4 ), true );
    m_doorLocalities.getItem( 25 ).setRoom( m_roomLocalities.getItem( 3 ), true );
    m_doorLocalities.getItem( 26 ).setRoom( m_roomLocalities.getItem( 7 ), true );
    m_doorLocalities.getItem( 27 ).setRoom( m_roomLocalities.getItem( 7 ), true );
    m_doorLocalities.getItem( 28 ).setRoom( m_roomLocalities.getItem( 7 ), true );

//  attach items to rooms:
    m_itemLocalities.getItem( 0 ).setRoom( m_roomLocalities.getItem( 7 ), true );

//  attach items to doors:
    m_itemLocalities.getItem( 0 ).setDoor( m_doorLocalities.getItem( 27 ), true );

//  mark doors which can be embedded in other surfaces:
    m_doorLocalities.getItem( 3 ).setEmbedded();
    m_doorLocalities.getItem( 4 ).setEmbedded();
    m_doorLocalities.getItem( 5 ).setEmbedded();
    m_doorLocalities.getItem( 6 ).setEmbedded();
    m_doorLocalities.getItem( 10 ).setEmbedded();
    m_doorLocalities.getItem( 13 ).setEmbedded();
    m_doorLocalities.getItem( 21 ).setEmbedded();
    m_doorLocalities.getItem( 22 ).setEmbedded();
    m_doorLocalities.getItem( 23 ).setEmbedded();
    m_doorLocalities.getItem( 24 ).setEmbedded();
    m_doorLocalities.getItem( 25 ).setEmbedded();
    m_doorLocalities.getItem( 26 ).setEmbedded();

//  mark doors which are solid when closed:
    m_doorLocalities.getItem( 1 ).setBlocking();        //  hexagonal in ramp room
    m_doorLocalities.getItem( 2 ).setBlocking();        //  hexagonal in ramp room
    m_doorLocalities.getItem( 27 ).setBlocking();       //  object passing door (face)
    m_doorLocalities.getItem( 28 ).setBlocking();       //  object passing door (back)

//  set door frames:
    m_doorLocalities.getItem( 0 ).setFrameAuto();       //  first hexagonal door
    m_doorLocalities.getItem( 1 ).setFrameAuto();       //  hexagonal in ramp room
    m_doorLocalities.getItem( 2 ).setFrameAuto();       //  hexagonal in aqua room
    m_doorLocalities.getItem( 3 ).setFrameAuto();       //  resizing door in ramp room (face)
    m_doorLocalities.getItem( 4 ).setFrameAuto();       //  resizing door in ramp room (back)
    m_doorLocalities.getItem( 5 ).setFrameAuto();       //  aqua to green room (face)
    m_doorLocalities.getItem( 6 ).setFrameAuto();       //  aqua to green room (back)
    m_doorLocalities.getItem( 7 ).setFrameNone();       //  rotating door 1
    m_doorLocalities.getItem( 8 ).setFrameNone();       //  rotating door 2
    m_doorLocalities.getItem( 9 ).setFrameNone();       //  rotating door 3
    m_doorLocalities.getItem( 10 ).setFrameAuto();      //  orange to blue room (face)
    m_doorLocalities.getItem( 11 ).setFrameAuto();      //  orange to blue room (back)
    m_doorLocalities.getItem( 12 ).setFrameAuto();      //  blue to aqua room (face)
    m_doorLocalities.getItem( 13 ).setFrameAuto();      //  blue to aqua room (back)
    m_doorLocalities.getItem( 14 ).setFrameNone();      //  corridor to nowhere (inside back)
    m_doorLocalities.getItem( 15 ).setFrameAuto();      //  corridor to nowhere (entry)
    m_doorLocalities.getItem( 16 ).setFrameNone();      //  corridor to nowhere (inside face)
    m_doorLocalities.getItem( 17 ).setFrameNone();      //  temple arch 1
    m_doorLocalities.getItem( 18 ).setFrameNone();      //  temple arch 2
    m_doorLocalities.getItem( 19 ).setFrameNone();      //  temple arch 3
    m_doorLocalities.getItem( 20 ).setFrameNone();      //  temple arch 4
    m_doorLocalities.getItem( 21 ).setFrameNone();      //  circular door (face)
    m_doorLocalities.getItem( 22 ).setFrameNone();      //  circular door (back)
    m_doorLocalities.getItem( 23 ).setFrameAuto();      //  door to sky (face)
    m_doorLocalities.getItem( 24 ).setFrameNone();      //  door to sky (back)
    m_doorLocalities.getItem( 25 ).setFrameAuto();      //  door into object room (face)
    m_doorLocalities.getItem( 26 ).setFrameAuto();      //  door into object room (back)
    m_doorLocalities.getItem( 27 ).setFrameAuto();      //  object passing door (face)
    m_doorLocalities.getItem( 28 ).setFrameAuto();      //  object passing door (back)

//  reposition the rooms:
    m_roomLocalities.getItem( 0 ).getLocation().local.Move( 0.0f, 0.0f, 0.0f );
    m_roomLocalities.getItem( 1 ).getLocation().local.Move( 14.0f, 0.0f, 0.0f );
    m_roomLocalities.getItem( 2 ).getLocation().local.Move( -14.0f, 0.0f, 0.0f );
    m_roomLocalities.getItem( 3 ).getLocation().local.Move( -14.0f, 8.0f, 0.0f );
    m_roomLocalities.getItem( 4 ).getLocation().local.Move( 0.0f, 64.0f, 0.0f );
    m_roomLocalities.getItem( 5 ).getLocation().local.Move( 14.0f, 8.0f, 0.0f );
    m_roomLocalities.getItem( 6 ).getLocation().local.Move( 0.0f, -16.0f, 0.0f );
    m_roomLocalities.getItem( 7 ).getLocation().local.Move( 0.0f, -32.0f, 0.0f );

//  reposition the items:
    m_itemLocalities.getItem( 0 ).getLocation().local.Move( -6.0f, 1.0f, 4.0f );

//  reposition the doors:
    m_doorLocalities.getItem( 0 ).getLocation().local.Move( 0.0f, 0.0f, 1.0f );
    m_doorLocalities.getItem( 1 ).getLocation().local.Move( -1.0f, 0.0f, 2.0f );
    m_doorLocalities.getItem( 2 ).getLocation().local.Move( 1.0f, 0.0f, 2.0f );
    m_doorLocalities.getItem( 3 ).getLocation().local.Move( 3.0f, 1.0f, 12.0f );

    //m_doorLocalities.getItem( 4 ).getLocation().local.Move( -2.0f, 4.0f, 12.0f );
    //m_doorLocalities.getItem( 4 ).getLocation().local.EndTwist( 180.0f * maths::consts::DEGTORAD );
    //m_doorLocalities.getItem( 4 ).getLocation().local.EndTurn( 180.0f * maths::consts::DEGTORAD );
    //m_doorLocalities.getItem( 4 ).getLocation().local.EndScale( 1.0f / 2.0f );

    m_doorLocalities.getItem( 4 ).getLocation().local.Move( -4.0f, 3.0f, 12.0f );
    m_doorLocalities.getItem( 4 ).getLocation().local.EndTwist( 90.0f * maths::consts::DEGTORAD );
    m_doorLocalities.getItem( 4 ).getLocation().local.EndTurn( 180.0f * maths::consts::DEGTORAD );
    m_doorLocalities.getItem( 4 ).getLocation().local.EndScale( 1.0f / 2.0f );

    m_doorLocalities.getItem( 5 ).getLocation().local.Move( -4.0f, 0.0f, 0.0f );
    m_doorLocalities.getItem( 5 ).getLocation().local.EndTurn( -90.0f * maths::consts::DEGTORAD );
    m_doorLocalities.getItem( 6 ).getLocation().local.Move( -16.0f, 0.0f, 0.0f );
    m_doorLocalities.getItem( 6 ).getLocation().local.EndTurn( 90.0f * maths::consts::DEGTORAD );
    m_doorLocalities.getItem( 8 ).getLocation().local.Move( 16.0f, 0.0f, 16.0f );
    m_doorLocalities.getItem( 8 ).getLocation().local.EndTurn( 120.0f * maths::consts::DEGTORAD );
    m_doorLocalities.getItem( 9 ).getLocation().local.EndTurn( 240.0f * maths::consts::DEGTORAD );
    m_doorLocalities.getItem( 10 ).getLocation().local.Move( -3.0f, 0.0f, -4.0f );
    m_doorLocalities.getItem( 10 ).getLocation().local.EndTurn( 180.0f * maths::consts::DEGTORAD );
    m_doorLocalities.getItem( 11 ).getLocation().local.Move( 6.0f, 0.0f, -19.0f );
    m_doorLocalities.getItem( 11 ).getLocation().local.EndTurn( 90.0f * maths::consts::DEGTORAD );
    m_doorLocalities.getItem( 12 ).getLocation().local.Move( 6.0f, 0.0f, -19.0f );
    m_doorLocalities.getItem( 12 ).getLocation().local.EndTurn( 90.0f * maths::consts::DEGTORAD );
    m_doorLocalities.getItem( 13 ).getLocation().local.Move( 1.0f, 0.0f, 4.0f );
    m_doorLocalities.getItem( 13 ).getLocation().local.EndTurn( 180.0f * maths::consts::DEGTORAD );
    m_doorLocalities.getItem( 14 ).getLocation().local.Move( 0.0f, 0.0f, 15.0f );
    m_doorLocalities.getItem( 15 ).getLocation().local.Move( 3.0f, 0.0f, -9.0f );
    m_doorLocalities.getItem( 16 ).getLocation().local.Move( 0.0f, 0.0f, -15.0f );
    m_doorLocalities.getItem( 15 ).getLocation().local.EndTurn( -16.0f * maths::consts::DEGTORAD );
    m_doorLocalities.getItem( 17 ).getLocation().local.Move( 0.0f, 0.0f, -5.0f );
    m_doorLocalities.getItem( 18 ).getLocation().local.Move( 0.0f, 0.0f, 5.0f );
    m_doorLocalities.getItem( 19 ).getLocation().local.Move( -5.0f, 0.0f, 0.0f );
    m_doorLocalities.getItem( 20 ).getLocation().local.Move( 5.0f, 0.0f, 0.0f );
    m_doorLocalities.getItem( 19 ).getLocation().local.EndTurn( 90.0f * maths::consts::DEGTORAD );
    m_doorLocalities.getItem( 20 ).getLocation().local.EndTurn( 90.0f * maths::consts::DEGTORAD );
    m_doorLocalities.getItem( 21 ).getLocation().local.Move( -16.0f, 0.0f, -16.0f );
    m_doorLocalities.getItem( 21 ).getLocation().local.EndTilt( 90.0f * maths::consts::DEGTORAD );
    m_doorLocalities.getItem( 21 ).getLocation().local.EndTurn( 225.0f * maths::consts::DEGTORAD );
    m_doorLocalities.getItem( 22 ).getLocation().local.Move( 0.0f, 3.0f, -2.0f );
    m_doorLocalities.getItem( 22 ).getLocation().local.EndTilt( 90.0f * maths::consts::DEGTORAD );

    //m_doorLocalities.getItem( 23 ).getLocation().local.EndTwist( 180.0f * maths::consts::DEGTORAD );
    //m_doorLocalities.getItem( 23 ).getLocation().local.EndTurn( 90.0f * maths::consts::DEGTORAD );
    //m_doorLocalities.getItem( 23 ).getLocation().local.EndScale( 1.0f / 2.0f );
    //m_doorLocalities.getItem( 23 ).getLocation().local.Move( 4.0f, 4.0f, -2.0f );

    m_doorLocalities.getItem( 23 ).getLocation().local.EndTilt( 270.0f * maths::consts::DEGTORAD );
    m_doorLocalities.getItem( 23 ).getLocation().local.EndTurn( 270.0f * maths::consts::DEGTORAD );
    m_doorLocalities.getItem( 23 ).getLocation().local.EndScale( 1.0f / 2.0f );
    m_doorLocalities.getItem( 23 ).getLocation().local.Move( -4.0f, 4.0f, -2.0f );

    m_doorLocalities.getItem( 24 ).getLocation().local.EndTwist( 180.0f * maths::consts::DEGTORAD );
    m_doorLocalities.getItem( 24 ).getLocation().local.EndTurn( 180.0f * maths::consts::DEGTORAD );
    m_doorLocalities.getItem( 24 ).getLocation().local.Move( 0.0f, 32.0f, 128.0f );
    m_doorLocalities.getItem( 25 ).getLocation().local.Move( -14.0f, 0.0f, 5.0f );
    m_doorLocalities.getItem( 26 ).getLocation().local.Move( 0.0f, 0.0f, -8.0f );
    m_doorLocalities.getItem( 27 ).getLocation().local.Move( -6.0f, 0.0f, 4.0f );
    m_doorLocalities.getItem( 28 ).getLocation().local.Move( -4.0f, 0.0f, 8.0f );
    m_doorLocalities.getItem( 28 ).getLocation().local.EndScale( 4.0f );
    m_doorLocalities.getItem( 28 ).getLocation().local.EndTurn( 90.0f * maths::consts::DEGTORAD );

//  initialise the scene hierarchy matrices:
    uint rootCount = m_nodeLocalities.getUsed();
    for( uint rootIndex = 0; rootIndex < rootCount; ++rootIndex )
    {
        NodeLocality& nodeLocality = m_nodeLocalities.getItem( rootIndex );
        nodeLocality.processNode();
        nodeLocality.prepareNode();
    }

//  initialise the views:
    {
        uint viewCount = m_viewLocalities.getUsed();
        for( uint viewIndex = 0; viewIndex < viewCount; ++viewIndex )
        {
            ViewLocality& viewLocality = m_viewLocalities.getItem( viewIndex );
            viewLocality.setWindow( m_primary.dimensions );
            viewLocality.getProjector().SetVerticalFoV( ( 50.0f * maths::consts::DEGTORAD ), ( static_cast< float >( m_primary.dimensions.width ) / static_cast< float >( m_primary.dimensions.height ) ) );
            viewLocality.setTargets( m_primary.primaryRTV.resource, m_primary.primaryDSV.resource );
        }
    }
}

const maths::joint& GameWorld::getViewer() const
{
    return( m_viewLocalities.getItem( 0 ).getObject() );
}

void GameWorld::updateViewer( const float time, const input::CGamePadState& gamePadState, const input::CKeyMouseState& keyMouseState )
{
    float fixedStep = 1.0f;
    float timerStep = ( time * 0.06f );
    float mouseStep = ( ( timerStep < ( 1.0f / 256.0f ) ) ? 256.0f : ( 1.0f / timerStep ) );

    const maths::vec2& lStick = gamePadState.lStick();
    const maths::vec2& rStick = gamePadState.rStick();

    float locateSpeed = 0.0f;
    float rotateSpeed = 0.0f;
    if( keyMouseState.isLocked() )
    {
        locateSpeed = fixedStep;
        rotateSpeed = ( fixedStep * maths::consts::DEGTORAD );
    }

    float padLocateSpeed = ( locateSpeed * ( exp2f( ( gamePadState.rTrigger() * 2.0f ) - ( gamePadState.lTrigger() * 4.0f ) ) * ( 1.0f / 16.0f ) ) );
    float padRotateSpeed = ( rotateSpeed * 1.0f );
    float keyLocateSpeed = ( locateSpeed * ( ( keyMouseState.isDown( input::EVKeyMouseR ) || keyMouseState.isDown( input::EVKeyShift ) ) ? ( 1.0f / 4.0f ) : ( 1.0f / 16.0f ) ) );
    float keyRotateSpeed = ( rotateSpeed * 1.5f );
    float mouseRotateSpeed = ( rotateSpeed * ( 1.0f / 8.0f ) * mouseStep );

    float padTurn = ( padRotateSpeed * rStick.x );
    float padPitch = ( padRotateSpeed * ( 0.0f - rStick.y ) );
    float padMoveX = ( padLocateSpeed * lStick.x );
    float padMoveZ = ( padLocateSpeed * lStick.y );

    float keyTurn = ( keyRotateSpeed * ( 
                    ( ( keyMouseState.isDown( input::EVKeyQ ) || keyMouseState.isDown( input::EVKeyLeft  ) ) ? -1.0f : 0.0f ) +
                    ( ( keyMouseState.isDown( input::EVKeyE ) || keyMouseState.isDown( input::EVKeyRight ) ) ?  1.0f : 0.0f ) ) );
    float keyMoveX = ( keyLocateSpeed * (
                    ( keyMouseState.isDown( input::EVKeyA ) ? -1.0f : 0.0f ) +
                    ( keyMouseState.isDown( input::EVKeyD ) ?  1.0f : 0.0f ) ) );

    //float keyTurn =  ( keyRotateSpeed * (
    //                ( keyMouseState.isDown( input::EVKeyA ) ? -1.0f : 0.0f ) +
    //                ( keyMouseState.isDown( input::EVKeyD ) ?  1.0f : 0.0f ) ) );
    //float keyMoveX = ( keyLocateSpeed * (
    //                ( ( keyMouseState.isDown( input::EVKeyQ ) || keyMouseState.isDown( input::EVKeyLeft  ) ? -1.0f : 0.0f ) +
    //                ( ( keyMouseState.isDown( input::EVKeyE ) || keyMouseState.isDown( input::EVKeyRight ) ?  1.0f : 0.0f ) ) );

    float keyPitch = ( keyRotateSpeed * (
                    ( ( keyMouseState.isDown( input::EVKeyR ) || keyMouseState.isDown( input::EVKeyUp   ) ) ? -0.5f : 0.0f ) +
                    ( ( keyMouseState.isDown( input::EVKeyF ) || keyMouseState.isDown( input::EVKeyDown ) ) ?  0.5f : 0.0f ) ) );
    float keyMoveZ = ( keyLocateSpeed * (
                    ( keyMouseState.isDown( input::EVKeyW ) ?  1.0f : 0.0f ) +
                    ( keyMouseState.isDown( input::EVKeyS ) ? -1.0f : 0.0f ) ) );

    float mouseTurn = ( mouseRotateSpeed * keyMouseState.getDeltaX() );
    float mousePitch = ( mouseRotateSpeed * keyMouseState.getDeltaY() );
    float mouseMoveZ = ( keyLocateSpeed * ( keyMouseState.isDown( input::EVKeyMouseL ) ?  1.0f : 0.0f ) );

    ViewLocality& view = m_viewLocalities.getItem( 0 );
    maths::joint& object = view.getLocation().local;

    object.EndTurn( padTurn + keyTurn + mouseTurn );
    object.EndPitch( padPitch + keyPitch + mousePitch );
    object.Pan( ( padMoveX + keyMoveX ), 0.0f, ( padMoveZ + keyMoveZ + mouseMoveZ ) );

    if( gamePadState.startPressed() || keyMouseState.keyPressed( input::EVKeyScroll ) )
    {
        view.setFreeCam( !view.isFreeCam() );
    }
    if( !view.isFreeCam() )
    {
        float dt = ( 0.009f * fixedStep * 16.6f );
        float dy = ( view.getLocation().local.v.y - view.getPrevious().local.v.y );
        dy -= dt;
        object.Move( 0.0f, dy, 0.0f );
    }
    if( gamePadState.backPressed() || keyMouseState.keyPressed( input::EVKeyHome ) )
    {
        object.SetIdentity();
        object.v.y = 0.5f;
        object.v.z = -2.0f;
        m_nodeLocalities.getItem( 1 ).getLocation().local = maths::consts::JOINT::IDENTITY;
        view.setRoom( m_roomLocalities.getItem( 0 ) );
    }

//  update the rotating doors:
    float tempDoorRotate = ( fixedStep * maths::consts::DEGTORAD );
    m_doorLocalities.getItem( 7 ).getLocation().local.EndTurn( tempDoorRotate );
    m_doorLocalities.getItem( 8 ).getLocation().local.EndTurn( tempDoorRotate );
    m_doorLocalities.getItem( 9 ).getLocation().local.EndTurn( tempDoorRotate );

//  update the test object moving through a door:
    if( !view.isFreeCam() )
    {
        static float tempObjectMove = 0.0f;
        tempObjectMove += ( time * ( 1.0f / ( 4096.0f * 4.0f ) ) );
        tempObjectMove -= floorf( tempObjectMove );
        m_itemLocalities.getItem( 0 ).getLocation().local.v.z = ( ( sinf( tempObjectMove * maths::consts::PI2 ) * 3.0f ) + 4.0f );
    }
}

void GameWorld::preloadRooms()
{
    uint viewCount = m_viewLocalities.getUsed();
    for( uint viewIndex = 0; viewIndex < viewCount; ++viewIndex )
    {
        ViewLocality& viewLocality = m_viewLocalities.getItem( viewIndex );
        viewLocality.resetStencilRef();
        viewLocality.evaluate();
        RoomInstance roomInstance;
        roomInstance.zero();
        roomInstance.topLevel = true;
        roomInstance.observer = &viewLocality;
        roomInstance.viewToWorld = viewLocality.getViewToWorld();
        roomInstance.projection = viewLocality.getProjection();
        roomInstance.scissoring = viewLocality.getScissoring();
        roomInstance.viewport = viewLocality.getViewport();
        if( viewLocality.getRoom() != NULL )
        {
            roomInstance.locality = viewLocality.getRoom();
            roomInstance.localToLight = roomInstance.locality->getObject();
            getLocalToView( roomInstance.locality->getObject(), roomInstance.viewToWorld, roomInstance.localToView );
            roomInstance.stencilRef = viewLocality.getStencilNew();
            m_roomInstances.fetch() = roomInstance;
        }
        else
        {
            uint roomCount = m_roomLocalities.getUsed();
            for( uint roomIndex = 0; roomIndex < roomCount; ++roomIndex )
            {
                roomInstance.locality = &m_roomLocalities.getItem( roomIndex );
                roomInstance.localToLight = roomInstance.locality->getObject();
                getLocalToView( roomInstance.locality->getObject(), roomInstance.viewToWorld, roomInstance.localToView );
                roomInstance.stencilRef = viewLocality.getStencilNew();
                m_roomInstances.fetch() = roomInstance;
            }
        }
    }
}

void GameWorld::resetRenderPools()
{
//  reset the renderables
    m_renderables.reset();
    m_roomRenderables.reset();
    m_itemRenderables.reset();
    m_doorRenderables.reset();

//  reset the instances
    uint roomCount = m_roomInstances.getUsed();
    for( uint roomIndex = 0; roomIndex < roomCount; ++roomIndex ) m_roomInstances.getItem( roomIndex ).zero();
    m_roomInstances.reset();
    uint itemCount = m_itemInstances.getUsed();
    for( uint itemIndex = 0; itemIndex < itemCount; ++itemIndex ) m_itemInstances.getItem( itemIndex ).zero();
    m_itemInstances.reset();
    uint doorCount = m_doorInstances.getUsed();
    for( uint doorIndex = 0; doorIndex < doorCount; ++doorIndex ) m_doorInstances.getItem( doorIndex ).zero();
    m_doorInstances.reset();

//  reset the constant buffer pools
    m_objectBuffers.reset();
    m_viewerBuffers.reset();
}

void GameWorld::updateRoomConsts( RoomInstance& roomInstance )
{
    const BaseGeometry* geometry = roomInstance.locality->getGeometry();
    ObjectConsts objectConsts;
    ViewerConsts viewerConsts;
    objectConsts.setScalingAndOffset( geometry->getScaling(), geometry->getOffset() );
    objectConsts.setLocalToLight( roomInstance.localToLight  );
    objectConsts.setEyePosition( roomInstance.viewToWorld.v );
    CConstants& objectBuffer = m_objectBuffers.fetch();
    objectBuffer.create( static_cast< UINT >( sizeof( objectConsts ) ), &objectConsts );
    m_system.factory.create( roomInstance.object );
    roomInstance.object.setBuffer( objectBuffer.getBuffer() );
    roomInstance.object.setOffset( 0 );
    roomInstance.object.setStride( objectBuffer.getBytes() );
    viewerConsts.setNearPlane( roomInstance.nearClip );
    viewerConsts.clearDoorPlane();
    viewerConsts.setLocalToView( roomInstance.localToView );
    viewerConsts.setProjection( roomInstance.projection );
    CConstants& viewerBuffer = m_viewerBuffers.fetch();
    viewerBuffer.create( static_cast< UINT >( sizeof( viewerConsts ) ), &viewerConsts );
    m_system.factory.create( roomInstance.viewer );
    roomInstance.viewer.setBuffer( viewerBuffer.getBuffer() );
    roomInstance.viewer.setOffset( 0 );
    roomInstance.viewer.setStride( viewerBuffer.getBytes() );
}

void GameWorld::updateDoorConsts( DoorInstance& doorInstance, const RoomInstance& roomInstance )
{
    const BaseGeometry* geometry = doorInstance.locality->getGeometry();
    ObjectConsts objectConsts;
    ViewerConsts viewerConsts;
    objectConsts.setScalingAndOffset( geometry->getScaling(), geometry->getOffset() );
    objectConsts.setLocalToLight( doorInstance.localToLight );
    objectConsts.setEyePosition( roomInstance.viewToWorld.v, ( 1.0f / ( doorInstance.localToView.q.GetScaling() + maths::consts::FLOAT_MIN_NORM ) ) );
    CConstants& objectBuffer = m_objectBuffers.fetch();
    objectBuffer.create( static_cast< UINT >( sizeof( objectConsts ) ), &objectConsts );
    m_system.factory.create( doorInstance.object );
    doorInstance.object.setBuffer( objectBuffer.getBuffer() );
    doorInstance.object.setOffset( 0 );
    doorInstance.object.setStride( objectBuffer.getBytes() );
    viewerConsts.setNearPlane( roomInstance.nearClip );
    viewerConsts.clearDoorPlane();
    viewerConsts.setLocalToView( doorInstance.localToView );
    viewerConsts.setProjection( roomInstance.projection );
    CConstants& viewerBuffer = m_viewerBuffers.fetch();
    viewerBuffer.create( static_cast< UINT >( sizeof( viewerConsts ) ), &viewerConsts );
    m_system.factory.create( doorInstance.viewer );
    doorInstance.viewer.setBuffer( viewerBuffer.getBuffer() );
    doorInstance.viewer.setOffset( 0 );
    doorInstance.viewer.setStride( viewerBuffer.getBytes() );
    doorInstance.lintel = m_lintelResource;
}

void GameWorld::updateItemConsts( ItemInstance& itemInstance, const RoomInstance& roomInstance )
{
    const BaseGeometry* geometry = itemInstance.locality->getGeometry();
    ObjectConsts objectConsts;
    objectConsts.setScalingAndOffset( geometry->getScaling(), geometry->getOffset() );
    objectConsts.setDoorPlaneScale( itemInstance.doorScale );
    objectConsts.setLocalToLight( itemInstance.localToLight );
    objectConsts.setEyePosition( roomInstance.viewToWorld.v );
    CConstants& objectBuffer = m_objectBuffers.fetch();
    objectBuffer.create( static_cast< UINT >( sizeof( objectConsts ) ), &objectConsts );
    m_system.factory.create( itemInstance.object );
    itemInstance.object.setBuffer( objectBuffer.getBuffer() );
    itemInstance.object.setOffset( 0 );
    itemInstance.object.setStride( objectBuffer.getBytes() );
    if( itemInstance.itemClone < 0 )
    {
        ViewerConsts viewerConsts;
        viewerConsts.setNearPlane( roomInstance.nearClip );
        viewerConsts.setDoorPlane( itemInstance.doorClip );
        viewerConsts.setLocalToView( itemInstance.localToView );
        viewerConsts.setProjection( roomInstance.projection );
        CConstants& viewerBuffer = m_viewerBuffers.fetch();
        viewerBuffer.create( static_cast< UINT >( sizeof( viewerConsts ) ), &viewerConsts );
        m_system.factory.create( itemInstance.viewer );
        itemInstance.viewer.setBuffer( viewerBuffer.getBuffer() );
        itemInstance.viewer.setOffset( 0 );
        itemInstance.viewer.setStride( viewerBuffer.getBytes() );
    }
    else
    {
        itemInstance.viewer = m_itemInstances.getItem( itemInstance.itemClone ).viewer;
    }
}

void GameWorld::relocateViews()
{
    uint viewCount = m_viewLocalities.getUsed();
    for( uint viewIndex = 0; viewIndex < viewCount; ++viewIndex )
    {
        ViewLocality& view = m_viewLocalities.getItem( viewIndex );
        maths::joint remap;
        DoorLocality* door = relocatePoint( view.getRoom(), view.getPrevious().global.v, view.getLocation().global.v, remap );
        if( door != NULL )
        {
            view.setRoom( door->getRoom() );
            NodeLocality* node = view.getParent();
            if( node == NULL ) node = &view;
            node->getLocation().local.Mul( remap );
            node->processNode();
        }
    }
}

//  relocatePoint()
//
//      Purpose:
//
//          relocates points through doors based on their movement
//
//      Parameters:
//
//          room (in)       -   pointer to the room currently containing the point
//
//          start (in)      -   the previous global space position of the point
//
//          end (in)        -   the current global space position of the point
//
//          world (out)     -   a transform describing the door transitions encountered
//
//      Returns:
//
//          DoorLocality*   -   the last door that the point exited or NULL if no doors were passed through
//
DoorLocality* GameWorld::relocatePoint( RoomLocality* room, const maths::vec3& start, const maths::vec3& end, maths::joint& world )
{
    world = maths::consts::JOINT::IDENTITY;
    maths::vec3 world_start, world_end;
    maths::vec3 local_start, local_end;
    float lower, upper, delta;
    world_start.Set( start );
    world_end.Set( end );
    bool face = false;
    DoorLocality* last = NULL;
    DoorLocality* best = NULL;
    lower = -1.0f;
    do
    {   //  pass through doors
        upper = 2.0f;
        best = NULL;
        if( room != NULL )
        {   //  find the closest door that the point passes through checking only the doors in the room that the view is attached to
            uint doorCount = room->getDoorCount();
            DoorLocality* door = room->getFirstDoor();
            for( uint doorIndex = 0; doorIndex < doorCount; ++doorIndex )
            {
                if( door != last )
                {
                    EDoorState state = door->getState();
                    if( state != EDoorClosed )
                    {
                        const DoorCollider& collider = door->getGeometry()->getCollider();
                        if( collider.active() )
                        {
                            door->getPrevious().global.InverseTransform( world_start, local_start );
                            door->getLocation().global.InverseTransform( world_end, local_end );
                            if( collider.traversePoint( state, local_start, local_end ) )
                            {
                                delta = ( local_start.z - local_end.z );
                                if( fabsf( delta ) < maths::consts::FLOAT_EPSILON )
                                {
                                    delta = 1.0f;
                                }
                                else
                                {
                                    delta = ( local_start.z / delta );
                                    if( delta > 1.0f ) delta = 1.0f;
                                }
                                if( ( delta > lower ) && ( delta < upper ) )
                                {
                                    best = door;
                                    face = ( local_start.z < local_end.z );
                                    upper = delta;
                                }
                            }
                        }
                    }
                }
                door = door->getNextByRoom();
            }
        }
        else
        {   //  find the closest door that the point passes through checking all the doors in the world
            uint doorCount = m_doorLocalities.getUsed();
            for( uint doorIndex = 0; doorIndex < doorCount; ++doorIndex )
            {
                DoorLocality* door = &m_doorLocalities.getItem( doorIndex );
                if( door != last )
                {
                    EDoorState state = door->getState();
                    if( state != EDoorClosed )
                    {
                        const DoorCollider& collider = door->getGeometry()->getCollider();
                        if( collider.active() )
                        {
                            door->getPrevious().global.InverseTransform( world_start, local_start );
                            door->getLocation().global.InverseTransform( world_end, local_end );
                            if( collider.traversePoint( state, local_start, local_end ) )
                            {
                                delta = ( local_start.z - local_end.z );
                                if( fabsf( delta ) < maths::consts::FLOAT_EPSILON )
                                {
                                    delta = 1.0f;
                                }
                                else
                                {
                                    delta = ( local_start.z / delta );
                                    if( delta > 1.0f ) delta = 1.0f;
                                }
                                if( ( delta > lower ) && ( delta < upper ) )
                                {
                                    best = door;
                                    face = ( local_start.z < local_end.z );
                                    upper = delta;
                                }
                            }
                        }
                    }
                }
            }
        }
        if( best != NULL )
        {   //  the view has passed through a door, relocate and prepare to check for the next door
            last = ( face ? best->getFaceTarget() : best->getBackTarget() );
            room = last->getRoom();
            maths::joint remap;
            best->getDoorRemap( last, remap );
            world.Mul( remap );
            world.Transform( start, world_start );
            world.Transform( end, world_end );
            last->getPrevious().global.InverseTransform( world_start, local_start );
            last->getLocation().global.InverseTransform( world_end, local_end );
            delta = ( local_start.z - local_end.z );
            if( fabsf( delta ) < maths::consts::FLOAT_EPSILON )
            {
                delta = 1.0f;
            }
            else
            {
                delta = ( local_start.z / delta );
                if( delta > 1.0f ) delta = 1.0f;
            }
            lower = ( delta + maths::consts::FLOAT_EPSILON );
        }
    }
    while( best != NULL );
    if( last != NULL )
    {   //  make sure we are not too close to the last door we passed through
        last->getLocation().global.InverseTransform( world_end, local_end );
        bool shift = false;
        if( face )
        {
            delta = ( local_end.z - ( maths::consts::FLOAT_EPSILON * 16.0f ) );
            if( delta < 0.0f )
            {   //  we are within the tolerance of the door plane and need to shift the point away from the plane
                shift = true;
            }
        }
        else
        {
            delta = ( local_end.z + ( maths::consts::FLOAT_EPSILON * 16.0f ) );
            if( delta > 0.0f )
            {   //  we are within the tolerance of the door plane and need to shift the point away from the plane
                shift = true;
            }
        }
        if( shift )
        {
            maths::quat& q = last->getLocation().global.q;
            delta /= q.SqrLen();
            world.v.z -= ( delta * ( ( q.w * q.w ) - ( q.x * q.x ) - ( q.y * q.y ) + ( q.z * q.z ) ) );
            delta += delta;
			world.v.y -= ( delta * ( ( q.z * q.y ) - ( q.w * q.x ) ) );
			world.v.x -= ( delta * ( ( q.z * q.x ) + ( q.w * q.y ) ) );
        }
    }
    return( last );
}

void GameWorld::getLocalToView( const maths::joint& localToWorld, const maths::joint& viewToWorld, maths::joint& localToView )
{
    maths::joint worldToView;
    worldToView.q.SetInverse( viewToWorld.q );
    worldToView.v.SetSub( localToWorld.v, viewToWorld.v );
    localToView.q.SetMul( localToWorld.q, worldToView.q );
    worldToView.q.Transform( worldToView.v, localToView.v );
}

void GameWorld::getClipPlane( const maths::joint& localToView, maths::plane& clipPlane, const bool reverse )
{
    const maths::quat& q = localToView.q;
	float x = ( ( ( q.z * q.x ) + ( q.w * q.y ) ) * 2.0f );
	float y = ( ( ( q.z * q.y ) - ( q.w * q.x ) ) * 2.0f );
    float z = ( ( q.w * q.w ) - ( q.x * q.x ) - ( q.y * q.y ) + ( q.z * q.z ) );
    float n = ( ( reverse ? -1.0f : 1.0f ) / ( sqrtf(( x * x ) + ( y * y ) + ( z * z ) ) + maths::consts::FLOAT_MIN_NORM ) );
    clipPlane.x = ( x * n );
    clipPlane.y = ( y * n );
    clipPlane.z = ( z * n );
    clipPlane.w = ( 0.0f - ( localToView.v.x * clipPlane.x ) - ( localToView.v.y * clipPlane.y ) - ( localToView.v.z * clipPlane.z ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end locality namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

};	//	namespace locality

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

