#include <aho_corasick.h>

#include <algorithm>
#include <cassert>
#include <queue>
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

struct DictEntryNode {
  DictEntry entry;
  DictEntryNode *next;
};

PrefixTrie::PrefixTrie()
: root(nullptr), words(nullptr) {}

PrefixTrie::PrefixTrie(PrefixTrie&& other)
: root(other.root), words(other.words) {
  other.root = nullptr;
  other.words = nullptr;
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
    delete node;
  }

  for (DictEntryNode *word = words; word;) {
    auto current = word;
    word = word->next;
    delete current;
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
    words = new DictEntryNode{
      .entry = DictEntry{.word = word},
      .next = words
    };
    node->word = &words->entry; 
  }
}

Searcher::Searcher(PrefixTrie&& dictionary)
: trie(std::move(dictionary)) {
  // Set the `fail` pointers and the `DictEntry::next` pointers by walking the
  // trie breadth-first.

  // `[parent] ----edge----> [node]` when we're visiting `node`.
  struct Visit {
    Node *node;
    Node *parent;
    char edge;
  };
  std::queue<Visit> queue;

  if (!trie.root) {
    return;
  }

  // Traversal begins with the grandchildren of the root. The children of the
  // root have the root as their `fail` node. 
  for (auto& [_, parent] : trie.root->kids) {
    parent->fail = trie.root;
    for (const auto& [edge, kid] : parent->kids) {
      queue.push(Visit{.node = kid, .parent = parent, .edge = edge});
    }
  }
  
  while (!queue.empty()) {
    const auto& [node, parent, edge] = queue.front();
    Node *candidate = parent->fail;
    for (;;) {
      const auto& kids = candidate->kids;
      const auto has_edge = [&](const auto& pair) { return pair.first == edge; };
      auto found = std::find_if(kids.begin(), kids.end(), has_edge);
      if (found != kids.end()) {
        candidate = found->second;
        // `candidate` now points to the longest prefix in the trie that is
        // also a strict suffix of `node`.
        break;
      }
      if (!candidate->fail) {
        // `candidate` is the root node. Nowhere else to go, so that's our fail
        // node.
        break;
      }
      // Look further up the tree.
      candidate = candidate->fail;
    }

    node->fail = candidate;
    // Update the chain of matching words, too, if applicable.
    if (node->word) {
      node->word->next = candidate->word;
    } else {
      node->word = candidate->word;
    }

    // Continue breadth-first.
    for (const auto& [kid_edge, kid] : node->kids) {
      queue.push(Visit{.node = kid, .parent = node, .edge = kid_edge});
    }
    queue.pop();
  };
}

std::pair<Iterator, Iterator> Searcher::find_all(std::string_view text) const {
  if (!trie.root) {
    Iterator null_iter;
    null_iter.state = nullptr;
    null_iter.next = nullptr;
    null_iter.end = nullptr;
    null_iter.word = nullptr;
    return {null_iter, null_iter};
  }

  Iterator begin;
  begin.state = trie.root;
  begin.next = text.data();
  begin.end = text.data() + text.size();
  begin.word = nullptr;
  ++begin; // Search for the first match.

  Iterator end; 
  end.state = nullptr; // not part of iterator comparison
  end.next = text.data() + text.size();
  end.end = end.next;
  end.word = nullptr;

  return {begin, end};
}

Iterator& Iterator::operator++() {
  if (word) {
    word = word->next;
  }

  while (!word && next != end) {
    const char ch = *next;
    ++next;
    const Node *node = state;
    for (;;) {
      const auto char_matches = [&](auto& pair) { return pair.first == ch; };
      const auto found = std::find_if(node->kids.begin(), node->kids.end(), char_matches);
      if (found != node->kids.end()) {
        node = found->second;
        break;
      }
      if (!node->fail) {
        // root node
        break;
      }
      node = node->fail;
    }
    
    word = node->word;
    state = node;
  }

  if (word) {
    const auto size = word->word.size();
    result = std::string_view(next - size, size);
  }

  return *this;
}

Iterator Iterator::operator++(int) {
  Iterator old = *this;
  ++(*this);
  return old;
}

} // namespace AhoCorasick
