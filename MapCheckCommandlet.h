// ----------------------------------------------------------------------------
// Copyright (c) Wellwick
// Licensed under the MIT license.  
// See LICENSE.TXT in the project root for license information.
// ----------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "Misc/DataValidation.h"
#include "MapCheckCommandlet.generated.h"

#if WITH_EDITOR
/**
 * Commandlet that exposes Map Check functionality via commandlet logic. Some
 * functionality is exposed to blueprint which also makes it available to
 * python scripts, so you can filter either by commandlet argument or more
 * complex logic within scripts.
 * 
 * Note this should only ever be run with the Unreal editor!
 */
UCLASS()
class UMapCheckCommandlet : public UCommandlet
{
	GENERATED_BODY()

	/** Execution when this commandlet is run. Other methods are publically exposed for scripting purposes. */
	virtual int32 Main(const FString& Params) override;

	/** Runs map check for an asset. Returns validation result and data validation context. */
	UFUNCTION(BlueprintCallable)
	EDataValidationResult RunMapCheck(const FAssetData& WorldAsset, FDataValidationContext& OutValidationContext) const;

	/** Runs map check for an asset. Returns validation result and text arrays for errors and warnings. */
	UFUNCTION(BlueprintCallable)
	EDataValidationResult RunMapCheck(const FAssetData& WorldAsset, TArray<FText>& OutValidationErrors, TArray<FText>& OutValidationWarnings) const;

	/** Runs map check for an asset. Returns a FWorldAssetMapCheck which effectively represents a report on a map. */
	FWorldAssetMapCheck RunMapCheck(const FAssetData& WorldAsset);

	/** Helper function to retrieve asset path for an asset. */
	UFUNCTION(BlueprintCallable)
	static FString GetAssetPath(const FAssetData& WorldAsset);
};
#endif // WITH_EDITOR

/**
 * Structure to capture map check information that can be exported to JSON
 */
USTRUCT()
struct FWorldAssetMapCheck
{
	GENERATED_BODY()

	UPROPERTY()
	FString Name = "";

	UPROPERTY()
	FString AssetPath = "";

	UPROPERTY()
	EDataValidationResult Validity = EDataValidationResult::NotValidated;

	UPROPERTY()
	TArray<FText> Errors;

	UPROPERTY()
	TArray<FText> Warnings;

	/** Returns true if there are any errors or warnings. */
	bool HasIssues() const { return !Errors.IsEmpty() || !Warnings.IsEmpty(); };
};

/**
 * Wrapper struct to contain many world asset map checks
 */
USTRUCT()
struct FMapCheckCollection
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FWorldAssetMapCheck> WorldAssets;
};
