#ifndef RECOMMENDATIONSYSTEM_H
#define RECOMMENDATIONSYSTEM_H

#include "Movie.h"
#include "User.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include <ostream>

class RecommendationSystem {
private:
    std::unordered_map<sp_movie, std::vector<double>, decltype(&sp_movie_hash), decltype(&sp_movie_equal)> movies;

public:
    RecommendationSystem() : movies(0, &sp_movie_hash, &sp_movie_equal) {}
    sp_movie add_movie_to_rs(const std::string& name, int year, const std::vector<double>& features);
    sp_movie get_movie(const std::string& name, int year) const;
    const std::vector<double>& get_movie_features(const sp_movie& movie) const;

    // Overloaded functions
    sp_movie recommend_by_content(const rank_map& user_ratings) const;
    sp_movie recommend_by_content(const User& user) const; 
    

    sp_movie recommend_by_cf(const User& user, int k) const;
    double predict_movie_score(const User& user, const sp_movie& movie, int k) const;

    const std::unordered_map<sp_movie, std::vector<double>, decltype(&sp_movie_hash), decltype(&sp_movie_equal)>& get_movies() const;

    friend std::ostream& operator<<(std::ostream& os, const RecommendationSystem& rs);
};

#endif
