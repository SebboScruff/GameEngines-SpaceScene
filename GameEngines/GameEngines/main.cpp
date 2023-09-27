#define SDL_MAIN_HANDLED
#include "GameEngine.h"

#include <sstream>

using namespace GE;

int main() {
	GameEngine ge;

	if (!ge.init()) {
		displayInfoMessage("Couldn't start SDL. Check console output for more details.");

		return -1;
	}

	Uint32 lastTime = SDL_GetTicks(), currentTime = 0;

	int frameCount = 0;

	while (ge.keepRunning()) {
		ge.update();
		ge.processInput();
		ge.draw();

		frameCount++;
		currentTime = SDL_GetTicks();
		if (currentTime - lastTime > 1000) {
			std::ostringstream msg;
			msg << "FPS = " << frameCount;

			ge.setWindowTitle(msg.str().c_str());

			frameCount = 0;
			lastTime = currentTime;
		}
	}

	ge.shutdown();

	return 0;
}