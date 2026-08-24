#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/OptionsLayer.hpp>
#include "utils.hpp"
#include "settings.hpp"

using namespace geode::prelude;

template<typename T>
static void musicSliderChangedHook(CCObject* pSender, T* pLayer) {
    const auto volume = BetterVolume::getVolume(pSender);

    BetterVolume::setVolume(MUSIC, volume);

    if (Settings::enabled) {
        auto fields = pLayer->m_fields.self();

        if (fields->m_musicInput) {
            fields->m_musicInput->setString(BetterVolume::formatVolumeStr(volume));

            BetterVolume::tryUpdateMuteButton(MUSIC, pLayer);
        }
    }
}
template<typename T>
static void sfxSliderChangedHook(CCObject* pSender, T* pLayer) {
    const auto volume = BetterVolume::getVolume(pSender);
    
    BetterVolume::setVolume(SFX, volume);

    if (Settings::enabled) {
        auto fields = pLayer->m_fields.self();

        if (fields->m_sfxInput) {
            fields->m_sfxInput->setString(BetterVolume::formatVolumeStr(volume));

            BetterVolume::tryUpdateMuteButton(SFX, pLayer);
        }
    }
}

class $modify(PauseLayer) {
	struct Fields {
		TextInput* m_musicInput = nullptr;
		TextInput* m_sfxInput = nullptr;
		Slider* m_musicSlider = nullptr;
		Slider* m_sfxSlider = nullptr;
	};

	void customSetup() {
		PauseLayer::customSetup();

		if (!Settings::enabled) {
			return;
		}

        auto musicSlider = this->getChildByIDRecursive("music-slider");
        auto sfxSlider = this->getChildByIDRecursive("sfx-slider");
        auto musicLabel = this->getChildByIDRecursive("music-label");
        auto sfxLabel = this->getChildByIDRecursive("sfx-label");

        if (!musicSlider || !sfxSlider || !musicLabel || !sfxLabel) {
            return;
        }

		if (Settings::swapSliders) {
            const auto musicSliderPos = musicSlider->getPosition();
            const auto sfxSliderPos = sfxSlider->getPosition();
            const auto musicLabelPos = musicLabel->getPosition();
            const auto sfxLabelPos = sfxLabel->getPosition();

            musicSlider->setPosition(sfxSliderPos);
            sfxSlider->setPosition(musicSliderPos);
            musicLabel->setPosition(sfxLabelPos);
            sfxLabel->setPosition(musicLabelPos);
		}
		
		constexpr float HORIZONTAL_SHIFT = 12.0f;
		const float musicShift = Settings::swapSliders ? HORIZONTAL_SHIFT : -HORIZONTAL_SHIFT;
		const float sfxShift = Settings::swapSliders ? -HORIZONTAL_SHIFT : HORIZONTAL_SHIFT;

        musicSlider->setPositionX(ui::x(musicSlider) + musicShift);
        musicLabel->setPositionX(ui::x(musicLabel) + musicShift);
        sfxSlider->setPositionX(ui::x(sfxSlider) + sfxShift);
        sfxLabel->setPositionX(ui::x(sfxLabel) + sfxShift);
		
		auto fields = m_fields.self();

		BetterVolume::setupSlider(
            MUSIC, this, 
            [this] (CCObject* pSender) {
                musicSliderChanged(pSender);
            }, 
            fields->m_musicInput, fields->m_musicSlider
        );
		BetterVolume::setupSlider(SFX, this,
            [this] (CCObject* pSender) { 
                sfxSliderChanged(pSender);
            }, 
            fields->m_sfxInput, fields->m_sfxSlider
        );
	}

	void musicSliderChanged(CCObject* pSender) {
		musicSliderChangedHook(pSender, this);
	}

    // on windows this is also called by optionslayer
	void sfxSliderChanged(CCObject* pSender) {
		sfxSliderChangedHook(pSender, this);
	}
};

class $modify(OptionsLayer) {
	struct Fields {
		TextInput* m_musicInput = nullptr;
		TextInput* m_sfxInput = nullptr;
	};

	void customSetup() {
		OptionsLayer::customSetup();

		if (!Settings::enabled) {
			return;
		}

        auto musicLabel = this->getChildByIDRecursive("music-label");
        auto sfxLabel = this->getChildByIDRecursive("sfx-label");

        if (!m_musicSlider || !m_sfxSlider || !musicLabel || !sfxLabel) {
            return;
        }

		if (Settings::swapSliders) {
            const auto musicSliderPos = m_musicSlider->getPosition();
            const auto sfxSliderPos = m_sfxSlider->getPosition();
            const auto musicLabelPos = musicLabel->getPosition();
            const auto sfxLabelPos = sfxLabel->getPosition();

            m_musicSlider->setPosition(sfxSliderPos);
            m_sfxSlider->setPosition(musicSliderPos);
            musicLabel->setPosition(sfxLabelPos);
            sfxLabel->setPosition(musicLabelPos);
		}

		auto fields = m_fields.self();

		BetterVolume::setupSlider(
            MUSIC, this, 
            [this] (CCObject* pSender) {
                musicSliderChanged(pSender);
            },
            fields->m_musicInput, m_musicSlider
        );
		BetterVolume::setupSlider(
            SFX, this,
            [this] (CCObject* pSender) {
                sfxSliderChanged(pSender);
            }, 
            fields->m_sfxInput, m_sfxSlider
        );
	}

	void musicSliderChanged(CCObject* pSender) {
        const auto wasMuted = BetterVolume::getVolume(MUSIC) <= 0.0f;

		musicSliderChangedHook(pSender, this);

        if (wasMuted && 0.0f < BetterVolume::getVolume(MUSIC)) {
            GameManager::get()->playMenuMusic();
        }
	}

    #ifndef GEODE_IS_WINDOWS
        void sfxSliderChanged(CCObject* pSender) {
            sfxSliderChangedHook(pSender, this);
        }
    #endif
};

$on_mod(Loaded) {
    SettingsManager::get()->load();
}