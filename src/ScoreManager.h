
class ScoreManager
{
public:
	static ScoreManager& GetInstance()
	{
		static ScoreManager instance{};
		return instance;
	}

	virtual ~ScoreManager() = default;

	ScoreManager(const ScoreManager& other) = delete;
	ScoreManager& operator=(const ScoreManager& other) = delete;
	ScoreManager(ScoreManager&& other) = delete;
	ScoreManager& operator=(ScoreManager&& other) = delete;

	void AddScore(int score);
	int GetScore();

protected:
	ScoreManager();

private:
	int m_Score;
};