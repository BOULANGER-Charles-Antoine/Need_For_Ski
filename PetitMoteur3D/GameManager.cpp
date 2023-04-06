#include "stdafx.h"
#include "GameManager.h"
#include "Time.h"
#include "MoteurWindows.h"
#include "Player.h"

void GameManager::Update()
{
	if (!onPause && IsInLevel())
	{
		if (!IsPlayerDead())
		{
			chrono += Time::GetDeltaTime();

			//recup vitesse du joueur
			auto& a = static_cast<physx::PxRigidDynamic&>(PM3D::CMoteurWindows::GetInstance().GetCurrentScene()->GetPlayer().lock()->GetActor());
			playerSpeed = a.getLinearVelocity().magnitude();

			if (playerSpeed < 15) {
				speedMultiplier = 1;
			}
			else if (playerSpeed < 24) {
				speedMultiplier = 2;
			}
			else {
				speedMultiplier = 4;
			}

			score += (speedMultiplier + freestyleMultiplier) * 1;
		}
	}
}

void GameManager::StartLevel()
{
	inLevel = true;
	onPause = false;

	chrono = 0.0;

	speedMultiplier = 1;
	freestyleMultiplier = 1;
	score = 0;
	playerSpeed = 0;
}

void GameManager::EndLevel()
{
	inLevel = false;
}

int GameManager::GetScore() const
{
	return score;
}


void GameManager::ResetFreestyleMultiplier()
{
	freestyleMultiplier = 0;
}

void GameManager::IncreaseFreestyleMultiplier(int amount)
{
	freestyleMultiplier += amount;
	if (freestyleMultiplier >=6) {
		freestyleMultiplier = 6;
	}
}

int GameManager::GetSpeedMultiplier() const
{
	return speedMultiplier;
}

int GameManager::GetFreestyleMultiplier() const
{
	return freestyleMultiplier;
}

int GameManager::GetCombo() const
{
	int c = freestyleMultiplier + speedMultiplier;
	c > 10 ? c = 10 : c;
	return c;
}

int GameManager::GetPlayerSpeed() const
{
	return playerSpeed;
}

double GameManager::GetChrono() const
{
	return chrono;
}

bool GameManager::IsPaused() const
{
	return onPause;
}

void GameManager::SetPause(bool pause)
{
	this->onPause = pause;
}

bool GameManager::IsInLevel() const
{
	return inLevel;
}

bool GameManager::IsPlayerDead() const
{
	if (!IsInLevel()) return false;

	std::weak_ptr<Player> player = PM3D::CMoteurWindows::GetInstance().GetCurrentScene()->GetPlayer();
	if (player.expired()) return true;

	return player.lock()->IsDead();
}
