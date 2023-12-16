// ----------------------------------------------------------------------------
// Copyright (c) Wellwick
// Licensed under the MIT license.  
// See LICENSE.TXT in the project root for license information.
// ----------------------------------------------------------------------------

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

DECLARE_LOG_CATEGORY_EXTERN(LogMapCheckAutomation, All, All)

class FMapCheckAutomation final : public IModuleInterface
{
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};

