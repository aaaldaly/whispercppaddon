const path = require("path");
const fs = require("fs");

const { init_whisper, free, transcribe } = require(path.join(
  __dirname,
  "../../../build/Release/addon.node"
));
const { promisify } = require("util");

const transcribeAsync = promisify(transcribe);

const whisperParams = { // change the followig object to a type when moving to TypeScript
  language: "en",
  model: "/Users/daly/work/AI-agent/models/ggml-base.en.bin",
  fname_inp: "/Users/daly/work/AI-agent/ezyZip.wav",
  use_gpu: true,
  flash_attn: false,
  no_prints: true,
  comma_in_time: false,
  translate: false,
  no_timestamps: false,
  audio_ctx: 0,
  n_threads: 4,
  n_processors: 1,
  // pcmf32: []
};


console.log("whisperParams =", whisperParams);

async function main() {
  try {
    console.log("Initializing Whisper...");
    const result1 = init_whisper({
      language: whisperParams.language,
      model: whisperParams.model,
      use_gpu: whisperParams.use_gpu,
      flash_attn: whisperParams.flash_attn,
      no_prints: whisperParams.no_prints,
      no_timestamps: whisperParams.no_timestamps,
      audio_ctx: whisperParams.audio_ctx,
      comma_in_time: whisperParams.comma_in_time,
      n_threads: whisperParams.n_threads,
      n_processors: whisperParams.n_processors,
      diarize: whisperParams.diarize
    });
    console.log("Whisper initialized!", result1);

    console.log("Reading WAV file...");
    const pcmf32 = fs.readFileSync(whisperParams.fname_inp);

    console.log("Starting transcription...");
    const result = await transcribeAsync(pcmf32);

    console.log("Transcription result:", result);

    console.log("Freeing Whisper context...");
    await free();
    console.log("Whisper context freed!");
  } catch (error) {
    console.log("Error:", error);
  }
}

main().then(() => {
  console.log("Main function completed");
}).catch((error) => {
  console.log("Main function error:", error);
});
