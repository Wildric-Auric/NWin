#include "keyboard.h"
#include <cstring>

#define MAX(a,b)  ( ( (a) >= (b) ) ? (a) : (b)  )
#define MIN(a,b)  ( ( (a) <= (b) ) ? (a) : (b)  )
namespace NWin {
	void Keyboard::create(int bufLength) {
		delete[] _eventsQueue;
		_eventsQueue = new KeyEvent[bufLength];
		if (bufLength > 0) _eventsQueue[0].eventType = KeyEventEnum::NWIN_KeyEventNONE;	
		_length = bufLength;
		_timer.initialize();
	}
	void Keyboard::destroy() {
		delete[] _eventsQueue;
	}
	void Keyboard::resizeEventQueue(int newLength) {
		auto temp = new KeyEvent[newLength];
		memcpy(temp,_eventsQueue, sizeof(KeyEvent) * MAX(_length, newLength));
		_index =  MIN(_index, newLength - 1);
		delete[] _eventsQueue;
		_eventsQueue = temp;
		_length	     = newLength;
	}
	bool Keyboard::record(const KeyEvent& event,bool failIfFull) {

		//Length not sufficient to record
		if (_length <= 0) return 0;
		if (_index == _length) {
			if (failIfFull) return 0;
			_index = 0;
		}

		auto kmPair = keyMaster.find(event.key);
		//Insert in map if not yet in it.
		if (kmPair == keyMaster.end()) {
			kmPair = keyMaster.emplace(std::pair<Key, KeyMasterData>(event.key, {})).first;	
		}
		timeMl t	   = _timer.getTime();
		timeMl tDiff   = t - kmPair->second.lastEventTime;

		//Compute info
		//0.Two successive releases are not possible
		if (kmPair->second._lastEvent == NWIN_KeyReleased && event.eventType == NWIN_KeyReleased) {
			return 0;
		}
		//1.Check Cooldown
		if (kmPair->second._cooldownTimeBuffer > 0) {
			if (kmPair->second._cooldownTime > tDiff ) {
				kmPair->second._cooldownTimeBuffer = kmPair->second._cooldownTime - tDiff;
				return 0;
			}
			kmPair->second._cooldownTimeBuffer = 0;
		}
			
		//2.Increment pressing event
		//TODO::Check time of last event too
		if (kmPair->second._lastEvent == NWIN_KeyPressed 
			&& event.eventType		  == NWIN_KeyPressed) {
			kmPair->second.hasBeenPressedFor += tDiff;
		}
		//Set cooldown buffer and press time length
		if (event.eventType == NWIN_KeyReleased) {
			kmPair->second.hasBeenPressedFor = 0;
		}
		if (event.eventType == NWIN_KeyPressed) {
			kmPair->second._cooldownTimeBuffer = kmPair->second._cooldownTime;
		}

		kmPair->second.lastEventTime      = t;
		kmPair->second._lastEvent         = event.eventType;
		//3.Push event
		_eventsQueue[_index]	  = event;
		if (++_index == _length) return 1;
		_eventsQueue[_index].eventType = NWIN_KeyEventNONE;
		return 1;	
	}

	const KeyEventQueue Keyboard::getEventQueue() {
		return _eventsQueue;
	}

	bool Keyboard::isEventInQueue(const KeyEvent& event) {
		int i = 0;
		while (i != _length && _eventsQueue[i].eventType != NWIN_KeyEventNONE) {
			if (_eventsQueue[i].key == event.key && _eventsQueue[i].eventType == event.eventType)
				return 1;
			++i;
		}
		return 0;
	}

	bool Keyboard::isKeyPressed(Key key) {
		return isEventInQueue({key, NWIN_KeyPressed});
	}
	
	bool Keyboard::isKeyReleased(Key key) {
		return isEventInQueue({key, NWIN_KeyReleased});
	}

	void Keyboard::setKeyCooldown(Key key, timeMl t) {
		auto kmPair = keyMaster.find(key);
		if (kmPair == keyMaster.end()) {
			kmPair = keyMaster.emplace(std::pair<Key, KeyMasterData>(key, {})).first;
		}
		kmPair->second._cooldownTime	   = t;
		kmPair->second._cooldownTimeBuffer = 0;
	}

	const KeyMasterData* Keyboard::getKeyData(Key key) {
		auto kmPair = keyMaster.find(key);
		if (kmPair == keyMaster.end()) return nullptr;
		return &kmPair->second;
	}

	timeMl Keyboard::getKeyPressDuration(Key key) {
		auto kmPair = keyMaster.find(key);
		if (kmPair == keyMaster.end()) return 0;
		return kmPair->second.hasBeenPressedFor;
	}

	void Keyboard::update() {
		_timer.update();
		if (_length <= 0) return;
		_eventsQueue[0].eventType = NWIN_KeyEventNONE;
		_index					  =  0;
	}
}