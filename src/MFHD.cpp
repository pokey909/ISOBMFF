#include <ISOBMFF/MFHD.hpp>
#include <ISOBMFF/ContainerBox.hpp>

template<>
class XS::PIMPL::Object< ISOBMFF::MFHD >::IMPL
{
public:

    IMPL()                  = default;
    IMPL( const IMPL & o )  = default;
    ~IMPL()                 = default;

    uint32_t sequence_number{0};
};

#define XS_PIMPL_CLASS ISOBMFF::MFHD
#include <XS/PIMPL/Object-IMPL.hpp>

namespace ISOBMFF{

    MFHD::MFHD() : FullBox("mfhd") {

    }

    void MFHD::ReadData(IParser *parser, BinaryStream &stream) {
        FullBox::ReadData(parser, stream);
        SetSequenceNumber( stream.ReadBigEndianUInt32() );
    }

    KeyValueStringList MFHD::GetDisplayableProperties() const {
        auto props = FullBox::GetDisplayableProperties();
        props.push_back( { "Sequence_Number", std::to_string( GetSequenceNumber() ) } );
        return props;
    }

    void MFHD::WriteDescription(std::ostream &os, std::size_t indentLevel) const {
        FullBox::WriteDescription( os, indentLevel );
    }

    uint32_t MFHD::GetSequenceNumber() const {
        return impl->sequence_number;
    }

    void MFHD::SetSequenceNumber(uint32_t value) {
        impl->sequence_number = value;
    }
}
