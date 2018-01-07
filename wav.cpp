#include <array>
#include <vector>
#include <cstddef>

struct WavFile {
        /// Chunk id, RIFF, big endian
        /// Not included in chunk size
        std::array<std::byte, 4> chunk_id;

        /// Chunk size, 4 + (8 * fmt subchunk) + (8 * data sub chunk)
        /// Little endian
        int32_t chunk_size;

        /// Format, contains WAVE.
        std::array<std::byte, 4> format; 

        /// This is the format subchunk, describing the 
        /// format of the file
        std::array<std::byte, 4> fmtchk_id;

        /// 16 for PCM.
        int32_t fmtchk_size;

        /// Audio format, 1 for uncompressed
        int16_t fmtchk_format;

        /// Number of channels
        int16_t fmtchk_numchannels;

        /// Sample rate
        int32_t fmtchk_samplerate;

        /// Byte rate (SampleRate * NumChannels * BitsPerSample/8)
        int32_t fmtchk_byterate;

        /// block align Number of bytes for one sample
        /// including all channels.
        int32_t fmtchk_blockalign;

        /// Bits per sample
        int32_t fmtchk_bitspersample;

        // Data chunk
        
        /// data chunk identifier
        std::array<std::byte, 4> datachk_id;

        /// size of the data
        int32_t datachk_size;

        /// Actual sound data
        std::vector<std::byte> data;


        WavFile(WavFile&& rhs) = default;
        WavFile(const WavFile& wf) = default;
        WavFile& operator=(const WavFile& wf) = default;
        WavFile& operator=(WavFile& wf) = default;

private:
        WavFile(){}
        friend WavFile generateWavFromData(std::vector<std::byte>);
};

template <typename... Args>
auto arrayOfBytes(Args... args)
{
        return std::array<std::byte, sizeof...(args)>{(std::byte)args...,};
}

WavFile generateWavFromData (std::vector<std::byte> data)
{
        WavFile file;

        return file;
}

#ifdef STUB_MAIN
int main()
{
        return 0;
}
#endif
