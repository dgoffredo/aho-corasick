#include <aho_corasick.h>

#include <algorithm>
#include <cassert>
#include <vector>

namespace AhoCorasick {

struct Node {
  std::vector<std::pair<char, Node*>> kids;
  Node *fail = nullptr;
  DictEntry *word = nullptr;
};

struct DictEntry {
  std::string_view word;
  DictEntry *next = nullptr;
};

PrefixTrie::PrefixTrie()
: root(nullptr) {}

PrefixTrie::PrefixTrie(PrefixTrie&& other)
: root(other.root) {
  other.root = nullptr;
}

PrefixTrie::~PrefixTrie() {
  std::vector<Node*> stack;
  if (root) {
    stack.push_back(root);
  }
  while (!stack.empty()) {
    Node *node = stack.back();
    stack.pop_back();
    for (const auto& [ch, kid] : node->kids) {
      stack.push_back(kid);
    }
    if (node->word) {
      delete node->word;
    }
    delete node;
  }
}

void PrefixTrie::insert(std::string_view word) {
  assert(!word.empty());

  if (!root) {
    root = new Node;
  }

  Node *node = root;
  for (auto p = word.begin(); p != word.end(); ++p) {
    const auto begin = node->kids.begin(), end = node->kids.end();
    const auto char_matches = [&](auto& pair) { return pair.first == *p; };
    const auto kid = std::find_if(begin, end, char_matches);
    if (kid != end) {
      node = kid->second;
    } else {
      Node *const new_node = new Node;
      node->kids.emplace_back(*p, new_node);
      node = new_node;
    }
  }

  if (!node->word) {
    node->word = new DictEntry{.word = word, .next = nullptr};
  }
}

} // namespace AhoCorasick
