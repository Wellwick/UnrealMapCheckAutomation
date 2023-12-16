// ----------------------------------------------------------------------------
// Copyright (c) Wellwick
// Licensed under the MIT license.  
// See LICENSE.TXT in the project root for license information.
// ----------------------------------------------------------------------------


#include "MapCheckCommandlet.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Editor/EditorEngine.h"
#include "GenericPlatform/GenericPlatformOutputDevices.h"
#include "JsonObjectConverter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Misc/FeedbackContext.h"
#include "Misc/FileHelper.h"
#include "Subsystems/AssetEditorSubsystem.h"

#if WITH_EDITOR
int32 UMapCheckCommandlet::Main(const FString& Params)
{
	TArray<FString> Tokens;
	TArray<FString> Switches;
	TMap<FString, FString> ParamMap;
	ParseCommandLine(*Params, Tokens, Switches, ParamMap);

	// Whether a json file should be output (and where)
	bool bProduceJSON = ParamMap.Contains("jsonoutput");
	// Whether a produced json file should be indented (adds plenty of whitespace)
	bool bPrettyJSON = Switches.Contains("prettify");
	// Whether maps that pass map check with no issues should be included in the report
	bool bReportSuccess = Switches.Contains("reportsuccess");

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FAssetData> AssetData;
	AssetRegistryModule.Get().GetAssetsByClass(UWorld::StaticClass()->GetClassPathName(), AssetData);

	FMapCheckCollection MapCheckCollection;

	for (const FAssetData& Asset : AssetData)
	{
		FWorldAssetMapCheck WorldCheck = RunMapCheck(Asset);
		if (bReportSuccess || WorldCheck.HasIssues())
		{
			MapCheckCollection.WorldAssets.Add(MoveTemp(WorldCheck));
		}
	}

	if (bProduceJSON)
	{
		FString JsonString;
		if (FJsonObjectConverter::UStructToJsonObjectString(MapCheckCollection, JsonString, 0, 0, 0, nullptr, bPrettyJSON))
		{
			FString Path = FPaths::ProjectSavedDir() / *ParamMap.Find("jsonoutput");
			FFileHelper::SaveStringToFile(JsonString, *Path);
		}
	}

    return 0;
}

EDataValidationResult UMapCheckCommandlet::RunMapCheck(const FAssetData& WorldAsset, FDataValidationContext& ValidationContext) const
{
	if (!WorldAsset.IsValid())
	{
		ValidationContext.AddError(FText::Format(INVTEXT("Could not access data asset {0}"), FText::FromString(WorldAsset.GetFullName())));
		return EDataValidationResult::Invalid;
	}

	EDataValidationResult Result = EDataValidationResult::Valid;
	if (UWorld* World = Cast<UWorld>(WorldAsset.GetAsset()))
	{
		if (UEditorEngine* Editor = Cast<UEditorEngine>(GEngine))
		{
			if (UAssetEditorSubsystem* AssetEditorSubsystem = Editor->GetEditorSubsystem<UAssetEditorSubsystem>())
			{
				// Load the map how we would if working on it in UnrealEditor
				AssetEditorSubsystem->OpenEditorForAsset(WorldAsset.GetSoftObjectPath());

				// Unfortunately, map check dumps out to the console log, so we're going to have to takeover the
				// context, temporarily
				FFeedbackContext* GWarnOld = GWarn;
				FFeedbackContext GWarnNew;
				GWarn = &GWarnNew;

				// Capture the message log scope for the MapCheck we are about to access
				FMessageLog MapCheckLog("MapCheck");
				/**
				 * The Map_Check method is not publically exposed so we need to pass the request as a command
				 * to the engine. We also need to specify an FOutputDevice, however this traditionally goes
				 * unused by most Map Check functionality.
				 */
				Editor->HandleMapCommand(TEXT("CHECK"), *FGenericPlatformOutputDevices::GetLog(), World);

				TArray<FString> Errors;
				GWarn->GetErrors(Errors);

				for (int32 Index = 0; Index < Errors.Num(); ++Index)
				{
					ValidationContext.AddError(FText::FromString(Errors[Index]));
				}

				TArray<FString> Warnings;
				GWarn->GetWarnings(Warnings);
				for (int32 Index = 0; Index < Warnings.Num(); ++Index)
				{
					ValidationContext.AddWarning(FText::FromString(Warnings[Index]));
				}

				// Sometimes there can be a discrepenancy between the amount of errors and warnings the FeedbackContext has
				const int32 MapCheckErrors = MapCheckLog.NumMessages(EMessageSeverity::Type::Error);
				const int32 MapCheckWarnings = MapCheckLog.NumMessages(EMessageSeverity::Type::Warning);

				if (Errors.Num() < MapCheckErrors || Warnings.Num() < MapCheckWarnings)
				{
					ValidationContext.AddWarning(INVTEXT("There may be additional MapCheck error or warnings, recommended to check locally!"));
				}

				// Return to the old global reporting
				GWarn = GWarnOld;
			}
			else
			{
				ValidationContext.AddError(INVTEXT("Could not access AssetEditorSubsystem to perform MapCheck"));
			}
		}
		else
		{
			ValidationContext.AddError(INVTEXT("Could not access engine to perform MapCheck"));
		}
	}
	else
	{
		ValidationContext.AddError(FText::Format(INVTEXT("Could not access data asset {0}"), FText::FromString(WorldAsset.GetFullName())));
	}

	return Result;
}

EDataValidationResult UMapCheckCommandlet::RunMapCheck(const FAssetData& WorldAsset, TArray<FText>& ValidationErrors, TArray<FText>& ValidationWarnings) const
{
	FDataValidationContext Context;
	EDataValidationResult Result = RunMapCheck(WorldAsset, Context);

	// Populate the error and warning arrays
	for (const FDataValidationContext::FIssue& Issue : Context.GetIssues())
	{
		if (Issue.Severity == EMessageSeverity::Error || Issue.Severity == EMessageSeverity::CriticalError)
		{
			ValidationErrors.Add(Issue.Message);
			Result = CombineDataValidationResults(Result, EDataValidationResult::Invalid);
		}
		else
		{
			// Anything which has been flagged as another severity we will treat as a warning
			ValidationWarnings.Add(Issue.Message);
		}
	}

	return Result;
}

FWorldAssetMapCheck UMapCheckCommandlet::RunMapCheck(const FAssetData& WorldAsset)
{
	FWorldAssetMapCheck WorldCheck;
	WorldCheck.Name = WorldAsset.AssetName.ToString();
	WorldCheck.AssetPath = GetAssetPath(WorldAsset);
	WorldCheck.Validity = RunMapCheck(WorldAsset, WorldCheck.Errors, WorldCheck.Warnings);
	return WorldCheck;
}

FString UMapCheckCommandlet::GetAssetPath(const FAssetData& WorldAsset)
{
	if (UObject* World = WorldAsset.GetAsset())
	{
		return UKismetSystemLibrary::GetSystemPath(World);
	}
	
	return FString();
}

#endif // WITH_EDITOR