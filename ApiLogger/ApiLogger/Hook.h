#pragma once
#include<Windows.h>

#ifdef __cplusplus
extern "C" {
#endif
	void  PrintInfo(UINT_PTR Address);
	 void  Install();

	 void InstallHook(UINT_PTR Addr);

#ifdef __cplusplus
}
#endif

