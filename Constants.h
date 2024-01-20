#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

// MATH
inline constexpr double DEG_TO_RAD = 0.01745329252;
inline constexpr double RAD_TO_DEG = 57.295779513;
inline constexpr double PI = 3.14159265359;

// WINDOW:
inline constexpr int SCREEN_X = 1200;
inline constexpr int SCREEN_Y = 900;
inline constexpr float SCREEN_DEPTH = 1000.0f;
inline constexpr float SCREEN_NEAR = 0.3f;
inline constexpr bool V_SYNC = true;
inline constexpr bool FULL_SCREEN = false;

// RENDERING:
inline constexpr bool BLENDING_ON = true;
inline constexpr bool SHADOWS_ENABLED = true;
inline constexpr bool FRUSTUM_CULLING = true;
inline constexpr bool FOG_ENABLED = true;
inline constexpr bool REFLECTION_ENABLED = true;
inline constexpr bool POST_PROCESSING_ENABLED = true;
inline constexpr int REFLECTION_FRAME_DELAY = 3; // Frames between each rendering of reflection textures for optimization

// 0 - CULL_NONE
// 1 - CULL_BACK_FACE
// 2 - FILL_WIREFRAME
inline constexpr int RENDER_MODE = 1;

// SHADOWS:
inline constexpr int SHADOW_MAP_SCENE_SIZE = 60; // Passed into depth view matrix
inline constexpr int SHADOW_MAP_RENDER_X = 2048; // Passed into render texture
inline constexpr int SHADOW_MAP_RENDER_Y = 2048; // Passed into render texture
inline constexpr float SHADOW_MAP_DEPTH = 75.0f; // Far clip-plane
inline constexpr float SHADOW_MAP_NEAR = 1.0f; // Near clip-plane
inline constexpr float SHADOW_MAP_DISTANCE = 28.0f; // Distance the light position is away from the origin for the view matrix
inline constexpr int NUM_POISSON_SAMPLES = 4;
inline constexpr bool SHOW_SHADOW_MAP = false;

// SCENE CUSTOMIZATION
inline constexpr float FOG_COLOR_R = 0.5f;
inline constexpr float FOG_COLOR_G = 0.5f;
inline constexpr float FOG_COLOR_B = 0.5f;
inline constexpr float FOG_COLOR_A = 1.0f;
inline constexpr int NUM_POINT_LIGHTS = 4;

// POST PROCESSING
inline constexpr float DOWN_SCALE_MULT = 0.5f;
inline constexpr int BLUR_SAMPLE_SPREAD = 4;

// GAMEPLAY
inline constexpr float CAMERA_SPEED = 4.0f;
inline constexpr float CAMERA_ROTATION_SPEED = 60.0f;

#endif