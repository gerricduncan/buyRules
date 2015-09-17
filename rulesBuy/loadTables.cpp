//
//  loadTables.cpp
//  rulesBuy
//
//  Created by Gary Duncan on 8/25/15.
//  Copyright (c) 2015 Gary Duncan. All rights reserved.
//

#include "loadTables.h"

void tableMeta::loadTables(db myData){
     myData.execStmt(buyProcessCreate);
    myData.execStmt(actionsCreate);
    myData.execStmt(rulesCreate);
    myData.loadTable("buy_process", "/Users/duncan4/buy/process.cleaned");
    myData.loadTable("actions", "/Users/duncan4/buy/actions");
    

};

