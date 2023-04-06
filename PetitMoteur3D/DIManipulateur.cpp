#include "stdafx.h"
#include "DIManipulateur.h"
#include "util.h"
#include "resource.h"
#include <chrono>

namespace PM3D
{
	bool CDIManipulateur::bDejaInit = false;

	bool CDIManipulateur::Init(HINSTANCE hInstance, HWND hWnd)
	{
		// Un seul objet DirectInput est permis
		if (!bDejaInit)
		{
			// Objet DirectInput
			DXEssayer(DirectInput8Create(hInstance,
				DIRECTINPUT_VERSION,
				IID_IDirectInput8,
				(void**)&pDirectInput,
				nullptr), ERREUR_CREATION_DIRECTINPUT);

			// Objet Clavier
			DXEssayer(pDirectInput->CreateDevice(GUID_SysKeyboard,
				&pClavier,
				nullptr),
				ERREUR_CREATION_CLAVIER);

			DXEssayer(pClavier->SetDataFormat(&c_dfDIKeyboard),
				ERREUR_CREATION_FORMAT_CLAVIER);

			pClavier->SetCooperativeLevel(hWnd,
				DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

			pClavier->Acquire();

			// Objet Souris
			DXEssayer(pDirectInput->CreateDevice(GUID_SysMouse, &pSouris, nullptr),
				ERREUR_CREATION_SOURIS);

			DXEssayer(pSouris->SetDataFormat(&c_dfDIMouse), ERREUR_CREATION_FORMAT_SOURIS);

			pSouris->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

			pSouris->Acquire();

			// Objet Joystick

			bDejaInit = true;
		}

		return true;
	}

	void CDIManipulateur::Update()
	{
		StatutClavier();
		SaisirEtatSouris();
	}

	void CDIManipulateur::Close()
	{
		if (pClavier)
		{
			pClavier->Unacquire();
			pClavier->Release();
			pClavier = nullptr;
		}

		if (pSouris)
		{
			pSouris->Unacquire();
			pSouris->Release();
			pSouris = nullptr;
		}

		if (pJoystick)
		{
			pJoystick->Unacquire();
			pJoystick->Release();
			pJoystick = nullptr;
		}

		if (pDirectInput)
		{
			pDirectInput->Release();
			pClavier = nullptr;
		}

		bDejaInit = false;
	}

	bool CDIManipulateur::ToucheAppuyee(UINT touche) const noexcept
	{
		assert(bDejaInit);
		return (tamponClavier[touche] & 0x80);
	}

	bool CDIManipulateur::ToucheEnfoncee(UINT touche) const noexcept
	{
		assert(bDejaInit);
		return (tamponClavier[touche] & 0x80) && !(previousClavier[touche] & 0x80);
	}

	bool CDIManipulateur::ToucheRelachee(UINT touche) const noexcept
	{
		assert(bDejaInit);
		return !(tamponClavier[touche] & 0x80) && (previousClavier[touche] & 0x80);
	}

	void CDIManipulateur::StatutClavier()
	{
		assert(bDejaInit);

		if (pClavier->Poll())
		{
			pClavier->Acquire();
		}

		std::copy(std::begin(tamponClavier), std::end(tamponClavier), std::begin(previousClavier));
		pClavier->GetDeviceState(sizeof(tamponClavier), (void*)&tamponClavier);
	}

	void CDIManipulateur::SaisirEtatSouris()
	{
		assert(bDejaInit);

		if (pSouris->Poll())
		{
			pSouris->Acquire();
		}

		previousMouseState = mouseState;
		pSouris->GetDeviceState(sizeof(mouseState), (void*)&mouseState);
	}

	const POINT CDIManipulateur::GetMousePosition(HWND hWnd) const noexcept
	{
		static auto start = std::chrono::system_clock::now();
		const auto end = std::chrono::system_clock::now();
		POINT p{};

		if (std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count() >= 500 && (EtatSouris().rgbButtons[0] & 0x80))
		{
			start = std::chrono::system_clock::now();
			GetCursorPos(&p);
			ScreenToClient(hWnd, &p);
		}
		
		return p;
	}

} // namespace PM3D
