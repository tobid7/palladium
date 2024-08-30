#pragma once

#include <pd/external/json.hpp>
#include <pd/palladium.hpp>

namespace Palladium {
namespace IDB {
void Start();
void Stop();
void Restart();
}  // namespace IDB
}  // namespace Palladium

using PDFlags = unsigned int;
enum PDFlags_ {
  PDFlags_None = 0,
  PDFlags_MemTrack = 1 << 0,
  PDFlags_SceneSystem = 1 << 1,
  PDFlags_Default = PDFlags_SceneSystem,
};

using PDMetrikOverlayFlags = unsigned int;
enum PDMetrikOverlayFlags_ {
  PDMetrikOverlayFlags_None = 0,       // Displays Nothing
  PDMetrikOverlayFlags_FPS = 1 << 0,   // Display FPS
  PDMetrikOverlayFlags_CPU = 1 << 1,   // Display CPU Usage
  PDMetrikOverlayFlags_GPU = 1 << 2,   // Display GPU Usage
  PDMetrikOverlayFlags_CMD = 1 << 3,   // Display GPU CMD Usage
  PDMetrikOverlayFlags_LMM = 1 << 4,   // Display Linear Space Free
  PDMetrikOverlayFlags_LVT = 1 << 5,   // Display Lithium Vertex Usage
  PDMetrikOverlayFlags_LID = 1 << 6,   // Display Lithium Indices Usage
  PDMetrikOverlayFlags_LDM = 1 << 7,   // Display Lithium Draw Command Num
  PDMetrikOverlayFlags_LDC = 1 << 8,   // Display Lithium Drawcalls Count
  PDMetrikOverlayFlags_PDO = 1 << 9,   // Display Overlay Info String
  PDMetrikOverlayFlags_MTD = 1 << 10,  // Display Memory Usage (if enabled)
  PDMetrikOverlayFlags_CGR = 1 << 11,  // Display CPU Graph
  PDMetrikOverlayFlags_GGR = 1 << 12,  // Display GPU Graph
  PDMetrikOverlayFlags_Default =
      PDMetrikOverlayFlags_FPS | PDMetrikOverlayFlags_CPU |
      PDMetrikOverlayFlags_GPU | PDMetrikOverlayFlags_CMD |
      PDMetrikOverlayFlags_LMM | PDMetrikOverlayFlags_LVT |
      PDMetrikOverlayFlags_LID | PDMetrikOverlayFlags_LDM |
      PDMetrikOverlayFlags_LDC | PDMetrikOverlayFlags_PDO |
      PDMetrikOverlayFlags_MTD,  // Enable All of Them exept Graphs
};

using PDFTraceOverlayFlags = unsigned int;
enum PDFTraceOverlayFlags_ {
  PDFTraceOverlayFlags_None = 0,                 // Displays Nothing
  PDFTraceOverlayFlags_DisplayName = 1 << 0,     // Display Tracename
  PDFTraceOverlayFlags_DisplayAverage = 1 << 1,  // Display Average Time
  PDFTraceOverlayFlags_DisplayMin = 1 << 2,      // Display Minimum Time
  PDFTraceOverlayFlags_DisplayMax = 1 << 3,      // Display Maximum Time
  PDFTraceOverlayFlags_FillBg = 1 << 4,          // Make Background Darker
  PDFTraceOverlayFlags_DisplayHelp = 1 << 5,     // Display Info for values
  PDFTraceOverlayFlags_Default =
      PDFTraceOverlayFlags_DisplayName | PDFTraceOverlayFlags_DisplayAverage |
      PDFTraceOverlayFlags_DisplayMin | PDFTraceOverlayFlags_DisplayMax |
      PDFTraceOverlayFlags_FillBg |
      PDFTraceOverlayFlags_DisplayHelp,  // Enable All of Them
};

// Outdated HidApi (HidV2Patched)
extern u32 d7_hDown;
extern u32 d7_hHeld;
extern u32 d7_hUp;
extern u32 d7_hRepeat;  // Inofficial lol
extern touchPosition d7_touch;

// Modern Global Api
extern C3D_RenderTarget *pd_top;
extern C3D_RenderTarget *pd_top_right;
extern C3D_RenderTarget *pd_bottom;
extern PDFlags pd_flags;
extern PDMetrikOverlayFlags pd_ovl_flags;
extern PDFTraceOverlayFlags pd_ftrace_ovl_flags;
// Draw2
extern float pd_draw2_tsm;