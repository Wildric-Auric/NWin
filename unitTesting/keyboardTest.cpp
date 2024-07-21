#include "unitTesting.h"
#include "window.h"
#include "timer.h"

namespace UnitTesting {
	void keyboardTest() {
		NWin::Window* w;
		NWin::WindowCrtInfo c{};
		c.description = "Keyboard Test";
		c.metrics.size = { 480, 360 };
		c.style = (NWin::Word)NWin::WindowStyle::Default;
		c.exStyle = (NWin::Word)NWin::WindowExStyle::Default;
		w = NWin::Window::stCreateWindow(c);
		w->dwmDarkModeFrame(1);
		//Render Loop--------------
		/*

			Press "pressTestKey"	to test press   event.
			Press "releaseTestKey"	to test release event.
			Press "cooldownTestKey" to test cooldown
		
		*/
		NWin::Timer timer;
		timer.initialize();

		NWin::Key pressTestKey	   = NWin::Key::NWIN_KEY_RIGHT;
		NWin::Key releaseTestKey   = NWin::Key::NWIN_KEY_RBUTTON;
		NWin::Key durationTestKey  = NWin::Key::NWIN_KEY_DOWN;
		NWin::Key cooldownTestKey  = NWin::Key::NWIN_KEY_LEFT;

		NWin::timeMl    durationTestTime = 1000;
		NWin::timeMl    cooldownTestTime = 2000;

		w->_getKeyboard().setKeyCooldown(cooldownTestKey, cooldownTestTime);

		while (w->shouldLoop()) { 
			w->_getKeyboard().update();
			w->update(); 
			static bool last = 0;
			bool current = w->_getKeyboard().isKeyPressed(pressTestKey);
			if (!current && last) {
				std::cout << " STOP! " << std::endl;
			}

			last = current;

			if (w->_getKeyboard().isKeyPressed(pressTestKey) ) {
				std::cout << " PRESS TEST " << std::endl;
			}
	
			if (w->_getKeyboard().onKeyRelease(releaseTestKey)) {
				std::cout << " RELEASE TEST " << std::endl;
			}
			if (w->_getKeyboard().getKeyData(NWin::NWIN_KEY_DOWN) )
			if (w->_getKeyboard().getKeyPressDuration(durationTestKey) >= durationTestTime && w->_getKeyboard().getKeyPressDuration(durationTestKey) < durationTestTime + 500) {
				std::cout << " DURATION TEST: " << durationTestTime << std::endl;
			}

			if (w->_getKeyboard().onKeyPress(cooldownTestKey)) {
				std::cout << " COOLDOWN TEST: " << cooldownTestTime << "   CurrentTime: " << timer.getTime() << std::endl;
			}

			NWin::Vec2 p;
			w->getMousePosition(p);
			//std::cout << p.x << "   " << p.y << std::endl;

		}

		NWin::Window::stClean(w);
	}
};
