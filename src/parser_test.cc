//
// File:        parser_test.cc
// Description: Test the parser
// Authors:     Jiaqing Liu
//

#include <cstdio>
#include <iostream>
#include "thisadb.h"
#include "parser.h"
#include "sm.h"
#include "ql.h"


//
// Global PF_Manager and RM_Manager variables
//
PF_Manager pfm;
RM_Manager rmm(pfm);
IX_Manager ixm(pfm);
SM_Manager smm(ixm, rmm);
QL_Manager qlm(smm, ixm, rmm);

int main(void)
{
    RC rc;

    if ((rc = smm.OpenDb("testdb"))) {
        PrintError(rc);
        return (1);
    }

    RBparse(pfm, smm, qlm);

    if ((rc = smm.CloseDb())) {
        PrintError(rc);
        return (1);
    }
}
