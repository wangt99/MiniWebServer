/**
 * @file sqlite.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-07-13
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#pragma once
#include <SQLiteCpp/Database.h>
#include <SQLiteCpp/SQLiteCpp.h>
#include <SQLiteCpp/Statement.h>
#include <SQLiteCpp/VariadicBind.h>
#include <memory>

namespace pine::db {

class Sqliter {
public:
    explicit Sqliter(const std::string &db_name) noexcept;

    Sqliter(const Sqliter &&other)  = delete;
    Sqliter& operator = (const Sqliter&& other)  = delete;

private:
    std::unique_ptr<SQLite::Database> db_;
};

}
