#include "seaculogger.hpp"

namespace seacu {
	template class SEACULOGGER_EXPORT logger<char>;
	using slogger = logger<char>;
#if 0
	template class SEACULOGGER_EXPORT logger<wchar_t>;
	using wlogger = logger<wchar_t>;
#endif
}
template seacu::slogger& seacu::slogger::operator<<(typename slogger::character_type) noexcept;
template seacu::slogger& seacu::slogger::operator<<(char) noexcept;
template seacu::slogger& seacu::slogger::operator<<(unsigned char) noexcept;
template seacu::slogger& seacu::slogger::operator<<(int) noexcept;
template seacu::slogger& seacu::slogger::operator<<(unsigned int) noexcept;
template seacu::slogger& seacu::slogger::operator<<(long long) noexcept;
template seacu::slogger& seacu::slogger::operator<<(unsigned long long) noexcept;

template seacu::slogger& seacu::slogger::operator<<(double) noexcept;
template seacu::slogger& seacu::slogger::operator<<(float) noexcept;

#if 0
template seacu::wlogger& seacu::wlogger::operator<<(typename slogger::character_type) noexcept;
template seacu::wlogger& seacu::wlogger::operator<<(char) noexcept;
template seacu::wlogger& seacu::wlogger::operator<<(unsigned char) noexcept;
template seacu::wlogger& seacu::wlogger::operator<<(int) noexcept;
template seacu::wlogger& seacu::wlogger::operator<<(unsigned int) noexcept;
template seacu::wlogger& seacu::wlogger::operator<<(long long) noexcept;
template seacu::wlogger& seacu::wlogger::operator<<(unsigned long long) noexcept;

template seacu::wlogger& seacu::wlogger::operator<<(double) noexcept;
template seacu::wlogger& seacu::wlogger::operator<<(float) noexcept;

template seacu::s8logger& seacu::s8logger::operator<<(typename slogger::character_type) noexcept;
template seacu::s8logger& seacu::s8logger::operator<<(char) noexcept;
template seacu::s8logger& seacu::s8logger::operator<<(unsigned char) noexcept;
template seacu::s8logger& seacu::s8logger::operator<<(int) noexcept;
template seacu::s8logger& seacu::s8logger::operator<<(unsigned int) noexcept;
template seacu::s8logger& seacu::s8logger::operator<<(long long) noexcept;
template seacu::s8logger& seacu::s8logger::operator<<(unsigned long long) noexcept;

template seacu::s8logger& seacu::s8logger::operator<<(double) noexcept;
template seacu::s8logger& seacu::s8logger::operator<<(float) noexcept;

template seacu::s16logger& seacu::s16logger::operator<<(typename slogger::character_type) noexcept;
template seacu::s16logger& seacu::s16logger::operator<<(char) noexcept;
template seacu::s16logger& seacu::s16logger::operator<<(unsigned char) noexcept;
template seacu::s16logger& seacu::s16logger::operator<<(int) noexcept;
template seacu::s16logger& seacu::s16logger::operator<<(unsigned int) noexcept;
template seacu::s16logger& seacu::s16logger::operator<<(long long) noexcept;
template seacu::s16logger& seacu::s16logger::operator<<(unsigned long long) noexcept;

template seacu::s16logger& seacu::s16logger::operator<<(double) noexcept;
template seacu::s16logger& seacu::s16logger::operator<<(float) noexcept;
#endif

