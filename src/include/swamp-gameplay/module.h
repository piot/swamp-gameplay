/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#ifndef swamp_gameplay_module_h
#define swamp_gameplay_module_h

#include <swamp-runtime/swamp.h>
#include <swamp-script-scope/script_scope.h>

struct ImprintMemory;
struct BurstFileLoader;

typedef uint32_t SwampGameplayStepId;
typedef uint32_t SwampGameplayStateId;

typedef struct SwampGameplayUpdater
{
    SwampScriptScope* gameplayScriptScope;
    SwampScriptState cachedLastState;
    bool showDebugOutput;
    uint8_t debugCounter;
    SwampGameplayStepId stepId;
} SwampGameplayUpdater;


int swampGameplayUpdaterInit(SwampGameplayUpdater* self, SwampScriptScope* gameplayScriptScope, SwampGameplayStepId stepId);
const SwampScriptState* swampGameplayUpdaterUpdate(SwampGameplayUpdater* self, SwampGameplayStepId inputForStateWithStepId, const SwampScriptState* input);

typedef struct swamp_gameplay_module {
    SwampScriptScope gameplay_script_scope;
    int time;
    const SwampScriptState* game_state;
    bool isInitialized;
} swamp_gameplay_module;

const SwampScriptState* swampGameplayCallInit(SwampScriptScope* self);

#endif
