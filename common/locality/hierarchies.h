
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   hierarchies.h
////    Author: Ritchie Brannan
////    Date:   16 Dec 14
////
////    Description:
////
////        "Locality" game hierarchy classes and structures
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

#ifndef	__HIERARCHIES_INCLUDED__
#define	__HIERARCHIES_INCLUDED__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    includes
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "libs/system/base/types.h"
#include "libs/memory/tmanaged.h"
#include "libs/maths/frect.h"
#include "libs/maths/joint.h"
#include "libs/maths/vec3.h"
#include "libs/maths/mat44.h"
#include "libs/maths/sphere.h"
#include "libs/maths/box_faces.h"
#include "libs/cameras/projector.h"
#include "libs/cameras/clipper.h"
#include "platforms/dx11/rendering/system/public/resources_api.h"

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
////    Import namespaces
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace rendering;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Forward declaration of external classes
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RoomGeometry;
class ItemGeometry;
class DoorGeometry;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Forward declaration of cross referenced internal classes
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RoomLocality;
class ItemLocality;
class DoorLocality;
class ViewLocality;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Door traversal state enumeration
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum EDoorState
{
    EDoorClosed = 0,                                        //  neither face of the door is traversable
    EDoorFaceOpen = 1,                                      //  the front face of the door is traversable
    EDoorBackOpen = 2,                                      //  the back face of the door is traversable
    EDoorBothOpen = ( EDoorFaceOpen | EDoorBackOpen ),      //  both faces of the door are traversable
    EDoorStateForce32 = 0x7fffffff
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Door frame type enumeration
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum EDoorFrame
{
    EDoorUnframed = 0,                                      //  the door has no frame
    EDoorChamferFace = 1,                                   //  the front face has a chamfered frame
    EDoorChamferBack = 2,                                   //  the back face has a chamfered frame
    EDoorChamferBoth = 3,                                   //  both faces have a chamfered frame
    EDoorDiamondFace = 4,                                   //  the front face has a diamond frame
    EDoorDiamondBack = 5,                                   //  the back face has a diamond frame
    EDoorDiamondBoth = 6,                                   //  both faces have a diamond frame
    EDoorFrameForce32 = 0x7fffffff
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    ItemLocation item location structure
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ItemLocation
{
    maths::joint            local;
    maths::joint            global;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    NodeLocality scene graph base class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class NodeLocality
{
public:
    inline                          NodeLocality() { zeroNode(); };
    virtual                         ~NodeLocality() { zeroNode(); };
    inline void                     reset() { zeroNode(); };
    inline void                     updateNode() { prepareNode(); processNode(); };
    void                            prepareNode();      //  copies location to previous
    void                            processNode();      //  updates location global
    void                            reparentNode( NodeLocality* parent );
    inline void                     reparentNode( NodeLocality& parent ) { reparentNode( &parent ); };
    inline ItemLocation&            getLocation() { return( m_location ); };
    inline const ItemLocation&      getLocation() const { return( m_location ); };
    inline ItemLocation&            getPrevious() { return( m_previous ); };
    inline const ItemLocation&      getPrevious() const { return( m_previous ); };
    inline maths::joint&            getObject() { return( m_location.global ); };
    inline const maths::joint&      getObject() const { return( m_location.global ); };
    inline NodeLocality*            getParent() const { return( m_parent ); };
protected:
    void                            zeroNode();
    ItemLocation                    m_previous;
    ItemLocation                    m_location;
    NodeLocality*                   m_parent;
    NodeLocality*                   m_prev_node;
    NodeLocality*                   m_next_node;
    NodeLocality*                   m_first_child;
    uint                            m_child_count;
    uint                            m_node_type;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    ItemLocality item scene graph node class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ItemLocality : public NodeLocality
{
public:
    inline                          ItemLocality() : NodeLocality() { zeroItem(); };
    virtual                         ~ItemLocality() { zeroItem(); };
    inline void                     reset() { zeroItem(); zeroNode(); };
    inline void                     setGeometry( ItemGeometry* const geometry ) { m_geometry = geometry; };
    inline void                     setGeometry( ItemGeometry& geometry ) { m_geometry = &geometry; };
    inline ItemGeometry*            getGeometry() const { return( m_geometry ); };
    void                            setRoom( RoomLocality* room, const bool reparent = false );
    inline void                     setRoom( RoomLocality& room, const bool reparent = false ) { setRoom( &room, reparent ); };
    inline RoomLocality*            getRoom() const { return( m_room ); };
    inline ItemLocality*            getNextByRoom() const { return( m_next_by_room ); };
    inline ItemLocality*            getPrevByRoom() const { return( m_prev_by_room ); };
    void                            setDoor( DoorLocality* door, const bool face );
    inline void                     setDoor( DoorLocality& door, const bool face ) { setDoor( &door, face ); };
    inline DoorLocality*            getDoor() const { return( m_door ); };
    inline bool                     getDoorFace() const { return( m_door_face ); };
    inline ItemLocality*            getNextByDoor() const { return( m_next_by_door ); };
    inline ItemLocality*            getPrevByDoor() const { return( m_prev_by_door ); };
protected:
    void                            zeroItem();
    ItemGeometry*                   m_geometry;
    RoomLocality*                   m_room;
    DoorLocality*                   m_door;
    ItemLocality*                   m_prev_by_room;
    ItemLocality*                   m_next_by_room;
    ItemLocality*                   m_prev_by_door;
    ItemLocality*                   m_next_by_door;
    bool                            m_door_face;
    friend class                    RoomLocality;
    friend class                    DoorLocality;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    DoorLocality door scene graph node class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class DoorLocality : public NodeLocality
{
public:
    inline                          DoorLocality() : NodeLocality() { zeroDoor(); };
    virtual                         ~DoorLocality() { zeroDoor(); };
    inline void                     reset() { zeroDoor(); zeroNode(); };
    inline void                     setGeometry( DoorGeometry* const geometry ) { m_geometry = geometry; };
    inline void                     setGeometry( DoorGeometry& geometry ) { m_geometry = &geometry; };
    inline DoorGeometry*            getGeometry() const { return( m_geometry ); };
    inline void                     setFrameNone() { m_frame_type = EDoorUnframed; };
    void                            setFrameAuto();
    inline void                     setFrameType( const EDoorFrame frame_type ) { m_frame_type = static_cast< uint >( frame_type ); };
    inline EDoorFrame               getFrameType() const { return( static_cast< EDoorFrame >( m_frame_type ) ); };
    inline uint                     getFaceItemCount() const { return( m_face_item_count ); };
    inline ItemLocality*            getFaceFirstItem() const { return( m_face_first_item ); };
    inline uint                     getBackItemCount() const { return( m_back_item_count ); };
    inline ItemLocality*            getBackFirstItem() const { return( m_back_first_item ); };
    void                            setRoom( RoomLocality* room, const bool reparent = false );
    inline void                     setRoom( RoomLocality& room, const bool reparent = false ) { setRoom( &room, reparent ); };
    inline RoomLocality*            getRoom() const { return( m_room ); };
    inline DoorLocality*            getNextByRoom() const { return( m_next_by_room ); };
    inline DoorLocality*            getPrevByRoom() const { return( m_prev_by_room ); };
    inline void                     setTargetLink( DoorLocality* const link_exit ) { if( m_face_target_door != NULL ) m_face_target_door->m_back_target_door = NULL; m_face_target_door = link_exit; if( m_face_target_door != NULL ) m_face_target_door->m_back_target_door = this; };
    inline void                     setTargetLink( DoorLocality& link_exit ) { setTargetLink( &link_exit ); };
    inline void                     setFaceTarget( DoorLocality* const face_target ) { m_face_target_door = face_target; };
    inline void                     setFaceTarget( DoorLocality& face_target ) { setFaceTarget( &face_target ); };
    inline DoorLocality*            getFaceTarget() const { return( m_face_target_door ); };
    inline void                     setBackTarget( DoorLocality* const back_target ) { m_back_target_door = back_target; };
    inline void                     setBackTarget( DoorLocality& back_target ) { setBackTarget( &back_target ); };
    inline DoorLocality*            getBackTarget() const { return( m_back_target_door ); };
    inline void                     setBlocking( const bool blocking = true ) { m_blocking = blocking; };
    inline void                     setEthereal( const bool ethereal = true ) { setBlocking( !ethereal ); };
    inline bool                     isBlocking() const { return( m_blocking ); };
    inline bool                     isEthereal() const { return( !isBlocking() ); };
    inline void                     setEmbedded( const bool embedded = true ) { m_embedded = embedded; };
    inline void                     setIsolated( const bool isolated = true ) { setEmbedded( !isolated ); };
    inline bool                     isEmbedded() const { return( m_embedded ); };
    inline bool                     isIsolated() const { return( !isEmbedded() ); };
    bool                            isFastPass( const bool face ) const;
    inline bool                     isFullPass( const bool face ) const { return( !isFastPass( face ) ); };
    float                           getRelativeZ( const maths::vec3& point ) const;
    inline void                     getDoorPlane( maths::plane& aplane, const bool face ) const { face ? getFacePlane( aplane ) : getBackPlane( aplane ); };
    void                            getFacePlane( maths::plane& aplane ) const;
    void                            getBackPlane( maths::plane& aplane ) const;
    void                            getDoorRemap( const DoorLocality& target, maths::joint& remap ) const;
    void                            getDoorRemap( const DoorLocality* const target, maths::joint& remap ) const;
    inline void                     getFaceRemap( maths::joint& remap ) const { getDoorRemap( m_face_target_door, remap ); };
    inline void                     getBackRemap( maths::joint& remap ) const { getDoorRemap( m_back_target_door, remap ); };
    bool                            excludeSphere( const maths::sphere& asphere, maths::vec3& reposition ) const;
    bool                            collideSphere( const maths::sphere& asphere, const maths::vec3& direction, const float min_t, const float max_t, float& t, maths::vec3& normal ) const;
    bool                            interceptSphere( const maths::sphere& asphere, const maths::vec3& direction, const float min_t, const float max_t, float& t, bool& face ) const;
    bool                            includingSphere( const maths::sphere& asphere, bool& face ) const;
    inline bool                     isLooped() const { return( ( m_face_target_door == this ) && ( m_back_target_door == this ) ); };
    inline bool                     isClosed() const { return( ( m_face_target_door == NULL ) && ( m_back_target_door == NULL ) ); };
    inline EDoorState               getState() const { uint state = EDoorClosed; if( m_face_target_door != NULL ) state |= EDoorFaceOpen; if( m_back_target_door != NULL ) state |= EDoorBackOpen; return( static_cast< EDoorState >( state ) ); };
protected:
    void                            zeroDoor();
    static void                     remapDoor( const maths::joint& src, const maths::joint& dst, maths::joint& remap );
    DoorGeometry*                   m_geometry;
    RoomLocality*                   m_room;
    DoorLocality*                   m_prev_by_room;
    DoorLocality*                   m_next_by_room;
    DoorLocality*                   m_face_target_door;
    DoorLocality*                   m_back_target_door;
    ItemLocality*                   m_face_first_item;
    ItemLocality*                   m_back_first_item;
    uint                            m_face_item_count;
    uint                            m_back_item_count;
    uint                            m_frame_type;
    bool                            m_blocking;
    bool                            m_embedded;
    friend class                    RoomLocality;
    friend class                    ItemLocality;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    RoomLocality room scene graph node class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class RoomLocality : public NodeLocality
{
public:
    inline                          RoomLocality() : NodeLocality() { zeroRoom(); };
    virtual                         ~RoomLocality() { zeroRoom(); };
    inline void                     reset() { zeroRoom(); zeroNode(); };
    inline void                     setGeometry( RoomGeometry* const geometry ) { m_geometry = geometry; };
    inline void                     setGeometry( RoomGeometry& geometry ) { m_geometry = &geometry; };
    inline RoomGeometry*            getGeometry() const { return( m_geometry ); };
    inline uint                     getItemCount() const { return( m_item_count ); };
    inline ItemLocality*            getFirstItem() const { return( m_first_item ); };
    inline uint                     getDoorCount() const { return( m_door_count ); };
    inline DoorLocality*            getFirstDoor() const { return( m_first_door ); };
    bool                            containsPoint( const maths::vec3& point ) const;
    bool                            excludeSphere( const maths::sphere& asphere, maths::vec3& reposition ) const;
    bool                            excludeSphere( const maths::sphere& asphere, const maths::plane& reject, maths::vec3& reposition ) const;
    bool                            collideSphere( const maths::sphere& asphere, const maths::vec3& direction, const float min_t, const float max_t, float& t, maths::vec3& normal ) const;
    bool                            sphereExclude( const maths::sphere& asphere, maths::vec3& reposition ) const;
    bool                            sphereCollide( const maths::sphere& asphere, const maths::vec3& direction, const float min_t, const float max_t, float& t, maths::vec3& normal ) const;
    inline void                     setRejectBias( const float bias ) { m_reject_bias = bias; };
    inline float                    getRejectBias() const { return( m_reject_bias ); };
protected:
    void                            zeroRoom();
    RoomGeometry*                   m_geometry;
    ItemLocality*                   m_first_item;
    DoorLocality*                   m_first_door;
    uint                            m_item_count;
    uint                            m_door_count;
    float                           m_reject_bias;
    friend class                    ItemLocality;
    friend class                    DoorLocality;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    ViewLocality view scene graph node class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ViewLocality : public NodeLocality
{
public:
    inline                          ViewLocality() : NodeLocality() { zeroView(); };
    virtual                         ~ViewLocality() { zeroView(); };
    inline void                     reset() { zeroView(); zeroNode(); };
    inline void                     setRoom( RoomLocality* room ) { m_room = room; };
    inline void                     setRoom( RoomLocality& room ) { m_room = &room; };
    inline RoomLocality*            getRoom() const { return( m_room ); };
    inline ViewLocality*            getNextView() const { return( m_next_view ); };
    inline ViewLocality*            getPrevView() const { return( m_prev_view ); };
    inline void                     setFreeCam( const bool set = true ) { m_freeCam = set; };
    inline bool                     isFreeCam() const { return( m_freeCam ); };
    inline void                     setRadius( const float radius ) { m_radius = radius; };
    inline float                    getRadius() const { return( m_radius ); };
    inline void                     setTargets( const ResourceRTV& targetRTV, const ResourceDSV& targetDSV ) { m_targetRTV = targetRTV; m_targetDSV = targetDSV; };
    inline void                     setTargetRTV( const ResourceRTV& targetRTV ) { m_targetRTV = targetRTV; };
    inline void                     setTargetDSV( const ResourceDSV& targetDSV ) { m_targetDSV = targetDSV; };
    inline const ResourceRTV&       getTargetRTV() const { return( m_targetRTV ); };
    inline const ResourceDSV&       getTargetDSV() const { return( m_targetDSV ); };
    inline maths::projector&        getProjector() { return( m_projector ); };
    inline const maths::projector&  getProjector() const { return( m_projector ); };
    inline maths::clipper&          getClipper() { return( m_clipper ); };
    inline const maths::clipper&    getClipper() const { return( m_clipper ); };
    inline const FBOX&              getViewport() const { return( m_viewport ); };
    inline const DIMS2D&            getDimensions() const { return( m_dimensions ); };
    inline const SRECT&             getScissoring() const { return( m_scissoring ); };
    inline const maths::box_faces&  getClipPlanes() const { return( m_clipPlanes ); };
    inline const maths::mat44&      getProjection() const { return( m_projection ); };
    inline const maths::joint&      getViewToWorld() const { return( m_viewToWorld ); };
    inline void                     resetStencilRef() { m_stencilRef = 0; };
    inline uint                     getStencilRef() const { return( m_stencilRef ); };
    inline uint                     getStencilNew() { uint stencilRef = m_stencilRef; m_stencilRef += 2; return( stencilRef ); };
    inline void                     incStencilRef() { m_stencilRef += 2; };
    void                            setWindow( const SRECT& window );
    void                            setWindow( const DIMS2D& dimensions );
    void                            setWindow( const uint width, const uint height );
    bool                            adjustWindow( const maths::frect& extents, SRECT& window ) const;
    void                            getWindowed( const SRECT& window, maths::mat44& projection ) const;
    void                            getOrigin( maths::vec3& origin );
    void                            evaluate();
protected:
    void                            zeroView();
    RoomLocality*                   m_room;
    ViewLocality*                   m_prev_view;
    ViewLocality*                   m_next_view;
    ResourceRTV                     m_targetRTV;
    ResourceDSV                     m_targetDSV;
    maths::projector                m_projector;
    maths::clipper                  m_clipper;
    FBOX                            m_viewport;
    SRECT                           m_scissoring;
    DIMS2D                          m_dimensions;
    maths::box_faces                m_clipPlanes;
    maths::mat44                    m_projection;
    maths::joint                    m_viewToWorld;
    uint                            m_stencilRef;
    bool                            m_freeCam;
    float                           m_radius;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    Localities pool types
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using NodeLocalities = TManagedReset< NodeLocality >;
using DoorLocalities = TManagedReset< DoorLocality >;
using ItemLocalities = TManagedReset< ItemLocality >;
using RoomLocalities = TManagedReset< RoomLocality >;
using ViewLocalities = TManagedReset< ViewLocality >;

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
////    include guard end
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif  //  #ifndef __HIERARCHIES_INCLUDED__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
