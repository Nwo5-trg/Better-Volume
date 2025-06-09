#include <Geode/Geode.hpp>
#include <Geode/modify//PauseLayer.hpp>

using namespace geode::prelude;

class $modify(PauseMenu, PauseLayer) {
    struct Fields {
        TextInput* musicInput;
        TextInput* sfxInput;
        Slider* musicSlider;
        Slider* sfxSlider;

        bool colorBars = Mod::get()->getSettingValue<bool>("colored-bars");
        ccColor3B musicColor = Mod::get()->getSettingValue<ccColor3B>("music-color");
        ccColor3B sfxColor =  Mod::get()->getSettingValue<ccColor3B>("sfx-color");
        int64_t rounding =  Mod::get()->getSettingValue<int64_t>("rounding");
        std::string musicCustomText = Mod::get()->getSettingValue<std::string>("music-text");
        std::string sfxCustomText = Mod::get()->getSettingValue<std::string>("sfx-text");
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

        CCTexture2D* colorableSliderTexture = CCTextureCache::sharedTextureCache()->addImage("sliderBar2.png", true);
        musicText->setString((fields->musicCustomText + "      %").c_str());
        sfxText->setString((fields->sfxCustomText + "      %").c_str());
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
        PauseLayer::sfxSliderChanged(p0);
        updateInputWithSlider(m_fields->sfxInput, m_fields->sfxSlider);
    }

    void updateInputWithSlider(TextInput* input, Slider* slider) {
        if (!input || !slider) return;
        input->setString(ftofstr(slider->getValue() * 100, m_fields->rounding));
    }

    std::string ftofstr(float num, int decimal) { // see i need this i added customizable rounding this is def not just justification for attachment issues to lines of fucking code even tho it would literally be less lines of code to switch case fmtformat
        if (decimal == 0) return std::to_string((int)round(num));
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(decimal) << num;
        std::string string = ss.str();
        string.erase(string.find_last_not_of('0') + 1, std::string::npos);
        if (string.back() == '.') string.pop_back();
        return string;
    }
};

