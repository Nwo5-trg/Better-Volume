#include "utils.hpp"
#include "settings.hpp"

using namespace geode::prelude;

namespace Utils {
    void setupSlider(
        bool pIsMusic, CCNode* pLayer,
        CopyableFunction<void(CCObject*)> pCallback, 
        TextInput*& pInputPtr, Slider*& pSliderPtr
    ) {
        auto slider = pSliderPtr ? pSliderPtr : typeinfo_cast<Slider*>(pLayer->getChildByIDRecursive(pIsMusic ? "music-slider" : "sfx-slider"));

        if (!slider) {
            return;
        }

        pSliderPtr = slider;
        slider->setZOrder(1);

        auto label = typeinfo_cast<CCLabelBMFont*>(pLayer->getChildByIDRecursive(pIsMusic ? "music-label" : "sfx-label"));

        if (!label) {
            return;
        }

        label->setVisible(false);

        auto newLabel = ui::node(Setup(ui::label(pIsMusic ? Settings::musicText.get() : Settings::sfxText.get(), label->getFntFile()))
            .id(pIsMusic ? "music-label"_spr : "sfx-label"_spr)
            .scale(label)
        );

        if (Settings::coloredBars) {
            if (auto sliderTexture = CCTextureCache::sharedTextureCache()->addImage("sliderBar2.png", true)) {
                slider->m_sliderBar->setTexture(sliderTexture);
                slider->m_sliderBar->setColor(pIsMusic ? Settings::musicColor : Settings::sfxColor);
            }
        }

        pInputPtr = ui::node(Setup(ui::input(40.0f, "0"))
            .id(pIsMusic ? "music-input"_spr : "sfx-input"_spr)
            .scale(0.65f)
            .filter("0123456789.")
            .string(Utils::formatVolumeStr(slider->getValue()))
            .callback([=] (const std::string& pStr) {
                if (pStr.empty() || pStr.ends_with('.') || pStr.starts_with('.')) {
                    return;
                }

                auto result = utils::numFromString<float>(pStr);
                if (!result) {
                    return;
                }

                auto newValue = std::clamp(result.unwrapOr(0.0f), 0.0f, 100.0f) / 100;
                slider->setValue(newValue);
                slider->updateBar();
                pCallback(slider->m_touchLogic->m_thumb);
            })
        );

        auto percentLabel = ui::node(Setup(ui::label("%", label->getFntFile()))
            .id(pIsMusic ? "music-percent-label"_spr : "sfx-percent-label"_spr)
            .scale(label)
        );

        auto parent = slider->getParent();

        if (!parent) {
            return;
        }

        auto labelMenu = ui::node(Setup(ui::menu(ui::horizontalDistrbLayout(5.0f, AxisAlignment::Center), false))
            .id(pIsMusic ? "music-label-menu"_spr : "sfx-label-menu"_spr)
            .pos(label)
            .size(CCSizeZero)
            .children(
                newLabel,
                pInputPtr,
                percentLabel
            )
            .parent(parent)
        );;

        if (!Settings::muteButton) {
            return;
        }

        auto muteToggle = ui::node(Setup(ui::togglerFrame(
                pIsMusic ? "GJ_musicOffBtn_001.png" : "GJ_fxOffBtn_001.png",
                pIsMusic ? "GJ_musicOnBtn_001.png" : "GJ_fxOnBtn_001.png",
                nullptr, 0.5f, 0.5f
            ))
                .id(pIsMusic ? "music-mute-toggle"_spr : "sfx-mute-toggle"_spr)
                .toggle(Mod::get()->getSavedValue<bool>(pIsMusic ? "music-muted" : "sfx-muted"))
                .pos(CCPointZero)
                .callback([=] (CCMenuItemToggler* pSender) {
                    const auto muted = !Mod::get()->getSavedValue<bool>(pIsMusic ? "music-muted" : "sfx-muted");

                    if (muted) {
                        Mod::get()->setSavedValue<float>(pIsMusic ? "music-volume-ret" : "sfx-volume-ret", slider->getValue());

                        slider->setValue(0.0f);
                        slider->updateBar();

                        pCallback(slider->m_touchLogic->m_thumb);
                    } else {
                        slider->setValue(
                            Mod::get()->getSavedValue<float>(pIsMusic ? "music-volume-ret" : "sfx-volume-ret")
                        );
                        slider->updateBar();
                        
                        pCallback(slider->m_touchLogic->m_thumb);

                        pSender->toggle(true);
                    }
                    
                    Mod::get()->setSavedValue<bool>(pIsMusic ? "music-muted" : "sfx-muted", muted);
                })
        );

        auto muteButtonMenu = ui::node(Setup(ui::menu(false))
            .id(pIsMusic ? "music-mute-menu"_spr : "sfx-mute-menu"_spr)
            .size(CCSizeZero)
            .children(muteToggle)
            .parent(parent)
        );

        if (Settings::muteButtonOnRight) {
            muteButtonMenu->setPosition(
                slider->getPositionX() + (slider->m_groove->getScaledContentWidth() + 10.0f) / 2,
                slider->getPositionY()
            );
            slider->setPosition(
                slider->getPositionX() - (muteToggle->getScaledContentWidth() + 10.0f) / 2,
                slider->getPositionY()
            );
        } else {
            muteButtonMenu->setPosition(
                labelMenu->getPositionX() + (slider->m_groove->getScaledContentWidth() * slider->getScale()) / 2,
                labelMenu->getPositionY()
            );
        }
    }

    void tryUpdateMuteButton(bool pIsMusic, CCNode* pLayer) {
        if (!Mod::get()->getSavedValue<bool>(pIsMusic ? "music-muted" : "sfx-muted")) {
            return;
        }

        auto toggler = typeinfo_cast<CCMenuItemToggler*>(
            pLayer->getChildByIDRecursive(pIsMusic ? "music-mute-toggle"_spr : "sfx-mute-toggle"_spr)
        );

        if (!toggler) {
            return;
        }

        Mod::get()->setSavedValue<bool>(pIsMusic ? "music-muted" : "sfx-muted", false);

        toggler->toggle(false);
    }

    void setVolume(bool pMusic, float pVolume) {
        static auto fmod = FMODAudioEngine::get();

        if (pMusic) {
            fmod->setBackgroundMusicVolume(pVolume);
        }
        else {
            fmod->setEffectsVolume(pVolume);
        }
    }
    float getVolume(cocos2d::CCObject* pSender) {
        if (auto thumb = typeinfo_cast<SliderThumb*>(pSender)) {
            return thumb->getValue();
        }
        else {
            return 0.0f;
        }
    }
    float getVolume(bool pMusic) {
        static auto fmod = FMODAudioEngine::get();

        return pMusic ? fmod->getBackgroundMusicVolume() : fmod->getEffectsVolume();
    }

    std::string formatVolumeStr(float pVal) {
        return nwo5::utils::numToString(pVal * 100, Settings::inputPrecision);
    }
}