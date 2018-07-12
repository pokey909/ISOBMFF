#pragma once

#include <algorithm>                       // copy, min
#include <iostream>                             // streamsize
#include <boost/iostreams/categories.hpp>
#include <boost/iostreams/concepts.hpp>
#include <ISOBMFF/Macros.hpp>

namespace ISOBMFF
{
    template<class Container>
    class ISOBMFF_EXPORT ContainerDevice : public boost::iostreams::device<boost::iostreams::bidirectional> {
    public:
        explicit ContainerDevice(Container& container) : m_container(container), m_pos(0) {}

        size_t freeBufferSpace() const {
            return m_container.capacity() - m_container.size();
        }
        std::streamsize read(char* s, std::streamsize n) {
            auto result = (std::min)(n, static_cast<std::streamsize>(m_container.size()));
            if (result != 0) {
                std::copy_n( m_container.begin(), result, s );
                m_container.erase(m_container.begin(), m_container.begin() + result);
                return result;
            } else {
                return -1; // EOF
            }
        }

        std::streamsize write(const char* s, std::streamsize n) {
            auto result = (std::min)(n, freeBufferSpace());
            std::copy_n(s, result, std::back_inserter(m_container));

            return result;
        }

        Container& container() { return m_container; }

    private:
        typedef typename Container::size_type   size_type;
        Container&                              m_container;
        size_type                               m_pos;
        std::iostream*                          m_upstream;
    };

}