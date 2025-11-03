#pragma once
#include <string>
#include <vector>

// Public-safe stub for secret systems
// Keeps API compatibility but disables hidden content

namespace Secret {

    // ---- Lifecycle ------------------------------------------------------------
    inline void Init() {}

    // ---- Title Screen / Input -------------------------------------------------
    inline void OnTitleKey(int) {}
    inline void TitleIdleTick() {}

    // ---- Session lifecycle ----------------------------------------------------
    inline void OnSessionStart() {}
    inline void OnSessionTick(int, int, int, bool) {}
    inline void OnHardDrop() {}
    inline void OnLockAndClear(int) {}

    // ---- Game Over / Name Entry ----------------------------------------------
    inline void OnPlayerNameEntered(const std::string&, int&, double&, bool&) {}

    // ---- Visual & Theme helpers ----------------------------------------------
    inline void ApplySeasonalBanner(std::string&, std::string&, std::string&) {}

    // ---- Feature Toggles ------------------------------------------------------
    inline bool ForceGhostAlwaysVisible() { return false; }
    inline bool HideScoreText() { return false; }
    inline bool UseRetroMonochrome() { return false; }
    inline bool UseWireframe() { return false; }
    inline bool UseRedTint() { return false; }
    inline bool UseGrayNight() { return false; }
    inline bool UseMatrixRain() { return false; }
    inline bool UseCRTOverlay() { return false; }
    inline bool UseRainbowPieces() { return false; }
    inline bool UseColorChaos() { return false; }
    inline bool UseShadowProtocol() { return false; }
    inline bool ShouldFlickerLowBattery() { return false; }
    inline bool ShouldShowHydration() { return false; }

    // ---- Optional Overlays ----------------------------------------------------
    inline void StartMenuOverpaintTick() {}
    inline void FireworksBurst() {}

} // namespace Secret
