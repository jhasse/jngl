#include "../engine.hpp"

#include "../constants.hpp"
#include "../Stream.hpp"

#include <AudioUnit/AudioUnit.h>
#include <cassert>
#include <string>

namespace jngl::audio {

constexpr AudioUnitElement OUTPUT_ELEMENT = 0;

struct engine::Impl {
	Impl(std::shared_ptr<Stream> output) : output(std::move(output)) {
		AudioComponentDescription desc{};
		desc.componentType = kAudioUnitType_Output;
		desc.componentSubType = kAudioUnitSubType_RemoteIO;
		desc.componentManufacturer = kAudioUnitManufacturer_Apple;
		desc.componentFlags = 0;
		desc.componentFlagsMask = 0;

		AudioComponent comp = AudioComponentFindNext(nullptr, &desc);
		if (comp == nullptr) {
			throw std::runtime_error("Could not find audio component");
		}

		OSStatus err = AudioComponentInstanceNew(comp, &audioUnit);
		if (err != noErr) {
			throw std::runtime_error("Could not create component instance: " + std::to_string(err));
		}

		AudioStreamBasicDescription streamFormat{};
		UInt32 size = sizeof(streamFormat);
		err = AudioUnitGetProperty(audioUnit, kAudioUnitProperty_StreamFormat,
		                           kAudioUnitScope_Output, OUTPUT_ELEMENT, &streamFormat, &size);
		if (err != noErr || size != sizeof(streamFormat)) {
			throw std::runtime_error("AudioUnitGetProperty failed");
		}

		streamFormat.mSampleRate = frequency;
		streamFormat.mChannelsPerFrame = 2; // stereo
		streamFormat.mFramesPerPacket = 1;
		streamFormat.mFormatFlags = kAudioFormatFlagsNativeEndian | kLinearPCMFormatFlagIsPacked |
		                            kLinearPCMFormatFlagIsFloat;
		streamFormat.mFormatID = kAudioFormatLinearPCM;
		streamFormat.mBitsPerChannel = 32;
		streamFormat.mBytesPerFrame =
		    streamFormat.mChannelsPerFrame * streamFormat.mBitsPerChannel / 8;
		streamFormat.mBytesPerPacket = streamFormat.mBytesPerFrame * streamFormat.mFramesPerPacket;

		err =
		    AudioUnitSetProperty(audioUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input,
		                         OUTPUT_ELEMENT, &streamFormat, sizeof(streamFormat));
		if (err != noErr) {
			throw std::runtime_error("AudioUnitSetProperty failed");
		}

		AURenderCallbackStruct input{};
		input.inputProc = callback;
		input.inputProcRefCon = this;

		err = AudioUnitSetProperty(audioUnit, kAudioUnitProperty_SetRenderCallback,
		                           kAudioUnitScope_Input, OUTPUT_ELEMENT, &input,
		                           sizeof(AURenderCallbackStruct));
		if (err != noErr) {
			throw std::runtime_error("AudioUnitSetProperty failed");
		}

		err = AudioUnitInitialize(audioUnit);
		if (err != noErr) {
			throw std::runtime_error("AudioUnitInitialize failed");
		}
	}

	static OSStatus callback(void* refCon, AudioUnitRenderActionFlags* ioActionFlags,
	                         const AudioTimeStamp* inTimeStamp, UInt32 inBusNumber,
	                         UInt32 inNumberFrames, AudioBufferList* ioData) noexcept {
		for (size_t i = 0; i < ioData->mNumberBuffers; ++i) {
			auto& buffer = ioData->mBuffers[i];
			static_cast<Impl*>(refCon)->output->read(static_cast<float*>(buffer.mData),
			                                         buffer.mDataByteSize / sizeof(float));
		}
		return noErr;
	}

	std::shared_ptr<Stream> output;
	AudioUnit audioUnit;
};

engine::engine(std::shared_ptr<Stream> output) : impl(std::make_unique<Impl>(std::move(output))) {
	setPause(false);
}

engine::~engine() = default;

void engine::setPause(bool pause) {
	const OSStatus err =
	    pause ? AudioOutputUnitStop(impl->audioUnit) : AudioOutputUnitStart(impl->audioUnit);
	assert(err == noErr);
}

} // namespace jngl::audio
