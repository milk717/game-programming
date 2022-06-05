#include "DemoApp.h"
#include "SoundManager.h"

BOOL DemoApp::SetVolume(LPDIRECTSOUNDBUFFER lpDSBuffer, LONG lVolume) {
	//lpDSBuffer->SetVolume(DSBVOLUME_MIN);
	if (!lpDSBuffer) return FALSE; //jong
	if (FAILED(lpDSBuffer->SetVolume(DSVOLUME_TO_DB(lVolume))))
		return FALSE;

	return TRUE;
}

BOOL DemoApp::addSound(HWND hWnd) {
	//SOUND
	soundManager = new CSoundManager;
	if (!soundManager->init(hWnd))
		return FALSE;

	// 사운드 파일을 추가함 (id=0부터 시작함)
	int id;

	if (!soundManager->add(L"./sound/jump.wav", &id))
		return FALSE;
	if (!soundManager->add(L"./sound/background.wav", &id))
		return FALSE;
	if (!soundManager->add(L"./sound/gameover.wav", &id))
		return FALSE;
	if (!soundManager->add(L"./sound/clear.wav", &id))
		return FALSE;
	return true;
}