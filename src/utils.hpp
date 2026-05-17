#pragma once

#include <nwo5.silly-api/include/ui/include.hpp>
#include <nwo5.silly-api/include/utils/include.hpp>

using namespace nwo5::utils::prelude;
using namespace nwo5::ui::prelude;

constexpr bool MUSIC = true;
constexpr bool SFX = false;

namespace Utils {
    void setupSlider(
        bool pIsMusic, cocos2d::CCNode* pLayer,
        geode::CopyableFunction<void(cocos2d::CCObject*)> pCallback, 
        geode::TextInput*& pInputPtr, Slider*& pSliderPtr
    );

    void tryUpdateMuteButton(bool pIsMusic, cocos2d::CCNode* pLayer);

    void setVolume(bool pMusic, float pVolume);
    float getVolume(cocos2d::CCObject* pSender);
    float getVolume(bool pMusic);

    std::string formatVolumeStr(float pVal);
}