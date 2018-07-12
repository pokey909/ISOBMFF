#pragma once

#include <XS/PIMPL/Object.hpp>
#include <ISOBMFF/Macros.hpp>
#include <ISOBMFF/FullBox.hpp>
#include <cstdint>

namespace ISOBMFF {
    /**
     * <h1>4cc = "{@value #TYPE}"</h1>
     * aligned(8) class TrackFragmentHeaderBox
     * extends FullBox('tfhd', 0, tf_flags){
     * unsigned int(32) track_ID;
     * // all the following are optional fields
     * unsigned int(64) base_data_offset;
     * unsigned int(32) sample_description_index;
     * unsigned int(32) default_sample_duration;
     * unsigned int(32) default_sample_size;
     * unsigned int(32) default_sample_flags
     * }
     */

    class ISOBMFF_EXPORT TFHD : public FullBox, public XS::PIMPL::Object<TFHD> {
    public:
        using XS::PIMPL::Object<TFHD>::impl;

        TFHD();

        void ReadData(IParser *parser, BinaryStream &stream) override;

        KeyValueStringList GetDisplayableProperties() const override;

        void WriteDescription(std::ostream &os, std::size_t indentLevel) const override;

        uint32_t GetTrackID() const;
        void SetTrackID(uint32_t value) const;

        // Optional
        uint64_t GetBaseDataOffset() const;
        uint32_t GetSampleDescriptionIndex() const;
        uint32_t GetDefaultSampleDuration() const;
        uint32_t GetDefaultSampleSize() const;
        uint32_t GetDefaultSampleFlags() const;

        void SetBaseDataOffset(uint64_t value) const;
        void SetSampleDescriptionIndex(uint32_t value) const;
        void SetDefaultSampleDuration(uint32_t value) const;
        void SetDefaultSampleSize(uint32_t value) const;
        void SetDefaultSampleFlags(uint32_t value) const;

        bool hasBaseDataOffset() const;
        bool hasSampleDescriptionIndex() const;
        bool hasDefaultSampleDuration() const;
        bool hasDefaultSampleSize() const;
        bool hasDefaultSampleFlags() const;

    };
}

