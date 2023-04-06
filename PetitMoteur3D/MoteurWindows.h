#pragma once
#include "Moteur.h"
#include "DispositifD3D11.h"

namespace PM3D
{
#define MAX_LOADSTRING 100

	class CMoteurWindows final : public CMoteur<CMoteurWindows, CDispositifD3D11>
	{
	public:
		void SetWindowsAppInstance(HINSTANCE hInstance);
		HWND getHMainWnd() const { return hMainWnd; };

	private:
		ATOM MyRegisterClass(HINSTANCE hInstance);
		bool InitAppInstance();
		int Show();

		// Les fonctions sp�cifiques
		virtual int InitialisationsSpecific() override;
		virtual bool RunSpecific() override;
		virtual void CloseSpecific() override;
		virtual CDispositifD3D11* CreationDispositifSpecific(const CDS_MODE cdsMode) override;
		virtual void BeginRenderSceneSpecific() override;
		virtual void EndRenderSceneSpecific() override;

		// Fonctions "Callback" -- Doivent �tre statiques
		static LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
		static INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

		HACCEL hAccelTable;						// handle Windows de la table des acc�l�rateurs
		static HINSTANCE hAppInstance;			// handle Windows de l'instance actuelle de l'application
		HWND hMainWnd;							// handle Windows de la fen�tre principale
		TCHAR szWindowClass[MAX_LOADSTRING];	// le nom de la classe de fen�tre principale
	};

} // namespace PM3D
