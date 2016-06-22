//***********************************************************************************************
//***********************************************************************************************
//							Componet Framework of Engine 702
//
//									Game Timer
//
//*******************************  All Rights Reserved  *****************************************
//***********************************************************************************************
#include <windows.h>
#include "GameTimer.h"

GameTimer::GameTimer()
:	mSecondsPerCount(0.0),
	mDeltaTime(-1.0),
	mBaseTime(0),
	mPausedTime(0),
	mStopTime(0),// perhaps dont initialize? ---- seems to be set when stop() is called
	mPrevTime(0),
	mCurrTime(0),
	mStopped(false)
{
	__int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*) &countsPerSec);
	mSecondsPerCount = 1.0 / static_cast<double>(countsPerSec);//replaced (double)
}

float GameTimer::TotalTime() const
{
	if ( mStopped )
		return static_cast<float>((((mStopTime - mPausedTime) - mBaseTime) * mSecondsPerCount));//replaced (float)

	else
		return static_cast<float>((((mCurrTime - mPausedTime) - mBaseTime) * mSecondsPerCount));
}

float GameTimer::DeltaTime() const
{
	return static_cast<float>(mDeltaTime);
}

void GameTimer::Reset()
{
	__int64 currTime;
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currTime));

	mBaseTime = currTime;
	mPrevTime = currTime;
	mStopTime = 0;
	mStopped  = false;
}

void GameTimer::Start()
{
	__int64 startTime;
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&startTime));

	if ( mStopped )
	{
		mPausedTime += (startTime - mStopTime);

		mPrevTime = startTime; // for animation/ frametime reasons?
		mStopTime = 0;
		mStopped  = false;
	}
}

void GameTimer::Stop()
{
	if ( !mStopped )
	{
		__int64 currTime;
		QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currTime));

		mStopTime = currTime;
		mStopped  = true;
	}
}

void GameTimer::Tick()
{
	if ( mStopped )
	{
		mDeltaTime = 0.0;
		return;
	}

	__int64 currTime;
	QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currTime));
	
	mCurrTime  = currTime;
	mDeltaTime = mCurrTime - mPrevTime;
	mPrevTime  = mCurrTime;

	if ( mDeltaTime < 0.0)
	{
		mDeltaTime = 0.0;
	}
}

//***********************************************************************************************
//***********************************  Questions  ***********************************************
//1. why initialize secondspercount only to chang its value, also why the negative for deltatime
//2.
//3.
//***********************************************************************************************
//***********************************************************************************************