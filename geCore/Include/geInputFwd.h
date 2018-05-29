/*****************************************************************************/
/**
 * @file    geInputFwd.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2018/05/26
 * @brief   Forward declarations of the Input system objects.
 *
 * Forward declarations of the Input system objects.
 *
 * @bug     No known bugs.
 */
/*****************************************************************************/
#pragma once

/*****************************************************************************/
/**
 * Includes
 */
/*****************************************************************************/
#include "gePrerequisitesCore.h"

#include <geVector2I.h>

namespace geEngineSDK {
  /**
   * @brief Contains all possible input buttons, including keyboard scan codes,
   *        mouse buttons and gamepad buttons.
   * @note  These codes are only keyboard scan codes. This means that exact
   *        scan code identifier might not correspond to that exact character
   *        on user's keyboard, depending on user's input locale. Only for US
   *        locale will these scan code names match the actual keyboard input.
   *        Think of the US key code names as only a convenience for more
   *        easily identifying which location on the keyboard a scan code
   *        represents.
   * @note  When storing these sequentially make sure to only reference the low
   *        order 2 bytes. Two high order bytes are used for various flags.
   */
  namespace BUTTON_CODE {
    enum E : uint32 {
      kUNASSIGNED = 0x00,
      kESCAPE = 0x01,
      k1 = 0x02,
      k2 = 0x03,
      k3 = 0x04,
      k4 = 0x05,
      k5 = 0x06,
      k6 = 0x07,
      k7 = 0x08,
      k8 = 0x09,
      k9 = 0x0A,
      k0 = 0x0B,
      kMINUS = 0x0C,      //"-" on main keyboard
      kEQUALS = 0x0D,
      kBACK = 0x0E,       //Backspace
      kTAB = 0x0F,
      kQ = 0x10,
      kW = 0x11,
      kE = 0x12,
      kR = 0x13,
      kT = 0x14,
      kY = 0x15,
      kU = 0x16,
      kI = 0x17,
      kO = 0x18,
      kP = 0x19,
      kLBRACKET = 0x1A,
      kRBRACKET = 0x1B,
      kRETURN = 0x1C,     //Enter on main keyboard
      kLCONTROL = 0x1D,
      kA = 0x1E,
      kS = 0x1F,
      kD = 0x20,
      kF = 0x21,
      kG = 0x22,
      kH = 0x23,
      kJ = 0x24,
      kK = 0x25,
      kL = 0x26,
      kSEMICOLON = 0x27,
      kAPOSTROPHE = 0x28,
      kGRAVE = 0x29,      //Accent
      kLSHIFT = 0x2A,
      kBACKSLASH = 0x2B,
      kZ = 0x2C,
      kX = 0x2D,
      kC = 0x2E,
      kV = 0x2F,
      kB = 0x30,
      kN = 0x31,
      kM = 0x32,
      kCOMMA = 0x33,
      kPERIOD = 0x34,     //"." on main keyboard
      kSLASH = 0x35,      //"/" on main keyboard
      kRSHIFT = 0x36,
      kMULTIPLY = 0x37,   //"*" on numeric keypad
      kLMENU = 0x38,      //Left Alt
      kSPACE = 0x39,
      kCAPITAL = 0x3A,
      kF1 = 0x3B,
      kF2 = 0x3C,
      kF3 = 0x3D,
      kF4 = 0x3E,
      kF5 = 0x3F,
      kF6 = 0x40,
      kF7 = 0x41,
      kF8 = 0x42,
      kF9 = 0x43,
      kF10 = 0x44,
      kNUMLOCK = 0x45,
      kSCROLL = 0x46,     // Scroll Lock
      kNUMPAD7 = 0x47,
      kNUMPAD8 = 0x48,
      kNUMPAD9 = 0x49,
      kSUBTRACT = 0x4A,   //"-" on numeric keypad
      kNUMPAD4 = 0x4B,
      kNUMPAD5 = 0x4C,
      kNUMPAD6 = 0x4D,
      kADD = 0x4E,        //"+" on numeric keypad
      kNUMPAD1 = 0x4F,
      kNUMPAD2 = 0x50,
      kNUMPAD3 = 0x51,
      kNUMPAD0 = 0x52,
      kDECIMAL = 0x53,    //"." on numeric keypad
      kOEM_102 = 0x56,    //"< > |" on UK/Germany keyboards
      kF11 = 0x57,
      kF12 = 0x58,
      kF13 = 0x64,        //(NEC PC98)
      kF14 = 0x65,        //(NEC PC98)
      kF15 = 0x66,        //(NEC PC98)
      kKANA = 0x70,       //(Japanese keyboard)
      kABNT_C1 = 0x73,    //"/ ?" on Portuguese (Brazilian) keyboards
      kCONVERT = 0x79,    //(Japanese keyboard)
      kNOCONVERT = 0x7B,  //(Japanese keyboard)
      kYEN = 0x7D,        //(Japanese keyboard)
      kABNT_C2 = 0x7E,    //Numpad "." on Portuguese (Brazilian) keyboards
      kNUMPADEQUALS = 0x8D, //"=" on numeric keypad (NEC PC98)
      kPREVTRACK = 0x90,    //Previous Track (CIRCUMFLEX on Japanese keyboard)
      kAT = 0x91,         //(NEC PC98)
      kCOLON = 0x92,      //(NEC PC98)
      kUNDERLINE = 0x93,  //(NEC PC98)
      kKANJI = 0x94,      //(Japanese keyboard)
      kSTOP = 0x95,       //(NEC PC98)
      kAX = 0x96,         //(Japan AX)
      kUNLABELED = 0x97,  //(J3100)
      kNEXTTRACK = 0x99,  //Next Track
      kNUMPADENTER = 0x9C,//Enter on numeric keypad
      kRCONTROL = 0x9D,
      kMUTE = 0xA0,       //Mute
      kCALCULATOR = 0xA1, //Calculator
      kPLAYPAUSE = 0xA2,  //Play / Pause
      kMEDIASTOP = 0xA4,  //Media Stop
      kVOLUMEDOWN = 0xAE, //Volume -
      kVOLUMEUP = 0xB0,   //Volume +
      kWEBHOME = 0xB2,    //Web home
      kNUMPADCOMMA = 0xB3,//"," on numeric keypad (NEC PC98)
      kDIVIDE = 0xB5,     //"/" on numeric keypad
      kSYSRQ = 0xB7,
      kRMENU = 0xB8,      //Right Alt
      kPAUSE = 0xC5,      //Pause
      kHOME = 0xC7,       //Home on arrow keypad
      kUP = 0xC8,         //UpArrow on arrow keypad
      kPGUP = 0xC9,       //PgUp on arrow keypad
      kLEFT = 0xCB,       //LeftArrow on arrow keypad
      kRIGHT = 0xCD,      //RightArrow on arrow keypad
      kEND = 0xCF,        //End on arrow keypad
      kDOWN = 0xD0,       //DownArrow on arrow keypad
      kPGDOWN = 0xD1,     //PgDn on arrow keypad
      kINSERT = 0xD2,     //Insert on arrow keypad
      kDELETE = 0xD3,     //Delete on arrow keypad
      kLWIN = 0xDB,       //Left Windows key
      kRWIN = 0xDC,       //Right Windows key
      kAPPS = 0xDD,       //AppMenu key
      kPOWER = 0xDE,      //System Power
      kSLEEP = 0xDF,      //System Sleep
      kWAKE = 0xE3,       //System Wake
      kWEBSEARCH = 0xE5,  //Web Search
      kWEBFAVORITES = 0xE6,//Web Favorites
      kWEBREFRESH = 0xE7, //Web Refresh
      kWEBSTOP = 0xE8,    //Web Stop
      kWEBFORWARD = 0xE9, //Web Forward
      kWEBBACK = 0xEA,    //Web Back
      kMYCOMPUTER = 0xEB, //My Computer
      kMAIL = 0xEC,       //Mail
      kMEDIASELECT = 0xED,//Media Select
      kMOUSE_LEFT = 0x800000EE, //Mouse buttons
      kMOUSE_RIGHT,
      kMOUSE_MIDDLE,
      kMOUSE_BTN4,
      kMOUSE_BTN5,
      kMOUSE_BTN6,
      kMOUSE_BTN7,
      kMOUSE_BTN8,
      kMOUSE_BTN9,
      kMOUSE_BTN10,
      kMOUSE_BTN11,
      kMOUSE_BTN12,
      kMOUSE_BTN13,
      kMOUSE_BTN14,
      kMOUSE_BTN15,
      kMOUSE_BTN16,
      kMOUSE_BTN17,
      kMOUSE_BTN18,
      kMOUSE_BTN19,
      kMOUSE_BTN20,
      kMOUSE_BTN21,
      kMOUSE_BTN22,
      kMOUSE_BTN23,
      kMOUSE_BTN24,
      kMOUSE_BTN25,
      kMOUSE_BTN26,
      kMOUSE_BTN27,
      kMOUSE_BTN28,
      kMOUSE_BTN29,
      kMOUSE_BTN30,
      kMOUSE_BTN31,
      kMOUSE_BTN32,
      kGAMEPAD_A = 0x4000010F,
      kGAMEPAD_B,
      kGAMEPAD_X,
      kGAMEPAD_Y,
      kGAMEPAD_LB,
      kGAMEPAD_RB,
      kGAMEPAD_LS,
      kGAMEPAD_RS,
      kGAMEPAD_BACK,
      kGAMEPAD_START,
      kGAMEPAD_DPAD_LEFT,
      kGAMEPAD_DPAD_RIGHT,
      kGAMEPAD_DPAD_UP,
      kGAMEPAD_DPAD_DOWN,
      kGAMEPAD_BTN1,
      kGAMEPAD_BTN2,
      kGAMEPAD_BTN3,
      kGAMEPAD_BTN4,
      kGAMEPAD_BTN5,
      kGAMEPAD_BTN6,
      kGAMEPAD_BTN7,
      kGAMEPAD_BTN8,
      kGAMEPAD_BTN9,
      kGAMEPAD_BTN10,
      kGAMEPAD_BTN11,
      kGAMEPAD_BTN12,
      kGAMEPAD_BTN13,
      kGAMEPAD_BTN14,
      kGAMEPAD_BTN15,
      kGAMEPAD_BTN16,
      kGAMEPAD_BTN17,
      kGAMEPAD_BTN18,
      kGAMEPAD_BTN19,
      kGAMEPAD_BTN20,
      kGAMEPAD_DPAD_UPLEFT,
      kGAMEPAD_DPAD_UPRIGHT,
      kGAMEPAD_DPAD_DOWNLEFT,
      kGAMEPAD_DPAD_DOWNRIGHT,

      //IMPORTANT: Make sure to update these if you modify the values above
      kNumKeys = kMEDIASELECT - kUNASSIGNED + 1, 
      kNumMouse = kMOUSE_BTN32 - kMOUSE_LEFT + 1,
      kNumGamepad = kGAMEPAD_DPAD_DOWNRIGHT - kGAMEPAD_A + 1,
      kCount = kNumKeys + kNumMouse + kNumGamepad
    };
  }

  /**
   * @brief Contains data about a button input event.
   */
  struct ButtonEvent
  {
   public:
    ButtonEvent() : m_isUsed(false) {}

    /**
     * @brief Button code this event is referring to.
     */
    BUTTON_CODE::E buttonCode;

    /**
     * @brief Timestamp in ticks when the event happened.
     */
    uint64 timestamp;

    /**
     * @brief Index of the device that the event originated from.
     */
    uint32 deviceIdx;

    /**
     * @brief Query is the pressed button a keyboard button.
     */
    bool
    isKeyboard() const {
      return (buttonCode & 0xC0000000) == 0;
    }

    /**
     * @brief Query is the pressed button a mouse button.
     */
    bool
    isMouse() const {
      return (buttonCode & 0x80000000) != 0;
    }

    /**
     * @brief Query is the pressed button a gamepad button.
     */
    bool
    isGamepad() const {
      return (buttonCode & 0x40000000) != 0;
    }

    /**
     * @brief Check if the event has been marked as used. Internally this means
     *        nothing but caller might choose to ignore an used event.
     */
    bool
    isUsed() const {
      return m_isUsed;
    }

    /**
     * @brief Mark the event as used. Internally this means nothing but caller
     *        might choose to ignore an used event.
     */
    void
    markAsUsed() const {
      m_isUsed = true;
    }
   private:
    mutable bool m_isUsed;
  };

  /**
   * @brief Pointer buttons. Generally these correspond to mouse buttons, but
   *        may be used in some form for touch input as well.
   */
  namespace POINTER_EVENT_BUTTON {
    enum E {
      kLeft,
      kMiddle,
      kRight,
      kCount
    };
  }

  /**
   * @brief Type of pointer event.
   */
  namespace POINTER_EVENT_TYPE {
    enum E {
      kCursorMoved,
      kButtonPressed,
      kButtonReleased,
      kDoubleClick
    };
  }

  /**
   * @brief Event that gets sent out when user interacts with the screen in
   *        some way, usually by moving the mouse cursor or using touch input.
   */
  struct PointerEvent
  {
  public:
    PointerEvent()
      : button(POINTER_EVENT_BUTTON::kLeft),
        type(POINTER_EVENT_TYPE::kCursorMoved),
        shift(false),
        control(false),
        alt(false),
        mouseWheelScrollAmount(0.0f),
        m_isUsed(false) {
      buttonStates[0] = false;
      buttonStates[1] = false;
      buttonStates[2] = false;
    }

    /**
     * @brief Screen position where the input event occurred.
     */
    Vector2I screenPos;

    /**
     * @brief Change in movement since last sent event.
     */
    Vector2I delta;

    /**
     * @brief States of the pointer buttons (for example mouse buttons).
     */
    bool buttonStates[(uint32)POINTER_EVENT_BUTTON::kCount];

    /**
     * @brief Button that triggered the pointer event. Might be irrelevant
     *        depending on event type. (for example move events don't
     *        correspond to a button.
     */
    POINTER_EVENT_BUTTON::E button;

    /**
     * @brief Type of the pointer event.
     */
    POINTER_EVENT_TYPE::E type;

    /**
     * @brief Is shift button on the keyboard being held down.
     */
    bool shift;
    
    /**
     * @brief Is control button on the keyboard being held down.
     */
    bool control;

    /**
     * @brief Is alt button on the keyboard being held down.
     */
    bool alt;

    /**
     * @brief If mouse wheel is being scrolled, what is the amount.
     *        Only relevant for move events.
     */
    float mouseWheelScrollAmount;

    /**
     * @brief Check if the event has been marked as used. Internally this means
     *        nothing but caller might choose to ignore an used event.
     */
    bool
    isUsed() const {
      return m_isUsed;
    }

    /**
     * @brief Mark the event as used. Internally this means nothing but caller
     *        might choose to ignore an used event.
     */
    void
    markAsUsed() const {
      m_isUsed = true;
    }

   private:
    mutable bool m_isUsed;
  };

  /**
   * @brief Types of special input commands.
   */
  namespace INPUT_COMMAND_TYPE {
    enum E {
      kCursorMoveLeft,
      kCursorMoveRight,
      kCursorMoveUp,
      kCursorMoveDown,
      kSelectLeft,
      kSelectRight,
      kSelectUp,
      kSelectDown,
      kEscape,
      kDelete,
      kBackspace,
      kReturn,
      kConfirm,
      kTab
    };
  }

  /**
   * @brief Event that gets sent out when user inputs some text. These events
   *        may be preceded by normal button events if user is typing on a
   *        keyboard.
   */
  struct TextInputEvent
  {
   public:
    TextInputEvent() : m_isUsed(false) {}

    /**
     * Character the that was input.
     */
    uint32 textChar;

    /**
     * @brief Check if the event has been marked as used. Internally this means
     *        nothing but caller might choose to ignore an used event.
     */
    bool
    isUsed() const {
      return m_isUsed;
    }

    /**
     * @brief Mark the event as used. Internally this means nothing but caller
     *        might choose to ignore an used event.
     */
    void
    markAsUsed() const {
      m_isUsed = true;
    }

   private:
    mutable bool m_isUsed;
  };

  /**
   * @brief Types of input devices.
   */
  namespace INPUT_DEVICE {
    enum E {
      kKeyboard,
      kMouse,
      kGamepad,
      kCount
    };
  }

  /**
   * @brief Common input axis types.
   */
  namespace INPUT_AXIS {
    enum E {
      /**
       * Mouse axis X. Provides unnormalized relative movement.
       */
      kMouseX,

      /**
       * Mouse axis Y. Provides unnormalized relative movement.
       */
      kMouseY,

      /**
       * Mouse wheel/scroll axis. Provides unnormalized relative movement.
       */
      kMouseZ,

      /**
       * Gamepad left stick X. Provides normalized ([-1, 1] range)
       * absolute position.
       */
      kLeftStickX,

      /**
       * Gamepad left stick Y. Provides normalized ([-1, 1] range)
       * absolute position.
       */
      kLeftStickY,

      /**
       * Gamepad right stick X. Provides normalized ([-1, 1] range)
       * absolute position.
       */
      kRightStickX,

      /**
       * Gamepad right stick Y. Provides normalized ([-1, 1] range)
       * absolute position.
       */
      kRightStickY,

      /**
       * Gamepad left trigger. Provides normalized ([-1, 1] range)
       * absolute position.
       */
      kLeftTrigger,

      /**
       * Gamepad right trigger. Provides normalized ([-1, 1] range)
       * absolute position.
       */
      kRightTrigger,

      kCount
    };
  }

  /**
   * @brief Modifiers used with along with keyboard buttons.
   */
  namespace BUTTON_MODIFIER {
    enum E {
      kNone = 0x00,
      kShift = 0x01,
      kCtrl = 0x02,
      kAlt = 0x04,
      kShiftCtrl = 0x03,
      kCtrlAlt = 0x06,
      kShiftAlt = 0x05,
      kShiftCtrlAlt = 0x07
    };
  }
}
