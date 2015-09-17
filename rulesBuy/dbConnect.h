//
//  dbConnect.h
//  rulesBuy
//
//  Created by Gary Duncan on 7/27/15.
//  Copyright (c) 2015 Gary Duncan. All rights reserved.
//

#ifndef __rulesBuy__dbConnect__
#define __rulesBuy__dbConnect__

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <map>
#include <set>
#include <boost/smart_ptr/scoped_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <driver/mysql_public_iface.h>
#include "mysql_driver.h"

class db {
    const std::string host;
    const std::string user;
    const std::string pwd;
    const std::string dbName;
    int rule1;
    int rule2;
    int diffActions;
    int diffConditions;
    bool foundValue(const std::string&, const std::string&, const int&);
    
    std::map<int, std::pair<int,std::multimap<std::string,std::string>>> mapRules;
    const std::set<std::string> actions = {
        "BPR_NAM_PROGRAM","BPR_CDE_TRANS_TO_SEND",
        "BPR_IND_UPDATE_MBR_DOD", "BPR_IND_UPDATE_MEDICARE_IND",
        "BPR_IND_UPDATE_MCN", "BPR_IND_CALL_ELD", "BAC_NAM_ACTION" };
    const std::vector<std::string> actionsToEvaluate = {
        "BPR_NAM_PROGRAM","BPR_CDE_TRANS_TO_SEND",
        "BPR_IND_UPDATE_MBR_DOD", "BPR_IND_UPDATE_MEDICARE_IND",
        "BPR_IND_UPDATE_MCN", "BPR_IND_CALL_ELD", "BAC_NAM_ACTION" };
    const std::vector<std::string> conditionsToEvaluate = {
        "BPR_CDE_HCFA_PART", "BPR_CDE_RIC",
        "BPR_CDE_TRAN","BPR_CDE_SUBCODE",
        "BPR_CDE_BUY_STATUS","BPR_CDE_MEDS_MEMBER_STATUS",
        "BPR_IND_AUD_STT_EQ_HCFA_STT","BPR_IND_AUD_STP_EQ_HCFA_STP",
        "BPR_IND_MBR_DOD_BLANK","BPR_IND_MBR_DOD_EQ_HCFA_STP",
        "BPR_IND_MBR_DOD_EQ_HCFA_DOD","BPR_IND_HCFA_STT_EQ_BUY_STT",
        "BPR_IND_BUY_STOP_EQ_HCFA_STP","BPR_IND_BUY_STP_NEQ_HCFA_STP",
        "BPR_IND_BUY_STP_GT_HCFA_STT","BPR_IND_HCFA_EFF_GT_BILL",
        "BPR_NUM_MEDS_PYMT_CAT","BPR_IND_MBR_INEL_GT_HCFA_STT",
        "BPR_IND_MBR_INEL_GT_HCFA_STP","BPR_IND_HCFA_TRN_EQ_LIST",
        "BPR_CDE_BUY_TRANS_EQ","BPR_IND_MBR_ELIG_GT_HCFA_STT",
        "BPR_IND_MED_EL_GT_HCFA_STT"};
    bool combinedRules();
    void execStmt(std::string);
    sql::ResultSet* execQuery(std::string);
public:
    sql::Connection *connection;
    db(std::string,std::string,std::string,std::string);
    void createTables();
    void loadData();
    void loadRules();
    void combineRules();
    void insertRules();    
 };

#endif /* defined(__rulesBuy__dbConnect__) */
