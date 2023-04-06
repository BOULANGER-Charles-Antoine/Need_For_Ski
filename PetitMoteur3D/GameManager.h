#pragma once
#include "Manager.h"

class GameManager : public Manager<GameManager>
{
	bool inLevel = false;
	int playerSpeed;
	bool onPause = false;

	double chrono = 0.0;

	int speedMultiplier = 1;
	int freestyleMultiplier = 0;
	int combo = 1;
	int score = 0;
	

public:
	void Start() override { /*vide*/ };
	void Update() override;
	void Close() override { /*vide*/ };

	void StartLevel();
	void EndLevel();

	//----- Score -----//
	void ResetFreestyleMultiplier();
	void IncreaseFreestyleMultiplier(int amount);

	//----- Getter & Setter -----//
	int GetScore() const;

	int GetSpeedMultiplier() const;
	int GetFreestyleMultiplier() const;
	int GetCombo() const;

	int GetPlayerSpeed() const;

	double GetChrono() const;

	bool IsPaused() const;
	void SetPause(bool pause);

	bool IsInLevel() const;
	bool IsPlayerDead() const;
};

