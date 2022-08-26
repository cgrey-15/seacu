#ifndef SEACULOG_HPP
#define SEACULOG_HPP
#include "seaculogger.h"
#include <cassert>
#include <array>
#include <charconv>
#include <ranges>

template<typename C>
typename seacu::logger<C>::display_type seacu::logger<C>::status() noexcept {
	if (lines_.empty()) {
		successfulLast_ = false;
		return std::basic_string_view<C>{};
	}
	else {
		successfulLast_ = true;
		auto [pos, len] = lines_.back();
		return display_type(logStore_.data() + pos, len);
	}
}

template<typename C>
template<std::integral T>
seacu::logger<C>& seacu::logger<C>::operator<<(T val) noexcept {
	if constexpr (std::is_same_v<T, C>) {
		logStore_.emplace_back(val);
		++currPos_;
		if (val == '\n') {
			lines_.emplace_back(lastLineBegin_, currPos_ - lastLineBegin_);
			lastLineBegin_ = currPos_;
		}
	}
	else {
		std::array<char, 26> buffer{};
		auto conv_res = std::to_chars(buffer.data(), buffer.data() + buffer.size(), val);
		assert(conv_res.ec != std::errc::value_too_large);
		std::ptrdiff_t digit_width;
		std::ptrdiff_t padding;
		if constexpr (std::signed_integral<T>) {
			digit_width = buffer[0] != '-' ? conv_res.ptr - buffer.data() : conv_res.ptr - buffer.data() - 1;
			padding = buffer[0] != '-' ? 0 : 1;
		}
		else {
			digit_width = conv_res.ptr - buffer.data();
			padding = 0;
		}
		logStore_.insert(logStore_.end(), buffer.data(), buffer.data() + (digit_width % 3) + padding);
		std::ptrdiff_t comma_count = 0;
		for (char const* ptr = buffer.data() + (digit_width % 3) + padding; ptr < conv_res.ptr; ptr += 3) {
			logStore_.emplace_back(',');
			logStore_.insert(logStore_.end(), ptr, ptr + 3);
			++comma_count;
		}
		if constexpr (std::unsigned_integral<T>) {
			logStore_.emplace_back('U');
			++currPos_;
		}
		currPos_ += digit_width + padding + comma_count;
	}
	successfulLast_ = true;
	return *this;
}

template <typename C>
template <std::floating_point T>
seacu::logger<C>& seacu::logger<C>::operator<<(T val) noexcept {
	std::array<char, 26> buffer{};
	auto conv_res = std::to_chars(buffer.data(), buffer.data() + buffer.size(), val, std::chars_format::general, 19);
	if (conv_res.ec == std::errc::value_too_large) {
		successfulLast_ = false;
	}
	else {
		logStore_.insert(logStore_.end(), buffer.data(), conv_res.ptr);
		currPos_ += conv_res.ptr - buffer.data();
		successfulLast_ = true;
	}
	return *this;
}

//template<std::ranges::view T>
template<typename C>
seacu::logger<C>& seacu::logger<C>::operator<<(std::basic_string_view<C> str) noexcept {

	using namespace std::string_view_literals;
	static constexpr C lit[] = { '\n' };
	constexpr std::basic_string_view<C> delim(lit, 1);
#if 1
	auto line_it_proxy = std::views::split(str, delim);
	for (auto line = line_it_proxy.begin(); line != line_it_proxy.end(); ++line) {
		auto put_element_count = (*line).end() - (*line).begin();
		logStore_.insert(logStore_.end(), (*line).begin(), (*line).end());
		if (auto _ = line; (++_) != line_it_proxy.end()) {
			logStore_.emplace_back('\n');
			++put_element_count;
			lines_.emplace_back(lastLineBegin_, static_cast<std::uint32_t>((currPos_ - lastLineBegin_) + put_element_count));
			lastLineBegin_ = currPos_ + put_element_count;
		}
		currPos_ += put_element_count;
	}
#endif
	return *this;
}

template<typename C>
typename seacu::logger<C>::traverser::value_type seacu::logger<C>::traverser::operator*() const {
	auto [pos, len] = *ptr_;
	return value_type(data_ + pos, len);
}
template<typename C>
typename seacu::logger<C>::traverser& seacu::logger<C>::traverser::operator++() { ++ptr_; return *this; }

template<typename C>
typename seacu::logger<C>::traverser seacu::logger<C>::traverser::operator++(int) { auto ret = *this; operator++(); return ret; }

template<typename C>
typename seacu::logger<C>::traverser& seacu::logger<C>::traverser::operator--() { --ptr_; return *this; }

template<typename C>
typename seacu::logger<C>::traverser seacu::logger<C>::traverser::operator--(int) { auto ret = *this; operator--(); return ret; }

template<typename C>
bool seacu::logger<C>::traverser::operator==(const traverser& rhs) const { return ptr_ == rhs.ptr_; }

template<typename C>
bool seacu::logger<C>::traverser::operator!=(const traverser& rhs) const { return !(*this == rhs); }

template<typename C>
typename seacu::logger<C>::traverser& seacu::logger<C>::traverser::operator+=(difference_type n) { ptr_ += n; return *this; }

template<typename C>
typename seacu::logger<C>::traverser seacu::logger<C>::traverser::operator+(difference_type delta) const { auto ret = *this; return ret += delta; }

template<typename C>
typename seacu::logger<C>::traverser& seacu::logger<C>::traverser::operator-=(difference_type n) { return operator+=(-n); }

template<typename C>
typename seacu::logger<C>::traverser seacu::logger<C>::traverser::operator-(difference_type delta) const { auto ret = *this; return ret -= delta; }

template<typename C>
typename seacu::logger<C>::traverser::difference_type seacu::logger<C>::traverser::operator-(traverser o) const { return ptr_ - o.ptr_; }

template<typename C>
typename seacu::logger<C>::traverser::value_type seacu::logger<C>::traverser::operator[](difference_type idx) const {
	auto [pos, len] = ptr_[idx];
	return value_type(data_ + pos, len);
}

template <typename C>
auto seacu::operator+(typename seacu::logger<C>::iterator::difference_type lhs, typename seacu::logger<C>::iterator rhs)
-> typename seacu::logger<C>::iterator
{
	return rhs + lhs;
}
#endif //SEACULOG_HPP

