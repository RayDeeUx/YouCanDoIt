#pragma once

// Manager.hpp structure by acaruso
// reused with explicit permission and strong encouragement

using namespace geode::prelude;

class Manager {

protected:
	static Manager* instance;
public:
	
	FMOD::Sound* sound;
	FMOD::Channel* channel;
	FMOD::System* system = FMODAudioEngine::sharedEngine()->m_system;
	FMOD::DSP* lowPassFilterDSP;

	static Manager* get() {
		if (!instance) instance = new Manager();
		return instance;
	}

};