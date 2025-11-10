#include <Geode/modify/PlayLayer.hpp>
#include "Manager.hpp"

using namespace geode::prelude;

bool enabled = false;
bool useBestPercentage = false;

bool addedMrJimBoree = false;
bool alreadyRan = false;
bool currentlyFormingSequence = false;

double percentageThreshold = -1.f;
double alternativePercentage = -1.f;

class $modify(MyPlayLayer, PlayLayer) {
	static CCSprite* createSpriteCustom(const char* pathToFile) {
		auto sprite = new CCSprite();
		if (sprite->initWithFile(pathToFile)) {
			sprite->autorelease();
			return sprite;
		}
		delete sprite;
		return nullptr;
	}
	void resetLevel() {
		PlayLayer::resetLevel();
		alreadyRan = false;
		if (!m_uiLayer || !m_uiLayer->getChildByID("you-can-do-it"_spr)) return;
		CCNode* mrJimBoree = m_uiLayer->getChildByID("you-can-do-it"_spr);
		mrJimBoree->stopAllActions();
		Manager::resetMrJimboree(static_cast<CCSprite*>(mrJimBoree));
	}
	bool init(GJGameLevel* level, bool p1, bool p2) {
		if (!PlayLayer::init(level, p1, p2)) return false;
		if (!m_uiLayer || !level || level->isPlatformer() || m_isPlatformer) return true;
		if (!Mod::get()->getSettingValue<bool>("enabled")) return true;

		const std::filesystem::path& imagePath = Manager::get()->imagePath;
		if (!std::filesystem::exists(imagePath) || (imagePath.extension() != ".png" && imagePath.extension() != ".gif")) return true; // moveToEasingType

		CCSprite* mrJimBoree = MyPlayLayer::createSpriteCustom(geode::utils::string::pathToString(imagePath).c_str());
		if (!mrJimBoree) return true;

		mrJimBoree->setID("you-can-do-it"_spr);
		m_uiLayer->addChild(mrJimBoree);

		Manager::resetMrJimboree(mrJimBoree);

		addedMrJimBoree = true;

		Manager* manager = Manager::get();
		if (!std::filesystem::exists(manager->sfxPath)) return true;
		manager->system->createSound(geode::utils::string::pathToString(manager->sfxPath).c_str(), FMOD_DEFAULT, nullptr, &manager->sound);

		return true;
	}
	void updateInfoLabel() {
		PlayLayer::updateInfoLabel();
		if (!m_uiLayer || !m_level || m_level->isPlatformer() || m_isPlatformer || !m_player1 || m_player1->m_isDead || m_isTestMode || m_isPracticeMode) return;
		if (!enabled || !addedMrJimBoree || alreadyRan || currentlyFormingSequence) return;
		if (!m_uiLayer->getChildByID("you-can-do-it"_spr)) return;

		const double percent = useBestPercentage ? m_level->m_normalPercent.value() : alternativePercentage;
		if (percent < 1 || percent > 99) return;
		if (std::abs(PlayLayer::getCurrentPercent() - static_cast<float>(percent)) > percentageThreshold) return;

		CCSprite* mrJimBoree = static_cast<CCSprite*>(m_uiLayer->getChildByID("you-can-do-it"_spr));
		Manager::resetMrJimboree(mrJimBoree);

		currentlyFormingSequence = true;
		Manager::animateMrJimboree(mrJimBoree);
		currentlyFormingSequence = false;
		alreadyRan = true;
	}
	void onQuit() {
		PlayLayer::onQuit();
		addedMrJimBoree = false;
		alreadyRan = false;
		currentlyFormingSequence = false;
	}
};

$on_mod(Loaded) {
	enabled = Mod::get()->getSettingValue<bool>("enabled");
	percentageThreshold = Mod::get()->getSettingValue<double>("percentageThreshold");
	useBestPercentage = Mod::get()->getSettingValue<bool>("useBestPercentage");
	alternativePercentage = Mod::get()->getSettingValue<double>("alternativePercentage");
	Manager::get()->sfxPath = Mod::get()->getSettingValue<std::filesystem::path>("sfx");
	Manager::get()->imagePath = Mod::get()->getSettingValue<std::filesystem::path>("image");
	Mod::get()->setLoggingEnabled(Mod::get()->getSettingValue<bool>("logging"));
	listenForSettingChanges<bool>("enabled", [](bool newEnabled) {
		enabled = newEnabled;
	});
	listenForSettingChanges<bool>("useBestPercentage", [](bool newUseBestPercentage) {
		useBestPercentage = newUseBestPercentage;
	});
	listenForSettingChanges<double>("percentageThreshold", [](double newPercentageThreshold) {
		percentageThreshold = newPercentageThreshold;
	});
	listenForSettingChanges<double>("alternativePercentage", [](double newAlternativePercentage) {
		alternativePercentage = newAlternativePercentage;
	});
	listenForSettingChanges<std::filesystem::path>("sfx", [](const std::filesystem::path& newSFXPath) {
		Manager::get()->sfxPath = newSFXPath;
	});
	listenForSettingChanges<std::filesystem::path>("image", [](const std::filesystem::path& newImagePath) {
		Manager::get()->imagePath = newImagePath;
	});
	listenForSettingChanges<bool>("logging", [](bool newLogging) {
		Mod::get()->setLoggingEnabled(newLogging);
	});
}