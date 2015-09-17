//
//  main.cpp
//  rulesBuy
//
//  Created by Gary Duncan on 7/27/15.
//  Copyright (c) 2015 Gary Duncan. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <string>
#include "dbConnect.h"

int main(int argc, const char * argv[]) {
    db myData = db("Localhost","root","","test");
    myData.createTables();
    myData.loadData();
    myData.loadRules();
    myData.combineRules();

    myData.insertRules();

    return 0;
}
