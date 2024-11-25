#pragma once

#include <forward_list>
#include <string_view>
#include <utility>

namespace AhoCorasick {

struct DictEntry;
struct Node;

class PrefixTrie {
  Node *root;
  std::forward_list<DictEntry> words;
  friend class Searcher; 
 public:
  PrefixTrie();
  ~PrefixTrie();
  PrefixTrie(PrefixTrie&&);
  PrefixTrie(const PrefixTrie&) = delete;

  void insert(std::string_view word);
};

class Iterator {
  std::string_view result;
  const Node *state;
  const char *next;
  const char *end;
  const DictEntry *word;
  friend class Searcher;
 public:
  bool operator==(const Iterator&) const;
  bool operator!=(const Iterator&) const;
  Iterator& operator++();
  Iterator operator++(int);
  const std::string_view& operator*() const;
  const std::string_view *operator->() const;
};

inline
bool Iterator::operator==(const Iterator& other) const {
  return word == other.word && next == other.next;
}

inline
bool Iterator::operator!=(const Iterator& other) const {
  return word != other.word || next != other.next;
}

inline
const std::string_view& Iterator::operator*() const {
  return result;
}

inline
const std::string_view *Iterator::operator->() const {
  return &result;
}

class Searcher {
  PrefixTrie trie;
  void finish_trie();
 public:
  explicit Searcher(PrefixTrie&&);
  template <typename DictIter, typename DictEndIter>
  Searcher(DictIter begin, DictEndIter end);

  std::pair<Iterator, Iterator> find_all(std::string_view) const;
};

template <typename DictIter, typename DictEndIter>
Searcher::Searcher(DictIter begin, DictEndIter end) {
  for (DictIter it = begin; it != end; ++it) {
    trie.insert(*it);
  }
  finish_trie();
}

} // namespace AhoCorasick
