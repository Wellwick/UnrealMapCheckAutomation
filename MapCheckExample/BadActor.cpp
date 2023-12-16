// ----------------------------------------------------------------------------
// Copyright (c) Wellwick
// Licensed under the MIT license.  
// See LICENSE.TXT in the project root for license information.
// ----------------------------------------------------------------------------

#include "BadActor.h"

#include "Logging/MessageLog.h"
#include "Misc/UObjectToken.h"


ABadActor::ABadActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

#if WITH_EDITOR
void ABadActor::CheckForErrors()
{
	Super::CheckForErrors();

	if (InBadState)
	{
		FMessageLog("MapCheck").Error() // Specify what type of message we need to report
			->AddToken(FUObjectToken::Create(this)) // Create a token to point at this actor
			->AddToken(FTextToken::Create(FText::FromString(GetActorLabel()))) // Make it clear what the actor is called in the scene
			->AddToken(FTextToken::Create(INVTEXT("Actor is in a bad state!"))); // Provide information about what the issue is
	}

	const double ZPos = GetActorLocation()[2];

	if (ZPos > 1000)
	{
		FMessageLog("MapCheck").Error()
			->AddToken(FUObjectToken::Create(this))
			->AddToken(FTextToken::Create(FText::FromString(GetActorLabel())))
			->AddToken(FTextToken::Create(INVTEXT("Actor is too high!")));
	}
	else if (ZPos > 750)
	{

		FMessageLog("MapCheck").Warning()
			->AddToken(FUObjectToken::Create(this))
			->AddToken(FTextToken::Create(FText::FromString(GetActorLabel())))
			->AddToken(FTextToken::Create(INVTEXT("Actor is nearly too high!")));
	}
	else if (ZPos < 0)
	{
		FMessageLog("MapCheck").Error()
			->AddToken(FUObjectToken::Create(this))
			->AddToken(FTextToken::Create(FText::FromString(GetActorLabel())))
			->AddToken(FTextToken::Create(INVTEXT("Actor is too low!")));
	}
}
#endif
