#pragma once

#include <string_view>
#include <utility>

namespace AhoCorasick {

struct Node;

class PrefixTrie {
  Node *root;
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
  const Node *state;
  const char *current;
  const DictEntry *result;
  friend class Searcher;
 public:
  bool operator==(const Iterator&) const = default;
  Iterator& operator++();
  Iterator operator++(int);
  // TODO: It would be nice for `operator*` and `operator->` to be inline,
  // since they are trivial; but then I'd have to expose the structure of
  // `DictEntry`. That wouldn't really be a problem, but I want to avoid it.
  const std::string_view& operator*() const;
  const std::string_view *operator->() const;
};

class Searcher {
  PrefixTrie trie;
 public:
  explicit Searcher(PrefixTrie&&);

  std::pair<Iterator, Iterator> find_all(std::string_view) const;
};

} // namespace AhoCorasick
