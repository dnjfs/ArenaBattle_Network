// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

// 서버/클라이언트 로그 구분, GPlayInEditorID: 에디터 상 클라이언트 번호
#define LOG_NETMODEINFO ((GetNetMode() == ENetMode::NM_Client) ? *FString::Printf(TEXT("CLIENT%d"), static_cast<int32>(GPlayInEditorID)) : ((GetNetMode() == ENetMode::NM_Standalone) ? TEXT("STANDALONE") : TEXT("SERVER"))) 

// 로그 호출 시점의 함수 출력
#define LOG_CALLINFO ANSI_TO_TCHAR(__FUNCTION__)

// 커스텀 로그 매크로
#define AB_LOG(LogCat, Verbosity, Format, ...) UE_LOG(LogCat, Verbosity, TEXT("[%s] %s %s"), LOG_NETMODEINFO, LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))

DECLARE_LOG_CATEGORY_EXTERN(LogABNetwork, Log, All);