
#include "AtlasSQLite.h"
#include "../reputation_impl/AtlasDeviceFeatureType.h"
#include <boost/scope_exit.hpp>

namespace atlas {

namespace {

    const char *SQL_CREATE_TABLES = "BEGIN TRANSACTION;"\
                                    "CREATE TABLE IF NOT EXISTS Device("  \
                                    "Id INTEGER PRIMARY KEY AUTOINCREMENT," \
                                    "Identity TEXT NOT NULL UNIQUE," \
                                    "PSK TEXT NOT NULL );" \

                                    "CREATE TABLE IF NOT EXISTS FirewallStatistics("  \
                                    "Id INTEGER PRIMARY KEY AUTOINCREMENT," \
                                    "DeviceId INTEGER NOT NULL," \
                                    "RuleDroppedPkts INTEGER DEFAULT 0," \
                                    "RulePassedPkts INTEGER DEFAULT 0," \
                                    "TxDroppedPkts INTEGER DEFAULT 0," \
                                    "TxPassedPkts INTEGER DEFAULT 0," \
                                    "FOREIGN KEY (DeviceId) REFERENCES Device(Id));" \

                                    "CREATE TABLE IF NOT EXISTS NaiveBayesNetwork("  \
                                    "Id INTEGER PRIMARY KEY AUTOINCREMENT," \
                                    "DeviceId INTEGER NOT NULL," \
                                    "NetworkTypeId INTEGER NOT NULL," \
                                    "TotalTrans INTEGER DEFAULT 0," \
                                    "TotalSuccessTrans INTEGER DEFAULT 0," \
                                    "FOREIGN KEY (DeviceId) REFERENCES Device(Id));" \

                                    "CREATE TABLE IF NOT EXISTS NaiveBayesFeature("  \
                                    "Id INTEGER PRIMARY KEY AUTOINCREMENT," \
                                    "NetworkId INTEGER NOT NULL," \
                                    "FeatureTypeId INTEGER NOT NULL," \
                                    "SuccessTrans INTEGER DEFAULT 0," \
                                    "Weight REAL DEFAULT 0," \
                                    "FOREIGN KEY (NetworkId) REFERENCES NaiveBayesNetwork(Id));"\

                                    "CREATE TABLE IF NOT EXISTS Owner("  \
                                    "Id INTEGER PRIMARY KEY AUTOINCREMENT," \
                                    "SecretKey TEXT NOT NULL," \
                                    "Identity TEXT NOT NULL );" \

                                    "CREATE TABLE IF NOT EXISTS GatewayMisc("  \
                                    "Id INTEGER PRIMARY KEY AUTOINCREMENT," \
                                    "MaxSequenceNumber INTEGER NOT NULL);" \

                                    "INSERT INTO GatewayMisc(MaxSequenceNumber)" \
                                    "SELECT 0 WHERE NOT EXISTS(SELECT 1 FROM GatewayMisc WHERE Id = 1);" \

                                    "CREATE TABLE IF NOT EXISTS DeviceCommand("  \
                                    "Id INTEGER PRIMARY KEY AUTOINCREMENT," \
                                    "DeviceId INTEGER NOT NULL," \
                                    "SequenceNumber INTEGER NOT NULL," \
                                    "CommandType TEXT NOT NULL," \
                                    "CommandPayload TEXT NULL," \
                                    "IsExecuted INTEGER DEFAULT 0," \
                                    "FOREIGN KEY (DeviceId) REFERENCES Device(Id));" \

                                    "CREATE UNIQUE INDEX IF NOT EXISTS idxDeviceIdentity "\
                                    "ON Device(Identity);"\
                                    "COMMIT;";

    const char *SQL_INSERT_STATS = "INSERT INTO FirewallStatistics(DeviceId, RuleDroppedPkts, RulePassedPkts, TxDroppedPkts, TxPassedPkts) VALUES(?,?,?,?,?);";

    const char *SQL_UPDATE_STATS = "UPDATE FirewallStatistics "\
                                   "SET RuleDroppedPkts=?, RulePassedPkts=?, "\
                                   "TxDroppedPkts=?, TxPassedPkts=? "\
                                   "WHERE DeviceId IN (SELECT Id FROM Device WHERE Identity=?);";

    const char *SQL_GET_STATS = "SELECT RuleDroppedPkts, RulePassedPkts, TxDroppedPkts, TxPassedPkts "\
                                "FROM FirewallStatistics "\
                                "WHERE DeviceId IN (SELECT Id FROM Device WHERE Identity=?);";
                                    
    const char *SQL_CHECK_STATS = "SELECT 1 FROM FirewallStatistics "\
                                  "INNER JOIN Device ON Device.Id == FirewallStatistics.DeviceId "\
                                  "WHERE Device.Identity=?;";

    const char *SQL_INSERT_DEVICE = "INSERT INTO Device(Identity,PSK) VALUES(?,?);";
    const char *SQL_UPDATE_DEVICE = "UPDATE Device SET PSK=? WHERE Identity=?;";
    const char *SQL_GET_ID_DEVICE = "SELECT Id FROM Device WHERE Identity=?;";
    const char *SQL_SELECT_DEVICE = "SELECT PSK FROM Device WHERE Identity=?;";

    
    const char *SQL_INSERT_NETWORK = "INSERT INTO NaiveBayesNetwork(DeviceId, NetworkTypeId, TotalTrans, TotalSuccessTrans) VALUES(?,?,?,?);";

    const char *SQL_UPDATE_NETWORK = "UPDATE NaiveBayesNetwork "\
                                     "SET TotalTrans=?, TotalSuccessTrans=? "\
                                     "WHERE DeviceId IN (SELECT Id FROM Device WHERE Identity=?) AND NetworkTypeId=?;";

    const char *SQL_GET_ID_NETWORK = "SELECT NaiveBayesNetwork.Id FROM NaiveBayesNetwork "\
                                     "INNER JOIN Device ON Device.Id == NaiveBayesNetwork.DeviceId "\
                                     "WHERE Device.Identity=? AND NaiveBayesNetwork.NetworkTypeId=?;";

    const char *SQL_GET_NETWORK = "SELECT NaiveBayesNetwork.TotalTrans, NaiveBayesNetwork.TotalSuccessTrans FROM NaiveBayesNetwork "\
                                  "INNER JOIN Device ON Device.Id == NaiveBayesNetwork.DeviceId "\
                                  "WHERE Device.Identity=? AND NaiveBayesNetwork.NetworkTypeId=?;";


    const char *SQL_INSERT_FEATURE = "INSERT INTO NaiveBayesFeature(NetworkId, FeatureTypeId, SuccessTrans, Weight) VALUES(?,?,?,?);";

    const char *SQL_UPDATE_FEATURE = "UPDATE NaiveBayesFeature "\
                                     "SET SuccessTrans=? "\
                                     "WHERE NetworkId IN "\
                                     "(SELECT NaiveBayesNetwork.Id FROM NaiveBayesNetwork "\
                                     "INNER JOIN Device ON Device.Id == NaiveBayesNetwork.DeviceId "\
                                     "WHERE Device.Identity=? AND NaiveBayesNetwork.NetworkTypeId=?) "\
                                     "AND FeatureTypeId=?;";

    const char *SQL_GET_FEATURE = "SELECT NaiveBayesFeature.FeatureTypeId, NaiveBayesFeature.Weight, NaiveBayesFeature.SuccessTrans FROM NaiveBayesFeature "\
                                  "INNER JOIN NaiveBayesNetwork ON NaiveBayesNetwork.Id == NaiveBayesFeature.NetworkId "\
                                  "INNER JOIN Device ON Device.Id == NaiveBayesNetwork.DeviceId "\
                                  "WHERE Device.Identity=? AND NaiveBayesNetwork.NetworkTypeId=?;";

    const char *SQL_CHECK_FEATURE = "SELECT 1 FROM NaiveBayesFeature "\
                                    "INNER JOIN NaiveBayesNetwork ON NaiveBayesNetwork.Id == NaiveBayesFeature.NetworkId "\
                                    "INNER JOIN Device ON Device.Id == NaiveBayesNetwork.DeviceId "\
                                    "WHERE Device.Identity=? AND NaiveBayesNetwork.NetworkTypeId=?;";

    const char *SQL_INSERT_OWNER = "INSERT INTO Owner(SecretKey, Identity) VALUES (?,?);";

    const char *SQL_GET_OWNER = "SELECT Owner.SecretKey, Owner.Identity FROM Owner;";

    const char *SQL_INSERT_DEVICE_COMMAND = "INSERT INTO DeviceCommand(DeviceId, SequenceNumber, CommandType, CommandPayload) VALUES (?,?,?,?);";
    const char *SQL_CHECK_DEVICE_COMMAND_BY_SEQ_NO =  "SELECT 1 FROM DeviceCommand WHERE SequenceNumber=?;";
    const char *SQL_CHECK_DEVICE_COMMAND_BY_IDENTITY =  "SELECT 1 FROM DeviceCommand "\
                                                        "INNER JOIN Device ON Device.Id == DeviceCommand.DeviceId "\
                                                        "WHERE Device.Identity=?;";
    const char *SQL_CHECK_DEVICE_COMMAND_EXECUTION_BY_SEQ_NO =  "SELECT 1 FROM DeviceCommand WHERE SequenceNumber=? AND IsExecuted=1;";
    const char *SQL_MARK_AS_EXECUTED_DEVICE_COMMAND =  "UPDATE DeviceCommand SET IsExecuted=1 WHERE SequenceNumber=?;";
    const char *SQL_GET_DEVICE_COMMAND_BY_IDENTITY = "SELECT DeviceCommand.SequenceNumber, DeviceCommand.CommandType, DeviceCommand.CommandPayload, DeviceCommand.IsExecuted FROM DeviceCommand "\
                                                     "INNER JOIN Device ON Device.Id == DeviceCommand.DeviceId "\
                                                     "WHERE Device.Identity=?;";
    const char *SQL_DELETE_DEVICE_COMMAND_BY_SEQ_NO = "DELETE FROM DeviceCommand "\
                                                      "WHERE DeviceCommand.SequenceNumber=?;";

    const char *SQL_GET_MAX_SEQ_NO =    "SELECT GatewayMisc.MaxSequenceNumber FROM GatewayMisc;";
    const char *SQL_UPDATE_MAX_SEQ_NO =    "UPDATE GatewayMisc SET MaxSequenceNumber=?;";

} // anonymous namespace

AtlasSQLite& AtlasSQLite::getInstance()
{
    static AtlasSQLite instance;

    return instance;
}

AtlasSQLite::AtlasSQLite()
{
    isConnected_ = false;
    pCon_ = nullptr;
}

AtlasSQLite::~AtlasSQLite()
{
    closeConnection();
}

void AtlasSQLite::closeConnection()
{
    if(!pCon_)
        return;

    sqlite3_close(pCon_);

    pCon_ = nullptr;

    ATLAS_LOGGER_DEBUG("Connection closed for local.db");
}

bool AtlasSQLite::isConnected()
{
    return isConnected_;
}

bool AtlasSQLite::initDB(const std::string &databasePath)
{
    bool commit = false;

    /* it will be executed either in the end or on any exception*/
    BOOST_SCOPE_EXIT(&commit, this_) {
        if(!commit) {
            sqlite3_close(this_->pCon_);
        }
    } BOOST_SCOPE_EXIT_END

    /* Open database, or create */
    if(sqlite3_open(databasePath.c_str(), &pCon_) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Cannot open " + databasePath + ", error:" + sqlite3_errmsg(pCon_));
        return false;
    }

    /* Constraints are enabled per connection*/
    if(sqlite3_db_config(pCon_, SQLITE_DBCONFIG_ENABLE_FKEY, 1, 0) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Cannot set PRAGMA FKEY on " + databasePath + ", stmt:SQLITE_DBCONFIG_ENABLE_FKEY, error:" + sqlite3_errmsg(pCon_));
        return false;
    }

    /* Create tables if not exist*/
    if(sqlite3_exec(pCon_,SQL_CREATE_TABLES, NULL, 0, 0) != SQLITE_OK ) {
        ATLAS_LOGGER_ERROR("Cannot create tables on " + databasePath + ", stmt:SQL_CREATE_TABLES, error:" + sqlite3_errmsg(pCon_));
        return false;
    } 

    commit = true;
    return true;
}

bool AtlasSQLite::openConnection(const std::string &databasePath)
{
    isConnected_ = true;

    if(!initDB(databasePath))
    {
        isConnected_ = false;
        return isConnected_;
    }   

    ATLAS_LOGGER_DEBUG("Connection opened for local.db");

    return isConnected_;
}

bool AtlasSQLite::insertDevice(const std::string &identity, const  std::string &psk)
{
    sqlite3_stmt *stmt = nullptr;

    BOOST_SCOPE_EXIT(&stmt) {
        sqlite3_finalize(stmt);
    } BOOST_SCOPE_EXIT_END
    
    if(!isConnected_)
        return false;
	
    /*check unique value for identity*/
    if(sqlite3_prepare_v2(pCon_, SQL_GET_ID_DEVICE,  -1, &stmt, 0) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not prepare, fct:insertDevice, stmt:SQL_GET_ID_DEVICE, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    if (sqlite3_bind_text(stmt, 1, identity.c_str(), identity.length(), SQLITE_STATIC) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not bind, fct:insertDevice, stmt:SQL_GET_ID_DEVICE, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    int stat = sqlite3_step(stmt);
    if (stat != SQLITE_DONE && stat != SQLITE_ROW) {
        ATLAS_LOGGER_ERROR("Could not step, fct:insertDevice, stmt:SQL_GET_ID_DEVICE, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    sqlite3_reset(stmt);

    if(stat != SQLITE_ROW) {

        /*insert identity*/
        if(sqlite3_prepare_v2(pCon_, SQL_INSERT_DEVICE, -1, &stmt, 0) != SQLITE_OK) {
            ATLAS_LOGGER_ERROR("Could not prepare, fct:insertDevice, stmt:SQL_INSERT_DEVICE, error:" + std::string(sqlite3_errmsg(pCon_)));
            return false;
        }

        if (sqlite3_bind_text(stmt, 1, identity.c_str(), identity.length(), SQLITE_STATIC) != SQLITE_OK ||
	        sqlite3_bind_text(stmt, 2, psk.c_str(), psk.length(), SQLITE_STATIC) != SQLITE_OK) {
            ATLAS_LOGGER_ERROR("Could not bind, fct:insertDevice, stmt:SQL_INSERT_DEVICE, error:" + std::string(sqlite3_errmsg(pCon_)));
            return false;
	    }

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            ATLAS_LOGGER_ERROR("Could not step, fct:insertDevice, stmt:SQL_INSERT_DEVICE, error:" + std::string(sqlite3_errmsg(pCon_)));
            return false;
	    }

    } else {

        /*update identity*/	
        if(sqlite3_prepare_v2(pCon_, SQL_UPDATE_DEVICE,  -1, &stmt, 0) != SQLITE_OK) {
            ATLAS_LOGGER_ERROR("Could not prepare, fct:insertDevice, stmt:SQL_UPDATE_DEVICE, error:" + std::string(sqlite3_errmsg(pCon_)));
            return false;
        }

        if (sqlite3_bind_text(stmt, 1, psk.c_str(), psk.length(), SQLITE_STATIC) != SQLITE_OK ||
	        sqlite3_bind_text(stmt, 2, identity.c_str(), identity.length(), SQLITE_STATIC) != SQLITE_OK) {
            ATLAS_LOGGER_ERROR("Could not bind, fct:insertDevice, stmt:SQL_UPDATE_DEVICE, error:" + std::string(sqlite3_errmsg(pCon_)));
	        return false;
	    }

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            ATLAS_LOGGER_ERROR("Could not step, fct:insertDevice, stmt:SQL_UPDATE_DEVICE, error:" + std::string(sqlite3_errmsg(pCon_)));
            return false;
	    }
    }

    return true;
}

bool AtlasSQLite::insertBayesParams(const std::string &identity, int networkTypeId, AtlasDeviceFeatureManager &manager)
{
    if ((!insertNetwork(identity, networkTypeId, manager)) ||
        (!insertFeatures(identity, networkTypeId, manager))) {
        return false;
    }
    return true;
}

bool AtlasSQLite::insertNetwork(const std::string &identity, int networkTypeId, AtlasDeviceFeatureManager &manager)
{
    sqlite3_stmt *stmt = nullptr;
    int stat = -1, deviceId = -1;

    BOOST_SCOPE_EXIT(&stmt) {
        sqlite3_finalize(stmt);
    } BOOST_SCOPE_EXIT_END
    
    if(!isConnected_)
        return false;
	
    /*get deviceId from db*/
    if(sqlite3_prepare_v2(pCon_, SQL_GET_ID_DEVICE,  -1, &stmt, 0) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not prepare, fct:insertNetwork, stmt:SQL_GET_ID_DEVICE, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    if (sqlite3_bind_text(stmt, 1, identity.c_str(), identity.length(),	SQLITE_STATIC) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not bind, fct:insertNetwork, stmt:SQL_GET_ID_DEVICE, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    stat = sqlite3_step(stmt);
    if (stat != SQLITE_DONE && stat != SQLITE_ROW) {
        ATLAS_LOGGER_ERROR("Could not step, fct:insertNetwork, stmt:SQL_GET_ID_DEVICE, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }
    deviceId = sqlite3_column_int(stmt, 0);
    sqlite3_reset(stmt);

    /*insert network*/	
    if(sqlite3_prepare_v2(pCon_, SQL_INSERT_NETWORK,  -1, &stmt, 0) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not prepare, fct:insertNetwork, stmt:SQL_INSERT_NETWORK, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    if (sqlite3_bind_int(stmt, 1, deviceId) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 2, networkTypeId) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 3, manager.getTotalTransactions()) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 4, manager.getTotalSuccessfulTransactions()) != SQLITE_OK) { 
        ATLAS_LOGGER_ERROR("Could not bind, fct:insertNetwork, stmt:SQL_INSERT_NETWORK, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        ATLAS_LOGGER_ERROR("Could not step, fct:insertNetwork, stmt:SQL_INSERT_NETWORK, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }
    
    return true;
}

bool AtlasSQLite::insertFeatures(const std::string &identity, int networkTypeId, AtlasDeviceFeatureManager &manager)
{
    sqlite3_stmt *stmt = nullptr;
    int stat = -1, networkId = -1;
    
    BOOST_SCOPE_EXIT(&stmt) {
        sqlite3_finalize(stmt);
    } BOOST_SCOPE_EXIT_END

    if(!isConnected_)
        return false;
	
    /*get networkId fron db*/
    if(sqlite3_prepare_v2(pCon_, SQL_GET_ID_NETWORK,  -1, &stmt, 0) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not prepare, fct:insertFeature, stmt:SQL_GET_ID_NETWORK, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    if (sqlite3_bind_text(stmt, 1, identity.c_str(), identity.length(),	SQLITE_STATIC) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 2, networkTypeId) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not bind, fct:insertFeature, stmt:SQL_GET_ID_NETWORK, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    stat = sqlite3_step(stmt);
    if (stat != SQLITE_DONE && stat != SQLITE_ROW) {
        ATLAS_LOGGER_ERROR("Could not step, fct:insertFeature, stmt:SQL_GET_ID_NETWORK, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }
    networkId = sqlite3_column_int(stmt, 0);
    sqlite3_reset(stmt);

    /*insert feature*/	
    if(sqlite3_prepare_v2(pCon_, SQL_INSERT_FEATURE,  -1, &stmt, 0) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not prepare, fct:insertFeature, stmt:SQL_INSERT_FEATURE, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    for (auto &feature : manager.getDeviceFeatures())
    {
        if (sqlite3_bind_int(stmt, 1, networkId) != SQLITE_OK ||
            sqlite3_bind_int(stmt, 2, int(feature.getFeatureType())) != SQLITE_OK ||
            sqlite3_bind_int(stmt, 3, feature.getSuccessfulTransactions()) != SQLITE_OK ||
            sqlite3_bind_double(stmt, 4, feature.getWeight()) != SQLITE_OK) {
            ATLAS_LOGGER_ERROR("Could not bind, fct:insertFeature, stmt:SQL_INSERT_FEATURE, error:" + std::string(sqlite3_errmsg(pCon_)));
            return false;
        }

        stat = sqlite3_step(stmt);
        if (stat == SQLITE_DONE) {
            sqlite3_reset(stmt);
            continue;
        }

        if (stat != SQLITE_ROW) {
            ATLAS_LOGGER_ERROR("Could not step, fct:insertFeature, stmt:SQL_INSERT_FEATURE, error:" + std::string(sqlite3_errmsg(pCon_)));
            return false;
        }
    }

    return true;
}

 bool AtlasSQLite::insertStats(const std::string &identity, const AtlasFirewallStats &stats)
 {
     sqlite3_stmt *stmt = nullptr;
    int stat = -1, deviceId = -1;

    BOOST_SCOPE_EXIT(&stmt) {
        sqlite3_finalize(stmt);
    } BOOST_SCOPE_EXIT_END
    
    if(!isConnected_)
        return false;
	
    /*get deviceId fron db*/
    if(sqlite3_prepare_v2(pCon_, SQL_GET_ID_DEVICE,  -1, &stmt, 0) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not prepare, fct:insertStats, stmt:SQL_GET_ID_DEVICE, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    if (sqlite3_bind_text(stmt, 1, identity.c_str(), identity.length(),	SQLITE_STATIC) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not bind, fct:insertStats, stmt:SQL_GET_ID_DEVICE, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    stat = sqlite3_step(stmt);
    if (stat != SQLITE_DONE && stat != SQLITE_ROW) {
        ATLAS_LOGGER_ERROR("Could not step, fct:insertStats, stmt:SQL_GET_ID_DEVICE, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }
    deviceId = sqlite3_column_int(stmt, 0);
    sqlite3_reset(stmt);

    /*insert stats*/	
    if(sqlite3_prepare_v2(pCon_, SQL_INSERT_STATS,  -1, &stmt, 0) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not prepare, fct:insertStats, stmt:SQL_INSERT_STATS, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    if (sqlite3_bind_int(stmt, 1, deviceId) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 2, stats.getRuleDroppedPkts()) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 3, stats.getRulePassedPkts()) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 4, stats.getTxDroppedPkts()) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 5, stats.getTxPassedPkts()) != SQLITE_OK) { 
        ATLAS_LOGGER_ERROR("Could not bind, fct:insertStats, stmt:SQL_INSERT_STATS, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        ATLAS_LOGGER_ERROR("Could not step, fct:insertStats, stmt:SQL_INSERT_STATS, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }
    
    return true;
 }

std::string AtlasSQLite::selectDevicePsk(const std::string &identity) 
{
    sqlite3_stmt *stmt = nullptr;
    std::string res = "";
    
    BOOST_SCOPE_EXIT(&stmt) {
        sqlite3_finalize(stmt);
    } BOOST_SCOPE_EXIT_END

    if(!isConnected())
        return "";

    /*select PSK for given identity*/
    if(sqlite3_prepare_v2(pCon_, SQL_SELECT_DEVICE,-1, &stmt, 0) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not prepare, fct:selectDevicePsk, stmt:SQL_SELECT_DEVICE, error:" + std::string(sqlite3_errmsg(pCon_)));
	    return "";
    }

    if (sqlite3_bind_text(stmt, 1, identity.c_str(), identity.length(),	SQLITE_STATIC) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not bind, fct:selectDevicePsk, stmt:SQL_SELECT_DEVICE, error:" + std::string(sqlite3_errmsg(pCon_)));
        return "";
    }

    int stat = sqlite3_step(stmt);
    if (stat != SQLITE_DONE && stat != SQLITE_ROW) {
        ATLAS_LOGGER_ERROR("Could not step, fct:selectDevicePsk, stmt:SQL_SELECT_DEVICE, error:" + std::string(sqlite3_errmsg(pCon_)));
	    return "";
    }

    if (stat == SQLITE_ROW && sqlite3_column_text(stmt, 0))
        res = std::string(reinterpret_cast<const char *> (sqlite3_column_text(stmt, 0)));

    return res;
}

bool AtlasSQLite::selectBayesParams(const std::string &identity, int networkTypeId, AtlasDeviceFeatureManager &manager)
{
    if ((!selectNetwork(identity, networkTypeId, manager)) ||
        (!selectFeatures(identity, networkTypeId, manager))) {
           return false;
       }
    return true;
}

bool AtlasSQLite::selectNetwork(const std::string &identity, int networkTypeId,  AtlasDeviceFeatureManager &manager) 
{
    sqlite3_stmt *stmt = nullptr;

    BOOST_SCOPE_EXIT(&stmt) {
        sqlite3_finalize(stmt);
    } BOOST_SCOPE_EXIT_END

    if(!isConnected())
        return false;

    /*select network for given identity*/
    if(sqlite3_prepare_v2(pCon_, SQL_GET_NETWORK,-1, &stmt, 0) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not prepare, fct:selectNetwork, stmt:SQL_GET_NETWORK, error:" + std::string(sqlite3_errmsg(pCon_)));
	    return false;
    }

    if (sqlite3_bind_text(stmt, 1, identity.c_str(), identity.length(),	SQLITE_STATIC) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 2, networkTypeId) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not bind, fct:selectNetwork, stmt:SQL_GET_NETWORK, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    int stat = sqlite3_step(stmt);
    if (stat != SQLITE_DONE && stat != SQLITE_ROW) {
        ATLAS_LOGGER_ERROR("Could not step, fct:selectNetwork, stmt:SQL_GET_NETWORK, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    if (stat == SQLITE_ROW) {
        manager.setTotalTransactions(sqlite3_column_int(stmt, 0));
        manager.setTotalSuccessfulTransactions(sqlite3_column_int(stmt, 1));
    }

    return true;
}

bool AtlasSQLite::selectFeatures(const std::string &identity, int networkTypeId,  AtlasDeviceFeatureManager &manager) 
{
    sqlite3_stmt *stmt = nullptr;
    int stat = -1;
    
    BOOST_SCOPE_EXIT(&stmt) {
        sqlite3_finalize(stmt);
    } BOOST_SCOPE_EXIT_END

    if(!isConnected())
        return false;

    /*select features for given identity*/
    if(sqlite3_prepare_v2(pCon_, SQL_GET_FEATURE,-1, &stmt, 0) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not prepare, fct:selectFeatures, stmt:SQL_GET_FEATURE, error:" + std::string(sqlite3_errmsg(pCon_)));
	    return false;
    }

    if (sqlite3_bind_text(stmt, 1, identity.c_str(), identity.length(),	SQLITE_STATIC) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 2, networkTypeId) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not bind, fct:selectFeatures, stmt:SQL_GET_FEATURE, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    for (;;)
    {
        stat = sqlite3_step(stmt);
        if (stat == SQLITE_DONE) {
            break;
        }

        if(stat != SQLITE_ROW) {
            ATLAS_LOGGER_ERROR("Could not step, fct:selectFeatures, stmt:SQL_GET_FEATURE, error:" + std::string(sqlite3_errmsg(pCon_)));
            return false;
        }
        manager.addFeature((AtlasDeviceFeatureType)sqlite3_column_int(stmt, 0), sqlite3_column_double(stmt, 1));
        manager[(AtlasDeviceFeatureType)sqlite3_column_int(stmt, 0)].setSuccessfulTransactions(sqlite3_column_int(stmt, 2));
    }
    
    return true;
}

bool AtlasSQLite::selectStats(const std::string &identity, AtlasFirewallStats &stats)
{
    sqlite3_stmt *stmt = nullptr;

    BOOST_SCOPE_EXIT(&stmt) {
        sqlite3_finalize(stmt);
    } BOOST_SCOPE_EXIT_END

    if(!isConnected())
        return false;

    /*select statistics for given identity*/
    if(sqlite3_prepare_v2(pCon_, SQL_GET_STATS,-1, &stmt, 0) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not prepare, fct:selectStats, stmt:SQL_GET_STATS, error:" + std::string(sqlite3_errmsg(pCon_)));
	    return false;
    }

    if (sqlite3_bind_text(stmt, 1, identity.c_str(), identity.length(),	SQLITE_STATIC) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not bind, fct:selectStats, stmt:SQL_GET_STATS, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    int stat = sqlite3_step(stmt);
    if (stat != SQLITE_DONE && stat != SQLITE_ROW) {
        ATLAS_LOGGER_ERROR("Could not step, fct:selectStats, stmt:SQL_GET_STATS, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    if (stat == SQLITE_ROW) {
        stats.addRuleDroppedPkts(sqlite3_column_int(stmt, 0));
        stats.addRulePassedPkts(sqlite3_column_int(stmt, 1));
        stats.addTxDroppedPkts(sqlite3_column_int(stmt, 2));
        stats.addTxPassedPkts(sqlite3_column_int(stmt, 3));
    }

    return true;
}

bool AtlasSQLite::updateBayesParams(const std::string &identity, int networkTypeId, AtlasDeviceFeatureManager &manager)
{
    if ((!updateNetwork(identity, networkTypeId, manager)) ||
        (!updateFeatures(identity, networkTypeId, manager))) {
        return false;
    }
    return true;
}

bool AtlasSQLite::updateNetwork(const std::string &identity, int networkTypeId,  AtlasDeviceFeatureManager &manager) 
{
    sqlite3_stmt *stmt = nullptr;
    
    BOOST_SCOPE_EXIT(&stmt) {
        sqlite3_finalize(stmt);
    } BOOST_SCOPE_EXIT_END

    if(!isConnected())
        return false;

    /*update network for given identity*/
    if(sqlite3_prepare_v2(pCon_, SQL_UPDATE_NETWORK,-1, &stmt, 0) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not prepare, fct:updateNetwork, stmt:SQL_UPDATE_NETWORK, error:" + std::string(sqlite3_errmsg(pCon_)));
	    return false;
    }

    if (sqlite3_bind_int(stmt, 1, manager.getTotalTransactions()) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 2, manager.getTotalSuccessfulTransactions()) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 3, identity.c_str(), identity.length(), SQLITE_STATIC) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 4, networkTypeId) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not bind, fct:updateNetwork, stmt:SQL_UPDATE_NETWORK, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    int stat = sqlite3_step(stmt);
    if (stat != SQLITE_DONE && stat != SQLITE_ROW) {
        ATLAS_LOGGER_ERROR("Could not step, fct:updateNetwork, stmt:SQL_UPDATE_NETWORK, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    return true;
}

bool AtlasSQLite::updateFeatures(const std::string &identity, int networkTypeId,  atlas::AtlasDeviceFeatureManager &manager) 
{
    sqlite3_stmt *stmt = nullptr;
    int stat = -1;
    
    BOOST_SCOPE_EXIT(&stmt) {
        sqlite3_finalize(stmt);
    } BOOST_SCOPE_EXIT_END

    if(!isConnected())
        return false;

    /*update features for given identity*/
    if(sqlite3_prepare_v2(pCon_, SQL_UPDATE_FEATURE,-1, &stmt, 0) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not prepare, fct:updateFeatures, stmt:SQL_UPDATE_FEATURE, error:" + std::string(sqlite3_errmsg(pCon_)));
	    return false;
    }

    for (auto &feature : manager.getDeviceFeatures())
    {
        if (sqlite3_bind_int(stmt, 1, feature.getSuccessfulTransactions()) != SQLITE_OK ||
            sqlite3_bind_text(stmt, 2, identity.c_str(), identity.length(),	SQLITE_STATIC) != SQLITE_OK ||
            sqlite3_bind_int(stmt, 3, networkTypeId) != SQLITE_OK ||
            sqlite3_bind_int(stmt, 4, int(feature.getFeatureType())) != SQLITE_OK) {
            ATLAS_LOGGER_ERROR("Could not bind, fct:updateFeatures, stmt:SQL_UPDATE_FEATURE, error:" + std::string(sqlite3_errmsg(pCon_)));
            return false;
        }

        stat = sqlite3_step(stmt);
        if (stat == SQLITE_DONE) {
            sqlite3_reset(stmt);
            continue;
        }

        if (stat != SQLITE_ROW) {
            ATLAS_LOGGER_ERROR("Could not step, fct:updateFeatures, stmt:SQL_UPDATE_FEATURE, error:" + std::string(sqlite3_errmsg(pCon_)));
            return false;
        }
    }

    return true;
}

bool AtlasSQLite::updateStats(const std::string &identity, const AtlasFirewallStats &stats)
{
    sqlite3_stmt *stmt = nullptr;
    
    BOOST_SCOPE_EXIT(&stmt) {
        sqlite3_finalize(stmt);
    } BOOST_SCOPE_EXIT_END

    if(!isConnected())
        return false;

    /*update network for given identity*/
    if(sqlite3_prepare_v2(pCon_, SQL_UPDATE_STATS,-1, &stmt, 0) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not prepare, fct:updateStats, stmt:SQL_UPDATE_STATS, error:" + std::string(sqlite3_errmsg(pCon_)));
	    return false;
    }

    if (sqlite3_bind_int(stmt, 1, stats.getRuleDroppedPkts()) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 2, stats.getRulePassedPkts()) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 3, stats.getTxDroppedPkts()) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 4, stats.getTxPassedPkts()) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 5, identity.c_str(), identity.length(), SQLITE_STATIC) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not bind, fct:updateStats, stmt:SQL_UPDATE_STATS, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    int stat = sqlite3_step(stmt);
    if (stat != SQLITE_DONE && stat != SQLITE_ROW) {
        ATLAS_LOGGER_ERROR("Could not step, fct:updateStats, stmt:SQL_UPDATE_STATS, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    return true;
}

bool AtlasSQLite::checkDeviceForFeatures(const std::string &identity, int networkTypeId)
{
    sqlite3_stmt *stmt = nullptr;
    
    BOOST_SCOPE_EXIT(&stmt) {
        sqlite3_finalize(stmt);
    } BOOST_SCOPE_EXIT_END

    if(!isConnected())
        return false;

    if(sqlite3_prepare_v2(pCon_, SQL_CHECK_FEATURE,-1, &stmt, 0) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not prepare, fct:checkDeviceForFeatures, stmt:SQL_CHECK_FEATURE, error:" + std::string(sqlite3_errmsg(pCon_)));
	    return false;
    }

    if (sqlite3_bind_text(stmt, 1, identity.c_str(), identity.length(),	SQLITE_STATIC) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 2, networkTypeId) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not bind, fct:checkDeviceForFeatures, stmt:SQL_CHECK_FEATURE, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    int stat = sqlite3_step(stmt);
    if (stat != SQLITE_DONE && stat != SQLITE_ROW) {
        ATLAS_LOGGER_ERROR("Could not step, fct:checkDeviceForFeatures, stmt:SQL_CHECK_FEATURE, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    if (stat == SQLITE_ROW) {
        return true;
    }

    return false;
}

bool AtlasSQLite::checkDeviceForStats(const std::string &identity)
{
    sqlite3_stmt *stmt = nullptr;
    
    BOOST_SCOPE_EXIT(&stmt) {
        sqlite3_finalize(stmt);
    } BOOST_SCOPE_EXIT_END

    if(!isConnected())
        return false;

    if(sqlite3_prepare_v2(pCon_, SQL_CHECK_STATS,-1, &stmt, 0) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not prepare, fct:checkDeviceForStats, stmt:SQL_CHECK_STATS, error:" + std::string(sqlite3_errmsg(pCon_)));
	    return false;
    }

    if (sqlite3_bind_text(stmt, 1, identity.c_str(), identity.length(),	SQLITE_STATIC) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not bind, fct:checkDeviceForStats, stmt:SQL_CHECK_STATS, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    int stat = sqlite3_step(stmt);
    if (stat != SQLITE_DONE && stat != SQLITE_ROW) {
        ATLAS_LOGGER_ERROR("Could not step, fct:checkDeviceForStats, stmt:SQL_CHECK_STATS, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    if (stat == SQLITE_ROW) {
        return true;
    }

    return false;
}

bool AtlasSQLite::insertOwner(const std::string &secretKey, const std::string &identity)
{
    sqlite3_stmt *stmt = nullptr;

    BOOST_SCOPE_EXIT(&stmt) {
        sqlite3_finalize(stmt);
    } BOOST_SCOPE_EXIT_END
    
    if(!isConnected_)
        return false;
	
    if(sqlite3_prepare_v2(pCon_, SQL_INSERT_OWNER,  -1, &stmt, 0) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not prepare, fct:insertOwner, stmt:SQL_INSERT_OWNER, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    if (sqlite3_bind_text(stmt, 1, secretKey.c_str(), secretKey.length(), SQLITE_STATIC) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not bind, fct:insertOwner, stmt:SQL_INSERT_OWNER, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    if (sqlite3_bind_text(stmt, 2, identity.c_str(), identity.length(), SQLITE_STATIC) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not bind, fct:insertOwner, stmt:SQL_INSERT_OWNER, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    int stat = sqlite3_step(stmt);
    if (stat != SQLITE_DONE && stat != SQLITE_ROW) {
        ATLAS_LOGGER_ERROR("Could not step, fct:insertOwner, stmt:SQL_INSERT_OWNER, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    return true;
}

bool AtlasSQLite::selectOwnerInfo(std::string &secretKey, std::string &identity)
{
    sqlite3_stmt *stmt = nullptr;
    int stat = -1;

    BOOST_SCOPE_EXIT(&stmt) {
        sqlite3_finalize(stmt);
    } BOOST_SCOPE_EXIT_END

    if(!isConnected())
        return false;

    /* Select owner information */
    if(sqlite3_prepare_v2(pCon_, SQL_GET_OWNER,-1, &stmt, 0) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not prepare, fct:selectOwnerInfo, stmt:SQL_GET_OWNER, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    stat = sqlite3_step(stmt);

    if(stat != SQLITE_ROW) {
        ATLAS_LOGGER_ERROR("Could not step, fct:selectOwnerInfo, stmt:SQL_GET_OWNER, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    /* Assign owner secret key and identity */
    secretKey = std::string((const char *) sqlite3_column_text(stmt, 0));
    identity = std::string((const char *) sqlite3_column_text(stmt, 1));

    return true;
}

bool AtlasSQLite::insertDeviceCommand(const uint32_t sequenceNumber, const std::string &commandType,
                                      const std::string &commandPayload, const std::string &deviceIdentity)
{
    sqlite3_stmt *stmt = nullptr;
    int stat = -1, deviceId = -1;

    BOOST_SCOPE_EXIT(&stmt) {
        sqlite3_finalize(stmt);
    } BOOST_SCOPE_EXIT_END
    
    if(!isConnected_)
        return false;

    /*get deviceId from db*/
    if(sqlite3_prepare_v2(pCon_, SQL_GET_ID_DEVICE,  -1, &stmt, 0) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not prepare, fct:insertDeviceCommand, stmt:SQL_GET_ID_DEVICE, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    if (sqlite3_bind_text(stmt, 1, deviceIdentity.c_str(), deviceIdentity.length(),	SQLITE_STATIC) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not bind, fct:insertDeviceCommand, stmt:SQL_GET_ID_DEVICE, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    stat = sqlite3_step(stmt);
    if (stat != SQLITE_DONE && stat != SQLITE_ROW) {
        ATLAS_LOGGER_ERROR("Could not step, fct:insertDeviceCommand, stmt:SQL_GET_ID_DEVICE, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }
    deviceId = sqlite3_column_int(stmt, 0);
    sqlite3_reset(stmt);

    /*insert device command*/	
    if(sqlite3_prepare_v2(pCon_, SQL_INSERT_DEVICE_COMMAND,  -1, &stmt, 0) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not prepare, fct:insertDeviceCommand, stmt:SQL_INSERT_DEVICE_COMMAND, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    if (sqlite3_bind_int(stmt, 1, deviceId) != SQLITE_OK ||
        sqlite3_bind_int(stmt, 2, sequenceNumber) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 3, commandType.c_str(), commandType.length(), SQLITE_STATIC) != SQLITE_OK ||
        sqlite3_bind_text(stmt, 4, commandPayload.c_str(), commandPayload.length(), SQLITE_STATIC) != SQLITE_OK) { 
        ATLAS_LOGGER_ERROR("Could not bind, fct:insertDeviceCommand, stmt:SQL_INSERT_DEVICE_COMMAND, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    stat = sqlite3_step(stmt);
    if (stat != SQLITE_DONE && stat != SQLITE_ROW) {
        ATLAS_LOGGER_ERROR("Could not step, fct:insertDeviceCommand, stmt:SQL_INSERT_DEVICE_COMMAND, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    /*insert current sequence number in owner table*/
    bool ret = updateMaxSequenceNumber(sequenceNumber);
    if (!ret) {
        ATLAS_LOGGER_ERROR("Error in updateMaxSequenceNumber call");
        return false;
    }
    return true;
}

bool AtlasSQLite::checkDeviceCommandBySeqNo(const uint32_t sequenceNumber)
{
    sqlite3_stmt *stmt = nullptr;
    
    BOOST_SCOPE_EXIT(&stmt) {
        sqlite3_finalize(stmt);
    } BOOST_SCOPE_EXIT_END

    if(!isConnected())
        return false;

    if(sqlite3_prepare_v2(pCon_, SQL_CHECK_DEVICE_COMMAND_BY_SEQ_NO,-1, &stmt, 0) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not prepare, fct:checkDeviceCommand, stmt:SQL_CHECK_DEVICE_COMMAND_BY_SEQ_NO, error:" + std::string(sqlite3_errmsg(pCon_)));
	    return false;
    }

    if (sqlite3_bind_int(stmt, 1, sequenceNumber) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not bind, fct:checkDeviceCommand, stmt:SQL_CHECK_DEVICE_COMMAND_BY_SEQ_NO, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    int stat = sqlite3_step(stmt);
    if (stat != SQLITE_DONE && stat != SQLITE_ROW) {
        ATLAS_LOGGER_ERROR("Could not step, fct:checkDeviceCommand, stmt:SQL_CHECK_DEVICE_COMMAND_BY_SEQ_NO, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    if (stat == SQLITE_ROW) {
        return true;
    }

    return false;
}

bool AtlasSQLite::checkDeviceCommandByIdentity(const std::string &identity)
{
    sqlite3_stmt *stmt = nullptr;
    
    BOOST_SCOPE_EXIT(&stmt) {
        sqlite3_finalize(stmt);
    } BOOST_SCOPE_EXIT_END

    if(!isConnected())
        return false;

    if(sqlite3_prepare_v2(pCon_, SQL_CHECK_DEVICE_COMMAND_BY_IDENTITY,-1, &stmt, 0) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not prepare, fct:checkDeviceCommandByIdentity, stmt:SQL_CHECK_DEVICE_COMMAND_BY_IDENTITY, error:" + std::string(sqlite3_errmsg(pCon_)));
	    return false;
    }

    if (sqlite3_bind_text(stmt, 1, identity.c_str(), identity.length(),	SQLITE_STATIC) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not bind, fct:checkDeviceCommandByIdentity, stmt:SQL_CHECK_DEVICE_COMMAND_BY_IDENTITY, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    int stat = sqlite3_step(stmt);
    if (stat != SQLITE_DONE && stat != SQLITE_ROW) {
        ATLAS_LOGGER_ERROR("Could not step, fct:checkDeviceCommandByIdentity, stmt:SQL_CHECK_DEVICE_COMMAND_BY_IDENTITY, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    if (stat == SQLITE_ROW) {
        return true;
    }

    return false;
}

bool AtlasSQLite::checkDeviceCommandForExecution(const uint32_t sequenceNumber) 
{
    sqlite3_stmt *stmt = nullptr;
    
    BOOST_SCOPE_EXIT(&stmt) {
        sqlite3_finalize(stmt);
    } BOOST_SCOPE_EXIT_END

    if(!isConnected())
        return false;

    if(sqlite3_prepare_v2(pCon_, SQL_CHECK_DEVICE_COMMAND_EXECUTION_BY_SEQ_NO,-1, &stmt, 0) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not prepare, fct:checkDeviceCommandForExecution, stmt:SQL_CHECK_DEVICE_COMMAND_EXECUTION_BY_SEQ_NO, error:" + std::string(sqlite3_errmsg(pCon_)));
	    return false;
    }

    if (sqlite3_bind_int(stmt, 1, sequenceNumber) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not bind, fct:checkDeviceCommandForExecution, stmt:SQL_CHECK_DEVICE_COMMAND_EXECUTION_BY_SEQ_NO, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    int stat = sqlite3_step(stmt);
    if (stat != SQLITE_DONE && stat != SQLITE_ROW) {
        ATLAS_LOGGER_ERROR("Could not step, fct:checkDeviceCommandForExecution, stmt:SQL_CHECK_DEVICE_COMMAND_EXECUTION_BY_SEQ_NO, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    if (stat == SQLITE_ROW) {
        return true;
    }

    return false;
}

bool AtlasSQLite::markExecutedDeviceCommand(const uint32_t sequenceNumber) 
{

    sqlite3_stmt *stmt = nullptr;
    int stat = -1;

    BOOST_SCOPE_EXIT(&stmt) {
        sqlite3_finalize(stmt);
    } BOOST_SCOPE_EXIT_END
    
    if(!isConnected_)
        return false;

    /*update features for given identity*/
    if(sqlite3_prepare_v2(pCon_, SQL_MARK_AS_EXECUTED_DEVICE_COMMAND,-1, &stmt, 0) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not prepare, fct:markExecutedDeviceCommand, stmt:SQL_MARK_AS_EXECUTED_DEVICE_COMMAND, error:" + std::string(sqlite3_errmsg(pCon_)));
	    return false;
    }

    if (sqlite3_bind_int(stmt, 1, sequenceNumber) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not bind, fct:markExecutedDeviceCommand, stmt:SQL_MARK_AS_EXECUTED_DEVICE_COMMAND, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    stat = sqlite3_step(stmt);
    if (stat != SQLITE_DONE && stat != SQLITE_ROW) {
        ATLAS_LOGGER_ERROR("Could not step, fct:markExecutedDeviceCommand, stmt:SQL_MARK_AS_EXECUTED_DEVICE_COMMAND, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    return true;
}

bool AtlasSQLite::selectDeviceCommand(AtlasDevice &device)
{
    sqlite3_stmt *stmt = nullptr;
    int stat = -1;
    
    BOOST_SCOPE_EXIT(&stmt) {
        sqlite3_finalize(stmt);
    } BOOST_SCOPE_EXIT_END

    if(!isConnected())
        return false;

    /*select device commands for given identity*/
    if(sqlite3_prepare_v2(pCon_, SQL_GET_DEVICE_COMMAND_BY_IDENTITY,-1, &stmt, 0) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not prepare, fct:selectDeviceCommand, stmt:SQL_GET_DEVICE_COMMAND_BY_IDENTITY, error:" + std::string(sqlite3_errmsg(pCon_)));
	    return false;
    }

    const std::string &deviceIdentity = device.getIdentity();

    if (sqlite3_bind_text(stmt, 1, deviceIdentity.c_str(), deviceIdentity.length(),	SQLITE_STATIC) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not bind, fct:selectDeviceCommand, stmt:SQL_GET_DEVICE_COMMAND_BY_IDENTITY, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }
 
    for (;;)
    {
	    stat = sqlite3_step(stmt);
	    if (stat == SQLITE_DONE) {
            break;
        }

        if(stat != SQLITE_ROW) {
            ATLAS_LOGGER_ERROR("Could not step, fct:selectDeviceCommand, stmt:SQL_GET_DEVICE_COMMAND_BY_IDENTITY, error:" + std::string(sqlite3_errmsg(pCon_)));
            return false;
        }

        AtlasCommandDevice cmd(device.getIdentity(), sqlite3_column_int(stmt, 0), 
                               std::string((const char *) sqlite3_column_text(stmt, 1)), 
                               std::string((const char *) sqlite3_column_text(stmt, 2)));

        if(sqlite3_column_int(stmt, 3) == 0) {
            device.addRecvDeviceCommand(std::move(cmd));
        } else {
            device.addExecDeviceCommand(std::move(cmd));
        }
            
    }
    
    return true;
}

bool AtlasSQLite::deleteDeviceCommand(const uint32_t sequenceNumber) 
{

    sqlite3_stmt *stmt = nullptr;
    int stat = -1;

    BOOST_SCOPE_EXIT(&stmt) {
        sqlite3_finalize(stmt);
    } BOOST_SCOPE_EXIT_END
    
    if(!isConnected_)
        return false;

    /*update features for given identity*/
    if(sqlite3_prepare_v2(pCon_, SQL_DELETE_DEVICE_COMMAND_BY_SEQ_NO,-1, &stmt, 0) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not prepare, fct:deleteDeviceCommand, stmt:SQL_DELETE_DEVICE_COMMAND_BY_SEQ_NO, error:" + std::string(sqlite3_errmsg(pCon_)));
	    return false;
    }

    if (sqlite3_bind_int(stmt, 1, sequenceNumber) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not bind, fct:deleteDeviceCommand, stmt:SQL_DELETE_DEVICE_COMMAND_BY_SEQ_NO, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    stat = sqlite3_step(stmt);
    if (stat != SQLITE_DONE && stat != SQLITE_ROW) {
        ATLAS_LOGGER_ERROR("Could not step, fct:deleteDeviceCommand, stmt:SQL_MARK_AS_DONE_DEVICE_COMMAND, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    return true;
}
bool AtlasSQLite::getMaxSequenceNumber() 
{
    sqlite3_stmt *stmt = nullptr;
    int stat = -1;

    BOOST_SCOPE_EXIT(&stmt) {
        sqlite3_finalize(stmt);
    } BOOST_SCOPE_EXIT_END
    
    if(!isConnected_)
        return false;

    if(sqlite3_prepare_v2(pCon_, SQL_GET_MAX_SEQ_NO,-1, &stmt, 0) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not prepare, fct:getMaxSequenceNumber, stmt:SQL_GET_MAX_SEQ_NO, error:" + std::string(sqlite3_errmsg(pCon_)));
	    return false;
    }

    stat = sqlite3_step(stmt);
    if (stat != SQLITE_DONE && stat != SQLITE_ROW) {
        ATLAS_LOGGER_ERROR("Could not step, fct:getMaxSequenceNumber, stmt:SQL_GET_MAX_SEQ_NO, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    if (stat == SQLITE_DONE) {
        ATLAS_LOGGER_INFO("No owner entry in database. No sequence number initialization!");
        return true;
    }

    AtlasApprove::getInstance().setSequenceNumber(sqlite3_column_int(stmt, 0));
    
    return true;
}

bool AtlasSQLite::updateMaxSequenceNumber(const uint32_t sequenceNumber) 
{
    sqlite3_stmt *stmt = nullptr;
    int stat = -1;

    BOOST_SCOPE_EXIT(&stmt) {
        sqlite3_finalize(stmt);
    } BOOST_SCOPE_EXIT_END
    
    if(!isConnected_)
        return false;

    if(sqlite3_prepare_v2(pCon_, SQL_UPDATE_MAX_SEQ_NO,-1, &stmt, 0) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not prepare, fct:updateMaxSequenceNumber, stmt:SQL_UPDATE_MAX_SEQ_NO, error:" + std::string(sqlite3_errmsg(pCon_)));
	    return false;
    }

    if (sqlite3_bind_int(stmt, 1, sequenceNumber) != SQLITE_OK) {
        ATLAS_LOGGER_ERROR("Could not bind, fct:updateMaxSequenceNumber, stmt:SQL_UPDATE_MAX_SEQ_NO, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    stat = sqlite3_step(stmt);
    if (stat != SQLITE_DONE && stat != SQLITE_ROW) {
        ATLAS_LOGGER_ERROR("Could not step, fct:updateMaxSequenceNumber, stmt:SQL_UPDATE_MAX_SEQ_NO, error:" + std::string(sqlite3_errmsg(pCon_)));
        return false;
    }

    return true;
}

} // namespace atlas
