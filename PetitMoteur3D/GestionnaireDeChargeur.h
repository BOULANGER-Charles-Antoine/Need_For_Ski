#pragma once
#include "ChargeurOBJ.h"
#include "Singleton.h"
#include <memory>
#include <map>
#include <string>

namespace PM3D
{
	class CGestionnaireDeChargeurOBJ : public CSingleton<CGestionnaireDeChargeurOBJ>
	{
		std::map<std::string, std::unique_ptr<CChargeurOBJ>> chargeurs;

		void AddChargeur(std::string nomChargeur, CParametresChargement params);

	public:
		CChargeurOBJ& GetChargeur(CParametresChargement params);
	};
}	