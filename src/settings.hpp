#pragma once

#include <nwo5.silly-api/include/settings/include.hpp>

using namespace nwo5::settings::prelude;

namespace Settings {
    inline Setting<bool> enabled{"enabled"};

    inline Setting<int> inputPrecision{"input-precision"};
    inline Setting<bool> swapSliders{"swap-sliders"};
    inline Setting<bool> muteButton{"mute-button"};
    inline Setting<bool> muteButtonOnRight{"mute-button-on-right"};

    inline Setting<bool> coloredBars{"colored-bars"};
    inline Setting<cocos2d::ccColor3B> musicColor{"music-color"};
    inline Setting<cocos2d::ccColor3B> sfxColor{"sfx-color"};
    inline Setting<std::string> musicText{"music-text"};
    inline Setting<std::string> sfxText{"sfx-text"};
}