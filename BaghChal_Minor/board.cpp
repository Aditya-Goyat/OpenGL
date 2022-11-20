#include "board.h"

Board::Board() {
		//1st Line
		nodes[0].SetPosition(glm::vec2(1033, 47));

		//2nd Line
		nodes[1].SetPosition(glm::vec2(510, 322));
		nodes[2].SetPosition(glm::vec2(892, 320));
		nodes[3].SetPosition(glm::vec2(984, 320));
		nodes[4].SetPosition(glm::vec2(1082, 320));
		nodes[5].SetPosition(glm::vec2(1171, 320));
		nodes[6].SetPosition(glm::vec2(1556, 320));

		//3rd Line
		nodes[7].SetPosition(glm::vec2(510, 475));
		nodes[8].SetPosition(glm::vec2(817, 476));
		nodes[9].SetPosition(glm::vec2(958, 476));
		nodes[10].SetPosition(glm::vec2(1111, 476));
		nodes[11].SetPosition(glm::vec2(1248, 476));
		nodes[12].SetPosition(glm::vec2(1555, 476));

		//4th Line
		nodes[13].SetPosition(glm::vec2(510, 661));
		nodes[14].SetPosition(glm::vec2(722, 661));
		nodes[15].SetPosition(glm::vec2(925, 661));
		nodes[16].SetPosition(glm::vec2(1144, 661));
		nodes[17].SetPosition(glm::vec2(1340, 661));
		nodes[18].SetPosition(glm::vec2(1555, 661));

		//5th Line
		nodes[19].SetPosition(glm::vec2(572, 962));
		nodes[20].SetPosition(glm::vec2(873, 962));
		nodes[21].SetPosition(glm::vec2(1197, 962));
		nodes[22].SetPosition(glm::vec2(1492, 962));

		nodes[0].connected = std::vector<Node>{ nodes[2], nodes[3], nodes[4], nodes[5] };

		nodes[1].connected = std::vector<Node>{ nodes[2], nodes[7] };
		nodes[2].connected = std::vector<Node>{ nodes[0], nodes[1], nodes[3], nodes[8] };
		nodes[3].connected = std::vector<Node>{ nodes[0], nodes[2], nodes[4], nodes[9] };
		nodes[4].connected = std::vector<Node>{ nodes[0], nodes[3], nodes[5], nodes[10] };
		nodes[5].connected = std::vector<Node>{ nodes[0], nodes[4], nodes[6], nodes[11] };
		nodes[6].connected = std::vector<Node>{ nodes[5], nodes[12] };

		nodes[7].connected = std::vector<Node>{ nodes[1], nodes[8], nodes[13] };
		nodes[8].connected = std::vector<Node>{ nodes[2], nodes[7], nodes[14], nodes[9] };
		nodes[9].connected = std::vector<Node>{ nodes[3], nodes[8], nodes[15], nodes[10] };
		nodes[10].connected = std::vector<Node>{ nodes[4], nodes[9], nodes[16], nodes[11] };
		nodes[11].connected = std::vector<Node>{ nodes[5], nodes[10], nodes[17], nodes[12] };
		nodes[12].connected = std::vector<Node>{ nodes[6], nodes[11], nodes[18] };

		nodes[13].connected = std::vector<Node>{ nodes[7], nodes[14] };
		nodes[14].connected = std::vector<Node>{ nodes[8], nodes[13], nodes[19], nodes[15] };
		nodes[15].connected = std::vector<Node>{ nodes[9], nodes[14], nodes[20], nodes[16] };
		nodes[16].connected = std::vector<Node>{ nodes[10], nodes[15], nodes[21], nodes[17] };
		nodes[17].connected = std::vector<Node>{ nodes[11], nodes[16], nodes[22], nodes[18] };
		nodes[18].connected = std::vector<Node>{ nodes[12], nodes[17] };

		nodes[19].connected = std::vector<Node>{ nodes[14], nodes[20] };
		nodes[20].connected = std::vector<Node>{ nodes[15], nodes[19], nodes[21] };
		nodes[21].connected = std::vector<Node>{ nodes[16], nodes[20], nodes[22] };
		nodes[22].connected = std::vector<Node>{ nodes[17], nodes[21] };
}