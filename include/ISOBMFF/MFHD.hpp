#pragma once

#include <XS/PIMPL/Object.hpp>
#include <ISOBMFF/Macros.hpp>
#include <ISOBMFF/FullBox.hpp>
#include <cstdint>

namespace ISOBMFF
{
    class ISOBMFF_EXPORT MFHD: public FullBox, public XS::PIMPL::Object< MFHD >
    {
    public:
        using XS::PIMPL::Object< MFHD >::impl;

        MFHD();

        void               ReadData(IParser *parser, BinaryStream &stream) override;
        KeyValueStringList GetDisplayableProperties() const override;
        void WriteDescription( std::ostream & os, std::size_t indentLevel ) const override;

        uint32_t GetSequenceNumber() const;

        void SetSequenceNumber( uint32_t value );
    };
}

