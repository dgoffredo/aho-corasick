#include <aho_corasick.h>

// #include <iostream>

int main() {
  AhoCorasick::PrefixTrie dictionary;
  dictionary.insert("a");
  dictionary.insert("aa");
  dictionary.insert("aab");
  dictionary.insert("baa");
  dictionary.insert("baab");
  dictionary.insert("aac");
    
   /*
  AhoCorasick::Searcher searcher{std::move(dictionary)};
  const char text[] = "aabaaabaab";
  for (auto [it, end] = searcher.find_all(text); it != end; ++it) {
   std::cout << *it << ": found at offset " << (it->data() - text) << '\n';
   }
   */
}