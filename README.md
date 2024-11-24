```c++
#include <aho_corasick.h>

#include <iostream>

int main() {
    AhoCorasick::PrefixTrie dictionary;
    dictionary.insert("a");
    dictionary.insert("aa");
    dictionary.insert("aab");
    dictionary.insert("baa");
    dictionary.insert("baab");
    dictionary.insert("aac");
    
    AhoCorasick::Searcher searcher{std::move(dictionary)};
    const char text[] = "aabaaabaab";
    for (auto [it, end] = searcher.find_all(text); it != end; ++it) {
       std::cout << *it << ": found at offset " << (it->data() - text) << '\n';
    }
}
```

```text
a: found at offset 0
aa: found at offset 0
a: found at offset 1
aab: found at offset 0
a: found at offset 3
baa: found at offset 2
aa: found at offset 3
a: found at offset 4
aa: found at offset 4
a: found at offset 5
a: found at offset 7
baa: found at offset 6
aa: found at offset 7
a: found at offset 8
baab: found at offset 6
aab: found at offset 7
```

The searcher examines each character exactly once.
