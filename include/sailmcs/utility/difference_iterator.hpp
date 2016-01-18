#ifndef SAILMCS_DIFFERENCE_ITERATOR_HPP
#define SAILMCS_DIFFERENCE_ITERATOR_HPP

#include <vector>
#include <utility>
#include <iterator>

namespace sailmcs {
	template<typename T, typename InputIter>
	class difference_iterator : public std::iterator<std::input_iterator_tag, T> {
		public:
			difference_iterator() { }

			difference_iterator(
				InputIter it1,
				InputIter it1_end,
				InputIter it2,
				InputIter it2_end
			)
			: it1(it1)
			, it1_end(it1_end)
			, it2(it2)
			, it2_end(it2_end)
			{
				search();
			}

			bool operator==(const difference_iterator &other) {
				return it1 == other.it1;
			}

			bool operator!=(const difference_iterator &other) {
				return it1 != other.it1;
			}

			const T operator*() { return *it1; }
			const T *operator->() { return &(*it1); }

			// Prefix
			difference_iterator &operator++() {
				advance();
				return *this;
			}

			// Postfix
			difference_iterator operator++(int) {
				difference_iterator out(*this);
				++(*this);
				return out;
			}

		private:
			inline void advance() {
				it1++;
				search();
			}

			inline void search() {
				for(; it1 != it1_end; ++it1) {
					while(it2 != it2_end && *it2 < *it1) ++it2;
					if(it2 == it2_end || *it2 > *it1) break;
				}
			}

			InputIter it1, it1_end;
			InputIter it2, it2_end;
	};

	template<typename T, typename InputIter>
	inline std::pair<
		difference_iterator<T, InputIter>,
		difference_iterator<T, InputIter>
	>
	make_difference_range(
		InputIter list_begin,
		InputIter list_end,
		InputIter exclude_begin,
		InputIter exclude_end
	) {
		return std::make_pair(
			difference_iterator<T, InputIter>(list_begin, list_end, exclude_begin, exclude_end),
			difference_iterator<T, InputIter>(list_end, list_end, exclude_end, exclude_end)
		);
	}
}

#endif
