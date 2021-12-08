#pragma once

#include "sqlite3.h"
#include <string>
#include <iostream>
#include <tuple>
#include <memory>


class DBController
{
public:
    DBController(const char* path);
    ~DBController();
    bool IsOpen();
    /*
    * In: Column name, table name, searching conditions
    * Out:Return pair - lengh in bytes and std::unique_ptr with deleter
    */
    decltype(auto) DBController::Load(std::string&& name, std::string&& from, std::string&& Where)
    {
        constexpr std::size_t BufSize = 128;
        char buffer[BufSize];
        sqlite3_stmt* statement = nullptr;

        int res = snprintf(buffer, BufSize, SQL_Request_Select, name.c_str(), from.c_str(), Where.c_str());

        res = sqlite3_prepare(_pDB, buffer, res, &statement, nullptr);

        auto deleter = [statement](void const* data) {sqlite3_finalize(statement); };

        if (res == SQLITE_OK)
            if (sqlite3_step(statement) == SQLITE_ROW)
                return std::make_pair<int, std::unique_ptr<void, decltype(deleter)>>(sqlite3_column_bytes(statement, 0), std::unique_ptr<void, decltype(deleter)>((void*)sqlite3_column_blob(statement, 0), deleter));

        return std::make_pair(0, std::unique_ptr<void, decltype(deleter)>(nullptr, deleter));
    }

private:
    std::string _mpath;
    sqlite3* _pDB{ nullptr };
    bool _bOpen{ false };

    const char* SQL_Request_Select = "SELECT %s FROM %s WHERE %s;";
};