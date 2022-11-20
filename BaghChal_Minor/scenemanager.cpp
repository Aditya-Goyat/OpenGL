#include "scenemanager.h"

void SceneManager::LoadScene(int scene, GameState* state) {
	switch (scene) {
	case 0:
		*state = GAME_MENU;
		break;
	case 1:
		*state = GAME_PLAY;
		break;
	case 2:
		*state = GAME_OVER;
		break;
	case 3:
		*state = GAME_OPTIONS;
		break;
	default:
		break;

	}
}
