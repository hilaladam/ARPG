﻿// Fill out your copyright notice in the Description page of Project Settings.

#include "ARPG_Editor.h"
#include <PropertyEditorModule.h>
#include "XD_PropertyCustomizationEx.h"
#include "ARPG_Item_Customization.h"
#include "AssetToolsModule.h"
#include "ISettingsModule.h"
#include "ISettingsSection.h"
#include "ARPG_EditorSettings.h"
#include "XD_CampConfig_Customization.h"
#include "ARPG_NavPath.h"
#include "NavPathVisualizer.h"

#define LOCTEXT_NAMESPACE "ARPG_Editor"

uint32 FARPG_EditorModule::ARPG_AssetCategory;

void FARPG_EditorModule::StartupModule()
{
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

		RegisterCustomProperty(struct FARPG_Item, FARPG_Item_Customization);
		RegisterCustomProperty(struct FARPG_CampConfig, FXD_CampConfig_Customization);
	}

	{
		IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
		ARPG_AssetCategory = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("ARPG")), LOCTEXT("ARPG", "ARPG"));
	}

	{
		ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");

		if (SettingsModule != nullptr)
		{
			ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings("Project", "Plugins", "XD_UnrealLibrarySettings",
				LOCTEXT("ARPG_EditorSettings", "ARPG_EditorSettings"),
				LOCTEXT("ARPG_EditorSettingsDescription", "Configure the ARPG_EditorSettings plug-in."),
				GetMutableDefault<UARPG_EditorSettings>()
			);
		}
	}

	{
		TSharedPtr<FNavPathVisualizer> NavPathVisualizer = MakeShareable(new FNavPathVisualizer);
		GUnrealEd->RegisterComponentVisualizer(UARPG_NavPathVisualHelper::StaticClass()->GetFName(), NavPathVisualizer);
		NavPathVisualizer->OnRegister();
	}
}


void FARPG_EditorModule::ShutdownModule()
{
	if (GUnrealEd)
	{
		GUnrealEd->UnregisterComponentVisualizer(UARPG_NavPathVisualHelper::StaticClass()->GetFName());
	}
}

IMPLEMENT_MODULE(FARPG_EditorModule, ARPG_Editor);

#undef LOCTEXT_NAMESPACE