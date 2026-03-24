
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    File:   game_pad.h
////    Author: Ritchie Brannan
////    Date:   16 Dec 14
////
////    Description:
////
////        Game pad controller input classes.
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

#ifndef	__GAME_PAD_INCLUDED__
#define	__GAME_PAD_INCLUDED__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    includes
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "libs/system/base/types.h"
#include "libs/maths/vec2.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    begin input namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace input
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    EGamePadButton enumeration of game pad button identifiers
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum EGamePadButton
{
    EGamePadStart = 0,
    EGamePadBack,
    EGamePadStickL,
    EGamePadStickR,
    EGamePadShoulderL,
    EGamePadShoulderR,
    EGamePadUp,
    EGamePadDown,
    EGamePadLeft,
    EGamePadRight,
    EGamePadA,
    EGamePadB,
    EGamePadX,
    EGamePadY,
    EGamePadButtonCount,
    EGamePadGreen = EGamePadA,
    EGamePadRed = EGamePadB,
    EGamePadBlue = EGamePadX,
    EGamePadYellow = EGamePadY,
    EGamePadButtonForce32 = 0x7fffffff
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    EGamePadBits enumeration of game pad button bits
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

enum EGamePadBits
{
    EGamePadBitStart        = ( 1 << EGamePadStart ),
    EGamePadBitBack         = ( 1 << EGamePadBack ),
    EGamePadBitStickL       = ( 1 << EGamePadStickL ),
    EGamePadBitStickR       = ( 1 << EGamePadStickR ),
    EGamePadBitShoulderL    = ( 1 << EGamePadShoulderL ),
    EGamePadBitShoulderR    = ( 1 << EGamePadShoulderR ),
    EGamePadBitUp           = ( 1 << EGamePadUp ),
    EGamePadBitDown         = ( 1 << EGamePadDown ),
    EGamePadBitLeft         = ( 1 << EGamePadLeft ),
    EGamePadBitRight        = ( 1 << EGamePadRight ),
    EGamePadBitA            = ( 1 << EGamePadA ),
    EGamePadBitB            = ( 1 << EGamePadB ),
    EGamePadBitX            = ( 1 << EGamePadX ),
    EGamePadBitY            = ( 1 << EGamePadY ),
    EGamePadBitGreen        = ( 1 << EGamePadGreen ),
    EGamePadBitRed          = ( 1 << EGamePadRed ),
    EGamePadBitBlue         = ( 1 << EGamePadBlue ),
    EGamePadBitYellow       = ( 1 << EGamePadYellow ),
    EGamePadBitsForce32     = 0x7fffffff
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    CGamePadState game pad controller state class
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CGamePadState
{
public:
    inline                      CGamePadState() { zero(); };
    inline                      ~CGamePadState() { zero(); };
    inline float                lTrigger() const { return( m_lTrigger ); };
    inline float                rTrigger() const { return( m_rTrigger ); };
    inline const maths::vec2&   lStick() const { return( m_lStick ); };
    inline const maths::vec2&   rStick() const { return( m_rStick ); };
    inline const maths::vec2&   dPad() const { return( m_dPad ); };
    inline uint                 getState() const { return( m_buttonState ); };
    inline bool                 buttonState( const EGamePadButton button ) const { return( ( m_buttonState & ( 1 << button ) ) ? true : false ); };
    inline bool                 buttonPressed( const EGamePadButton button ) const { return( ( m_buttonPressed & ( 1 << button ) ) ? true : false ); };
    inline bool                 buttonReleased( const EGamePadButton button ) const { return( ( m_buttonReleased & ( 1 << button ) ) ? true : false ); };
    inline bool                 startState() const { return( ( m_buttonState & EGamePadBitStart ) ? true : false ); };
    inline bool                 startPressed() const { return( ( m_buttonPressed & EGamePadBitStart ) ? true : false ); };
    inline bool                 startReleased() const { return( ( m_buttonReleased & EGamePadBitStart ) ? true : false ); };
    inline bool                 backState() const { return( ( m_buttonState & EGamePadBitBack ) ? true : false ); };
    inline bool                 backPressed() const { return( ( m_buttonPressed & EGamePadBitBack ) ? true : false ); };
    inline bool                 backReleased() const { return( ( m_buttonReleased & EGamePadBitBack ) ? true : false ); };
    inline bool                 lStickState() const { return( ( m_buttonState & EGamePadBitStickL ) ? true : false ); };
    inline bool                 lStickPressed() const { return( ( m_buttonPressed & EGamePadBitStickL ) ? true : false ); };
    inline bool                 lStickReleased() const { return( ( m_buttonReleased & EGamePadBitStickL ) ? true : false ); };
    inline bool                 rStickState() const { return( ( m_buttonState & EGamePadBitStickR ) ? true : false ); };
    inline bool                 rStickPressed() const { return( ( m_buttonPressed & EGamePadBitStickR ) ? true : false ); };
    inline bool                 rStickReleased() const { return( ( m_buttonReleased & EGamePadBitStickR) ? true : false ); };
    inline bool                 lShoulderState() const { return( ( m_buttonState & EGamePadBitShoulderL ) ? true : false ); };
    inline bool                 lShoulderPressed() const { return( ( m_buttonPressed & EGamePadBitShoulderL ) ? true : false ); };
    inline bool                 lShoulderReleased() const { return( ( m_buttonReleased & EGamePadBitShoulderL ) ? true : false ); };
    inline bool                 rShoulderState() const { return( ( m_buttonState & EGamePadBitShoulderR ) ? true : false ); };
    inline bool                 rShoulderPressed() const { return( ( m_buttonPressed & EGamePadBitShoulderR ) ? true : false ); };
    inline bool                 rShoulderReleased() const { return( ( m_buttonReleased & EGamePadBitShoulderR ) ? true : false ); };
    inline bool                 upState() const { return( ( m_buttonState & EGamePadBitUp ) ? true : false ); };
    inline bool                 upPressed() const { return( ( m_buttonPressed & EGamePadBitUp ) ? true : false ); };
    inline bool                 upReleased() const { return( ( m_buttonReleased & EGamePadBitUp ) ? true : false ); };
    inline bool                 downState() const { return( ( m_buttonState & EGamePadBitDown ) ? true : false ); };
    inline bool                 downPressed() const { return( ( m_buttonPressed & EGamePadBitDown ) ? true : false ); };
    inline bool                 downReleased() const { return( ( m_buttonReleased & EGamePadBitDown ) ? true : false ); };
    inline bool                 leftState() const { return( ( m_buttonState & EGamePadBitLeft ) ? true : false ); };
    inline bool                 leftPressed() const { return( ( m_buttonPressed & EGamePadBitLeft ) ? true : false ); };
    inline bool                 leftReleased() const { return( ( m_buttonReleased & EGamePadBitLeft ) ? true : false ); };
    inline bool                 rightState() const { return( ( m_buttonState & EGamePadBitRight ) ? true : false ); };
    inline bool                 rightPressed() const { return( ( m_buttonPressed & EGamePadBitRight ) ? true : false ); };
    inline bool                 rightReleased() const { return( ( m_buttonReleased & EGamePadBitRight ) ? true : false ); };
    inline bool                 greenState() const { return( ( m_buttonState & EGamePadBitGreen ) ? true : false ); };
    inline bool                 greenPressed() const { return( ( m_buttonPressed & EGamePadBitGreen ) ? true : false ); };
    inline bool                 greenReleased() const { return( ( m_buttonReleased & EGamePadBitGreen ) ? true : false ); };
    inline bool                 redState() const { return( ( m_buttonState & EGamePadBitRed ) ? true : false ); };
    inline bool                 redPressed() const { return( ( m_buttonPressed & EGamePadBitRed ) ? true : false ); };
    inline bool                 redReleased() const { return( ( m_buttonReleased & EGamePadBitRed ) ? true : false ); };
    inline bool                 blueState() const { return( ( m_buttonState & EGamePadBitBlue ) ? true : false ); };
    inline bool                 bluePressed() const { return( ( m_buttonPressed & EGamePadBitBlue ) ? true : false ); };
    inline bool                 blueReleased() const { return( ( m_buttonReleased & EGamePadBitBlue ) ? true : false ); };
    inline bool                 yellowState() const { return( ( m_buttonState & EGamePadBitYellow ) ? true : false ); };
    inline bool                 yellowPressed() const { return( ( m_buttonPressed & EGamePadBitYellow) ? true : false ); };
    inline bool                 yellowReleased() const { return( ( m_buttonReleased & EGamePadBitYellow ) ? true : false ); };
    inline bool                 aState() const { return( ( m_buttonState & EGamePadBitA ) ? true : false ); };
    inline bool                 aPressed() const { return( ( m_buttonPressed & EGamePadBitA ) ? true : false ); };
    inline bool                 aReleased() const { return( ( m_buttonReleased & EGamePadBitA ) ? true : false ); };
    inline bool                 bState() const { return( ( m_buttonState & EGamePadBitB ) ? true : false ); };
    inline bool                 bPressed() const { return( ( m_buttonPressed & EGamePadBitB ) ? true : false ); };
    inline bool                 bReleased() const { return( ( m_buttonReleased & EGamePadBitB ) ? true : false ); };
    inline bool                 xState() const { return( ( m_buttonState & EGamePadBitX ) ? true : false ); };
    inline bool                 xPressed() const { return( ( m_buttonPressed & EGamePadBitX ) ? true : false ); };
    inline bool                 xReleased() const { return( ( m_buttonReleased & EGamePadBitX ) ? true : false ); };
    inline bool                 yState() const { return( ( m_buttonState & EGamePadBitY ) ? true : false ); };
    inline bool                 yPressed() const { return( ( m_buttonPressed & EGamePadBitY ) ? true : false ); };
    inline bool                 yReleased() const { return( ( m_buttonReleased & EGamePadBitY ) ? true : false ); };
protected:
    void                        zero();
    float                       m_lTrigger;
    float                       m_rTrigger;
    maths::vec2                 m_lStick;
    maths::vec2                 m_rStick;
    maths::vec2                 m_dPad;
    uint                        m_buttonState;
    uint                        m_buttonPressed;
    uint                        m_buttonReleased;
protected:
    static const float          c_deadZone;
    static const float          c_maxRange;
    static const float          c_constant;
    static const float          c_numerator;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end input namespace
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

};	//	namespace input

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    include guard end
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif  //  #ifndef __GAME_PAD_INCLUDED__

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    end of file
////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
