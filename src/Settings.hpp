#pragma once

#include <Geode/loader/SettingV3.hpp>
#include "Manager.hpp"

using namespace geode::prelude;

class MyButtonSettingV3 : public SettingV3 {
public:
	static Result<std::shared_ptr<SettingV3>> parse(std::string const& key, std::string const& modID, matjson::Value const& json) {
		auto res = std::make_shared<MyButtonSettingV3>();
		auto root = checkJson(json, "MyButtonSettingV3");
		res->init(key, modID, root);
		res->parseNameAndDescription(root);
		res->parseEnableIf(root);
		return root.ok(std::static_pointer_cast<SettingV3>(res));
	}
	bool load(matjson::Value const& json) override {
		return true;
	}
	bool save(matjson::Value& json) const override {
		return true;
	}
	bool isDefaultValue() const override {
		return true;
	}
	void reset() override {}
	// SettingNodeV3* createNode(float width) override; // this line is [absolutely] certified cringe
	SettingNodeV3 *createNode(float width) override; // this line is NOT certified cringe
};

class MyButtonSettingNodeV3 : public SettingNodeV3 {
public:
	void onCommit() {}
	void onResetToDefault() {}
	bool hasUncommittedChanges() const {
		return false;
	}
	bool hasNonDefaultValue() const {
		return false;
	}
	void onPreviewJimButton(CCObject*) {
		Manager::createSFX();
		CCSprite* mrJimboree = Manager::createSpriteCustom(geode::utils::string::pathToString(Manager::get()->imagePath).c_str());
		if (!mrJimboree) return FLAlertLayer::create("Oh No!", "You don't have a valid image file for Jim!\n\nPlease choose a valid image in the mod settings.", "I Understand")->show();
		CCScene::get()->addChild(mrJimboree);
		Manager::resetMrJimboree(mrJimboree);
		Manager::animateMrJimboree(mrJimboree);
	}
	bool init(std::shared_ptr<MyButtonSettingV3> setting, float width) {
		if (!SettingNodeV3::init(setting, width)) return false;
		this->setContentSize({ width, 40.f });
		std::string name = setting->getName().value();
		auto theMenu = CCMenu::create();
		auto theLabel = CCLabelBMFont::create(name.c_str(), "bigFont.fnt");

		// copied a bit from louis ck jr
		theLabel->setScale(.35f);
		theLabel->limitLabelWidth(346.f, .35f, .25f); // added by Ery. max width is 346.f

		auto theLabelAsAButton = CCMenuItemSpriteExtra::create(theLabel, this, menu_selector(MyButtonSettingNodeV3::onPreviewJimButton));

		theLabelAsAButton->setPositionX(0);
		theLabelAsAButton->setID("preview-jim-shortcut-button"_spr);

		theMenu->addChild(theLabelAsAButton);
		theMenu->setPosition(width / 2, 20.f);
		theMenu->setID("preview-jim-shortcut-menu"_spr);

		this->addChild(theMenu);

		for (const auto &node : CCArrayExt<CCNode*>(this->getChildren())) {
			node->setVisible(!node->getID().empty() && utils::string::startsWith(node->getID(), Mod::get()->getID()));
		}

		return true;
	}
	static MyButtonSettingNodeV3* create(std::shared_ptr<MyButtonSettingV3> setting, float width) {
		auto ret = new MyButtonSettingNodeV3();
		if (ret && ret->init(setting, width)) {
			ret->autorelease();
			return ret;
		}
		CC_SAFE_DELETE(ret);
		return nullptr;
	}
};