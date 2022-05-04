#include <limits>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <type_traits>
#include <concepts>
#include <bitset>
#include <span>
#if defined(_DEBUG) || defined(_GLIBCXX_DEBUG) || defined(_LIBCPP_DEBUG)
#define SEACUDIFF_DEBUG
#endif
#ifdef SEACUDIFF_DEBUG
#include <fmt/format.h>
#endif
#include <algorithm>
#include <absl/hash/hash.h>
#include "diff_standard.h"

namespace seacudiff {
    static uint32_t figure_bits(uint32_t n);
    namespace internal {
        static int to_arr_index(int k, int offset) { return k + offset; }

        template<typename T>
        concept Const = std::is_const<T>::value;
        template<typename T>
        concept NonConst = !std::is_const<T>::value;
        template<typename T, typename U>
        concept Unconsted = NonConst<T> && Const<U> && std::same_as<T, std::remove_const_t<U>>;

        template<typename T>
        class v_wrapper_t {
            gsl::span<T> v_;
            std::size_t offset_;
            std::size_t d_lvl_;
            int delta_;
        public:

            friend class v_wrapper_t<std::add_const_t<T>>;

            template<typename U=T>
            v_wrapper_t(std::vector<U>& v, std::size_t offset, int delta=0) : 
                v_{ v.data(), v.size() }, 
                offset_{ offset }, 
                d_lvl_{ 0 },
                delta_{ delta }
            {}

            template<Unconsted<T> V>
            v_wrapper_t(std::vector<V>& v, std::size_t offset, int delta=0) : 
                v_{ v.data(), v.size() }, 
                offset_{ offset }, 
                d_lvl_{ 0 },
                delta_{delta}
            {}
#if 0
            template<Const V>
            v_wrapper_t(std::vector<std::remove_const_t<V>>& v, std::size_t offset) : v_{ v.data(), v.size() }, offset_{ offset }{}
            template<NonConst V>
            v_wrapper_t(std::vector<std::remove_const_t<V>>& v, std::size_t offset) : v_{ v.data(), v.size() }, offset_{ offset }{}
#endif
            template<NonConst V>
            v_wrapper_t(const v_wrapper_t<V>& other) : v_{ other.v_ }, offset_{ other.offset_ }, d_lvl_{ other.d_lvl_ }, delta_{ other.delta_ }{}

            T& operator[](int i) const { return v_[i + offset_]; }

            v_wrapper_t<T>& operator++() { ++d_lvl_; return *this; }
            v_wrapper_t<T> operator++(int) { auto v = *this; operator++(); return v; }
        };

        //template class v_wrapper_t<int>;
        //template class v_wrapper_t<const int>;
    }
}
struct frp_t {
    int d;
    int k;
    int x;
};
using seq_v = gsl::span<const char>;

template<bool B=false>
std::enable_if_t<B> do_print_vertices(const std::vector<std::array<int,2>>& res, size_t offset, size_t size, int delta, bool extra_newline=false) {
    auto viewie = std::span<std::pair<std::array<int, 2>, std::array<int, 2>>>
    { std::bit_cast<std::pair<std::array<int,2>,std::array<int,2>>*>(res.data() + offset), size };
    std::int64_t d_lvl = (static_cast<int64_t>(size) - 1)* -1;
    std::for_each(viewie.begin(), viewie.end(), [d_lvl, delta](auto& el) mutable {std::cout << fmt::format("[{2:>3}]:{{{0:2}, {1:2}}}", el.first[0], el.first[1], d_lvl+delta) << " "; d_lvl += 2; });
    char c = extra_newline ? '\n' : '\0';
    std::cout << "\n" << c;
}
#ifdef SEACUDIFF_DEBUG
#define PRINT_VERTICES(r, o, s, d, b) do_print_vertices<true>(r, o, s, d, b)
#else
#define PRINT_VERTICES(r, o, s, d, b)
#endif

static int get_forward_frp(int k, seq_v a, seq_v b, int d_lvl,
    seacudiff::internal::v_wrapper_t<const int> v)
{
    auto n = a.size();
    auto m = b.size();

    int x = 0;

    if (k == -d_lvl || (k != d_lvl && v[k - 1] < v[k + 1]))
        x = v[k + 1];
    else
        x = v[k - 1] + 1;

    int y = x - k;
    while (x < n && y < m && a[x] == b[y]) {
        ++x;
        ++y;
    }
    return x;
}
static int get_backward_frp(int k_2, seq_v a, seq_v b, int d_lvl,
    seacudiff::internal::v_wrapper_t<const int> v_2) {
    auto n = a.size();
    auto m = b.size();

    int delta = n - m;
    k_2 += delta;

    int x = n;

    if (k_2 == -d_lvl + delta || (k_2 != d_lvl + delta && v_2[k_2 - 1] > v_2[k_2 + 1]))
        x = v_2[k_2 + 1] - 1;
    else
        x = v_2[k_2 - 1];

    int y = x - k_2;
    int old_x = x;
    while (x > 0 && y > 0 && a[x-1] == b[y-1]) {
        --x;
        --y;
    }
    //return std::min(x + 1, old_x);//x;
    return x;

}



auto seacudiff::impl::e_graph::middle_snake(gsl::span<const char> a, gsl::span<const char> b)->std::pair<int, seacudiff::impl::e_graph::snake_ep_t> {
    auto n = a.size(), m = b.size();
    const int delta = a.size() - b.size();
    const bool deltaIsEven = std::div(delta, 2).rem == 0;

    constexpr auto int_t_min = std::numeric_limits<int>::min();

    using namespace seacudiff;
    auto v_raw_a = std::vector<int>((m + n) * 2 + 1 + std::abs(delta), int_t_min);
    auto v_a = internal::v_wrapper_t<typename decltype(v_raw_a)::value_type>{ v_raw_a, delta < 0 ? m+n+std::abs(delta) : m+n };
    auto res_a = std::vector<std::array<int, 2>>{ v_raw_a.size(), {int_t_min, int_t_min} };
    auto res_view_a = internal::v_wrapper_t<typename decltype(res_a)::value_type>{ res_a, delta < 0 ? m + n + std::abs(delta) : m + n};

    auto v_raw_b = std::vector<int>((m + n) * 2 + 1 + abs(delta), int_t_min);
    auto v_b = internal::v_wrapper_t<typename decltype(v_raw_b)::value_type>{ v_raw_b, delta < 0 ? m + n + std::abs(delta) : m + n, delta };
    auto res_b = std::vector<std::array<int, 2>>{ v_raw_b.size(), {int_t_min, int_t_min} };
    auto res_view_b = internal::v_wrapper_t<typename decltype(res_b)::value_type>{ res_b, delta < 0 ? m + n + std::abs(delta) : m + n, delta };


    v_a[1] = 0; // start with vertex (0, -1)
    v_b[1 + delta] = n + 1;

    int ses_res = 0;

    int sx = 0xcccccccc, sy = 0xcccccccc, su = 0xcccccccc, sv = 0xcccccccc;
    int x1 = 0, y1 = 0;
    int x2 = /*this->*/n, y2 = /*this->*/m;

    for (int d_lvl = 0, hf = std::ceil((a.size() + b.size()) / 2.0); d_lvl <= hf; ++d_lvl) {

        for (int k = -d_lvl; k <= d_lvl; k += 2) {
            x1 = get_forward_frp(k, a, b, d_lvl, v_a);
            y1 = x1 - k;
            v_a[k] = x1;
            res_view_a[k] = { x1, y1 };
            // TODO if d_lvl==0 ... possible corner case?
            if (!deltaIsEven && (k >= delta - (d_lvl - 1) && k <= delta + (d_lvl - 1))) {
                //if (x1 - y1 == x2 - y2 && x1 >= x2) {
                if (v_a[k] >= v_b[k]) {
                    ses_res = (d_lvl * 2) - 1;
                    if (k == -d_lvl) {
                        sx = v_a[k + 1];
                    }
                    else if (k == d_lvl) {
                        sx = v_a[k - 1] + 1;
                    }
                    else {
                        sx = std::max(v_a[k + 1], v_a[k - 1] + 1);
                    }
                    //sx = v_b[k];//v_b[k];
                    sy = sx - k;//sx - k;
                    su = v_a[k];//x1;
                    sv = y1;//y1;
                    PRINT_VERTICES(res_a, (m + n + (delta < 0 ? -delta : 0) - d_lvl), static_cast<size_t>(d_lvl) + 1, 0, true);
                    return { (d_lvl*2) - 1, {sx, sy, su, sv}};
                }
            }
        }
        PRINT_VERTICES(res_a, (m + n + (delta < 0 ? -delta : 0) - d_lvl), static_cast<size_t>(d_lvl) + 1, 0, false);

        for (int k = -d_lvl; k <= d_lvl; k += 2) {
            int k2 = k + delta;
            x2 = get_backward_frp(k, a, b, d_lvl, v_b);
            y2 = x2 - k2;
            v_b[k2] = x2;
            res_view_b[k2] = { x2, y2 };
            if (deltaIsEven && (k2 >= -d_lvl && k2 <= d_lvl)) {
                //if (x2 - y2 == x1 - y1 && x1 >= x2) {
                if (v_a[k2] >= v_b[k2]) {
                    ses_res = (d_lvl * 2);
                    sx = x2;
                    sy = y2;
                    su = v_a[k2];
                    sv = su - k2;
                    PRINT_VERTICES(res_b, (m + n + (delta < 0 ? -delta : 0) - d_lvl + delta), static_cast<size_t>(d_lvl) + 1, delta, true);
                    //STDCOUT_NEWLINE();
                    return { d_lvl * 2, {sx, sy, su, sv} };
                }
            }
        }
        PRINT_VERTICES(res_b, (m + n + (delta < 0 ? -delta : 0) - d_lvl + delta), static_cast<size_t>(d_lvl) + 1, delta, true);
        //STDCOUT_NEWLINE();
        ++v_a;
        ++v_b;
        ++res_view_a;
        ++res_view_b;
    }

    return { -1, {} };
}

auto seacudiff::impl::e_graph::find_lcs(seq_v a, seq_v b)->std::string {
    auto n = a.size(), m = b.size();
    std::string res;
    if (n > 0 && m > 0) {
        auto [d_lvl, s] = middle_snake(a, b);
        if (d_lvl > 1) {
            res.append(find_lcs(a.subspan(0, s.x), b.subspan(0, s.y)));
            res.append(a.data() + s.x, s.u - s.x);
            res.append(find_lcs(a.subspan(s.u), b.subspan(s.v)));
            //auto sa = find_lcs(a.subspan(s.u), b.subspan(s.v));
            return res;
        }
        else if (m > n) {
            return { a.begin(), a.end()};
        }
        else {
            return { b.begin(), b.end() };
        }
    }
    return "";
}

namespace seacudiff {
    class SimpleSequence {
        std::vector<uint16_t> h;
    public:
        explicit SimpleSequence(std::string_view* b, std::string_view* e);
    };
}

seacudiff::SimpleSequence::SimpleSequence(std::string_view* b, std::string_view* e) : h(figure_bits(e-b), 0) {
    //auto maxValBitWidth = figure_bits(e - b);
    //h.reserve(2 * maxValBitWidth);

    absl::Hash<std::string_view> hsh{};
    for (auto& el : h) {
        if (b == e) break;
        el = hsh(*(b++));
    }
}

uint32_t seacudiff::figure_bits(uint32_t n) {
    uint32_t res = 1;
    while (res < n) {
        res *= 2;
    }
    return res;
}
