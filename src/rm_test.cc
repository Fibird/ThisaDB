#include <cstdio>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <cstdlib>
#include <vector>
#include <sstream>
#include <fstream>

#include "thisadb.h"
#include "pf.h"
#include "rm.h"

using namespace std;

void pauseProgram()
{
    cout << "Press any key to continue..." << endl;
    getchar();
}
//
// Defines
//
const char * FILENAME =  "rm_test_file";       // test file name
#define STRLEN      29               // length of string in testrec
#define PROG_UNIT   50               // how frequently to give progress
                                      //   reports when adding lots of recs
#define FEW_RECS  100 // number of records added in

//
// Computes the offset of a field in a record (should be in <stddef.h>)
//
#ifndef offsetof
#       define offsetof(type, field)   ((size_t)&(((type *)0) -> field))
#endif

// use Customer.tbl to test
struct Customer
{
    int cust_key;
    string name;
    string address;
    int nation_key;
    string phone;
    float acct_bal;
    string mktsegment;
    string comment;
};

struct TestRec {
    int c_custkey;
    char c_name[25];
    char c_address[40];
    int c_nationkey;
    char c_phone[15];
    float c_acctbal;
    char c_mktsegment[10];
    char c_comment[117];
};

void SplitString(const string& s, vector<string>& v, const string& c)
{
    string::size_type pos1, pos2;
    pos2 = s.find(c);
    pos1 = 0;
    while(string::npos != pos2)
    {
        v.push_back(s.substr(pos1, pos2-pos1));

        pos1 = pos2 + c.size();
        pos2 = s.find(c, pos1);
    }
    if(pos1 != s.length())
        v.push_back(s.substr(pos1));
}

void str2int(int &int_temp,const string &string_temp)
{
    stringstream stream(string_temp);
    stream>>int_temp;
}

void str2float(float &float_temp, const string &string_temp)
{
    stringstream stream(string_temp);
    stream>>float_temp;
}
void parse_customer_file(vector<Customer> &custs, string file_path, int test_num)
{
    ifstream fin(file_path.c_str());
    
    if (!fin.is_open())
    {
        cout << "[TestUtil Error]: can not opeing file" << endl;
        return;
    }

    for (int i = 0; i < test_num; i++)
    {
        Customer s;
        vector<string> v;
        string temp;
        getline(fin, temp);
        SplitString(temp, v, "|");
        str2int(s.cust_key, v[0]);
        s.name = v[1];
        s.address = v[2];
        str2int(s.nation_key, v[3]);
        s.phone = v[4];
        str2float(s.acct_bal, v[5]);
        s.mktsegment = v[6];
        s.comment = v[7];
        custs.push_back(s);
    }
}
//
// Global PF_Manager and RM_Manager variables
//
PF_Manager pfm;
RM_Manager rmm(pfm);

//
// Function declarations
//
void PrintErrorAll(RC rc);
void PrintRecord(TestRec &recBuf);
RC GetRecs(RM_FileHandle &fh, RID rid);
RC AddRecs(RM_FileHandle &fh, int numRecs);

RC QueryRecords(RM_FileHandle &fh, int numRecs);
RC PrintFile(RM_FileScan &fh);

RC CreateFile(const char *fileName, int recordSize);
RC DestroyFile(const char *fileName);
RC OpenFile(const char *fileName, RM_FileHandle &fh);
RC CloseFile(const char *fileName, RM_FileHandle &fh);
RC InsertRec(RM_FileHandle &fh, char *record, RID &rid);
RC UpdateRec(RM_FileHandle &fh, RM_Record &rec);
RC DeleteRec(RM_FileHandle &fh, RID rid);

RC GetNextRecScan(RM_FileScan &fs, RM_Record &rec);


//
// main
//
int main(int argc, char *argv[])
{
    RC   rc;
    char *progName = argv[0];   // since we will be changing argv
    int  testNum;
    TestRec   *pRecBuf;

    // Write out initial starting message
    cerr.flush();
    cout.flush();
    cout << "Starting RM component test.\n";
    cout.flush();

    // Delete files from last time
    unlink(FILENAME);

    RM_FileHandle fh;
    int query_num = 5;

    cout << "Test for inserting records." << endl;
    if ((rc = CreateFile(FILENAME, sizeof(TestRec))) ||
        (rc = OpenFile(FILENAME, fh)) ||
        (rc = AddRecs(fh, FEW_RECS)) ||
        (rc = QueryRecords(fh, FEW_RECS)))
        return (rc);
    
    cout << "Test for querying records by rid" << endl;
    pauseProgram();

    GetRecs(fh, RID(1, 3));
    GetRecs(fh, RID(2, 4));
    GetRecs(fh, RID(4, 5));

    cout << "Test for deleting records by rid" << endl;
    pauseProgram();
    DeleteRec(fh, RID(1, 3));
    cout << "Delete RID(1, 3)." << endl;

    QueryRecords(fh, FEW_RECS);
    
    rc = CloseFile(FILENAME, fh);
    return 0;
}


//
// PrintRecord
//
// Desc: Print the TestRec record components
//
void PrintRecord(TestRec &recBuf)
{
    printf("[%d, %s, %s, %d, %s, %f, %s, %s]\n", recBuf.c_custkey, recBuf.c_name, recBuf.c_address, recBuf.c_nationkey, recBuf.c_phone, recBuf.c_acctbal, recBuf.c_mktsegment, recBuf.c_comment);
}

RC GetRecs(RM_FileHandle &fh, RID rid)
{
    RC        rc;
    int       n;
    TestRec   *pRecBuf;
    RID       r;
    char      stringBuf[STRLEN];
    RM_Record rec;

    RM_FileScan fs;
    if ((rc=fs.OpenScan(fh,INT,sizeof(int),offsetof(TestRec, c_custkey),
                        NO_OP, NULL, NO_HINT)))
        return (rc);

    // For each record in the file
    for (rc = GetNextRecScan(fs, rec), n = 0;
         rc == 0;
         rc = GetNextRecScan(fs, rec), n++) {

        if ((rc = rec.GetData((char *&)pRecBuf)) ||
            (rc = rec.GetRid(r)))
            goto err;
        
        if (rid.Page() == r.Page() && rid.Slot() == r.Slot())
            PrintRecord(*pRecBuf);
    }

    if (rc != RM_EOF)
        goto err;

    if ((rc=fs.CloseScan()))
        goto err;

    // Return ok
    rc = 0;
err:
    fs.CloseScan();
    return (rc);
}

//
// AddRecs
//
// Desc: Add a number of records to the file
//
RC AddRecs(RM_FileHandle &fh, int numRecs)
{
    RC      rc;
    int     i;
    TestRec recBuf;
    RID     rid;
    PageNum pageNum;
    SlotNum slotNum;

    // We set all of the TestRec to be 0 initially.  This heads off
    // warnings that Purify will give regarding UMR since sizeof(TestRec)
    // is 40, whereas actual size is 37.
    memset((void *)&recBuf, 0, sizeof(recBuf));
    vector<Customer> custs;
    
    parse_customer_file(custs, "../data/customer.tbl", numRecs);

    printf("\nadding %d records\n", numRecs);
    for (i = 0; i < numRecs; i++) {
        recBuf.c_custkey = custs[i].cust_key;
        strcpy(recBuf.c_name, custs[i].name.c_str());
        strcpy(recBuf.c_address, custs[i].address.c_str());
        recBuf.c_nationkey = custs[i].nation_key;
        strcpy(recBuf.c_phone, custs[i].phone.c_str());
        recBuf.c_acctbal = custs[i].acct_bal;
        strcpy(recBuf.c_mktsegment, custs[i].mktsegment.c_str());
        strcpy(recBuf.c_comment, custs[i].comment.c_str());
        
        if ((rc = InsertRec(fh, (char *)&recBuf, rid)) ||
            (rc = rid.GetPageNum(pageNum)) ||
            (rc = rid.GetSlotNum(slotNum)))
            return (rc);

        if ((i + 1) % PROG_UNIT == 0){
            printf("%d  ", i + 1);
            fflush(stdout);
        }
    }
    if (i % PROG_UNIT != 0)
        printf("%d\n", i);
    else
        putchar('\n');

    // Return ok
    return 0;
}

//
//
//
RC QueryRecords(RM_FileHandle &fh, int numRecs)
{
    RC        rc;
    int       n;
    TestRec   *pRecBuf;
    RID       rid;
    char      stringBuf[STRLEN];
    char      *found;
    RM_Record rec;

    found = new char[numRecs];
    memset(found, 0, numRecs);

    RM_FileScan fs;
    if ((rc=fs.OpenScan(fh,INT,sizeof(int),offsetof(TestRec, c_custkey),
                        NO_OP, NULL, NO_HINT)))
        return (rc);

    cout << "Print all records." << endl;
    pauseProgram();

    // For each record in the file
    for (rc = GetNextRecScan(fs, rec), n = 0;
         rc == 0;
         rc = GetNextRecScan(fs, rec), n++) {

        if ((rc = rec.GetData((char *&)pRecBuf)) ||
            (rc = rec.GetRid(rid)))
            goto err;
        
        cout << "[" << rid.Page() << "," << rid.Slot() << "]";
        // print all records
        PrintRecord(*pRecBuf);
    }

    if (rc != RM_EOF)
        goto err;

    if ((rc=fs.CloseScan()))
        goto err;

    // make sure we had the right number of records in the file
    if (n != numRecs) {
        delete[] found;
        printf("%d records in file (supposed to be %d)\n",
               n, numRecs);
        exit(1);
    }

    // Return ok
    rc = 0;

err:
    fs.CloseScan();
    delete[] found;
    return (rc);
}

//
// PrintFile
//
// Desc: Print the contents of the file
//
RC PrintFile(RM_FileScan &fs)
{
    RC        rc;
    int       n;
    TestRec   *pRecBuf;
    RID       rid;
    RM_Record rec;

    printf("\nprinting file contents\n");

    // for each record in the file
    for (rc = GetNextRecScan(fs, rec), n = 0;
         rc == 0;
         rc = GetNextRecScan(fs, rec), n++) {

        // Get the record data and record id
        if ((rc = rec.GetData((char *&)pRecBuf)) ||
            (rc = rec.GetRid(rid)))
            return (rc);

        // Print the record contents
        PrintRecord(*pRecBuf);
    }

    if (rc != RM_EOF)
        return (rc);

    printf("%d records found\n", n);

    // Return ok
    return (0);
}

////////////////////////////////////////////////////////////////////////
// The following functions are wrappers for some of the RM component  //
// methods.  They give you an opportunity to add debugging statements //
// and/or set breakpoints when testing these methods.                 //
////////////////////////////////////////////////////////////////////////


//
// CreateFile
//
// Desc: call RM_Manager::CreateFile
//
RC CreateFile(const char *fileName, int recordSize)
{
    printf("\ncreating %s\n", fileName);
    return (rmm.CreateFile(fileName, recordSize));
}

//
// DestroyFile
//
// Desc: call RM_Manager::DestroyFile
//
RC DestroyFile(const char *fileName)
{
    printf("\ndestroying %s\n", fileName);
    return (rmm.DestroyFile(fileName));
}


//
// OpenFile
//
// Desc: call RM_Manager::OpenFile
//
RC OpenFile(const char *fileName, RM_FileHandle &fh)
{
    printf("\nopening %s\n", fileName);
    return (rmm.OpenFile(fileName, fh));
}

//
// CloseFile
//
// Desc: call RM_Manager::CloseFile
//
RC CloseFile(const char *fileName, RM_FileHandle &fh)
{
    if (fileName != NULL)
        printf("\nClosing %s\n", fileName);
    return (rmm.CloseFile(fh));
}


//
// InsertRec
//
// Desc: call RM_FileHandle::InsertRec
//
RC InsertRec(RM_FileHandle &fh, char *record, RID &rid)
{
    return (fh.InsertRec(record, rid));
}

//
// DeleteRec
//
// Desc: call RM_FileHandle::DeleteRec
//
RC DeleteRec(RM_FileHandle &fh, RID rid)
{
    return (fh.DeleteRec(rid));
}

//
// UpdateRec
//
// Desc: call RM_FileHandle::UpdateRec
//
RC UpdateRec(RM_FileHandle &fh, RM_Record &rec)
{
    return (fh.UpdateRec(rec));
}

//
// GetNextRecScan
//
// Desc: call RM_FileScan::GetNextRec
//
RC GetNextRecScan(RM_FileScan &fs, RM_Record &rec)
{
    return (fs.GetNextRec(rec));
}

