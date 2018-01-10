#include "stdafx.h"
#include "Sample.h"
#include <cassert>
#include "WavReader.h"

const std::byte& Sample::Channel::operator[](std::size_t idx) const
{
    auto bytes_per_sample{ (*s.file).fmtchk_bitspersample / 8 };
    assert(idx >= 0 && static_cast<int>(idx) < bytes_per_sample);

    return (*s.file).data_it[s.sample_offset + ch * bytes_per_sample + idx];
}

Sample::Channel::Channel(const Sample& sample, int channel) :s{ sample }, ch{ channel } {}

Sample::Channel Sample::operator[](int channel) const
{
    assert(channel < number_of_channels());
    return Channel(*this, channel);
}

const std::byte* Sample::operator&() const
{
    return &((*this)[0][0]);
}

int Sample::number_of_channels() const { return (*file).fmtchk_numchannels; }
int Sample::channel_size() const { return (*file).fmtchk_bitspersample; }

Sample::Sample(std::shared_ptr<WavFile> f, int32_t off) : file(std::move(f)), sample_offset(off)
{
    bits_per_sample = (*file).fmtchk_bitspersample;
}