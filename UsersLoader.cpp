#include "UsersLoader.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>

#define YEAR_SEPARATOR '-'
#define ERROR_MSG "input file is incorrect"
#define INIT_BUCKET_SIZE 8

std::vector<User> UsersLoader::create_users(const std::string &file_path,
                                            std::shared_ptr<RecommendationSystem> rs)
{
    // Keep RS alive if the caller did std::move(rs)
    static std::shared_ptr<RecommendationSystem> keep_alive;
    if (rs)
    {
        keep_alive = rs;
    }

    std::vector<User> users;

    std::ifstream file(file_path);
    if (!file.is_open())
    {
        std::cerr << "[ERROR] Failed to open file: " << file_path << std::endl;
        throw std::runtime_error("Failed to open file: " + file_path);
    }
    std::string line;

    if (!std::getline(file, line))
    {
        std::cerr << "[ERROR] Missing movie names/years line in file\n";
        throw std::runtime_error("Invalid file format: missing movie names line.");
    }

    std::istringstream movie_line(line);
    std::vector<std::pair<std::string, int>> movies;
    {
        std::string movie_with_year;
        while (movie_line >> movie_with_year)
        {
            size_t dash_pos = movie_with_year.find('-');
            if (dash_pos == std::string::npos)
            {
                std::cerr << "[ERROR] Invalid movie token (no '-'): " << movie_with_year << std::endl;
                throw std::runtime_error("Invalid movie name format: " + movie_with_year);
            }
            std::string movie_name = movie_with_year.substr(0, dash_pos);
            int year = std::stoi(movie_with_year.substr(dash_pos + 1));

            movies.emplace_back(movie_name, year);
        }
    }


    while (std::getline(file, line))
    {
        if (line.empty())
        {
            continue;
        }
        std::istringstream user_line(line);

        // read user name
        std::string user_name;
        if (!(user_line >> user_name))
        {
            std::cerr << "[ERROR] Missing user name in line => " << line << std::endl;
            throw std::runtime_error("Invalid file format: missing user name.");
        }
        

        //Create one rank_map for this user's ratings
        rank_map user_ratings(INIT_BUCKET_SIZE, &sp_movie_hash, &sp_movie_equal);

        //For each (m_name, m_year), read a rating (or 'NA')
        int movie_count = 0;
        for (const auto &[m_name, m_year] : movies)
        {
            std::string rating_str;
            if (!(user_line >> rating_str))
            {
                std::cerr << "[ERROR] Missing rating for movie \"" << m_name 
                          << "\" in user line => " << line << std::endl;
                throw std::runtime_error("Invalid file format: missing rating for movie " + m_name);
            }


            if (rating_str == "NA")
            {
                continue;
            }

            try
            {
                sp_movie movie = rs->get_movie(m_name, m_year);
                if (!movie)
                {
                    std::cerr << "[ERROR] rs->get_movie(...) returned nullptr for " 
                              << m_name << " (" << m_year << ")\n";
                    throw std::runtime_error("Movie not found.");
                }

                double rating = std::stod(rating_str);
                if (rating < 0 || rating > 10)
                {
                    std::cerr << "[ERROR] rating " << rating 
                              << " out of [0..10] for movie: " << m_name << "\n";
                    throw std::invalid_argument("Rating must be between 0 and 10.");
                }

                user_ratings[movie] = rating;
                movie_count++;
            }
            catch (const std::exception &e)
            {
                std::cerr << "[EXCEPTION] Error processing movie: " << m_name
                          << " (" << m_year << "). Exception: " << e.what() << "\n";
                throw; // Re-throw
            }
        } // end for each movie

        User user(user_name, rs);

        for (const auto &[movie, rating] : user_ratings)
        {
            try
            {
                const std::vector<double>& feats = rs->get_movie_features(movie);
                

                user.add_movie_to_user(movie->get_name(),
                                       movie->get_year(),
                                       feats,
                                       rating);
            }
            catch (const std::exception &e)
            {
                std::cerr << "[EXCEPTION] Error adding movie to user: " 
                          << movie->get_name()
                          << " (" << movie->get_year() 
                          << "). Exception: " << e.what() << "\n";
                throw;
            }
        }

        users.push_back(std::move(user));
    } 

    file.close();
    return users;
}