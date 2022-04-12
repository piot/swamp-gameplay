/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Peter Bjorklund. All rights reserved.
 *  Licensed under the MIT License. See LICENSE in the project root for license information.
 *--------------------------------------------------------------------------------------------*/
#include <burst/burst_file_loader.h>
#include <clog/clog.h>
#include <swamp-dump/dump_ascii.h>
#include <swamp-gameplay/module.h>


const SwampScriptState* swampGameplayCallInit(SwampScriptScope* self)
{
    if (!self->staticCode->updateFn) {
        CLOG_ERROR("must have gameplay script loaded before calling init()");
        return 0;
    }

    const SwampFunc* initFunc = swampScriptScopeFindFunction(self, "init");
    if (initFunc == 0) {
        CLOG_SOFT_ERROR("gameplay: couldn't find an init function");
        return 0;
    }

    const bool verbose_flag = false;

    SwampResult result;
    result.expectedOctetSize = initFunc->returnOctetSize;

    SwampParameters parameters;
    parameters.parameterCount = 0;
    parameters.octetSize = 0;

    int runErr = swampRun(&result, &self->machineContext, initFunc, parameters, verbose_flag);
    if (runErr < 0) {
        CLOG_ERROR("could not run init")
        return 0;
    }

    SwampScriptState* state = &self->lastState;

    state->stateOctetCount = initFunc->returnOctetSize;
    state->state = self->machineContext.bp;
    state->align = initFunc->returnAlign;
    state->debugType = self->staticCode->returnType;

    return &self->lastState;
}


int swampGameplayUpdaterInit(SwampGameplayUpdater* self, SwampScriptScope* gameplayScriptScope, SwampGameplayStepId stepId)
{
//#define BUF_DUMP_COUNT (32*1024)
    //static char tempBuf[BUF_DUMP_COUNT];

    //CLOG_OUTPUT("setting state:%s", swampDumpToAsciiString(initialGameState->state, initialGameState->debugType, 0, tempBuf, BUF_DUMP_COUNT));

    self->cachedLastState.state = 0;
    self->debugCounter = 0;
    self->showDebugOutput = 0;
    self->gameplayScriptScope = gameplayScriptScope;
    self->stepId = stepId;

    return 1;
}

const SwampScriptState* swampGameplayUpdaterUpdate(SwampGameplayUpdater* self, SwampGameplayStepId inputForStateWithStepId, const SwampScriptState* playerInputState)
{
    if (!self->gameplayScriptScope->staticCode->updateFn) {
        return 0;
    }

    if (self->stepId != inputForStateWithStepId) {
        CLOG_ERROR("you can not feed some other input into the updater. expected %08X and received %08X", self->stepId, inputForStateWithStepId);
        return 0;
    }

#if 1
    swampScriptStateDebugOutput(playerInputState, "updater. input");
    swampScriptStateDebugOutput(&self->gameplayScriptScope->lastState, "updater. Last state");
#endif

    SwampMemoryPosition pos = 0;

    pos += self->gameplayScriptScope->staticCode->updateFn->returnOctetSize;

    swampMemoryPositionAlign(&pos, self->gameplayScriptScope->lastState.align);
    SwampMemoryPosition firstPos = pos;
    //swampScriptStateDebugOutput(self->gameplayState, "copy old game state");
    tc_memcpy_octets(self->gameplayScriptScope->machineContext.bp + pos, self->gameplayScriptScope->lastState.state,
                     self->gameplayScriptScope->lastState.stateOctetCount);
    pos += self->gameplayScriptScope->lastState.stateOctetCount;

    swampMemoryPositionAlign(&pos, playerInputState->align);
    //swampScriptStateDebugOutput(playerInputState, "copy player input state");;
    tc_memcpy_octets(self->gameplayScriptScope->machineContext.bp + pos, playerInputState->state,
                     playerInputState->stateOctetCount);

    pos += playerInputState->stateOctetCount;

    tc_memset_octets(self->gameplayScriptScope->machineContext.bp + 0, 0x1f,
                     self->gameplayScriptScope->staticCode->updateFn->returnOctetSize);

    SwampScriptExecuteInfo info;
    info.parameterCount = 2;
    info.parameterOctetCount = pos - firstPos;
    info.expectedReturnOctetSize = self->gameplayScriptScope->staticCode->updateFn->returnOctetSize;

    swampScriptScopeExecute(self->gameplayScriptScope, &info);

    self->stepId++;

    if (self->showDebugOutput) {
        swampScriptStateDebugOutput(&self->gameplayScriptScope->lastState, "swampGameplayUpdaterUpdate");
    }

    if ((self->debugCounter % 60) == 0) {
        //swampScriptStateDebugOutput(self->gameplayState, "game state");
    }
    self->debugCounter++;

    return &self->gameplayScriptScope->lastState;
}
