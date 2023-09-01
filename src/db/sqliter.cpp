#include "db/sqliter.h"

namespace pine::db {

Sqliter::Sqliter(const std::string &db_name) noexcept
    : db_(make_unique<SQLite::Database>(db_name)) {}


}