#pragma once

// Manager.hpp structure by acaruso
// reused with explicit permission and strong encouragement

using namespace geode::prelude;

class Manager {

protected:
	static Manager* instance;
public:
	
	FMOD::Sound* sound {};
	FMOD::Channel* channel {};
	FMOD::DSP* lowPassFilterDSP {};
	FMOD::System* system = FMODAudioEngine::sharedEngine()->m_system;

	std::filesystem::path sfxPath;
	std::filesystem::path imagePath;

	static Manager* get() {
		if (!instance) instance = new Manager();
		return instance;
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
	static void resetMrJimboree(CCSprite* mrJimBoree) {
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
		mrJimBoree->setColor(Mod::get()->getSettingValue<ccColor3B>("initialColor"));

		mrJimBoree->setCascadeColorEnabled(false);
		mrJimBoree->setCascadeOpacityEnabled(false);
	}

	static void animateMrJimboree(CCSprite* mrJimBoree) {
		const float moveForDuration = std::clamp<float>(static_cast<float>(Mod::get()->getSettingValue<double>("moveForDuration")), .1f, 10.f);
		const float returnToDuration = std::clamp<float>(static_cast<float>(Mod::get()->getSettingValue<double>("returnToDuration")), .1f, 10.f);

		const ccColor3B cocosIsFuckingStupid = Mod::get()->getSettingValue<ccColor3B>("colorTo");

		const CCPoint originalPosition = mrJimBoree->getPosition();
		const GLubyte originalOpacity = mrJimBoree->getOpacity();
		const float originalRotation = mrJimBoree->getRotation();
		const ccColor3B originalColor = mrJimBoree->getColor();
		const float originalScale = mrJimBoree->getScale();

		const CCSize winSize = CCDirector::get()->getWinSize();
		const float xPosAfterCalculation = winSize.width * (static_cast<float>(std::clamp<int>(static_cast<int>(Mod::get()->getSettingValue<int64_t>("moveToX")), 0, 100)) / 100.f);
		const float yPosAfterCalculation = winSize.height * (static_cast<float>(std::clamp<int>(static_cast<int>(Mod::get()->getSettingValue<int64_t>("moveToY")), 0, 100)) / 100.f);

		CCActionInterval* moveToAction = Manager::getEaseTypeForCustomScaleAnimation(CCMoveTo::create(moveForDuration, {xPosAfterCalculation, yPosAfterCalculation}), Mod::get()->getSettingValue<std::string>("moveToEasingType"), std::clamp<float>(static_cast<float>(Mod::get()->getSettingValue<double>("moveToEasingRate")), .1f, 4.f));
		CCActionInterval* rotateToAction = Manager::getEaseTypeForCustomScaleAnimation(CCRotateTo::create(moveForDuration, 0), Mod::get()->getSettingValue<std::string>("scaleToEasingType"), std::clamp<float>(static_cast<float>(Mod::get()->getSettingValue<double>("scaleToEasingRate")), .1f, 4.f));
		CCActionInterval* scaleToAction = Manager::getEaseTypeForCustomScaleAnimation(CCScaleTo::create(moveForDuration, std::clamp<float>(static_cast<float>(Mod::get()->getSettingValue<double>("scaleTo")), .5f, 1.f)), Mod::get()->getSettingValue<std::string>("rotateToEasingType"), std::clamp<float>(static_cast<float>(Mod::get()->getSettingValue<double>("rotateToEasingRate")), .1f, 4.f));
		CCFadeTo* fadeToAction = CCFadeTo::create(moveForDuration, 255);
		CCTintTo* tintToAction = CCTintTo::create(moveForDuration, cocosIsFuckingStupid.r, cocosIsFuckingStupid.g, cocosIsFuckingStupid.b);
		CCSpawn* spawnToAction = CCSpawn::create(moveToAction, rotateToAction, scaleToAction, fadeToAction, tintToAction, nullptr);

		CCDelayTime* holdForDuration = CCDelayTime::create(std::clamp<float>(static_cast<float>(Mod::get()->getSettingValue<double>("holdForDuration")), .1f, 5.f) + moveForDuration);

		CCActionInterval* moveBackToAction = Manager::getEaseTypeForCustomScaleAnimation(CCMoveTo::create(returnToDuration, originalPosition), Mod::get()->getSettingValue<std::string>("moveBackToEasingType"), std::clamp<float>(static_cast<float>(Mod::get()->getSettingValue<double>("moveBackToEasingRate")), .1f, 4.f));
		CCActionInterval* rotateBackToAction = Manager::getEaseTypeForCustomScaleAnimation(CCRotateTo::create(returnToDuration, originalRotation), Mod::get()->getSettingValue<std::string>("scaleBackToEasingType"), std::clamp<float>(static_cast<float>(Mod::get()->getSettingValue<double>("scaleBackToEasingRate")), .1f, 4.f));
		CCActionInterval* scaleBackToAction = Manager::getEaseTypeForCustomScaleAnimation(CCScaleTo::create(returnToDuration, originalScale), Mod::get()->getSettingValue<std::string>("rotateBackToEasingType"), std::clamp<float>(static_cast<float>(Mod::get()->getSettingValue<double>("rotateBackToEasingRate")), .1f, 4.f));
		CCFadeTo* fadeBackToAction = CCFadeTo::create(returnToDuration, originalOpacity);
		CCTintTo* tintBackToAction = CCTintTo::create(returnToDuration, originalColor.r, originalColor.g, originalColor.b);
		CCSpawn* spawnBackToAction = CCSpawn::create(moveBackToAction, rotateBackToAction, scaleBackToAction, fadeBackToAction, tintBackToAction, nullptr);

		CCSequence* fullSequence = CCSequence::create(spawnToAction, holdForDuration, spawnBackToAction, nullptr);

		mrJimBoree->runAction(fullSequence);

		Manager* manager = Manager::get();
		if (!std::filesystem::exists(manager->sfxPath)) return;
		manager->system->playSound(manager->sound, nullptr, false, &manager->channel);
		manager->channel->setVolume(static_cast<float>(std::clamp<int>(static_cast<int>(Mod::get()->getSettingValue<int64_t>("volume")), 0, 100)) / 100.f);
	}
};