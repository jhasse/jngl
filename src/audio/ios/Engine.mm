#include "../engine.hpp"

#include "../constants.hpp"
#include "../../jngl/debug.hpp"

#include <AudioUnit/AudioUnit.h>
#include <cassert>
#include <string>
#include <thread>

namespace psemek::audio {

std::atomic_int8_t* gBuffersEmpty;

constexpr AudioUnitElement OUTPUT_ELEMENT = 0;

struct engine::Impl {
	Impl(std::shared_ptr<stream> output) : output(std::move(output)) {
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

        workerThread = std::thread([this]() {
            std::unique_lock lock(bufferMutex);
            while (!stopped) {
                bufferCV.wait(lock, [this]() { return emptyBuffers > 0 || stopped; });
                int wokeUp = emptyBuffers.load();
                if (stopped) {
                    break;
                }
                std::vector<float>* buffer = &buffers[writePos];
                this->output->read(buffer->data(), buffer->size());
                writePos = (writePos + 1) % buffers.size();
                if (--emptyBuffers > 1) {
                    jngl::debug("woke up with "); jngl::debug(wokeUp);
                    jngl::debug(" and finished with ");
                    jngl::debugLn(int(emptyBuffers.load()));
                }
            }
        });
        gBuffersEmpty = &emptyBuffers;
	}

    ~Impl() {
        stopped = true;
        bufferCV.notify_one();
        workerThread.join();
    }

	static OSStatus callback(void* refCon, AudioUnitRenderActionFlags* ioActionFlags,
	                         const AudioTimeStamp* inTimeStamp, UInt32 inBusNumber,
	                         UInt32 inNumberFrames, AudioBufferList* ioData) noexcept {
        assert(ioData->mNumberBuffers == 1);
        auto& buffer = ioData->mBuffers[0];
        auto& self = *static_cast<Impl*>(refCon);
        if (self.buffers.empty()) {
            self.buffers.resize(15);
            for (auto& buf : self.buffers) {
                buf.resize(buffer.mDataByteSize / sizeof(float));
            }
            self.emptyBuffers = self.buffers.size();
            self.bufferCV.notify_one();
            return noErr;
        }
        assert(self.buffers[0].size() == buffer.mDataByteSize / sizeof(float));
        assert(self.buffers[1].size() == buffer.mDataByteSize / sizeof(float));
        assert(self.buffers[2].size() == buffer.mDataByteSize / sizeof(float));
        while (self.emptyBuffers == self.buffers.size()) {
           // jngl::debugLn("WARNING: Audio worker thread too slow!");
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        std::vector<float>* bufferToRead = &self.buffers[self.readPos];
        std::copy(&(*bufferToRead)[0], &(*bufferToRead)[0] + (*bufferToRead).size(), static_cast<float*>(buffer.mData));
        self.readPos = (self.readPos + 1) % self.buffers.size();
        ++self.emptyBuffers;
        self.bufferCV.notify_one();
		return noErr;
	}

	std::shared_ptr<stream> output;
    std::thread workerThread;
    std::atomic_bool stopped = false;
    std::mutex bufferMutex;
    int8_t bufferToFill = -1; // -1 = not started, 0 = buffer1, 1 = buffer2
    std::condition_variable bufferCV;
    std::atomic_int8_t emptyBuffers = 0;
    std::atomic_int8_t writePos = 0;
    std::atomic_int8_t readPos = 0;
    std::vector<std::vector<float>> buffers;
	AudioUnit audioUnit;
};

engine::engine(std::shared_ptr<stream> output) : impl(std::make_unique<Impl>(std::move(output))) {
	setPause(false);
}

engine::~engine() = default;

void engine::setPause(bool pause) {
	const OSStatus err =
	    pause ? AudioOutputUnitStop(impl->audioUnit) : AudioOutputUnitStart(impl->audioUnit);
	assert(err == noErr);
}

} // namespace psemek::audio
