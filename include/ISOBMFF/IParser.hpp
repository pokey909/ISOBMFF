#pragma once

#include <string>
#include <ISOBMFF/Box.hpp>

namespace ISOBMFF {

    class IParser {
    public:
        /*!
         * @enum        StringType
         * @abstract    Possible types for strings in an ISO media file.
         * @discussion  ISO/IEC 14496-12 only specifies UTF-8 NULL
         *              terminated strings, but QuickTime actually uses
         *              Pascal strings.
         * @constant    NULLTerminated  UTF-8 NULL terminated strings.
         * @constant    Pascal          Pascal strings (length prefixed).
         */
        enum class StringType: int
        {
            NULLTerminated,
            Pascal
        };


        /*!
         * @enum        Options
         * @abstract    Parser options.
         * @constant    SkipMDATData    Do not keep data found in MDAT boxes.
         */
        enum Options: uint64_t
        {
            SkipMDATData            = 0x1u << 0u,
            SkipNotRequiredBoxes    = 0x1u << 1u,
            ShowBoxContentDebug     = 0x1u << 2u
        };

        virtual ~IParser() {}

        /*!
         * @function    CreateBox
         * @abstract    Creates a new box for a specific type.
         * @param       type    The box type (four character string).
         * @result      A new box.
         */
        virtual std::shared_ptr< ISOBMFF::Box > CreateBox( const std::string & type ) const = 0;


        /*!
         * @function    GetPreferredStringType
         * @abstract    Gets the preferred string type used in the parser.
         * @result      The preferred string type.
         * @see         StringType
         */
        virtual StringType GetPreferredStringType( void ) const = 0;

        /*!
         * @function    SetPreferredStringType
         * @abstract    Sets the preferred string type to use in the parser.
         * @param       value   The preferred string type.
         * @see         StringType
         */
        virtual void SetPreferredStringType( StringType value ) = 0;

        /*!
         * @function    GetOptions
         * @abstract    Gets the parser options.
         * @result      The parser options.
         * @see         Options
         */
        virtual uint64_t GetOptions( void ) const = 0;

        /*!
         * @function    SetOptions
         * @abstract    Sets the parser options.
         * @param       value   The parser options.
         * @see         Options
         */
        virtual void SetOptions( uint64_t value ) = 0;

        /*!
         * @function    AddOption
         * @abstract    Sets a specific parser option.
         * @param       option  The option to set/add.
         * @see         Options
         */
        virtual void AddOption( Options option ) = 0;

        /*!
         * @function    RemoveOption
         * @abstract    Removes a specific parser option.
         * @param       option  The option to remove.
         * @see         Options
         */
        virtual void RemoveOption( Options option ) = 0;

        /*!
         * @function    HasOption
         * @abstract    Checks if a specific parser option is set.
         * @param       option  The option to check for.
         * @result      true if the option is set, otherwise false.
         * @see         Options
         */
        virtual bool HasOption( Options option ) = 0;

        /*!
         * @function    GetInfo
         * @abstract    Gets an info value in the parser.
         * @param       key The info key.
         * @result      The value for the info key, or nullptr.
         * @see         SetInfo
         */
        virtual const void * GetInfo( const std::string & key ) = 0;

        /*!
         * @function    SetInfo
         * @abstract    Sets an info value in the parser.
         * @discussion  This method can be used to store any kind of
         *              contextual information that may be useful while
         *              parsing.
         * @param       key     The info key.
         * @param       value   The info value.
         * @see         GetInfo
         */
        virtual void SetInfo( const std::string & key, void * value ) = 0;

    };
}