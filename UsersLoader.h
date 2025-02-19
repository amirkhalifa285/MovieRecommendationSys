// File: UsersLoader.h
#ifndef USERSLOADER_H
#define USERSLOADER_H

#include "User.h"
#include "RecommendationSystem.h"
#include <string>
#include <vector>


typedef std::unordered_map<sp_movie, double, hash_func, equal_func> rank_map;

class UsersLoader {
public:
    /**
     * Loads users and their ratings from a file and associates them with a RecommendationSystem
     * @param file_path - Path to the input file
     * @param rs - Shared pointer to the RecommendationSystem
     * @return A vector of loaded User objects
     */
    static std::vector<User> create_users(const std::string& file_path, std::shared_ptr<RecommendationSystem> rs);
};

#endif // USERSLOADER_H
