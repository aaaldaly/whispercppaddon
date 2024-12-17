#include "asyncworker.h"
#include <cstdio>

void AsyncWorker::Execute()
{

    // Run transcription
    transcriptResult = whisperOp.transcript(audioData);
    if (transcriptResult == "-")
    {
        SetError("Failed to run transcription");
    }
}

void AsyncWorker::OnOK()
{
    Napi::HandleScope scope(Env());
    Callback().Call({Env().Null(), Napi::String::New(Env(), transcriptResult)});
}