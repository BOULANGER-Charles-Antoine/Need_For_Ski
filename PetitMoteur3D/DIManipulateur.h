#pragma once
#include "Manager.h"

namespace PM3D
{
	class CDIManipulateur : public Manager<CDIManipulateur>
	{
		IDirectInput8* pDirectInput = nullptr;
		IDirectInputDevice8* pClavier = nullptr;
		IDirectInputDevice8* pSouris = nullptr;
		IDirectInputDevice8* pJoystick = nullptr;

		static bool bDejaInit;

		char tamponClavier[256];
		char previousClavier[256];

		DIMOUSESTATE previousMouseState;
		DIMOUSESTATE mouseState;

	public:
		bool Init(HINSTANCE hInstance, HWND hWnd);
		void Start() override { }
		void Update() override;
		void Close() override;

		bool ToucheEnfoncee(UINT touche) const noexcept;
		bool ToucheAppuyee(UINT touche) const noexcept;
		bool ToucheRelachee(UINT touche) const noexcept;

		const DIMOUSESTATE& EtatSouris() const noexcept { return mouseState; }
		const POINT GetMousePosition(HWND hWnd) const noexcept;

	private:
		void StatutClavier();
		void SaisirEtatSouris();
	};

} // namespace PM3D

