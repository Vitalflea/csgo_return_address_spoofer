#include <Windows.h>

#include <utility>
#include <iostream>

std::uintptr_t g_client_base{ 0 };

class IBaseClientDLL {
private:
	enum class FunctionIndexes : std::uint8_t {

		GET_ALL_CLASSES = 8
	};

	const std::uintptr_t Get(const FunctionIndexes& i) {

		std::uintptr_t address{ 0 };

		auto table = *reinterpret_cast<std::uintptr_t**>(this);
		if (table != nullptr) {

			address = table[std::to_underlying(i)];
		}

		return address;
	}

public:

	void* GetAllClasses() {

		static auto gadget = g_client_base + 0xbb908;
		static auto function = Get(FunctionIndexes::GET_ALL_CLASSES);

		void* client_class{ nullptr };

		_asm {

			xor eax, eax
			push reta
			mov ecx, this
			push gadget
			jmp function
			reta :
			mov client_class, eax
		}

		return client_class;
	}
};


void Attach(const HINSTANCE& instance) {

	AllocConsole();

	FILE* output{ nullptr };
	freopen_s(&output, "CONOUT$", "w", stdout);
	freopen_s(&output, "CONIN$", "r", stdin);

	g_client_base = reinterpret_cast<std::uintptr_t>(GetModuleHandle("client.dll"));

	auto vclient018 = reinterpret_cast<IBaseClientDLL*>(g_client_base + 0x4DC9898);
	if (vclient018 != nullptr) {

		if (vclient018->GetAllClasses() != nullptr) {

			std::cout << "Hello, World!" << '\n';
		}
	}
}

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID /*p_pReserved*/) {

	if (reason == DLL_PROCESS_ATTACH) {

		DisableThreadLibraryCalls(instance);
		Attach(instance);
	}

	return TRUE;
}