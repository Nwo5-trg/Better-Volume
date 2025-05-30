#include <Geode/Geode.hpp>
#include <Geode/modify//PauseLayer.hpp>

using namespace geode::prelude;

auto mod = Mod::get();

CCTexture2D* colorableSliderTexture = CCTextureCache::sharedTextureCache()->addImage("sliderBar2.png", true);

class $modify(PauseMenu, PauseLayer) {
    struct Fields {
        TextInput* musicInput;
        TextInput* sfxInput;
        Slider* musicSlider;
        Slider* sfxSlider;

        bool colorBars = mod->getSettingValue<bool>("colored-bars");
        ccColor3B musicColor = mod->getSettingValue<ccColor3B>("music-color");
        ccColor3B sfxColor =  mod->getSettingValue<ccColor3B>("sfx-color");
    };

    void customSetup() {
        PauseLayer::customSetup();
        auto musicSlider = this->getChildByType<Slider>(0);
        auto sfxSlider = this->getChildByType<Slider>(1);
        auto musicText = this->getChildByType<CCLabelBMFont>(5);
        auto sfxText = this->getChildByType<CCLabelBMFont>(6);
        if (!musicSlider || !sfxSlider || !musicText || !sfxText) return;
        auto fields = m_fields.self();
        fields->musicSlider = musicSlider;
        fields->sfxSlider = sfxSlider;
        for (auto text : {musicText, sfxText}) text->setString((std::string(text->getString()) + "      %").c_str());
        for (auto slider : {musicSlider, sfxSlider}) {
            auto sliderType = (slider == musicSlider);

            if (fields->colorBars) {
                slider->m_sliderBar->setTexture(colorableSliderTexture);
                slider->m_sliderBar->setColor(sliderType ? fields->musicColor : fields->sfxColor);
            }

            auto input = TextInput::create(50.0f, "0");
            input->setPosition(sliderType ? ccp(musicText->getPositionX() + 16, musicText->getPositionY()) : ccp(sfxText->getPositionX() + 9, sfxText->getPositionY())); // hard coded positions bite me
            input->setScale(0.65f);
            input->setFilter("1234567890.");
            input->setCallback([slider, sliderType] (const std::string& input) {
                if (!input.empty()) {
                    auto value = std::strtof(input.c_str(), nullptr);
                    if (value <= 100) {
                        slider->setValue(value / 100);
                        slider->updateBar();
                        if (sliderType) FMODAudioEngine::get()->setBackgroundMusicVolume(value / 100);
                        else FMODAudioEngine::get()->setEffectsVolume(value / 100);
                    }
                }
            });
            (sliderType ? fields->musicInput : fields->sfxInput) = input;
            this->addChild(input);
        }
        updateInputWithSlider(fields->musicInput, musicSlider);
        updateInputWithSlider(fields->sfxInput, sfxSlider);
    }

    void musicSliderChanged(CCObject* p0) {
        PauseLayer::musicSliderChanged(p0);
        updateInputWithSlider(m_fields->musicInput, m_fields->musicSlider);
    }

    void sfxSliderChanged(CCObject* p0) {
        PauseLayer::musicSliderChanged(p0);
        updateInputWithSlider(m_fields->sfxInput, m_fields->sfxSlider);
    }

    void updateInputWithSlider(TextInput* input, Slider* slider) {
        if (!input || !slider) return;
        input->setString(floatToFormattedString(slider->getValue() * 100));
    }

    std::string floatToFormattedString(float num) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(1) << num;
        std::string string = ss.str();
        string.erase(string.find_last_not_of('0') + 1, std::string::npos);
        if (string.back() == '.') string.pop_back();
        return string;
    }
};

