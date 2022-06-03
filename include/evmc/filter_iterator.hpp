// EVMC: Ethereum Client-VM Connector API.
// Copyright 2022 The EVMC Authors.
// Licensed under the Apache License, Version 2.0.
#pragma once

#include <iterator>

namespace evmc
{
/// The constexpr variant of std::isspace().
inline constexpr bool isspace(char ch) noexcept
{
    // Implementation taken from LLVM's libc.
    return ch == ' ' || (static_cast<unsigned>(ch) - '\t') < 5;
}

inline constexpr bool is_not_space(char ch) noexcept
{
    return !isspace(ch);
}

/// The filter iterator adaptor creates a view of an iterator range in which some elements of the
/// range are skipped. A predicate function controls which elements are skipped. When the predicate
/// is applied to an element, if it returns true then the element is retained and if it returns
/// false then the element is skipped over. When skipping over elements, it is necessary for the
/// filter adaptor to know when to stop so as to avoid going past the end of the underlying range.
/// A filter iterator is therefore constructed with pair of iterators indicating the range of
/// elements in the unfiltered sequence to be traversed.
///
/// Similar to boost::filter_iterator.
template <typename BaseIterator,
          bool predicate(typename std::iterator_traits<BaseIterator>::value_type) noexcept>
struct filter_iterator
{
    using difference_type = typename std::iterator_traits<BaseIterator>::difference_type;
    using value_type = typename std::iterator_traits<BaseIterator>::value_type;
    using pointer = typename std::iterator_traits<BaseIterator>::pointer;
    using reference = typename std::iterator_traits<BaseIterator>::reference;
    using iterator_category = std::input_iterator_tag;

private:
    BaseIterator base;
    BaseIterator base_end;
    value_type value;

    constexpr void forward_to_next_value() noexcept
    {
        for (; base != base_end; ++base)
        {
            value = *base;
            if (predicate(value))
                break;
        }
    }

public:
    constexpr filter_iterator(BaseIterator it, BaseIterator end) noexcept : base{it}, base_end{end}
    {
        forward_to_next_value();
    }

    constexpr auto operator*() noexcept
    {
        // We should not read from an input base iterator twice. So the only read is in
        // forward_to_next_value() and here we return the cached value.
        return value;
    }

    constexpr void operator++() noexcept
    {
        ++base;
        forward_to_next_value();
    }

    constexpr bool operator!=(const filter_iterator& o) noexcept { return base != o.base; }
    constexpr bool operator==(const filter_iterator& o) noexcept { return base == o.base; }
};

/// The input filter iterator which skips whitespace characters from the base input iterator.
template <typename BaseIterator>
struct skip_space_iterator : filter_iterator<BaseIterator, is_not_space>
{
    using filter_iterator<BaseIterator, is_not_space>::filter_iterator;
};

template <typename BaseIterator>
skip_space_iterator(BaseIterator, BaseIterator) -> skip_space_iterator<BaseIterator>;
}  // namespace evmc
