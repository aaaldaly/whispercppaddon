#pragma once
#include <napi.h>
#include "whisperoperator.h"

class AsyncWorker : public Napi::AsyncWorker
{
public:
    AsyncWorker(Napi::Function &callback, WhisperOperator &whisperOp, std::vector<float> &audioData)
        : Napi::AsyncWorker(callback), whisperOp(whisperOp), audioData(audioData) {}

protected:
    void Execute() override;
    void OnOK() override;

private:
    WhisperOperator &whisperOp;
    std::string transcriptResult;
    std::vector<float> audioData;
};