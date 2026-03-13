#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/OptionsLayer.hpp>

using namespace geode::prelude;

std::string getStr(float val) {
    return std::to_string(static_cast<int>(std::round(val * 100.f)));
}

template <typename T>
void setupVolumeUI(T* layer, Slider*& mSl, Slider*& sSl, TextInput*& mIn, TextInput*& sIn) {
    auto mod = Mod::get();
    bool doColor = mod->getSettingValue<bool>("colored-bars");
    auto mCol = mod->getSettingValue<ccColor3B>("music-color");
    auto sCol = mod->getSettingValue<ccColor3B>("sfx-color");
    auto tex = CCTextureCache::sharedTextureCache()->addImage("sliderBar2.png", true);

    mSl = typeinfo_cast<Slider*>(layer->getChildByIDRecursive("music-slider"));
    sSl = typeinfo_cast<Slider*>(layer->getChildByIDRecursive("sfx-slider"));
    auto mLbl = typeinfo_cast<CCLabelBMFont*>(layer->getChildByIDRecursive("music-label"));
    auto sLbl = typeinfo_cast<CCLabelBMFont*>(layer->getChildByIDRecursive("sfx-label"));

    if (!mSl || !sSl || !mLbl || !sLbl) return;

    auto setupInput = [&](Slider* sl, CCLabelBMFont* lbl, TextInput*& in, ccColor3B col, bool isM) {
        if (doColor && tex) { sl->m_sliderBar->setTexture(tex); sl->m_sliderBar->setColor(col); }

        lbl->setPositionX(lbl->getPositionX() - 22.f);
        sl->setZOrder(1);

        in = TextInput::create(40.f, "0");
        in->setPosition(lbl->getPositionX() + lbl->getScaledContentSize().width / 2.f + 20.f, lbl->getPositionY());
        in->setScale(0.65f);
        in->setFilter("1234567890");
        in->setString(getStr(sl->getValue()));

        in->setCallback([layer, sl, isM](const std::string& val) {
            if (val.empty()) return;
            sl->setValue(std::clamp(std::strtof(val.c_str(), nullptr), 0.f, 100.f) / 100.f);
            sl->updateBar();
            isM ? layer->musicSliderChanged(sl->m_touchLogic->m_thumb) : layer->sfxSliderChanged(sl->m_touchLogic->m_thumb);
        });

        if (auto p = lbl->getParent()) {
            p->addChild(in);

            auto pct = CCLabelBMFont::create("%", lbl->getFntFile());
            pct->setPosition(in->getPositionX() + 28.f, in->getPositionY());
            pct->setScale(lbl->getScale());
            p->addChild(pct);
        }
    };

    setupInput(mSl, mLbl, mIn, mCol, true);
    setupInput(sSl, sLbl, sIn, sCol, false);
}

class $modify(MyPauseLayer, PauseLayer) {
    struct Fields { TextInput* mIn; TextInput* sIn; Slider* mSl; Slider* sSl; };

    void customSetup() {
        PauseLayer::customSetup();
        setupVolumeUI(this, m_fields->mSl, m_fields->sSl, m_fields->mIn, m_fields->sIn);
    }

    void musicSliderChanged(CCObject* p0) {
        PauseLayer::musicSliderChanged(p0);
        if (m_fields->mIn && m_fields->mSl) m_fields->mIn->setString(getStr(m_fields->mSl->getValue()));
    }

    void sfxSliderChanged(CCObject* p0) {
        PauseLayer::sfxSliderChanged(p0);
        if (m_fields->sIn && m_fields->sSl) m_fields->sIn->setString(getStr(m_fields->sSl->getValue()));
    }
};

class $modify(MyOptionsLayer, OptionsLayer) {
    struct Fields { TextInput* mIn; TextInput* sIn; Slider* mSl; Slider* sSl; };

    void customSetup() {
        OptionsLayer::customSetup();
        setupVolumeUI(this, m_fields->mSl, m_fields->sSl, m_fields->mIn, m_fields->sIn);
    }

    void musicSliderChanged(CCObject* p0) {
        OptionsLayer::musicSliderChanged(p0);
        if (m_fields->mIn && m_fields->mSl) m_fields->mIn->setString(getStr(m_fields->mSl->getValue()));
    }

    void sfxSliderChanged(CCObject* p0) {
        OptionsLayer::sfxSliderChanged(p0);
        if (m_fields->sIn && m_fields->sSl) m_fields->sIn->setString(getStr(m_fields->sSl->getValue()));
    }
};
