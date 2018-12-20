#include "sort.h"
#include "file_scan.h"
#include "index_scan.h"
#include "gtest/gtest.h"

class SortTest : public ::testing::Test {
};


TEST_F(SortTest, Cons) {
    RC rc;
    PF_Manager pfm;
    RM_Manager rmm(pfm);
    IX_Manager ixm(pfm);
    SM_Manager smm(ixm, rmm);

    const char * dbname = "test";
    stringstream command;
    command << "rm -rf " << dbname;
    rc = system (command.str().c_str());

    command.str("");
    command << "./dbcreate " << dbname;
    rc = system (command.str().c_str());
    ASSERT_EQ(rc, 0);

    command.str("");
    command << "echo \"create table in(in i, out f, bw c4);\" | ./thisadb " 
            << dbname;
    rc = system (command.str().c_str());
    ASSERT_EQ(rc, 0);

    command.str("");
    command << "echo \"create table stars(starid  i, stname  c20, plays  c12, soapid  i);\" | ./thisadb " 
            << dbname;
    rc = system (command.str().c_str());
    ASSERT_EQ(rc, 0);

    command.str("");
    command << "echo \"create index in(in);\" | ./thisadb " 
            << dbname;
    rc = system (command.str().c_str());
    ASSERT_EQ(rc, 0);

    command.str("");
    command << "echo \"create index in(bw);\" | ./thisadb " 
            << dbname;
    rc = system (command.str().c_str());
    ASSERT_EQ(rc, 0);

    command.str("");
    command << "echo \"create index stars(soapid);\" | ./thisadb " 
            << dbname;
    rc = system (command.str().c_str());
    ASSERT_EQ(rc, 0);

    command.str("");
    command << "echo \"create index stars(stname);\" | ./thisadb " 
            << dbname;
    rc = system (command.str().c_str());
    ASSERT_EQ(rc, 0);

    command.str("");
    command << "echo \"create index stars(plays);\" | ./thisadb " 
            << dbname;
    rc = system (command.str().c_str());
    ASSERT_EQ(rc, 0);

    command.str("");
    command << "echo \"load in(\\\"../data\\\");\" | ./thisadb " 
            << dbname;
    rc = system (command.str().c_str());
    ASSERT_EQ(rc, 0);

    command.str("");
    command << "echo \"load stars(\\\"../stars.data\\\");\" | ./thisadb " 
            << dbname;
    rc = system (command.str().c_str());
    ASSERT_EQ(rc, 0);


    // command.str("");
    // command << "echo \"help in;\" | ./thisadb " 
    //         << dbname;
    // rc = system (command.str().c_str());
    // ASSERT_EQ(rc, 0);

    Condition cond;
    cond.op = EQ_OP;
    cond.lhsAttr.relName = "in";
    cond.lhsAttr.attrName = "in";
    cond.rhsValue.data = NULL;

    rc = smm.OpenDb(dbname);
    ASSERT_EQ(rc, 0);

    RC status = -1;

    Iterator* lfs = new FileScan(smm, rmm, "in", status, cond);
    ASSERT_EQ(status, 0);
    Iterator* rfs = new FileScan(smm, rmm, "in", status, cond);
    ASSERT_EQ(status, 0);

    Condition jcond;
    jcond.op = EQ_OP;
    jcond.lhsAttr.relName = "in";
    jcond.rhsAttr.relName = "in";
    jcond.bRhsIsAttr = TRUE;
    jcond.lhsAttr.attrName = "in";
    jcond.rhsAttr.attrName = "in";

    Condition conds[5];
    conds[0] = jcond;

    // out
    Sort fs(lfs, STRING, 4, 8, status);
    ASSERT_EQ(status, 0);

    rc=fs.Open();
    ASSERT_EQ(rc, 0);

    Tuple t = fs.GetTuple();

    int ns = 0;
    while(1) {
      rc = fs.GetNext(t);
      if(rc ==  fs.Eof())
        break;
      EXPECT_EQ(rc, 0);
      if(rc != 0)
        PrintErrorAll(rc);
      cerr << "=-----= " << t << endl;
      ns++;
      if(ns > 20) ASSERT_EQ(1, 0);
    }
    
    EXPECT_EQ(5, ns);
    (rc=fs.Close());
    ASSERT_EQ(rc, 0);
}
