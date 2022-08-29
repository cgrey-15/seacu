#ifndef SEACULOGGER_H
#define SEACULOGGER_H
#include <cstddef>
#include <concepts>
#include <iterator>
#include <vector>
#include <string_view>
#include <tuple>
#include "seaculogger_export.h"

namespace seacu {
	template<typename C>
	class SEACULOGGER_EXPORT logger {
	public:
		using character_type = C;
		using display_type = std::basic_string_view<C>;
		display_type status() noexcept;

		template<std::integral T>
		logger& operator<<(T val) noexcept;

		template<std::floating_point T>
		logger& operator<<(T val) noexcept;

		logger& operator<<(std::basic_string_view<C> str) noexcept;

		class SEACULOGGER_EXPORT traverser {
		public:
			using value_type = display_type;
			using reference = value_type&;
			using pointer = value_type*;
			using difference_type = std::ptrdiff_t;
			using iterator_category = std::random_access_iterator_tag;

			using expression_type_ = std::tuple<std::size_t, std::uint32_t>;

			traverser() = default;
			explicit traverser(C const* data, expression_type_* ptr) : ptr_{ ptr }, data_{ data } {}
			traverser(const traverser& o) = default;
			traverser& operator=(const traverser& o) = default;

			~traverser() = default;

			value_type operator*() const;
			traverser& operator++();
			traverser operator++(int);
			traverser& operator--();
			traverser operator--(int);
			bool operator==(const traverser& rhs) const;
			bool operator!=(const traverser& rhs) const;

			traverser& operator+=(difference_type n);
			traverser operator+(difference_type delta) const;
			friend traverser operator+(difference_type lhs, traverser rhs);
			traverser& operator-=(difference_type n);
			traverser operator-(difference_type delta) const;
			difference_type operator-(traverser o) const;
			value_type operator[](difference_type idx) const;
			auto operator<=>(const traverser& rhs) const = default;
		private:
			expression_type_* ptr_ = nullptr;
			C const* data_ = nullptr;
		};
		operator bool() const noexcept { return successfulLast_; }

		traverser begin() { return traverser(logStore_.data(), lines_.data()); }
		traverser end() { return traverser(logStore_.data(), lines_.data() + lines_.size()); }


	private:
		std::vector<std::tuple<std::size_t, std::uint32_t>> lines_;
		std::vector<C> logStore_;
		std::size_t lastLineBegin_ = 0U;
		std::size_t currPos_ = 0U;
		bool successfulLast_ = true;
	};

	template <typename C>
	SEACULOGGER_EXPORT auto operator+(typename seacu::logger<C>::iterator::difference_type lhs, typename seacu::logger<C>::iterator rhs)
		-> typename seacu::logger<C>::iterator;
}

#ifndef SEACULOGGER_HEADER_ONLY
namespace seacu {
	extern template class SEACULOGGER_EXPORT logger<char>;
	using slogger = logger<char>;
}
//template class seacu::logger<char>;
extern template SEACULOGGER_EXPORT seacu::slogger& seacu::slogger::operator<<(typename slogger::character_type) noexcept;
extern template SEACULOGGER_EXPORT seacu::slogger& seacu::slogger::operator<<(char) noexcept;
extern template SEACULOGGER_EXPORT seacu::slogger& seacu::slogger::operator<<(unsigned char) noexcept;
extern template SEACULOGGER_EXPORT seacu::slogger& seacu::slogger::operator<<(int) noexcept;
extern template SEACULOGGER_EXPORT seacu::slogger& seacu::slogger::operator<<(unsigned int) noexcept;
extern template SEACULOGGER_EXPORT seacu::slogger& seacu::slogger::operator<<(long long) noexcept;
extern template SEACULOGGER_EXPORT seacu::slogger& seacu::slogger::operator<<(unsigned long long) noexcept;

extern template SEACULOGGER_EXPORT seacu::slogger& seacu::slogger::operator<<(double) noexcept;
extern template SEACULOGGER_EXPORT seacu::slogger& seacu::slogger::operator<<(float) noexcept;

#endif // SEACUDIFF_HEADER_ONLY

#endif // SEACULOGGER_H
