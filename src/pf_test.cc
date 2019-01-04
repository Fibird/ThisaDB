#include <cstdio>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include "pf.h"
#include "pf_internal.h"
#include "pf_hashtable.h"

using namespace std;

void pauseProgram()
{
    cout << "Press any key to continue..." << endl;
    getchar();
}

//
// Defines
//
#define TEST_FILE "pf_test_file"

//
// Function declarations
//
RC WriteFile(PF_Manager &pfm, char *fname);
RC PrintFile(PF_FileHandle &fh);
RC ReadFile(PF_Manager &pfm, char* fname);
RC TestPF();
RC TestHash();

RC WriteFile(PF_Manager &pfm, char *fname)
{
   PF_FileHandle fh;
   PF_PageHandle ph;
   RC            rc;
   char          *pData;
   PageNum       pageNum;
   int           i;

//   cout << "Opening file: " << fname << "\n";

   if ((rc = pfm.OpenFile(fname, fh)))
      return(rc);

   for (i = 0; i < PF_BUFFER_SIZE; i++) {
       // test for allcating page and getting page
      if ((rc = fh.AllocatePage(ph)) ||
            (rc = ph.GetData(pData)) ||
            (rc = ph.GetPageNum(pageNum)))
         return(rc);

      if (i != pageNum) {
         cout << "Page number incorrect: " << (int)pageNum << " " << i << "\n";
         exit(1);
      }

      memcpy(pData, (char *)&pageNum, sizeof(PageNum));

      cout << "Page allocated: " << (int)pageNum << "\n";
   }

   cout << "Allocated " << PF_BUFFER_SIZE << " Page." << endl;

   // Test pinning too many pages
   if ((rc = fh.AllocatePage(ph)) != PF_NOBUF) {
      cout << "Pin too many pages should fail: ";
      return(rc);
   }

   cout << "Unpinning pages and closing the file\n";

   for (i = 0; i < PF_BUFFER_SIZE; i++)
      if ((rc = fh.UnpinPage(i)))
         return(rc);

   if ((rc = pfm.CloseFile(fh)))
      return(rc);

   // Return ok
   return (0);
}

RC PrintFile(PF_FileHandle &fh)
{
   PF_PageHandle ph;
   RC            rc;
   char          *pData;
   PageNum       pageNum, temp;

   cout << "Reading file\n";

   if ((rc = fh.GetFirstPage(ph)))
      return(rc);

   do {
      if ((rc = ph.GetData(pData)) ||
            (rc = ph.GetPageNum(pageNum)))
         return(rc);

      memcpy((char *)&temp, pData, sizeof(PageNum));
      cout << "Got page: " << (int)pageNum << " " << (int)temp << "\n";

      if ((rc = fh.UnpinPage(pageNum)))
         return(rc);
   } while (!(rc = fh.GetNextPage(pageNum, ph)));

   if (rc != PF_EOF)
      return(rc);

   cout << "EOF reached\n";

   // Return ok
   return (0);
}

RC ReadFile(PF_Manager &pfm, char* fname)
{
   PF_FileHandle fh;
   RC            rc;

   cout << "Opening: " << fname << "\n";

   if ((rc = pfm.OpenFile(fname, fh)) ||
         (rc = PrintFile(fh)) ||
         (rc = pfm.CloseFile(fh)))
      return (rc);
   else
      return (0);
}

RC TestPF()
{
   PF_Manager    pfm;
   PF_FileHandle fh1, fh2;
   PF_PageHandle ph;
   RC            rc;
   char          *pData;
   PageNum       pageNum, temp;
   int           i;

   int len;
   pfm.GetBlockSize(len);
   printf("get bock size returned %d\n",len);
   cout << "Creating and opening file\n";

   if ((rc = pfm.CreateFile(TEST_FILE)) ||
         (rc = WriteFile(pfm, (char*)TEST_FILE)))
      return(rc);

   pauseProgram();

   if ((rc = ReadFile(pfm, (char*)TEST_FILE)) ||
         (rc = pfm.OpenFile(TEST_FILE, fh1))) 
      return(rc);

   cout << "Allocating additional pages in test file\n";
   pauseProgram();

   for (i = PF_BUFFER_SIZE; i < PF_BUFFER_SIZE * 2; i++) {
      if ((rc = fh1.AllocatePage(ph)) ||
            (rc = ph.GetData(pData)) ||
            (rc = ph.GetPageNum(pageNum)))
         return(rc);

      if (i != pageNum) {
         cout << "Page number is incorrect:" << (int)pageNum << " " << i << "\n";
         exit(1);
      }

      memcpy(pData, (char*)&pageNum, sizeof(PageNum));
      cout << "Page allocated: " << (int)pageNum << "\n";

      if ((rc = fh1.MarkDirty(pageNum)) ||
            (rc = fh1.UnpinPage(pageNum)))
         return(rc);
   }

   rc = pfm.CloseFile(fh1);
   pauseProgram();
   rc = ReadFile(pfm, (char*)TEST_FILE);

   if ((rc = pfm.DestroyFile(TEST_FILE)))
      return(rc);

   return 0;
}

RC TestHash()
{
   PF_HashTable ht(PF_HASH_TBL_SIZE);
   RC           rc;
   int          i, s;
   PageNum      p;

   cout << "Testing hash table.  Inserting entries\n";

   for (i = 1; i < 11; i++)
      for (p = 1; p < 11; p++)
         if ((rc = ht.Insert(i, p, i + p)))
            return(rc);

   cout << "Searching for entries\n";

   for (i = 1; i < 11; i++)
      for (p = 1; p < 11; p++)
         if ((rc = ht.Find(i, p, s)))
            return(rc);

   cout << "Deleting entries in reverse order\n";

   for (p = 10; p > 0; p--)
      for (i = 10; i > 0; i--)
         if ((rc = ht.Delete(i,p)))
            return(rc);

   cout << "Ensuring all entries were deleted\n";

   for (i = 1; i < 11; i++)
      for (p = 1; p < 11; p++)
         if ((rc = ht.Find(i, p, s)) != PF_HASHNOTFOUND) {
            cout << "Find deleted hash entry should fail: ";
            return(rc);
         }

   // Return ok
   return (0);
}

int main()
{
   RC rc;

   // Write out initial starting message
   cerr.flush();
   cout.flush();
   cout << "Starting PF test.\n";
   cout.flush();
   pauseProgram();

   // Delete files from last time
   unlink(TEST_FILE);

   // Do tests
   if ((rc = TestPF())) {
      PF_PrintError(rc);
      return 1;
   }

   pauseProgram();

   if ((rc = TestHash())) {
      PF_PrintError(rc);
      return 1;
   }

   return (0);
}

