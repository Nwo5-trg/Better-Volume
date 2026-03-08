#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/OptionsLayer.hpp>
#include <Geode/modify/SliderTouchLogic.hpp>

using namespace geode::prelude;

std::string ftofstr(float num, int decimal) {
    if (decimal == 0) return std::to_string((int)round(num));
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(decimal) << num;
    std::string string = ss.str();
    string.erase(string.find_last_not_of('0') + 1, std::string::npos);
    if (string.back() == '.') string.pop_back();
    return string;
}

class $modify(MySliderTouchLogic, SliderTouchLogic) {
    struct Fields {
        std::function<void(float)> m_callback;
    };

    void setCallback(std::function<void(float)> callback) {
        m_fields->m_callback = callback;
    }

    void ccTouchMoved(cocos2d::CCTouch* p0, cocos2d::CCEvent* p1) {
        SliderTouchLogic::ccTouchMoved(p0, p1);
        if (m_fields->m_callback) m_fields->m_callback(m_thumb->getValue());
    }
};

class $modify(PauseMenu, PauseLayer) {
    struct Fields {
        TextInput* musicInput;
        TextInput* sfxInput;
        Slider* musicSlider;
        Slider* sfxSlider;
        int64_t rounding;
    };

    void customSetup() {
        PauseLayer::customSetup();

        bool colorBars = Mod::get()->getSettingValue<bool>("colored-bars");
        ccColor3B musicColor = Mod::get()->getSettingValue<ccColor3B>("music-color");
        ccColor3B sfxColor = Mod::get()->getSettingValue<ccColor3B>("sfx-color");
        m_fields->rounding = Mod::get()->getSettingValue<int64_t>("rounding");
        std::string musicCustomText = Mod::get()->getSettingValue<std::string>("music-text");
        std::string sfxCustomText = Mod::get()->getSettingValue<std::string>("sfx-text");

        auto musicSlider = typeinfo_cast<Slider*>(this->getChildByIDRecursive("music-slider"));
        auto sfxSlider = typeinfo_cast<Slider*>(this->getChildByIDRecursive("sfx-slider"));
        auto musicText = typeinfo_cast<CCLabelBMFont*>(this->getChildByIDRecursive("music-label"));
        auto sfxText = typeinfo_cast<CCLabelBMFont*>(this->getChildByIDRecursive("sfx-label"));

        if (!musicSlider || !sfxSlider || !musicText || !sfxText) return;

        m_fields->musicSlider = musicSlider;
        m_fields->sfxSlider = sfxSlider;

        CCTexture2D* colorableTexture = CCTextureCache::sharedTextureCache()->addImage("sliderBar2.png", true);
        musicText->setString((musicCustomText + "      %").c_str());
        sfxText->setString((sfxCustomText + "      %").c_str());

        for (auto slider : {musicSlider, sfxSlider}) {
            bool isMusic = (slider == musicSlider);

            if (colorBars) {
                slider->m_sliderBar->setTexture(colorableTexture);
                slider->m_sliderBar->setColor(isMusic ? musicColor : sfxColor);
            }

            auto input = TextInput::create(50.0f, "0");
            input->setZOrder(1);
            slider->setZOrder(3);
            input->setPosition(isMusic ? ccp(musicText->getPositionX() + 16, musicText->getPositionY()) : ccp(sfxText->getPositionX() + 9, sfxText->getPositionY()));
            input->setScale(0.65f);
            input->setFilter("1234567890.");
            
            input->setCallback([slider, isMusic](const std::string& val) {
                if (!val.empty()) {
                    auto value = std::strtof(val.c_str(), nullptr);
                    if (value <= 100) {
                        slider->setValue(value / 100);
                        slider->updateBar();
                        if (isMusic) FMODAudioEngine::get()->setBackgroundMusicVolume(value / 100);
                        else FMODAudioEngine::get()->setEffectsVolume(value / 100);
                    }
                }
            });
            
            (isMusic ? m_fields->musicInput : m_fields->sfxInput) = input;
            this->addChild(input);
        }
        
        updateInputWithSlider(m_fields->musicInput, musicSlider);
        updateInputWithSlider(m_fields->sfxInput, sfxSlider);
    }

    void musicSliderChanged(CCObject* p0) {
        PauseLayer::musicSliderChanged(p0);
        updateInputWithSlider(m_fields->musicInput, m_fields->musicSlider);
    }

    void sfxSliderChanged(CCObject* p0) {
        PauseLayer::sfxSliderChanged(p0);
        updateInputWithSlider(m_fields->sfxInput, m_fields->sfxSlider);
    }

    void updateInputWithSlider(TextInput* input, Slider* slider) {
        if (!input || !slider) return;
        input->setString(ftofstr(slider->getValue() * 100, m_fields->rounding));
    }
};

class $modify(MyOptionsLayer, OptionsLayer) {
    struct Fields {
        TextInput* musicInput;
        TextInput* sfxInput;
        Slider* musicSlider;
        Slider* sfxSlider;
    };

    void customSetup() {
        OptionsLayer::customSetup();

        bool colorBars = Mod::get()->getSettingValue<bool>("colored-bars");
        ccColor3B musicColor = Mod::get()->getSettingValue<ccColor3B>("music-color");
        ccColor3B sfxColor = Mod::get()->getSettingValue<ccColor3B>("sfx-color");
        int64_t rounding = Mod::get()->getSettingValue<int64_t>("rounding");
        std::string musicCustomText = Mod::get()->getSettingValue<std::string>("music-text");
        std::string sfxCustomText = Mod::get()->getSettingValue<std::string>("sfx-text");

        auto musicSlider = typeinfo_cast<Slider*>(this->getChildByIDRecursive("music-slider"));
        auto sfxSlider = typeinfo_cast<Slider*>(this->getChildByIDRecursive("sfx-slider"));
        auto musicLabel = typeinfo_cast<CCLabelBMFont*>(this->getChildByIDRecursive("music-label"));
        auto sfxLabel = typeinfo_cast<CCLabelBMFont*>(this->getChildByIDRecursive("sfx-label"));

        if (!musicSlider || !sfxSlider || !musicLabel || !sfxLabel) return;

        m_fields->musicSlider = musicSlider;
        m_fields->sfxSlider = sfxSlider;

        musicLabel->setString((musicCustomText + "      %").c_str());
        sfxLabel->setString((sfxCustomText + "      %").c_str());

        CCTexture2D* colorableTexture = CCTextureCache::sharedTextureCache()->addImage("sliderBar2.png", true);

        for (auto slider : {musicSlider, sfxSlider}) {
            bool isMusic = (slider == musicSlider);

            if (colorBars) {
                slider->m_sliderBar->setTexture(colorableTexture);
                slider->m_sliderBar->setColor(isMusic ? musicColor : sfxColor);
            }

            auto input = TextInput::create(50.0f, "0");
            input->setZOrder(1);
            slider->setZOrder(3);
            input->setPosition(isMusic ? ccp(musicLabel->getPositionX() + 16, musicLabel->getPositionY()) : ccp(sfxLabel->getPositionX() + 9, sfxLabel->getPositionY()));
            input->setScale(0.65f);
            input->setFilter("1234567890."); 
            input->setString(ftofstr(slider->getValue() * 100, rounding));

            input->setCallback([this, slider, isMusic](const std::string& val) {
                if (!val.empty()) {
                    auto value = std::strtof(val.c_str(), nullptr);
                    if (value <= 100) {
                        slider->setValue(value / 100);
                        slider->updateBar();
                        if (isMusic) {
                            FMODAudioEngine::get()->setBackgroundMusicVolume(value / 100);
                            this->musicSliderChanged(slider->m_touchLogic->m_thumb);
                        } else {
                            FMODAudioEngine::get()->setEffectsVolume(value / 100);
                            this->sfxSliderChanged(slider->m_touchLogic->m_thumb);
                        }
                    }
                }
            });

            static_cast<MySliderTouchLogic*>(slider->m_touchLogic)->setCallback([input, rounding](float val) {
                input->setString(ftofstr(val * 100, rounding));
            });

            slider->getParent()->addChild(input);
            (isMusic ? m_fields->musicInput : m_fields->sfxInput) = input;
        }
    }
};
