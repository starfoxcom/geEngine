/*****************************************************************************/
/**
 * @file    geFwdDeclUtil.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2015/02/16
 * @brief   Utility header that makes forward declarations
 *
 * This header makes forward declarations for all the needed types
 * of the geUtility library
 *
 * @bug	    No known bugs.
 */
/*****************************************************************************/
#pragma once

namespace geEngineSDK {
  /**
   * @brief	Generic axis enum (mirrored for property use in Object.h)
   */
  namespace AXIS {
    enum E {
      kNone,
      kX,
      kY,
      kZ,
    };
  }

  /**
   * @brief Values that represent in which order are Euler angles applied when
   *        used in transformations.
   */
  namespace EULER_ANGLE_ORDER {
    enum E {
      kXYZ,
      kXZY,
      kYXZ,
      kYZX,
      kZXY,
      kZYX
    };
  }

  /**
   * @brief	Enum that defines possible window border styles.
   */
  namespace WINDOW_BORDER {
    enum E {
      kNormal,
      kNone,
      kFixed
    };
  }

  /**
   * @brief	Values that represent that a structure or class will be initialized
   *        with a "forced value" and what kind.
   */
	namespace FORCE_INIT {
    enum E {
      kForceInit,
      kForceInitToZero
    };
  }

  /***************************************************************************/
	/**
   * Forward declaration of utility classes
   */
  /***************************************************************************/
  class Angle;
  class AABox;
  class Degree;
  class Math;
  class Matrix3;
  class Matrix4;
  class Plane;
  class Quaternion;
  class Radian;
  class Ray;
  class Sphere;
  class Vector2;
  class Vector3;
  class Vector4;
  struct Vector2I;
  class Rect2I;
  class Rect2;
  class Rect3;
  class Color;
  class LinearColor;

  class DynLib;
  class DynLibManager;

  class DataStream;
  class MemoryDataStream;
  class FileDataStream;

  class MeshData;
  class FileSystem;
  class Timer;
  class Task;

  class GpuResourceData;
  class PixelData;

  class HString;
  class StringTable;
  struct LocalizedStringData;
  class Path;

  class HThread;
  class TestSuite;
  class TestOutput;
  class AsyncOpSyncData;

  struct RTTIField;
  struct RTTIReflectablePtrFieldBase;

  struct SerializedObject;
  struct SerializedInstance;
  class FrameAlloc;
  class LogEntry;

  /***************************************************************************/
  /**
   * Reflection interfaces
   */
  /***************************************************************************/
  class IReflectable;
  class RTTITypeBase;

  /***************************************************************************/
  /**
   * Serialization interfaces
   */
  /***************************************************************************/
  class ISerializable;
  class SerializableType;

  /***************************************************************************/
  /**
   * Smart pointer types
   */
  /***************************************************************************/
  typedef std::shared_ptr<DataStream> DataStreamPtr;
  typedef std::shared_ptr<MemoryDataStream> MemoryDataStreamPtr;
  typedef std::shared_ptr<FileDataStream> FileDataStreamPtr;
  typedef std::shared_ptr<MeshData> MeshDataPtr;
  typedef std::shared_ptr<PixelData> PixelDataPtr;
  typedef std::shared_ptr<GpuResourceData> GpuResourceDataPtr;
  typedef std::shared_ptr<DataStream> DataStreamPtr;
  typedef std::shared_ptr<MemoryDataStream> MemoryDataStreamPtr;
  typedef std::shared_ptr<Task> TaskPtr;
  typedef std::shared_ptr<TestSuite> TestSuitePtr;
  typedef std::shared_ptr<AsyncOpSyncData> AsyncOpSyncDataPtr;

  typedef List<DataStreamPtr> DataStreamList;
  typedef std::shared_ptr<DataStreamList> DataStreamListPtr;

  namespace TYPEID_UTILITY {
  	enum E {
      /**
       * TID_Abstract is a special type ID used for Abstract classes.
       * Only type ID that may be used by more than one class.
       */
      TID_Abstract              = 50,
      
      TID_StringID              = 60,
      TID_String                = 61,
      TID_WString               = 61,
      TID_Path                  = 62,

      TID_Vector                = 70,
      TID_Map                   = 71,
      TID_UnorderedMap          = 72,
      TID_Pair                  = 73,
      TID_Set                   = 74,
      TID_UnorderedSet          = 75,

      TID_SerializedInstance    = 80,
      TID_SerializedField       = 81,
      TID_SerializedObject      = 82,
      TID_SerializedArray       = 83,
      TID_SerializedEntry       = 84,
      TID_SerializedArrayEntry  = 85,
      TID_SerializedSubObject   = 86,
    };
  }
}
