#pragma once
#include "framework.h"

#define NLPLAYER_STREAMTYPE_STREAM 0

void NLPlayer_Init(HWND hWnd);

class NLPlayerMusic
{
private:
	DWORD handle = NULL;
	LPCWSTR lpURL;
	BOOL bPlaying;

	DWORD _Open(LPCWSTR url, INT* streamType);
public:
	void Free();
	BOOL Load(LPCWSTR url);
	DWORD GetHandle();
	BOOL IsLoad();
	void Play();
	void Pause();
	BOOL IsPlaying();
	DOUBLE GetLength();
	DOUBLE GetPos();
	void SetPos(DOUBLE fPos);
	void SetVolume(INT nVolume);
};
