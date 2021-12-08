#include "../../Headers/Loaders/DBController.h"

DBController::DBController(const char* path):_mpath(path)
{
    if (sqlite3_open(path, &_pDB))
        std::cerr << "Didn't open base date " << sqlite3_errmsg(_pDB) << std::endl;
    _bOpen = !_bOpen;
}

bool DBController::IsOpen() 
{
    return _bOpen;
}

DBController::~DBController()
{
    sqlite3_close(_pDB);
}