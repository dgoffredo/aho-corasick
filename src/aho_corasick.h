#pragma once

#include <string_view>
#include <utility>

namespace AhoCorasick {

struct DictEntryNode;
struct Node;

class PrefixTrie {
  Node *root;
  DictEntryNode *words;
  friend class Searcher; 
 public:
  PrefixTrie();
  ~PrefixTrie();
  PrefixTrie(PrefixTrie&&);
  PrefixTrie(const PrefixTrie&) = delete;

  void insert(std::string_view word);
};

struct DictEntry;

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
 public:
  explicit Searcher(PrefixTrie&&);

  std::pair<Iterator, Iterator> find_all(std::string_view) const;
};

} // namespace AhoCorasick
