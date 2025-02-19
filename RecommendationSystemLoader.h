#ifndef RECOMMENDATIONSYSTEMLOADER_H
#define RECOMMENDATIONSYSTEMLOADER_H

#include "RecommendationSystem.h"
#include <string>
#include <memory>

class RecommendationSystemLoader {
public:
    static std::shared_ptr<RecommendationSystem> create_rs_from_movies(const std::string& file_path);
};

#endif // RECOMMENDATIONSYSTEMLOADER_H
