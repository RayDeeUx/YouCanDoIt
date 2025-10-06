#include <Geode/modify/PlayLayer.hpp>
#include "Manager.hpp"

using namespace geode::prelude;

bool enabled = false;
bool addedMrJimBoree = false;
bool alreadyRan = false;
bool currentlyFormingSequence = false;

double percentageThreshold = -1.f;

std::filesystem::path sfxPath;

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
	static CCActionInterval* getEaseTypeForCustomScaleAnimation(CCActionInterval* action, const std::string& modStringSetting, const float easingRate) {
		if (!action) return nullptr;
		const std::string& easeType = utils::string::toLower(modStringSetting);

		if (easeType == "none (linear)" || easeType == "none" || easeType == "linear") return action;

		if (easeType == "ease in") return CCEaseIn::create(action, easingRate);
		if (easeType == "ease out") return CCEaseOut::create(action, easingRate);
		if (easeType == "ease in out") return CCEaseInOut::create(action, easingRate);

		if (easeType == "back in") return CCEaseBackIn::create(action);
		if (easeType == "back out") return CCEaseBackOut::create(action);
		if (easeType == "back in out") return CCEaseBackInOut::create(action);

		if (easeType == "bounce in") return CCEaseBounceIn::create(action);
		if (easeType == "bounce out") return CCEaseBounceOut::create(action);
		if (easeType == "bounce in out") return CCEaseBounceInOut::create(action);

		if (easeType == "elastic in") return CCEaseElasticIn::create(action, easingRate);
		if (easeType == "elastic out") return CCEaseElasticOut::create(action, easingRate);
		if (easeType == "elastic in out") return CCEaseElasticInOut::create(action, easingRate);

		if (easeType == "exponential in") return CCEaseExponentialIn::create(action);
		if (easeType == "exponential out") return CCEaseExponentialOut::create(action);
		if (easeType == "exponential in out") return CCEaseExponentialInOut::create(action);

		if (easeType == "sine in") return CCEaseSineIn::create(action);
		if (easeType == "sine out") return CCEaseSineOut::create(action);
		if (easeType == "sine in out") return CCEaseSineInOut::create(action);

		return action;
	}
	void resetLevel() {
		PlayLayer::resetLevel();
		alreadyRan = false;
		if (!m_uiLayer || !m_uiLayer->getChildByID("you-can-do-it"_spr)) return;
		CCNode* mrJimBoree = m_uiLayer->getChildByID("you-can-do-it"_spr);
		mrJimBoree->stopAllActions();
		MyPlayLayer::resetMrJimboree(static_cast<CCSprite*>(mrJimBoree));
	}
	void resetMrJimboree(CCSprite* mrJimBoree) {
		const CCSize winSize = CCDirector::get()->getWinSize();
		const std::string& startingPosition = geode::utils::string::toLower(Mod::get()->getSettingValue<std::string>("startMovingFrom"));
		if (startingPosition == "top") {
			mrJimBoree->setAnchorPoint({.5f, 0.f});
			mrJimBoree->setPosition({winSize.width * .5f, winSize.height + 100.f});
		} else if (startingPosition == "bottom") {
			mrJimBoree->setAnchorPoint({.5f, 1.f});
			mrJimBoree->setPosition({winSize.width * .5f, -100.f});
		} else if (startingPosition == "right") {
			mrJimBoree->setAnchorPoint({0.f, .5f});
			mrJimBoree->setPosition({winSize.width + 100.f, winSize.height * .5f});
		} else if (startingPosition == "left") {
			mrJimBoree->setAnchorPoint({1.f, .5f});
			mrJimBoree->setPosition({-100.f, winSize.height * .5f});
		} else if (startingPosition == "top right") {
			mrJimBoree->setAnchorPoint({0.f, 0.f});
			mrJimBoree->setPosition({winSize.width + 100.f, winSize.height + 100.f});
		} else if (startingPosition == "top left") {
			mrJimBoree->setAnchorPoint({1.f, 0.f});
			mrJimBoree->setPosition({-100.f, winSize.height + 100.f});
		} else if (startingPosition == "bottom right") {
			mrJimBoree->setAnchorPoint({0.f, 1.f});
			mrJimBoree->setPosition({winSize.width + 100.f, -100.f});
		} else if (startingPosition == "bottom left") {
			mrJimBoree->setAnchorPoint({1.f, 1.f});
			mrJimBoree->setPosition({-100.f, -100.f});
		} else {
			mrJimBoree->setAnchorPoint({0.f, .5f});
			mrJimBoree->setPositionX(winSize.width + 100.f);
		}

		mrJimBoree->setScale(std::clamp<float>(static_cast<float>(Mod::get()->getSettingValue<double>("initialScale")), 0.f, 1.f));
		mrJimBoree->setOpacity(std::clamp<int>(static_cast<int>(Mod::get()->getSettingValue<int64_t>("initialOpacity")), 0, 255));
		mrJimBoree->setRotation(std::clamp<float>(static_cast<float>(Mod::get()->getSettingValue<double>("initialRotation")), 0.f, 360.f));
	}
	bool init(GJGameLevel* level, bool p1, bool p2) {
		if (!PlayLayer::init(level, p1, p2)) return false;
		if (!m_uiLayer || !level || level->isPlatformer() || m_isPlatformer) return true;
		if (!Mod::get()->getSettingValue<bool>("enabled")) return true;

		const std::filesystem::path& imagePath = Mod::get()->getSettingValue<std::filesystem::path>("image");
		if (!std::filesystem::exists(imagePath) || (imagePath.extension() != "png" && imagePath.extension() != "gif")) return true; // moveToEasingType

		CCSprite* mrJimBoree = MyPlayLayer::createSpriteCustom(geode::utils::string::pathToString(imagePath).c_str());
		if (!mrJimBoree) return true;

		mrJimBoree->setID("you-can-do-it"_spr);
		m_uiLayer->addChild(mrJimBoree);

		MyPlayLayer::resetMrJimboree(mrJimBoree);

		addedMrJimBoree = true;

		if (!std::filesystem::exists(sfxPath)) return true;

		Manager* manager = Manager::get();
		manager->system->createSound(geode::utils::string::pathToString(sfxPath).c_str(), FMOD_DEFAULT, nullptr, &manager->sound);

		return true;
	}
	void updateInfoLabel() {
		PlayLayer::updateInfoLabel();
		if (!m_uiLayer || !m_level || m_level->isPlatformer() || m_isPlatformer || !m_player1 || m_player1->m_isDead || m_isTestMode || m_isPracticeMode) return;
		if (!enabled || !addedMrJimBoree || alreadyRan || currentlyFormingSequence) return;

		const int percent = m_level->m_normalPercent.value();
		if (percent < 1 || percent > 99) return;
		if (std::abs(PlayLayer::getCurrentPercent() - static_cast<float>(percent)) > percentageThreshold) return;

		CCSprite* mrJimBoree = static_cast<CCSprite*>(m_uiLayer->getChildByID("you-can-do-it"_spr));
		const float moveForDuration = std::clamp<float>(static_cast<float>(Mod::get()->getSettingValue<double>("moveForDuration")), .1f, 10.f);
		const float returnToDuration = std::clamp<float>(static_cast<float>(Mod::get()->getSettingValue<double>("returnToDuration")), .1f, 10.f);

		const CCPoint originalPosition = mrJimBoree->getPosition();
		const GLubyte originalOpacity = mrJimBoree->getOpacity();
		const float originalRotation = mrJimBoree->getRotation();
		const float originalScale = mrJimBoree->getScale();

		const CCSize winSize = CCDirector::get()->getWinSize();
		const float xPosAfterCalculation = winSize.width * (static_cast<float>(std::clamp<int>(static_cast<int>(Mod::get()->getSettingValue<int64_t>("moveToX")), 0, 100)) / 100.f);
		const float yPosAfterCalculation = winSize.height * (static_cast<float>(std::clamp<int>(static_cast<int>(Mod::get()->getSettingValue<int64_t>("moveToY")), 0, 100)) / 100.f);

		currentlyFormingSequence = true;

		CCActionInterval* moveToAction = MyPlayLayer::getEaseTypeForCustomScaleAnimation(CCMoveTo::create(moveForDuration, {xPosAfterCalculation, yPosAfterCalculation}), Mod::get()->getSettingValue<std::string>("moveToEasingType"), std::clamp<float>(static_cast<float>(Mod::get()->getSettingValue<double>("moveToEasingRate")), .1f, 4.f));
		CCActionInterval* rotateToAction = MyPlayLayer::getEaseTypeForCustomScaleAnimation(CCRotateTo::create(moveForDuration, 0), Mod::get()->getSettingValue<std::string>("scaleToEasingType"), std::clamp<float>(static_cast<float>(Mod::get()->getSettingValue<double>("scaleToEasingRate")), .1f, 4.f));
		CCActionInterval* scaleToAction = MyPlayLayer::getEaseTypeForCustomScaleAnimation(CCScaleTo::create(moveForDuration, 1.f), Mod::get()->getSettingValue<std::string>("rotateToEasingType"), std::clamp<float>(static_cast<float>(Mod::get()->getSettingValue<double>("rotateToEasingRate")), .1f, 4.f));
		CCFadeTo* fadeToAction = CCFadeTo::create(moveForDuration, 255);
		CCSpawn* spawnToAction = CCSpawn::create(moveToAction, rotateToAction, scaleToAction, fadeToAction, nullptr);

		CCDelayTime* holdForDuration = CCDelayTime::create(std::clamp<float>(static_cast<float>(Mod::get()->getSettingValue<double>("holdForDuration")), .1f, 5.f) + moveForDuration);

		CCActionInterval* moveBackToAction = MyPlayLayer::getEaseTypeForCustomScaleAnimation(CCMoveTo::create(returnToDuration, originalPosition), Mod::get()->getSettingValue<std::string>("moveBackToEasingType"), std::clamp<float>(static_cast<float>(Mod::get()->getSettingValue<double>("moveBackToEasingRate")), .1f, 4.f));
		CCActionInterval* rotateBackToAction = MyPlayLayer::getEaseTypeForCustomScaleAnimation(CCRotateTo::create(returnToDuration, originalRotation), Mod::get()->getSettingValue<std::string>("scaleBackToEasingType"), std::clamp<float>(static_cast<float>(Mod::get()->getSettingValue<double>("scaleBackToEasingRate")), .1f, 4.f));
		CCActionInterval* scaleBackToAction = MyPlayLayer::getEaseTypeForCustomScaleAnimation(CCScaleTo::create(returnToDuration, originalScale), Mod::get()->getSettingValue<std::string>("rotateBackToEasingType"), std::clamp<float>(static_cast<float>(Mod::get()->getSettingValue<double>("rotateBackToEasingRate")), .1f, 4.f));
		CCFadeTo* fadeBackToAction = CCFadeTo::create(returnToDuration, originalOpacity);
		CCSpawn* spawnBackToAction = CCSpawn::create(moveBackToAction, rotateBackToAction, scaleBackToAction, fadeBackToAction, nullptr);

		CCSequence* fullSequence = CCSequence::create(spawnToAction, holdForDuration, spawnBackToAction, nullptr);
		currentlyFormingSequence = false;

		mrJimBoree->runAction(fullSequence);
		alreadyRan = true;

		if (!std::filesystem::exists(sfxPath)) return;
		Manager* manager = Manager::get();
		manager->system->playSound(manager->sound, nullptr, false, &manager->channel);
		manager->channel->setVolume(static_cast<float>(std::clamp<int>(static_cast<int>(Mod::get()->getSettingValue<int64_t>("volume")), 0, 100)) / 100.f);
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
	sfxPath = Mod::get()->getSettingValue<std::filesystem::path>("sfx");
	Mod::get()->setLoggingEnabled(Mod::get()->getSettingValue<bool>("logging"));

	listenForSettingChanges<bool>("enabled", [](bool newEnabled) {
		enabled = newEnabled;
	});
	listenForSettingChanges<double>("percentageThreshold", [](double newPercentageThreshold) {
		percentageThreshold = newPercentageThreshold;
	});
	listenForSettingChanges<std::filesystem::path>("sfx", [](const std::filesystem::path& newSFXPath) {
		sfxPath = newSFXPath;
	});
	listenForSettingChanges<bool>("logging", [](bool newLogging) {
		Mod::get()->setLoggingEnabled(newLogging);
	});
}