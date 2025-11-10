#include <Geode/modify/PlayLayer.hpp>
#include "Settings.hpp"
#include "Manager.hpp"

using namespace geode::prelude;

bool enabled = false;
bool useBestPercentage = false;
bool useAltPercentOnlyOnCompletedLevels = false;

bool addedMrJimBoree = false;
bool alreadyRan = false;
bool currentlyFormingSequence = false;

double percentageThreshold = -1.f;
double alternativePercentage = -1.f;

class $modify(MyPlayLayer, PlayLayer) {
	void resetLevel() {
		PlayLayer::resetLevel();
		alreadyRan = false;
		CCNode* mrJimBoree = m_uiLayer->getChildByID("you-can-do-it"_spr);
		if (!m_uiLayer || !mrJimBoree) return;
		mrJimBoree->stopAllActions();
		Manager::resetMrJimboree(static_cast<CCSprite*>(mrJimBoree));
	}
	bool init(GJGameLevel* level, bool p1, bool p2) {
		if (!PlayLayer::init(level, p1, p2)) return false;
		if (!m_uiLayer || !level || level->isPlatformer() || m_isPlatformer) return true;
		if (!Mod::get()->getSettingValue<bool>("enabled")) return true;

		const std::filesystem::path& imagePath = Manager::get()->imagePath;
		const std::string& extension = geode::utils::string::pathToString(imagePath.extension());
		if (!std::filesystem::exists(imagePath) || (extension != ".png" && extension != ".gif")) return true;

		CCSprite* mrJimBoree = Manager::createSpriteCustom(geode::utils::string::pathToString(imagePath).c_str());
		if (!mrJimBoree) return true;

		mrJimBoree->setID("you-can-do-it"_spr);
		m_uiLayer->addChild(mrJimBoree);

		Manager::resetMrJimboree(mrJimBoree);

		addedMrJimBoree = true;

		Manager::createSFX();

		return true;
	}
	void updateInfoLabel() {
		PlayLayer::updateInfoLabel();
		if (!m_uiLayer || !m_level || m_level->isPlatformer() || m_isPlatformer || !m_player1 || m_player1->m_isDead || m_isTestMode || m_isPracticeMode) return;
		if (!enabled || !addedMrJimBoree || alreadyRan || currentlyFormingSequence) return;

		CCNode* jim = m_uiLayer->getChildByID("you-can-do-it"_spr);
		if (!m_uiLayer->getChildByID("you-can-do-it"_spr)) return;

		const int normalPercent = m_level->m_normalPercent.value();
		if (normalPercent < 1) return;

		double percent = normalPercent;
		if (!useBestPercentage) {
			if (useAltPercentOnlyOnCompletedLevels && normalPercent > 99) percent = alternativePercentage;
			else percent = normalPercent;
		}
		if (percent >= 100.) return;
		if (std::abs(PlayLayer::getCurrentPercent() - static_cast<float>(percent)) > percentageThreshold) return;

		CCSprite* mrJimBoree = static_cast<CCSprite*>(jim);
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
	(void) Mod::get()->registerCustomSettingType("previewjim", &MyButtonSettingV3::parse);
	enabled = Mod::get()->getSettingValue<bool>("enabled");
	useBestPercentage = Mod::get()->getSettingValue<bool>("useBestPercentage");
	useAltPercentOnlyOnCompletedLevels = Mod::get()->getSettingValue<bool>("useAltPercentOnlyOnCompletedLevels");
	percentageThreshold = std::clamp<double>(Mod::get()->getSettingValue<double>("percentageThreshold"), 0.f, 100.f);
	alternativePercentage = std::clamp<double>(Mod::get()->getSettingValue<double>("alternativePercentage"), 1.f, 100.f);
	Manager::get()->sfxPath = Mod::get()->getSettingValue<std::filesystem::path>("sfx");
	Manager::get()->imagePath = Mod::get()->getSettingValue<std::filesystem::path>("image");
	Mod::get()->setLoggingEnabled(Mod::get()->getSettingValue<bool>("logging"));
	listenForSettingChanges<bool>("enabled", [](bool newEnabled) {
		enabled = newEnabled;
	});
	listenForSettingChanges<bool>("useBestPercentage", [](bool newUseBestPercentage) {
		useBestPercentage = newUseBestPercentage;
	});
	listenForSettingChanges<bool>("useAltPercentOnlyOnCompletedLevels", [](bool newUseAltPercentOnlyOnCompletedLevels) {
		useAltPercentOnlyOnCompletedLevels = newUseAltPercentOnlyOnCompletedLevels;
	});
	listenForSettingChanges<double>("percentageThreshold", [](double newPercentageThreshold) {
		percentageThreshold = std::clamp<double>(newPercentageThreshold, 0.f, 100.f);
	});
	listenForSettingChanges<double>("alternativePercentage", [](double newAlternativePercentage) {
		alternativePercentage = std::clamp<double>(newAlternativePercentage, 1.f, 100.f);
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