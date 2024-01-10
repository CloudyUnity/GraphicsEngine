#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

// WINDOW:
inline constexpr int SCREEN_X = 1200;
inline constexpr int SCREEN_Y = 900;
inline constexpr float SCREEN_DEPTH = 1000.0f;
inline constexpr float SCREEN_NEAR = 0.3f;
inline constexpr bool V_SYNC = false;
inline constexpr bool FULL_SCREEN = false;

// RENDERING:
inline constexpr bool BLENDING_ON = true;
inline constexpr bool REFLECTION_ENABLED = true;
inline constexpr int REFLECTION_FRAME_DELAY = 0; // Frames between each rendering of reflection textures for optimization. (15-30) is reccomended

// 0 - CULL_NONE
// 1 - CULL_BACK_FACE
// 2 - FILL_WIREFRAME
inline constexpr int RENDER_MODE = 1; 

// 0 - WRAP
// 1 - CLAMP
inline constexpr int SAMPLER_MODE = 0;

// SCENE CUSTOMIZATION
inline constexpr float FOG_COLOR_R = 0.5f;
inline constexpr float FOG_COLOR_G = 0.5f;
inline constexpr float FOG_COLOR_B = 0.5f;
inline constexpr float FOG_COLOR_A = 1.0f;

#endif