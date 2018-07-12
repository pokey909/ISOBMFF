#include <ISOBMFF/TFHD.hpp>

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

template<>
class XS::PIMPL::Object< ISOBMFF::TFHD >::IMPL
{
public:

    IMPL()                  = default;
    IMPL( const IMPL & o )  = default;
    ~IMPL()                 = default;

    uint32_t track_ID{0};
    uint64_t base_data_offset{0};
    uint32_t sample_description_index{0};
    uint32_t default_sample_duration{0};
    uint32_t default_sample_size{0};
    uint32_t default_sample_flags{0};

};

#define XS_PIMPL_CLASS ISOBMFF::TFHD
#include <XS/PIMPL/Object-IMPL.hpp>

namespace ISOBMFF {

    TFHD::TFHD() : FullBox("tfhd") {

    }

    void TFHD::ReadData(IParser *parser, BinaryStream &stream) {
        FullBox::ReadData(parser, stream);
        SetTrackID( stream.ReadBigEndianUInt32() );

        if (hasBaseDataOffset()) {
            SetBaseDataOffset(stream.ReadBigEndianUInt64());
        }
        if (hasSampleDescriptionIndex()) {
            SetSampleDescriptionIndex(stream.ReadBigEndianUInt32());
        }
        if (hasDefaultSampleDuration()) {
            SetDefaultSampleDuration(stream.ReadBigEndianUInt32());
        }
        if (hasDefaultSampleSize()) {
            SetDefaultSampleSize(stream.ReadBigEndianUInt32());
        }
        if (hasDefaultSampleFlags()) {
            SetDefaultSampleFlags(stream.ReadBigEndianUInt32());
        }
    }

    KeyValueStringList TFHD::GetDisplayableProperties() const {
        auto props = FullBox::GetDisplayableProperties();
        props.push_back( { "TrackID", std::to_string( GetTrackID()) } );
        props.push_back( { "BaseDataOffset", std::to_string( GetBaseDataOffset()) } );
        props.push_back( { "SampleDescriptionIndex", std::to_string(GetSampleDescriptionIndex()) } );
        props.push_back( { "DefaultSampleDuration", std::to_string(GetDefaultSampleDuration()) } );
        props.push_back( { "DefaultSampleSize", std::to_string(GetDefaultSampleSize()) } );
        props.push_back( { "DefaultSampleFlags", std::to_string(GetDefaultSampleFlags()) } );
        return props;
    }

    void TFHD::WriteDescription(std::ostream &os, std::size_t indentLevel) const {
        DisplayableObject::WriteDescription(os, indentLevel);
    }

    uint32_t TFHD::GetTrackID() const {
        return impl->track_ID;
    }

    void TFHD::SetTrackID(uint32_t value) const {
        impl->track_ID = value;
    }

    uint64_t TFHD::GetBaseDataOffset() const {
        return impl->base_data_offset;
    }

    uint32_t TFHD::GetSampleDescriptionIndex() const {
        return impl->sample_description_index;
    }

    uint32_t TFHD::GetDefaultSampleDuration() const {
        return impl->default_sample_duration;
    }

    uint32_t TFHD::GetDefaultSampleSize() const {
        return impl->default_sample_size;
    }

    uint32_t TFHD::GetDefaultSampleFlags() const {
        return impl->default_sample_flags;
    }

    void TFHD::SetBaseDataOffset(uint64_t value) const {
        impl->base_data_offset = value;
    }

    void TFHD::SetSampleDescriptionIndex(uint32_t value) const {
        impl->sample_description_index = value;
    }

    void TFHD::SetDefaultSampleDuration(uint32_t value) const {
        impl->default_sample_duration = value;
    }

    void TFHD::SetDefaultSampleSize(uint32_t value) const {
        impl->default_sample_size = value;
    }

    void TFHD::SetDefaultSampleFlags(uint32_t value) const {
        impl->default_sample_flags = value;
    }

    bool TFHD::hasBaseDataOffset() const {
        return (GetFlags() & 0x1u) != 0;
    }

    bool TFHD::hasSampleDescriptionIndex() const {
        return (GetFlags() & 0x2u) != 0;
    }

    bool TFHD::hasDefaultSampleDuration() const {
        return (GetFlags() & 0x8u) != 0;
    }

    bool TFHD::hasDefaultSampleSize() const {
        return (GetFlags() & 0x10u) != 0;
    }

    bool TFHD::hasDefaultSampleFlags() const {
        return (GetFlags() & 0x20u) != 0;
    }
}