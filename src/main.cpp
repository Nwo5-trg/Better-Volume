#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/OptionsLayer.hpp>

using namespace geode::prelude;

std::string getStr(float pVal) {
    return std::to_string(static_cast<int>(std::round(pVal * 100)));
}

template <typename T>
void setupVolumeUI(
    T* pLayer, 
    Slider*& pMusicSlider, 
    Slider*& pSfxSlider, 
    TextInput*& pMusicInput, 
    TextInput*& pSfxInput
) {
    auto mod = Mod::get();
    bool doColor = mod->getSettingValue<bool>("colored-bars");
    ccColor3B musicColor = mod->getSettingValue<ccColor3B>("music-color");
    ccColor3B sfxColor = mod->getSettingValue<ccColor3B>("sfx-color");
    auto texture = CCTextureCache::sharedTextureCache()->addImage("sliderBar2.png", true);

    pMusicSlider = typeinfo_cast<Slider*>(pLayer->getChildByIDRecursive("music-slider"));
    pSfxSlider = typeinfo_cast<Slider*>(pLayer->getChildByIDRecursive("sfx-slider"));
    auto musicLabel = typeinfo_cast<CCLabelBMFont*>(pLayer->getChildByIDRecursive("music-label"));
    auto sfxLabel = typeinfo_cast<CCLabelBMFont*>(pLayer->getChildByIDRecursive("sfx-label"));

    if (!pMusicSlider || !pSfxSlider || !musicLabel || !sfxLabel) {
        return;
    }

    auto setupInput = [&] (
        Slider* pSlider, 
        CCLabelBMFont* pLabel, 
        TextInput*& pInput, 
        ccColor3B pColor, 
        bool pIsMusic
    ) {
        if (doColor && texture) {
            pSlider->m_sliderBar->setTexture(texture);
            pSlider->m_sliderBar->setColor(pColor);
        }

        pLabel->setPositionX(pLabel->getPositionX() - 22.0f);
        pSlider->setZOrder(1);

        pInput = TextInput::create(40.0f, "0");
        pInput->setPosition(
            pLabel->getPositionX() + pLabel->getScaledContentSize().width / 2 + 20.0f, 
            pLabel->getPositionY()
        );
        pInput->setScale(0.65f);
        pInput->setFilter("1234567890");
        pInput->setString(getStr(pSlider->getValue()));

        pInput->setCallback([pLayer, pSlider, pIsMusic] (const std::string& pVal) {
            if (pVal.empty()) {
                return;
            }

            pSlider->setValue(std::clamp(std::strtof(pVal.c_str(), nullptr), 0.0f, 100.0f) / 100);
            pSlider->updateBar();

            if (pIsMusic) {
                pLayer->musicSliderChanged(pSlider->m_touchLogic->m_thumb);
            } else {
                pLayer->sfxSliderChanged(pSlider->m_touchLogic->m_thumb);
            }
        });

        auto parent = pLabel->getParent();

        if (parent) {
            parent->addChild(pInput);

            auto percentLabel = CCLabelBMFont::create("%", pLabel->getFntFile());
            percentLabel->setPosition(
                pInput->getPositionX() + 28.0f, 
                pInput->getPositionY()
            );
            percentLabel->setScale(pLabel->getScale());
            
            parent->addChild(percentLabel);
        }
    };

    setupInput(
        pMusicSlider, 
        musicLabel, 
        pMusicInput, 
        musicColor, 
        true
    );

    setupInput(
        pSfxSlider, 
        sfxLabel, 
        pSfxInput, 
        sfxColor, 
        false
    );
}

class $modify(MyPauseLayer, PauseLayer) {
    struct Fields { 
        TextInput* m_musicInput; 
        TextInput* m_sfxInput; 
        Slider* m_musicSlider; 
        Slider* m_sfxSlider; 
    };

    void customSetup() {
        PauseLayer::customSetup();

        setupVolumeUI(
            this, 
            m_fields->m_musicSlider, 
            m_fields->m_sfxSlider, 
            m_fields->m_musicInput, 
            m_fields->m_sfxInput
        );
    }

    void musicSliderChanged(CCObject* pSender) {
        PauseLayer::musicSliderChanged(pSender);

        if (m_fields->m_musicInput && m_fields->m_musicSlider) {
            m_fields->m_musicInput->setString(getStr(m_fields->m_musicSlider->getValue()));
        }
    }

    void sfxSliderChanged(CCObject* pSender) {
        PauseLayer::sfxSliderChanged(pSender);

        if (m_fields->m_sfxInput && m_fields->m_sfxSlider) {
            m_fields->m_sfxInput->setString(getStr(m_fields->m_sfxSlider->getValue()));
        }
    }
};

class $modify(MyOptionsLayer, OptionsLayer) {
    struct Fields { 
        TextInput* m_musicInput; 
        TextInput* m_sfxInput; 
        Slider* m_musicSlider; 
        Slider* m_sfxSlider; 
    };

    void customSetup() {
        OptionsLayer::customSetup();

        setupVolumeUI(
            this, 
            m_fields->m_musicSlider, 
            m_fields->m_sfxSlider, 
            m_fields->m_musicInput, 
            m_fields->m_sfxInput
        );
    }

    void musicSliderChanged(CCObject* pSender) {
        OptionsLayer::musicSliderChanged(pSender);

        if (m_fields->m_musicInput && m_fields->m_musicSlider) {
            m_fields->m_musicInput->setString(getStr(m_fields->m_musicSlider->getValue()));
        }
    }

    void sfxSliderChanged(CCObject* pSender) {
        OptionsLayer::sfxSliderChanged(pSender);

        if (m_fields->m_sfxInput && m_fields->m_sfxSlider) {
            m_fields->m_sfxInput->setString(getStr(m_fields->m_sfxSlider->getValue()));
        }
    }
};