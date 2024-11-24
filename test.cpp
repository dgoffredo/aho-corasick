#include <aho_corasick.h>

#include <iostream>
#include <string>
#include <utility>

int main() {
  const char *const words[] = {"a", "aa", "aab", "baa", "baab", "aac"};
  AhoCorasick::Searcher simple{std::begin(words), std::end(words)};
  const char subject[] = "aabaaabaab";
  for (auto [it, end] = simple.find_all(subject); it != end; ++it) {
      std::cout << *it << ": found at offset " << (it->data() - subject) << '\n';
  }

  AhoCorasick::PrefixTrie dictionary;
  dictionary.insert("a");
  dictionary.insert("aa");
  dictionary.insert("aab");
  dictionary.insert("baa");
  dictionary.insert("baab");
  dictionary.insert("aac");

  AhoCorasick::Searcher searcher{std::move(dictionary)};
  std::string text;
  while (std::cout << "> " << std::flush, std::getline(std::cin, text)) {
    for (auto [it, end] = searcher.find_all(text); it != end; ++it) {
      std::cout << *it << ": found at offset " << (it->data() - text.data()) << '\n';
    }
  }
  std::cout << '\n';
}

