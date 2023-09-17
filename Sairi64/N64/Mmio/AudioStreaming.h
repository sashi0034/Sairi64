#pragma once

namespace N64::Mmio
{
	constexpr uint32 DefaultAudioSampleRate_44100 = 44100;

	class AudioStreaming : public IAudioStream
	{
	public:
		AudioStreaming();
		~AudioStreaming() override = default;
		void getAudio(float* left, float* right, size_t samplesToWrite) override;
		bool hasEnded() override { return false; };
		void rewind() override { return; };

		void PushSample(float left, float right);

		int BufferRemaining() const;
		int BufferSize() const;

	private:
		Wave m_wave{};
		int m_headIndex{};
		int m_tailIndex{};
	};
}
