#pragma once

#include <iostream>
#include <vector>
#include <algorithm>

template <typename Iterator>
class IteratorRange {
public:
    IteratorRange(Iterator range_begin, Iterator range_end)
        : first_(range_begin), last_(range_end), size_(distance(first_, last_))
    {
    }

    Iterator begin() const {
        return first_;
    }

    Iterator end() const {
        return last_;
    }

    Iterator size() const {
        return size_;
    }

private:
    Iterator first_, last_;
    size_t size_;
};

template <typename Iterator>
std::ostream& operator<<(std::ostream& out, const IteratorRange<Iterator>& range) {
    for (Iterator it = range.begin(); it != range.end(); ++it) {
        out << *it;
    }
    return out;
}

template <typename Iterator>
class Paginator {
public:
    Paginator(Iterator pages_begin, Iterator pages_end, size_t page_size) {
        for (size_t left = distance(pages_begin, pages_end); left > 0;) {
            const size_t current_page_size = std::min(page_size, left);
            const Iterator current_pages_end = next(pages_begin, current_page_size);
            pages_.push_back({ pages_begin, current_pages_end });

            left -= current_page_size;
            pages_begin = current_pages_end;
        }
    }

    auto begin() const {
        return pages_.begin();
    }

    auto end() const {
        return pages_.end();
    }

    auto size() const {
        return pages_.size();
    }

private:
    std::vector<IteratorRange<Iterator>> pages_;
};

template <typename Container>
auto Paginate(const Container& c, size_t page_size) {
    return Paginator(begin(c), end(c), page_size);
}