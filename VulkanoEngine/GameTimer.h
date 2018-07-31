#pragma once
class GameTimer final
{
public:
	GameTimer(void);
	~GameTimer(void);

	double GetElapsedSeconds() const { return m_ElapsedSeconds; }
	double GetTotalSeconds() const { return m_TotalSeconds; }
	float GetElapsedSecondsF() const { return (float)m_ElapsedSeconds; }
	float GetTotalSecondsF() const { return (float)m_TotalSeconds; }
	int GetFPS() const { return m_FPS; }
	void ForceElapsedUpperbound(bool force, float upperBound = 0.03f) { m_ForceElapsedUpperBound = force; m_ElapsedUpperBound = upperBound; }
	bool IsRunning() const { return !m_IsStopped; }

	void Reset();
	void Start();
	void Stop();
	void Update();

	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	GameTimer(const GameTimer& t) = delete;
	GameTimer& operator=(const GameTimer& t) = delete;

private:

	double m_ElapsedSeconds;
	double m_TotalSeconds;

	double m_BaseTime;
	double m_PausedTime;
	double m_StopTime;
	double m_PrevTime;
	double m_CurrTime;

	bool m_IsStopped;

	int m_FPS;
	double m_FpsTimer;
	int m_FpsCount;
	bool m_ForceElapsedUpperBound;
	double m_ElapsedUpperBound;
};

