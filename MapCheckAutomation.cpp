// ----------------------------------------------------------------------------
// Copyright (c) Wellwick
// Licensed under the MIT license.  
// See LICENSE.TXT in the project root for license information.
// ----------------------------------------------------------------------------

#include "MapCheckAutomation.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_GAME_MODULE(FMapCheckAutomation, MapCheckAutomation);

DEFINE_LOG_CATEGORY(LogMapCheckAutomation)

void FMapCheckAutomation::StartupModule()
{
    UE_LOG(LogMapCheckAutomation, Log, TEXT("MapCheckAutomation: Log Started"));
}

void FMapCheckAutomation::ShutdownModule()
{
    UE_LOG(LogMapCheckAutomation, Log, TEXT("MapCheckAutomation: Log Ended"));
}
