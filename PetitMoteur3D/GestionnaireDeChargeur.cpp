#include "stdafx.h"
#include "GestionnaireDeChargeur.h"

using namespace std;

namespace PM3D
{
	void CGestionnaireDeChargeurOBJ::AddChargeur(string nomChargeur, CParametresChargement params)
	{
		chargeurs[nomChargeur] = make_unique<CChargeurOBJ>();
		chargeurs[nomChargeur]->Chargement(params);
	}

	CChargeurOBJ& CGestionnaireDeChargeurOBJ::GetChargeur(CParametresChargement params)
	{
		string nom = params.NomChemin + params.NomFichier;

		if (!chargeurs.contains(nom))
		{
			AddChargeur(nom, params);
		}

		return *chargeurs[nom];
	}
}


