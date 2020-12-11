#include "remove_duplicates.h"

#include <algorithm>
#include <set>
#include <iostream>

void RemoveDuplicates(SearchServer& search_server) {

    const int document_count = search_server.GetDocumentCount();
    std::set<int> dup;
    for (int i = 0; i < document_count; ++i) {
        for (int j = i + 1; j < document_count; ++j) {
            int lhsid = search_server.GetDocumentId(i);
            int rhsid = search_server.GetDocumentId(j);
            const TagData& map1 = search_server.GetWordFrequencies(lhsid);
            const TagData& map2 = search_server.GetWordFrequencies(rhsid);

            std::size_t sz1 = map1.size();
            std::size_t sz2 = map2.size();

            if (sz1 != sz2) {
                continue;
            }

            TagData::const_iterator map1It = map1.begin();
            TagData::const_iterator map2It = map2.begin();

            std::size_t cnt = 0;
            while (map2It != map2.end() && map1It != map1.end()) {
                if (map1It->first != map2It->first) {
                    break;
                }
                if (map1It->first == map2It->first) {
                    ++cnt;
                }
                ++map1It;
                ++map2It;
            }
            if (cnt == sz1) {
                dup.insert(rhsid);
            }
        }
    }
    for (int i : dup) {
        std::cout << "Found duplicate document id " << i << std::endl;
        search_server.RemoveDocument(i);
    }
}