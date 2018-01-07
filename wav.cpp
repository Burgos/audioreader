#include <array>
#include <deque>
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
        std::deque<std::byte> data;


        WavFile(WavFile&& rhs) = default;
        WavFile(const WavFile& wf) = default;
        WavFile& operator=(const WavFile& wf) = default;
        WavFile& operator=(WavFile& wf) = default;

private:
        WavFile(){}
        friend WavFile generateWavFromData(std::deque<std::byte>);
};

template <typename... Args>
auto arrayOfBytes(Args... args)
{
        return std::array<std::byte, sizeof...(args)>{(std::byte)args...,};
}

/// Consumes the data from the deque.
template <typename T>
auto consumeFromVector (std::deque<std::byte>& data)
{
        static_assert(sizeof(T) > 0);

        if (data.size() < sizeof(T))
                throw new std::exception("Not enough data to read.");

        // assume little endian TODO
        auto val = *reinterpret_cast<T*>(&data[0]);
        data.erase(std::cbegin(data), std::cbegin(data) + sizeof(T));
        return val;
}

/// Generates the WavFile from the passed data deque.
WavFile generateWavFromData (std::deque<std::byte> data)
{
        WavFile file;

        std::move(std::begin(data),
                  std::begin(data) + 4,
                  std::begin(file.chunk_id));
        if (file.chunk_id != arrayOfBytes('R', 'I', 'F', 'F'))
                throw new std::exception("Bad chunk id in the input data.");

        file.chunk_size = consumeFromVector<int32_t>(data);

        std::move(std::begin(data),
                  std::begin(data) + 4,
                  std::begin(file.format));
        if (file.format != arrayOfBytes('W', 'A', 'V', 'E'))
                throw new std::exception("Bad chunk id in the input data.");

        std::move(std::begin(data),
                  std::begin(data) + 4,
                  std::begin(file.fmtchk_id));
        if (file.format != arrayOfBytes('f', 'm', 't', ' '))
                throw new std::exception("Bad chunk id in the input data.");

        file.fmtchk_size = consumeFromVector<int32_t>(data);
        file.fmtchk_format = consumeFromVector<int16_t>(data);
        file.fmtchk_numchannels = consumeFromVector<int16_t>(data);
        file.fmtchk_samplerate = consumeFromVector<int32_t>(data);
        file.fmtchk_byterate = consumeFromVector<int32_t>(data);
        file.fmtchk_blockalign = consumeFromVector<int32_t>(data);
        file.fmtchk_bitspersample = consumeFromVector<int32_t>(data);

        std::move(std::begin(data),
                  std::begin(data) + 4,
                  std::begin(file.datachk_id));
        if (file.format != arrayOfBytes('d', 'a', 't', 'a'))
                throw new std::exception("Bad chunk id in the input data.");

        file.datachk_size = consumeFromVector<int32_t>(data);
        file.data = std::move(data);

        return file;
}

#ifdef STUB_MAIN
int main()
{
        return 0;
}
#endif
