/*****************************************************************************/
/**
 * @file    gePrerequisitesCore.h
 * @author  Samuel Prince (samuel.prince.quezada@gmail.com)
 * @date    2017/11/01
 * @brief   Provides core engine functionality.
 *
 * Second lowest layer that provides core engine functionality and abstract
 * interfaces for various systems.
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
#include <gePrerequisitesUtil.h>

/**
 * Maximum number of surfaces that can be attached to a multi render target.
 */
#define GE_MAX_MULTIPLE_RENDER_TARGETS 8
#define GE_FORCE_SINGLETHREADED_RENDERING 0

/**
 * Maximum number of individual GPU queues, per type.
 */
#define GE_MAX_QUEUES_PER_TYPE 8

/**
 * Maximum number of hardware devices usable at once.
 */
#define GE_MAX_DEVICES 5U

/**
 * Maximum number of devices one resource can exist at the same time.
 */
#define GE_MAX_LINKED_DEVICES 4U

//DLL export
#if GE_PLATFORM == GE_PLATFORM_WIN32
# if GE_COMPILER == GE_COMPILER_MSVC
#   if defined( GE_STATIC_LIB )
#     define GE_CORE_EXPORT
#   else
#     if defined( GE_CORE_EXPORTS )
#       define GE_CORE_EXPORT __declspec( dllexport )
#     else
#       define GE_CORE_EXPORT __declspec( dllimport )
#     endif
#   endif
# else  //Any other Compiler
#   if defined( GE_STATIC_LIB )
#     define GE_CORE_EXPORT
#   else
#     if defined( GE_CORE_EXPORTS )
#       define GE_CORE_EXPORT __attribute__ ((dllexport))
#     else
#       define GE_CORE_EXPORT __attribute__ ((dllimport))
#     endif
#   endif
# endif
# define GE_CORE_HIDDEN
#else //Linux/Mac settings
# define GE_CORE_EXPORT __attribute__ ((visibility ("default")))
# define GE_CORE_HIDDEN __attribute__ ((visibility ("hidden")))
#endif

#include "geHString.h"

namespace geEngineSDK {
  static const StringID RenderAPIAny = "AnyRenderAPI";
  static const StringID RendererAny = "AnyRenderer";

  /***************************************************************************/
  /**
   * Forward declaration of Core classes
   */
   /***************************************************************************/
  class GpuProgram;
  class GpuProgramManager;
  class IndexBuffer;
  class VertexBuffer;
  class GpuBuffer;
  class GpuProgramManager;
  class GpuProgramFactory;
  class IndexData;
  class Pass;
  class Technique;
  class Shader;
  class Material;
  class RenderAPICapabilities;
  class RenderTarget;
  class RenderTexture;
  class RenderWindow;
  class RenderTargetProperties;
  class SamplerState;
  class TextureManager;
  class Viewport;
  class VertexDeclaration;

  class Input;
  struct PointerEvent;

  class RendererFactory;
  class AsyncOp;
  class HardwareBufferManager;
  class FontManager;
  class DepthStencilState;
  class RenderStateManager;
  class RasterizerState;
  class BlendState;
  class GpuParamBlock;
  class GpuParamBlockBuffer;
  class GpuParams;
  struct GpuParamDesc;
  struct GpuParamDataDesc;
  struct GpuParamObjectDesc;
  struct GpuParamBlockDesc;
  class ShaderInclude;

  class CoreObject;
  class ImportOptions;
  class TextureImportOptions;
  class FontImportOptions;
  class GpuProgramImportOptions;
  class MeshImportOptions;
  struct FontBitmap;
  class GameObject;
  class GPUResourceData;
  struct RenderOperation;
  class RenderQueue;
  struct ProfilerReport;
  class VertexDataDesc;
  class FrameAlloc;
  class FolderMonitor;
  class VideoMode;
  class VideoOutputInfo;
  class VideoModeInfo;
  struct SubMesh;
  class IResourceListener;
  class TextureProperties;
  class IShaderIncludeHandler;
  class Prefab;
  class PrefabDiff;
  class RendererMeshData;
  class Light;
  class Win32Window;
  class RenderAPIFactory;
  class PhysicsManager;
  class Physics;
  class FCollider;
  class Collider;
  class Rigidbody;
  class PhysicsMaterial;
  class BoxCollider;
  class SphereCollider;
  class PlaneCollider;
  class CapsuleCollider;
  class MeshCollider;
  class CCollider;
  class CRigidbody;
  class CBoxCollider;
  class CSphereCollider;
  class CPlaneCollider;
  class CCapsuleCollider;
  class CMeshCollider;
  class Joint;
  class FixedJoint;
  class DistanceJoint;
  class HingeJoint;
  class SphericalJoint;
  class SliderJoint;
  class D6Joint;
  class CharacterController;
  class CJoint;
  class CHingeJoint;
  class CDistanceJoint;
  class CFixedJoint;
  class CSphericalJoint;
  class CSliderJoint;
  class CD6Joint;
  class CCharacterController;
  class ShaderDefines;
  class ShaderImportOptions;
  class AudioListener;
  class AudioSource;
  class AudioClipImportOptions;
  class AnimationClip;
  class CCamera;
  class CRenderable;
  class CLight;
  class CAnimation;
  class CBone;
  class CAudioSource;
  class CAudioListener;
  class GpuPipelineParamInfo;
  class MaterialParams;
  template <class T> class TAnimationCurve;
  struct AnimationCurves;
  class Skeleton;
  class Animation;
  class GpuParamsSet;
  class Camera;
  class Renderable;
  class MorphShapes;
  class MorphShape;
  class MorphChannel;
  class GraphicsPipelineState;
  class ComputePipelineState;
  class ReflectionProbe;
  class CReflectionProbe;
  class CSkybox;
  class CLightProbeVolume;
  class Transform;
  class SceneActor;

  //Asset import
  class SpecificImporter;
  class Importer;

  //Resources
  class Resource;
  class Resources;
  class ResourceManifest;
  class Texture;
  class Mesh;
  class MeshBase;
  class TransientMesh;
  class MeshHeap;
  class Font;
  class ResourceMetaData;
  class DropTarget;
  class StringTable;
  class PhysicsMaterial;
  class PhysicsMesh;
  class AudioClip;
  class CoreObjectManager;
  struct CollisionData;

  //Scene
  class SceneObject;
  class Component;
  class SceneManager;

  //RTTI
  class MeshRTTI;

  //Desc structs
  struct SAMPLER_STATE_DESC;
  struct DEPTH_STENCIL_STATE_DESC;
  struct RASTERIZER_STATE_DESC;
  struct BLEND_STATE_DESC;
  struct RENDER_TARGET_BLEND_STATE_DESC;
  struct RENDER_TEXTURE_DESC;
  struct RENDER_WINDOW_DESC;
  struct FONT_DESC;
  struct CHAR_CONTROLLER_DESC;
  struct JOINT_DESC;
  struct FIXED_JOINT_DESC;
  struct DISTANCE_JOINT_DESC;
  struct HINGE_JOINT_DESC;
  struct SLIDER_JOINT_DESC;
  struct SPHERICAL_JOINT_DESC;
  struct D6_JOINT_DESC;
  struct AUDIO_CLIP_DESC;

  template<class T>
  class TCoreThreadQueue;
  class CommandQueueNoSync;
  class CommandQueueSync;

  namespace geCoreThread {
    class Renderer;
    class VertexData;
    class SamplerState;
    class IndexBuffer;
    class VertexBuffer;
    class RenderAPI;
    class RenderTarget;
    class RenderTexture;
    class RenderWindow;
    class DepthStencilState;
    class RasterizerState;
    class BlendState;
    class CoreObject;
    class Camera;
    class Renderable;
    class MeshBase;
    class Mesh;
    class TransientMesh;
    class Texture;
    class MeshHeap;
    class VertexDeclaration;
    class GpuBuffer;
    class GpuParamBlockBuffer;
    class GpuParams;
    class Shader;
    class Viewport;
    class Pass;
    class GpuParamsSet;
    class Technique;
    class Material;
    class GpuProgram;
    class Light;
    class ComputePipelineState;
    class GraphicsPipelineState;
    class Camera;
    class GpuParamsSet;
    class MaterialParams;
    class GpuPipelineParamInfo;
    class CommandBuffer;
    class EventQuery;
    class TimerQuery;
    class OcclusionQuery;
    class TextureView;
    class RenderableElement;
    class RenderWindowManager;
    class RenderStateManager;
    class HardwareBufferManager;
    class ReflectionProbe;
    class Skybox;
  }

  typedef TCoreThreadQueue<CommandQueueNoSync> CoreThreadQueue;

  namespace TYPEID_CORE {
    enum E {
      kID_Texture = 1001,
      kID_Mesh = 1002,
      kID_MeshData = 1003,
      kID_VertexDeclaration = 1004,
      kID_VertexElementData = 1005,
      kID_Component = 1006,
      kID_ResourceHandle = 1009,
      kID_GpuProgram = 1010,
      kID_ResourceHandleData = 1011,
      kID_CgProgram = 1012,
      kID_Pass = 1014,
      kID_Technique = 1015,
      kID_Shader = 1016,
      kID_Material = 1017,
      kID_SamplerState = 1021,
      kID_BlendState = 1023,
      kID_RasterizerState = 1024,
      kID_DepthStencilState = 1025,
      kID_BLEND_STATE_DESC = 1034,
      kID_SHADER_DATA_PARAM_DESC = 1035,
      kID_SHADER_OBJECT_PARAM_DESC = 1036,
      kID_SHADER_PARAM_BLOCK_DESC = 1047,
      kID_ImportOptions = 1048,
      kID_Font = 1051,
      kID_FONT_DESC = 1052,
      kID_CHAR_DESC = 1053,
      kID_FontImportOptions = 1056,
      kID_FontBitmap = 1057,
      kID_SceneObject = 1059,
      kID_GameObject = 1060,
      kID_PixelData = 1062,
      kID_GPUResourceData = 1063,
      kID_VertexDataDesc = 1064,
      kID_MeshBase = 1065,
      kID_GameObjectHandleBase = 1066,
      kID_ResourceManifest = 1067,
      kID_ResourceManifestEntry = 1068,
      kID_EmulatedParamBlock = 1069,
      kID_TextureImportOptions = 1070,
      kID_ResourceMetaData = 1071,
      kID_ShaderInclude = 1072,
      kID_Viewport = 1073,
      kID_ResourceDependencies = 1074,
      kID_ShaderMetaData = 1075,
      kID_MeshImportOptions = 1076,
      kID_Prefab = 1077,
      kID_PrefabDiff = 1078,
      kID_PrefabObjectDiff = 1079,
      kID_PrefabComponentDiff = 1080,
      kID_CGUIWidget = 1081,
      kID_ProfilerOverlay = 1082,
      kID_StringTable = 1083,
      kID_LanguageData = 1084,
      kID_LocalizedStringData = 1085,
      kID_MaterialParamColor = 1086,
      kID_WeakResourceHandle = 1087,
      kID_TextureParamData = 1088,
      kID_StructParamData = 1089,
      kID_MaterialParams = 1090,
      kID_MaterialRTTIParam = 1091,
      kID_PhysicsMaterial = 1092,
      kID_CCollider = 1093,
      kID_CBoxCollider = 1094,
      kID_CSphereCollider = 1095,
      kID_CCapsuleCollider = 1096,
      kID_CPlaneCollider = 1097,
      kID_CRigidbody = 1098,
      kID_PhysicsMesh = 1099,
      kID_CMeshCollider = 1100,
      kID_CJoint = 1101,
      kID_CFixedJoint = 1102,
      kID_CDistanceJoint = 1103,
      kID_CHingeJoint = 1104,
      kID_CSphericalJoint = 1105,
      kID_CSliderJoint = 1106,
      kID_CD6Joint = 1107,
      kID_CCharacterController = 1108,
      kID_FPhysicsMesh = 1109,
      kID_ShaderImportOptions = 1110,
      kID_AudioClip = 1111,
      kID_AudioClipImportOptions = 1112,
      kID_CAudioListener = 1113,
      kID_CAudioSource = 1114,
      kID_AnimationClip = 1115,
      kID_AnimationCurve = 1116,
      kID_KeyFrame = 1117,
      kID_NamedAnimationCurve = 1118,
      kID_Skeleton = 1119,
      kID_SkeletonBoneInfo = 1120,
      kID_AnimationSplitInfo = 1121,
      kID_CAnimation = 1122,
      kID_AnimationEvent = 1123,
      kID_ImportedAnimationEvents = 1124,
      kID_CBone = 1125,
      kID_MaterialParamData = 1126,
      kID_RenderSettings = 1127,
      kID_MorphShape = 1128,
      kID_MorphShapes = 1129,
      kID_MorphChannel = 1130,
      kID_ReflectionProbe = 1131,
      kID_CReflectionProbe = 1132,
      kID_CachedTextureData = 1133,
      kID_Skybox = 1134,
      kID_CSkybox = 1135,
      kID_LightProbeVolume = 1136,
      kID_SavedLightProbeInfo = 1137,
      kID_CLightProbeVolume = 1138,
      kID_Transform = 1139,
      kID_SceneActor = 1140,
      kID_AudioListener = 1141,
      kID_AudioSource = 1142,
      kID_ShaderVariationParam = 1143,
      kID_ShaderVariation = 1144,
      kID_GpuProgramBytecode = 1145,
      kID_GpuParamBlockDesc = 1146,
      kID_GpuParamDataDesc = 1147,
      kID_GpuParamObjectDesc = 1148,
      kID_GpuParamDesc = 1149,
      kID_BlendStateDesc = 1150,
      kID_RasterizerStateDesc = 1151,
      kID_DepthStencilStateDesc = 1152,
      kID_SerializedGpuProgramData = 1153,
      kID_SubShader = 1154,

      //Moved from Engine layer
      kID_CCamera = 30000,
      kID_Camera = 30003,
      kID_CRenderable = 30001,
      kID_Renderable = 30004,
      kID_Light = 30011,
      kID_CLight = 30012,
      kID_AutoExposureSettings = 30016,
      kID_TonemappingSettings = 30017,
      kID_WhiteBalanceSettings = 30018,
      kID_ColorGradingSettings = 30019,
      kID_DepthOfFieldSettings = 30020,
      kID_AmbientOcclusionSettings = 30021,
      kID_ScreenSpaceReflectionsSettings = 30022,
      kID_ShadowSettings = 30023
    };
  }
}
/*****************************************************************************/
/**
 * Resource references
 */
/*****************************************************************************/

#include "geResourceHandle.h"

namespace geEngineSDK {
  typedef ResourceHandle<Resource> HResource;
  typedef ResourceHandle<StringTable> HStringTable;
  typedef ResourceHandle<Font> HFont;
  typedef ResourceHandle<Texture> HTexture;
  typedef ResourceHandle<Mesh> HMesh;
  typedef ResourceHandle<Material> HMaterial;
  typedef ResourceHandle<ShaderInclude> HShaderInclude;
  typedef ResourceHandle<Shader> HShader;
  typedef ResourceHandle<Prefab> HPrefab;
  typedef ResourceHandle<PhysicsMaterial> HPhysicsMaterial;
  typedef ResourceHandle<PhysicsMesh> HPhysicsMesh;
  typedef ResourceHandle<AnimationClip> HAnimationClip;
  typedef ResourceHandle<AudioClip> HAudioClip;
}

#include "geGameObjectHandle.h"

namespace geEngineSDK {
  //Game object handles
  typedef GameObjectHandle<GameObject> HGameObject;
  typedef GameObjectHandle<SceneObject> HSceneObject;
  typedef GameObjectHandle<Component> HComponent;
  typedef GameObjectHandle<CCamera> HCamera;
  typedef GameObjectHandle<CRenderable> HRenderable;
  typedef GameObjectHandle<CLight> HLight;
  typedef GameObjectHandle<CAnimation> HAnimation;
  typedef GameObjectHandle<CBone> HBone;
  typedef GameObjectHandle<CRigidbody> HRigidbody;
  typedef GameObjectHandle<CCollider> HCollider;
  typedef GameObjectHandle<CBoxCollider> HBoxCollider;
  typedef GameObjectHandle<CSphereCollider> HSphereCollider;
  typedef GameObjectHandle<CCapsuleCollider> HCapsuleCollider;
  typedef GameObjectHandle<CPlaneCollider> HPlaneCollider;
  typedef GameObjectHandle<CJoint> HJoint;
  typedef GameObjectHandle<CHingeJoint> HHingeJoint;
  typedef GameObjectHandle<CSliderJoint> HSliderJoint;
  typedef GameObjectHandle<CDistanceJoint> HDistanceJoint;
  typedef GameObjectHandle<CSphericalJoint> HSphericalJoint;
  typedef GameObjectHandle<CFixedJoint> HFixedJoint;
  typedef GameObjectHandle<CD6Joint> HD6Joint;
  typedef GameObjectHandle<CCharacterController> HCharacterController;
  typedef GameObjectHandle<CReflectionProbe> HReflectionProbe;
  typedef GameObjectHandle<CSkybox> HSkybox;
  typedef GameObjectHandle<CLightProbeVolume> HLightProbeVolume;
  typedef GameObjectHandle<CAudioSource> HAudioSource;
  typedef GameObjectHandle<CAudioListener> HAudioListener;
}

namespace geEngineSDK {
  using std::basic_string;
  using std::char_traits;

  /**
   * @brief Defers function execution until the next frame. If this function is
   *        called within another deferred call, then it will be executed the
   *        same frame, but only after all existing deferred calls are done.
   * @note  This method can be used for breaking dependencies among other
   *        things. If a class A depends on class B having something done, but
   *        class B also depends in some way on class A, you can break up the
   *        initialization into two separate steps, queuing the second step
   *        using this method.
   * @note  Similar situation can happen if you have multiple classes being
   *        initialized in an undefined order but some of them depend on
   *        others. Using this method you can defer the dependent step until
   *        next frame, which will ensure everything was initialized.
   * @param[in]  callback  The callback.
   */
  GE_CORE_EXPORT void
  deferredCall(std::function<void()> callback);

  //Special types for use by profilers
  typedef basic_string<char, char_traits<char>, StdAlloc<char, ProfilerAlloc>> ProfilerString;

  template<typename T, typename A = StdAlloc<T, ProfilerAlloc>>
  using ProfilerVector = std::vector<T, A>;

  template <typename T, typename A = StdAlloc<T, ProfilerAlloc>>
  using ProfilerStack = std::stack<T, std::deque<T, A>>;

  /**
   * @brief geEngine thread policy that performs special startup / shutdown on
   *        threads managed by thread pool.
   */
  class GE_CORE_EXPORT geEngineThreadPolicy
  {
   public:
    static void
    onThreadStarted(const String& /*name*/) {
      MemStack::beginThread();
    }

    static void
    onThreadEnded(const String& /*name*/) {
      MemStack::endThread();
    }
  };

# define GE_ALL_LAYERS 0xFFFFFFFFFFFFFFFF
}

#include "geCommonTypes.h"
