#include <aho_corasick.h>

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <queue>
#include <vector>

namespace AhoCorasick {

struct Node {
  std::vector<std::pair<char, Node*>> kids;
  Node *fail = nullptr;
  DictEntry *word = nullptr;
};

struct DictEntry {
  std::size_t size;
  DictEntry *suffix = nullptr;
};

PrefixTrie::PrefixTrie()
: root(nullptr) {}

PrefixTrie::PrefixTrie(PrefixTrie&& other)
: root(other.root), words(std::move(other.words)) {
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
    delete node;
  }
}

void PrefixTrie::insert(std::string_view word) {
  assert(!word.empty());

  if (!root) {
    root = new Node;
  }

  Node *node = root;
  for (const char ch : word) {
    auto kid = std::find_if( node->kids.begin(), node->kids.end(),
      [&](const auto& pair) { return pair.first == ch; });
    if (kid != node->kids.end()) {
      node = kid->second;
    } else {
      Node *const new_node = new Node;
      node->kids.emplace_back(ch, new_node);
      node = new_node;
    }
  }

  if (!node->word) {
    words.push_front(DictEntry{.size = word.size()});
    node->word = &words.front(); 
  }
}

namespace {

Node *find_kid(const Node& node, char edge) {
  const auto by_edge = [](const auto& left, const auto& right) {
    return left.first < right.first;
  };
  const auto& kids = node.kids;
  const auto sentinel = std::make_pair(edge, nullptr);
  auto found = std::lower_bound(kids.begin(), kids.end(), sentinel, by_edge);
  if (found == node.kids.end() || found->first != edge) {
    return nullptr;
  }
  return found->second;
}

} // namespace

void Searcher::finish_trie() {
  // Set the `fail` pointers, the `DictEntry::next` pointers, and sort `kids`
  // by walking the trie breadth-first.

  // `parent ----edge----> node` when we're visiting `node`.
  struct Visit {
    Node *node;
    Node *parent;
    char edge;
  };
  std::queue<Visit> queue;

  if (!trie.root) {
    return;
  }

  const auto sort_kids = [](Node& node) {
    std::sort(node.kids.begin(), node.kids.end(), [](const auto& left, const auto& right) {
        return left.first < right.first;
    });
  };

  sort_kids(*trie.root);

  // Traversal begins with the grandchildren of the root. The children of the
  // root have the root as their `fail` node. 
  for (auto& [_, parent] : trie.root->kids) {
    parent->fail = trie.root;
    sort_kids(*parent);
    for (const auto& [edge, kid] : parent->kids) {
      queue.push(Visit{.node = kid, .parent = parent, .edge = edge});
    }
  }
  
  while (!queue.empty()) {
    const auto& [node, parent, edge] = queue.front();
    sort_kids(*node);
    Node *candidate = parent->fail;
    for (;;) {
      Node *kid = find_kid(*candidate, edge);
      if (kid) {
        candidate = kid;
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
      node->word->suffix = candidate->word;
    } else {
      node->word = candidate->word;
    }

    // Continue breadth-first.
    for (const auto& [kid_edge, kid] : node->kids) {
      queue.push(Visit{.node = kid, .parent = node, .edge = kid_edge});
    }
    queue.pop();
  }
}

Searcher::Searcher(PrefixTrie&& dictionary)
: trie(std::move(dictionary)) {
  finish_trie();
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
    word = word->suffix;
  }

  while (!word && next != end) {
    const char ch = *next;
    ++next;
    const Node *node = state;
    for (;;) {
      const Node *kid = find_kid(*node, ch);
      if (kid) {
        node = kid;
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
    result = std::string_view(next - word->size, word->size);
  }

  return *this;
}

Iterator Iterator::operator++(int) {
  Iterator old = *this;
  ++(*this);
  return old;
}

} // namespace AhoCorasick
