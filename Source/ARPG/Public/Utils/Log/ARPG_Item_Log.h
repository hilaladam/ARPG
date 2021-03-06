// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <LogMacros.h>

// #include "CoreMinimal.h"
// #include "UObject/NoExportTypes.h"
// #include "ItemUtility.generated.h"

/**
 * 
 */
DECLARE_LOG_CATEGORY_EXTERN(ARPG_Item_Log, Log, All);

#define Item_Display_LOG(FMT, ...) UE_LOG(ARPG_Item_Log, Display, TEXT(FMT), ##__VA_ARGS__)
#define Item_Warning_LOG(FMT, ...) UE_LOG(ARPG_Item_Log, Warning, TEXT(FMT), ##__VA_ARGS__)
#define Item_Error_Log(FMT, ...) UE_LOG(ARPG_Item_Log, Error, TEXT(FMT), ##__VA_ARGS__)