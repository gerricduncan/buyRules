//
// dbConnect.cpp
// rulesBuy
//
// Created by Gary Duncan on 7/27/15.
// Copyright (c) 2015 Gary Duncan. All rights reserved.
//

#include "dbConnect.h"

db::db(std::string h, std::string u,std::string p,std::string d) :
host(h), user(u),pwd(p),dbName(d) {
    connection = sql::mysql::get_driver_instance()->connect(host, user, pwd);
    connection->setSchema(dbName);
}

void db::createTables(){
    execStmt("DROP TABLE IF EXISTS buy_process;");
    execStmt("DROP TABLE IF EXISTS actions;");
    execStmt("DROP TABLE IF EXISTS rules;");
    
    execStmt("CREATE TABLE buy_process (BPR_CDE_HCFA_PART VARCHAR(1) NOT NULL, BPR_CDE_RIC VARCHAR(1) NOT NULL, BPR_CDE_TRAN VARCHAR(2) NOT NULL, BPR_CDE_SUBCODE VARCHAR(3) NOT NULL, BPR_CDE_BUY_STATUS VARCHAR(1) NOT NULL, BPR_CDE_MEDS_MEMBER_STATUS VARCHAR(1) NOT NULL, BPR_NUM_SEQUENCE VARCHAR(4) NOT NULL, BPR_NAM_PROGRAM VARCHAR(30), BPR_NAM_SECOND_PROGRAM VARCHAR(30), BPR_CDE_TRANS_TO_SEND VARCHAR(2), BPR_IND_AUD_STP_EQ_HCFA_STP VARCHAR(1), BPR_IND_HCFA_STP_EQ_MBR_INEL VARCHAR(1), BPR_IND_MBR_DOD_EQ_HCFA_STP VARCHAR(1), BPR_IND_BUY_STOP_EQ_HCFA_STP VARCHAR(1), BPR_IND_MED_EL_GT_HCFA_STT VARCHAR(1), BPR_NUM_MEDS_PYMT_CAT VARCHAR(2), BPR_IND_CALL_ELD VARCHAR(2), BPR_IND_UPDATE_MBR_DOD VARCHAR(1), BPR_IND_MBR_INEL_GT_HCFA_STT VARCHAR(1), BPR_IND_UPDATE_MEDICARE_IND VARCHAR(1), BPR_IND_BENDEX_FOUND VARCHAR(1), BPR_IND_MBR_INEL_GT_HCFA_STP VARCHAR(1), BPR_IND_BUY_STP_NEQ_HCFA_STP VARCHAR(1), BPR_IND_BUY_STP_GT_HCFA_STT VARCHAR(1), BPR_IND_BUY_TRN_EQ_HCFA_TRN VARCHAR(1), BPR_IND_HCFA_STT_EQ_BUY_STT VARCHAR(1), BPR_IND_HCFA_TRN_EQ_LIST VARCHAR(1), BPR_IND_MBR_DOD_EQ_HCFA_DOD VARCHAR(1), BPR_IND_AUD_STP_EQ_HCFA_RPL VARCHAR(1), BPR_IND_AUD_STT_EQ_HCFA_STT VARCHAR(1), BPR_CDE_BUY_TRANS_EQ VARCHAR(2), BPR_IND_MBR_ELIG_GT_HCFA_STT VARCHAR(1), BPR_IND_UPDATE_MCN VARCHAR(1), BPR_IND_HCFA_EFF_GT_BILL VARCHAR(1), BPR_IND_MBR_DOD_BLANK VARCHAR(1), BAC_CDE_ACTION_TYPE VARCHAR(1), BAC_NAM_ACTION VARCHAR(8))");
    execStmt("CREATE TABLE actions (actionField VARCHAR(30) NOT NULL, value VARCHAR(25), actionToTake VARCHAR(25));");
    execStmt("CREATE TABLE rules (ruleNum INT(5) NOT NULL, whenOrThen VARCHAR(30), value VARCHAR(30), combinedToRule int(5));");
}

void db::loadData(){
    execStmt("LOAD DATA INFILE '/Users/duncan4/buy/process.cleaned' INTO TABLE buy_process FIELDS TERMINATED BY ','");
    execStmt("LOAD DATA INFILE '/Users/duncan4/buy/actions' INTO TABLE actions FIELDS TERMINATED BY ','");
}

void db::loadRules(){
    sql::ResultSet *buyProcessRow = execQuery("SELECT * FROM buy_process;");
    std::string columnValue;
    int totalActions = 0;
    int numNextRule = 0;
    std::multimap<std::string,std::string> thisRule;
    
    sql::ResultSet *setActions = execQuery("SELECT * FROM actions WHERE actionToTake <> '';");
    std::set<std::string> goodActions;
    
    while (setActions->next())
        goodActions.insert(setActions->getString("actionField")+setActions->getString("value"));
    
    while (buyProcessRow->next()) {
        numNextRule++;
        thisRule.clear();
        totalActions = 0;
        
        for (auto &actionToEvaluate: actionsToEvaluate){
            //Get column value
            columnValue = buyProcessRow->getString(actionToEvaluate);
            if (columnValue == "")
                continue;

           if (goodActions.find(actionToEvaluate+columnValue) != goodActions.end()){
                thisRule.insert(std::pair<std::string,std::string>(actionToEvaluate,columnValue));
                totalActions++;
            }
        }
        
        for (auto &conditionToEvaluate: conditionsToEvaluate){
            //Get column value
            columnValue = buyProcessRow->getString(conditionToEvaluate);
            if (columnValue == "")
                continue;
            //If the column is a condition
            if (conditionToEvaluate == "BPR_IND_MED_EL_GT_HCFA_STT" &&
                buyProcessRow->getString("BPR_IND_MBR_ELIG_GT_HCFA_STT") == "")
            thisRule.insert(std::pair<std::string,std::string>("BPR_IND_MBR_ELIG_GT_HCFA_STT",columnValue));
            else
            thisRule.insert(std::pair<std::string,std::string>(conditionToEvaluate,columnValue));
        }
        if (totalActions == 0)
            mapRules[numNextRule] = std::make_pair(0,thisRule);
        else
            mapRules[numNextRule] = std::make_pair(numNextRule,thisRule);
        if (numNextRule % 2000 == 0)
            std::cout << "Creating rule: " << std::to_string(numNextRule) << std::endl;
    }
    delete buyProcessRow;
    delete setActions;
}

void db::insertRules(){
    std::string strInsertStatement;
    std::string firstComma = "";
    int activeRules = 0;
    for(auto i : mapRules){
        if (i.first == i.second.first)
            activeRules++;
        for (auto j : i.second.second){
            strInsertStatement.append(firstComma+"("+std::to_string(i.first)+",'"+
                                      j.first+"','"+
                                      j.second+"',"+
                                      std::to_string(i.second.first)+")");
            firstComma=",";
        }
        if (i.first % 2000 == 0){
            execStmt("INSERT INTO rules VALUES "+strInsertStatement+";");
            strInsertStatement = "";
            std::cout << "Inserting rule: " << std::to_string(i.first) << std::endl;
            firstComma = "";
        }
    }
    execStmt("INSERT INTO rules VALUES "+strInsertStatement+";");
    std::cout << "Active rules " << activeRules << std::endl;
}

void db::combineRules(){
    unsigned int thisLoop = 0;
    unsigned int changedThisLoop = 0;
    do {
        thisLoop++;
        changedThisLoop = 0;
        for(auto i : mapRules)
            if (i.first == i.second.first){
                rule1 = i.first;
                for (auto j = mapRules.upper_bound(i.first); j != mapRules.end(); ++j){
                    rule2=j->first;
                    if(rule2 == j->second.first && combinedRules()){
                        mapRules[j->first]=std::make_pair(i.first, j->second.second);
                        std::cout << "Loop " << thisLoop << " combined " << i.first << " and " << j->first << std::endl;
                        changedThisLoop++;
                    }
                }
            }
    } while (changedThisLoop > 0);
}

bool db::foundValue(const std::string& item, const std::string& value, const int& searchRule){
    auto ruleRange = mapRules[searchRule].second.equal_range(item);
    for (auto itrRule = ruleRange.first; itrRule != ruleRange.second;itrRule++)
        if (itrRule->second == value)
            return true;
    return false;
}

bool db::combinedRules(){
    std::string onlyConditionDifferenceAllowed;
    bool actionsDiffer = false;
    bool conditionsDiffer = false;
    bool isCondition;
    int compareThisRule = rule1;
    int toThisRule = rule2;
    
    if (rule1 == 8 && rule2 == 16)
        std::cout << "";
    
    for (unsigned int twoLoops = 0; twoLoops < 2; twoLoops++) {
        for (auto i: mapRules[compareThisRule].second) {
            isCondition = actions.find(i.first) == actions.end();
            auto toRuleRange = mapRules[toThisRule].second.equal_range(i.first);
            
            // If condition set is different, rule cannot be combined
            if (isCondition && toRuleRange.first == toRuleRange.second)
                return false;
            
            if (foundValue(i.first, i.second, toThisRule))
                continue;
            
            if (!isCondition) {
                actionsDiffer = true;
                if (conditionsDiffer)
                    return false;
                continue;
            }
            
            if (i.first != onlyConditionDifferenceAllowed && onlyConditionDifferenceAllowed != "" )
                return false;
        
            onlyConditionDifferenceAllowed = i.first;
            conditionsDiffer = true;
            if (actionsDiffer)
                return false;
        }
        compareThisRule = rule2;
        toThisRule = rule1;
    }
    
    for (auto i: mapRules[rule2].second)
        if (!foundValue(i.first, i.second, rule1))
            mapRules[rule1].second.insert(i);
    return true;

}


void db::execStmt(std::string execString){
    connection->createStatement()->execute(execString);
}

sql::ResultSet* db::execQuery(std::string execString){
    return(connection->createStatement()->executeQuery(execString));
}


