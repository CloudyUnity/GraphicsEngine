#ifndef _SETTINGS_H_
#define _SETTINGS_H_

// WINDOW
inline constexpr int SCREEN_X = 1200;
inline constexpr int SCREEN_Y = 900;
inline constexpr float SCREEN_DEPTH = 1000.0f;
inline constexpr float SCREEN_NEAR = 0.3f;
inline constexpr bool V_SYNC = true;
inline constexpr bool FULL_SCREEN = false;
inline constexpr bool DISPLAY_CURSOR = true;

// MATH
inline constexpr double DEG_TO_RAD = 0.01745329252;
inline constexpr double RAD_TO_DEG = 57.295779513;
inline constexpr double PI = 3.14159265359;

// CBUFFER CONSTANTS
inline constexpr int NUM_POINT_LIGHTS = 4;
inline constexpr int NUM_POISSON_SAMPLES = 4;
inline constexpr int BLUR_SAMPLE_SPREAD = 4;

inline constexpr bool DEPTH_STENCIL_FUNC_LESS_EQUAL = true;
inline constexpr bool BLENDING_ON = true;

class Settings
{
private:
    struct SettingsData // Set up for exhibit data
    {       
        // RENDERING
        bool FrustumCulling = true;
        bool FogEnabled = false;
        bool SkyboxEnabled = true;
        bool FreezeSkybox = false;
        bool ReflectionEnabled = true;
        int ReflectionFrameDelay = 3;
        int ShadowMapFramesDelay = 2;
        bool WireframeMode = false;
        bool ParticlesEnabled = false;
        bool DebugLinesEnabled = false;

        // SHADOWS
        bool ShadowsEnabled = true;
        bool ShowShadowMap = false;
        int ShadowMapSceneSize = 60;
        int ShadowMapRenderX = 2048;
        int ShadowMapRenderY = 2048;
        float ShadowMapDepth = 75;
        float ShadowMapNear = 1;
        float ShadowMapDistance = 28;

        // SCENE CUSTOMIZATION
        float FogColorR = 0.5f;
        float FogColorG = 0.5f;
        float FogColorB = 0.5f;
        float FogColorA = 1.0f;

        // POST PROCESSING
        bool PostProcessingEnabled = false;
        bool FiltersEnabled = true;
        bool BlurEnabled = false;
        float DownScaleMult = 0.5f;

        // GAMEPLAY
        float CameraSpeed = 6;
        float CameraRotationSpeed = 80;

    };

    SettingsData m_generalExhibitData1;
    SettingsData m_debugData2;
    SettingsData m_postProcessingExhibitData3;
    SettingsData m_performanceData4;
    SettingsData m_fogExhibitData5;
    SettingsData m_wireFrameData6;

public:
    SettingsData m_CurrentData;

public:
    Settings(int);

    void ChangeSettings(int);

};

#endif