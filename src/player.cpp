#include "player.h"

void NLPlayer_Init(HWND hWnd)
{
	BASS_Init(-1, 44100, 0, hWnd, NULL);
}

DWORD NLPlayerMusic::_Open(LPCWSTR url, INT* streamType)
{
	DWORD handle = NULL;
	DWORD flags =  BASS_UNICODE;
	handle = BASS_StreamCreateFile(FALSE, url, 0, 0, flags);
	return handle;
}

void NLPlayerMusic::Free()
{
	BASS_ChannelFree(handle);
	NL_MemFree((LPVOID)lpURL);
	handle = NULL;
}

BOOL NLPlayerMusic::Load(LPCWSTR url)
{
	INT streamType = 0;
	handle = _Open(url, &streamType);
	if (handle)
	{
		lpURL = StrDupW(url);
	}
	return handle != NULL;
}

DWORD NLPlayerMusic::GetHandle()
{
	return handle;
}

BOOL NLPlayerMusic::IsLoad()
{
	BOOL ret = FALSE;
	ret = handle != NULL;
	return ret;
}

void NLPlayerMusic::Play()
{
	BASS_ChannelPlay(handle, FALSE);
	bPlaying = TRUE;
}

void NLPlayerMusic::Pause()
{
	if (bPlaying)
	{
		BASS_ChannelPause(handle);
		bPlaying = FALSE;
	}
	else
	{
		BASS_ChannelPlay(handle, FALSE);
		bPlaying = TRUE;
	}
}

BOOL NLPlayerMusic::IsPlaying()
{
	BOOL ret = FALSE;
	ret = bPlaying;
	return ret;
}

void NLPlayerMusic::SetVolume(INT nVolume)
{
	BASS_ChannelSetAttribute(handle, BASS_ATTRIB_VOL, (FLOAT)nVolume / 100);
}

DOUBLE NLPlayerMusic::GetLength()
{
	DOUBLE ret = 0;
	ret = BASS_ChannelBytes2Seconds(handle, BASS_ChannelGetLength(handle, BASS_POS_BYTE));
	return ret;
}

DOUBLE NLPlayerMusic::GetPos()
{
	DOUBLE ret = 0;
	ret = BASS_ChannelBytes2Seconds(handle, BASS_ChannelGetPosition(handle, BASS_POS_BYTE));
	return ret;
}

void NLPlayerMusic::SetPos(DOUBLE fPos)
{
	BASS_ChannelSetPosition(handle, BASS_ChannelSeconds2Bytes(handle, fPos), BASS_POS_BYTE);
}
