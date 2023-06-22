#include"memory.h"
#include<thread>

namespace offsets {
	constexpr ::std::ptrdiff_t localPlayer = 0xDE997C;

	//for glow esp
	constexpr auto EntityList = 0x4DFEECC;
	constexpr auto GlowObjectManager = 0x5359980;
	constexpr auto TeamNum = 0xF4;
	constexpr auto GlowIndex = 0x10488;
}

//Globals
auto mem = Memory("csgo.exe");
const auto client = mem.GetModuleAddress("client.dll");

struct Color {
	constexpr Color(float r, float g, float b, float a = 1.f) noexcept :
		r(r), g(g), b(b), a(a) { }

	float r, g, b, a;
};

void glowESP() {

	const auto localPlayer = mem.Read<uintptr_t>(client + offsets::localPlayer);
	//const auto color = Color{ 1.f, 0.f, 0.f };

	//the list of objects that can glow
	const auto glowObjectManager = mem.Read<std::uintptr_t>(client + offsets::GlowObjectManager);

	//max people in the game is 64
	for (auto i = 0; i < 64; ++i) {
		const auto entity = mem.Read<uintptr_t>(client + offsets::EntityList + i * 0x10);

		//skipping teammates
		if (mem.Read<std::uintptr_t>(entity + offsets::TeamNum) == mem.Read<std::uintptr_t>(localPlayer + offsets::TeamNum)) {
			continue;
		}

		//the index of the glow object
		const auto glowIndex = mem.Read<std::int32_t>(entity + offsets::GlowIndex);

		mem.Write<Color>(glowObjectManager + (glowIndex * 0x38) + 0x8, { 1.f, 0.f, 0.f });

		//to glow all the time.
		mem.Write<bool>(glowObjectManager + (glowIndex * 0x38) + 0x27, true);
		mem.Write<bool>(glowObjectManager + (glowIndex * 0x38) + 0x28, true);
	}

}

int main()
{

	printf(R"EOF(

  /$$$$$$   /$$$$$$   /$$$$$$   /$$$$$$         /$$$$$$  /$$   /$$ /$$$$$$$$  /$$$$$$  /$$$$$$$$ /$$$$$$ 
 /$$__  $$ /$$__  $$ /$$__  $$ /$$__  $$       /$$__  $$| $$  | $$| $$_____/ /$$__  $$|__  $$__//$$__  $$
| $$  \__/| $$  \__/| $$  \__/| $$  \ $$      | $$  \__/| $$  | $$| $$      | $$  \ $$   | $$  | $$  \__/
| $$      |  $$$$$$ | $$ /$$$$| $$  | $$      | $$      | $$$$$$$$| $$$$$   | $$$$$$$$   | $$  |  $$$$$$ 
| $$       \____  $$| $$|_  $$| $$  | $$      | $$      | $$__  $$| $$__/   | $$__  $$   | $$   \____  $$
| $$    $$ /$$  \ $$| $$  \ $$| $$  | $$      | $$    $$| $$  | $$| $$      | $$  | $$   | $$   /$$  \ $$
|  $$$$$$/|  $$$$$$/|  $$$$$$/|  $$$$$$/      |  $$$$$$/| $$  | $$| $$$$$$$$| $$  | $$   | $$  |  $$$$$$/
 \______/  \______/  \______/  \______/        \______/ |__/  |__/|________/|__/  |__/   |__/   \______/

)EOF");

	if (mem.GetProcessID() != 0) {
		std::cout << "Game found and cheat activated.\n\n";
		std::cout << "Press F1 to toggle ESP." << std::endl;
	}
	else {
		std::cout << "Process not found." << std::endl;
		return 0;
	}

	bool toggleESP = false;

	while (true) {

		if (GetAsyncKeyState(VK_F1) & 1) {
			toggleESP = !toggleESP;
		}

		if (toggleESP) {
			glowESP();
		}

		//run every 5ms
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}

	return 0;
}