#ifndef USER_H
#define USER_H

#include "Movie.h"
#include <unordered_map>
#include <string>
#include <ostream>

class RecommendationSystem;

typedef std::unordered_map<sp_movie, double, hash_func, equal_func> rank_map;

class User {
private:
    std::string name;
    std::shared_ptr<RecommendationSystem> rs; 
    rank_map movie_ratings;

public:
    User(const std::string& name, std::shared_ptr<RecommendationSystem> rs);
    const rank_map& get_rank() const;
    std::string get_name() const;
    void add_movie_to_user(const std::string& name, int year, const std::vector<double>& features, double rating);
    sp_movie get_rs_recommendation_by_content();
    sp_movie get_rs_recommendation_by_cf(int k);
    double get_rs_prediction_score_for_movie(const std::string& name, int year, int k);

    const RecommendationSystem& get_rs() const;
    
    friend std::ostream& operator<<(std::ostream& os, const User& user);
};

#endif 
