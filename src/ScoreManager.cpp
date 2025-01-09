#include <iostream>
#include "ScoreManager.h"

ScoreManager::ScoreManager() :
	m_Score{ 0 }
{
}

void ScoreManager::AddScore(int score)
{
	m_Score += score; 

	std::cout << "---" << std::endl;
	std::cout << "The Player's score is: " << m_Score << std::endl;
	std::cout << "---" << std::endl; 
}

int ScoreManager::GetScore()
{
	return m_Score;
}