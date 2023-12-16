// ----------------------------------------------------------------------------
// Copyright (c) Wellwick
// Licensed under the MIT license.  
// See LICENSE.TXT in the project root for license information.
// ----------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BadActor.generated.h"

UCLASS()
class ABadActor : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	ABadActor();

#if WITH_EDITOR
	virtual void CheckForErrors() override;
#endif

	/** A property which should never be true! */
	UPROPERTY(EditAnywhere)
	bool InBadState = false;
};
