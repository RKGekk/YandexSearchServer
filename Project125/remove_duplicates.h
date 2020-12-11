#pragma once

#include "search_server.h"

using TagData = std::map<std::string, double>;

void RemoveDuplicates(SearchServer& search_server);