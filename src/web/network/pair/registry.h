#pragma once
#include "Arduino.h"
#include <vector>

struct NetworkPair {
    String name;
    String key;
    int priority;
};
static std::vector<NetworkPair> networkPairs;