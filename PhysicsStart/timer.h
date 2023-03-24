#pragma once
class Timer {
public:
	enum TimerSetup
	{
		RestartOnEnd,
		Stop
	};

	float duration, elapsed = 0;
	bool isFinished = false;
	bool isStarted = false;
	TimerSetup setup;
	void Update(float deltatime) {
		if (isStarted) {
			elapsed += deltatime;
		}
		if (duration - elapsed <= 0) {
			isFinished = true;
		}
		if (isFinished) {
			switch (setup)
			{
			case Timer::RestartOnEnd:
				elapsed = 0;
				break;
			case Timer::Stop:
				//isStarted = false;
				elapsed = 0;
				break;
			default:
				break;
			}
		}
	}

	Timer(float _duration, TimerSetup setup) {
		duration = _duration;
		this->setup = setup;
	}
};