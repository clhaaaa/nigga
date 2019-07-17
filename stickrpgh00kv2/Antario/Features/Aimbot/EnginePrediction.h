#pragma once

class EnginePrediction : public singleton<EnginePrediction> // singleton ftw
{
private:

	int iTickBaseBackup;
	int iFlagsBackup;
	int iButtonsBackup;
	int* m_pPredictionRandomSeed;
	int* m_pSetPredictionPlayer;

public:
	void Start();
	void End();
};
