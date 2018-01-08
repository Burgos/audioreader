#include "stdafx.h"

#include <array>
#include <vector>
#include <cstddef>
#include <cassert>

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
        int16_t fmtchk_blockalign;

        /// Bits per sample
        int16_t fmtchk_bitspersample;

        // Data chunk
        
        /// data chunk identifier
        std::array<std::byte, 4> datachk_id;

        /// size of the data
        int32_t datachk_size;

        /// Data container
        std::vector<std::byte> data;

        /// Iterator to the data segment.
        decltype(data)::iterator data_it;


        WavFile(WavFile&& rhs) = default;
        WavFile(const WavFile& wf) = default;
        WavFile& operator=(const WavFile& wf) = default;
        WavFile& operator=(WavFile& wf) = default;

private:
        WavFile(){}

        template <typename Container>
        friend WavFile generateWavFromData(Container&&);
};

template <typename... Args>
auto arrayOfBytes(Args... args)
{
        return std::array<std::byte, sizeof...(args)>{(std::byte)args...,};
}

template <typename T, typename It1, typename It2>
auto consumeFromIt (It1& it, const It2& end)
{
        static_assert(sizeof(T) > 0);

        if (std::distance(it, end) < sizeof(T))
                throw new std::exception("Not enough data to read.");

        // assume little endian TODO
        auto val = *reinterpret_cast<T*>(*it);
        it += sizeof(T);
        return val;
}

//
/// Generates the WavFile from the passed data.
template <typename Container>
WavFile generateWavFromData (Container&& data)
{
        WavFile file;

        file.data = std::forward<Container>(data);
        auto it = std::begin(file.data);

        std::move(it,
                  it + 4,
                  std::begin(file.chunk_id));
        if (file.chunk_id != arrayOfBytes('R', 'I', 'F', 'F'))
                throw new std::exception("Bad chunk id in the input data.");

        file.chunk_size = consumeFromIt<int32_t>(it, std::end(file.data));

        std::move(it,
                  it + 4,
                  std::begin(file.format));
        if (file.format != arrayOfBytes('W', 'A', 'V', 'E'))
                throw new std::exception("Bad chunk id in the input data.");

        std::move(it,
                  it + 4,
                  std::begin(file.fmtchk_id));
        if (file.format != arrayOfBytes('f', 'm', 't', ' '))
                throw new std::exception("Bad chunk id in the input data.");

        file.fmtchk_size = consumeFromIt<int32_t>(it, std::end(file.data));
        file.fmtchk_format = consumeFromIt<int16_t>(it, std::end(file.data));
        file.fmtchk_numchannels = consumeFromIt<int16_t>(it, std::end(file.data));
        file.fmtchk_samplerate = consumeFromIt<int32_t>(it, std::end(file.data));
        file.fmtchk_byterate = consumeFromIt<int32_t>(it, std::end(file.data));
        file.fmtchk_blockalign = consumeFromIt<int16_t>(it, std::end(file.data));
        file.fmtchk_bitspersample = consumeFromIt<int16_t>(it, std::end(file.data));

        std::move(it,
                  it + 4,
                  std::begin(file.datachk_id));
        if (file.format != arrayOfBytes('d', 'a', 't', 'a'))
                throw new std::exception("Bad chunk id in the input data.");

        file.datachk_size = consumeFromIt<int32_t>(it, std::end(file.data));

        // Iterator now points to the data segment.
        file.data_it = it;

        return file;
}

#ifdef STUB_MAIN
int main()
{
        // test it throws

        try
        {
                std::vector<std::byte> data{0};
                generateWavFromData(data);
                assert(false);
        }
        catch (...)
        {}

        try
        {
                std::vector<std::byte> data{0};
                generateWavFromData(std::move(data));
                assert(false);
        }
        catch (...)
        {}

        return 0;
}
#endif
