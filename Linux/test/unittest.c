/*========================================================================
** ring_test.c
** Circular buffer testing
** ECEN5813
**========================================================================*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "CUnit/Basic.h"

#include "ring.h"

/* Pointer to the file used by the tests. */
static FILE* temp_file = NULL;


/* The suite initialization function.
 * Opens the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int init_suite1(void)
{
   if (NULL == (temp_file = fopen("temp.txt", "w+"))) {
      return -1;
   }
   else {
      return 0;
   }
}

/* The suite cleanup function.
 * Closes the temporary file used by the tests.
 * Returns zero on success, non-zero otherwise.
 */
int clean_suite1(void)
{
   if (0 != fclose(temp_file)) {
      return -1;
   }
   else {
      temp_file = NULL;
      return 0;
   }
}

/* Simple test of fprintf().
 * Writes test data to the temporary file and checks
 * whether the expected number of bytes were written.
 */
void testINIT(void)
{
	int size;
	ring_t *r;
	if (NULL != temp_file) {
		fprintf(temp_file, "Ring Buffer Initialization tests.\n");
		//Test zero case

		r = init(0);
		fprintf(temp_file, "Ring Buffer size: %d.\n", 0);
		CU_ASSERT(r==NULL);
		fprintf(temp_file, "Zero case error test concluded.\n\n");

		//Maximum Size test
		fprintf(temp_file, "Maximum Buffer Size test.\n");
		fprintf(temp_file, "Maximum Ring Buffer size: %d.\n", BUFF_MAX_SIZE);
		r = init(BUFF_MAX_SIZE);
		CU_ASSERT(r!=NULL);
		CU_ASSERT(r->Length == BUFF_MAX_SIZE);
		CU_ASSERT(r->Outi == 0);
		CU_ASSERT(r->Ini == 0);
		fprintf(temp_file, "Maximum size test concluded.\n\n");

	
		fprintf(temp_file, "Exceed Maximum Buffer Size test.\n");
		fprintf(temp_file, "Ring Buffer size: %d.\n", BUFF_MAX_SIZE+1);
		r = init(BUFF_MAX_SIZE+1);
		CU_ASSERT(r==NULL);
		fprintf(temp_file, "Exceed Maximum size test concluded.\n\n");

		fprintf(temp_file, "Ring Buffer Random size tests.\n");
		
		int size2= rand() % 1000;
		fprintf(temp_file, "Number of tests: %d.\n", size2);
		for (int i=0; i< size2;i++)
		{
			size = rand() % BUFF_MAX_SIZE;
			fprintf(temp_file, "Random Size test %d.\n", i);
			r = init(size);
			fprintf(temp_file, "Ring Buffer size: %d.\n", size);
			CU_ASSERT(r->Length == size);
			CU_ASSERT(r->Outi == 0);
			CU_ASSERT(r->Ini == 0);
			free(r);
		}
		fprintf(temp_file, "Random sizes tests concluded.\n\n");
	}
}

/* Simple test of fread().
 * Reads the data previously written by testFPRINTF()
 * and checks whether the expected characters are present.
 * Must be run after testFPRINTF().
 */
void testInsert(void)
{

	ring_t *r = init((rand() % BUFF_MAX_SIZE));
	fprintf(temp_file,"Ring Buffer Size: %d.\n", r->Length);

	for( int i = 0; i < r->Length;++i)
	{
			insert(r, 'a');
	}
	// value 0 is char for NULL so it wont count it
	CU_ASSERT(r->Count == r->Length);
	//rerturn error that buffer is full
	CU_ASSERT(insert(r, 'a')==1);
		

	CU_ASSERT(insert(r, 'a')==1);
	
	r=init(1);
	CU_ASSERT(insert(r, 255)==0);
		
}

void testRemove(void)
{
	int size= rand() % BUFF_MAX_SIZE;
	char var='\0';
	ring_t *r;
	if (NULL != temp_file) {
		r = init(size);

		fprintf(temp_file,"Ring Buffer Size: %d.\n", r->Length);

		for(int i = 0; i <size;++i)
		{
			insert(r, 'a');
		}
		// value 0 is char for NULL so it wont count it
		CU_ASSERT(remove_ring(r,&var) == 0);
		CU_ASSERT(var=='a');
		CU_ASSERT(r->Buffer[r->Outi-1]=='\0');
		CU_ASSERT(r->Count == r->Length-1);
		fprintf(temp_file, "Single removed element concluded.\n");
	}
}

void testError(void)
{
	char var='\0';
	ring_t *r;
	if (NULL != temp_file) {
		//NUll tests
		r = init(0);
		CU_ASSERT(r == NULL);
		CU_ASSERT(insert(r,'a') == 3);
		CU_ASSERT(remove_ring(r,&var) == 3);
		CU_ASSERT(r == NULL);

		r=init(10);
		CU_ASSERT(r != NULL);
		r->Count = 10;
		CU_ASSERT(insert(r,'a') == 1);
		r->Count = 0;
		for(int i = 0; i <10; ++i)
		{
			insert(r, 'a');
		}
		CU_ASSERT(insert(r,'b') == 1);
		CU_ASSERT(resize(r, 5) == 1);
		r->Count = -1;
		CU_ASSERT(remove_ring(r,&var) == 1);
		r->Count = 10;
		for(int i = 0; i <10; ++i)
		{
			remove_ring(r, &var);
		}
		CU_ASSERT(remove_ring(r,&var) == 1);
		fprintf(temp_file, "Error detection tests concluded.\n");
	}
}

/* The main() function for setting up and running the tests.
 * Returns a CUE_SUCCESS on successful running, another
 * CUnit error code on failure.
 */
int main()
{
   CU_pSuite pSuite = NULL;

   /* initialize the CUnit test registry */
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

   /* add a suite to the registry */
   pSuite = CU_add_suite("Suite_1", init_suite1, clean_suite1);
   if (NULL == pSuite) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the suite */
   /* NOTE - ORDER IS IMPORTANT - MUST TEST fread() AFTER fprintf() */
   if ( (NULL == CU_add_test(pSuite, "test of Init()", testINIT)) ||
 		(NULL == CU_add_test(pSuite, "test of Insert()", testInsert)) ||
		(NULL == CU_add_test(pSuite, "test of Remove()", testRemove)) ||
		(NULL == CU_add_test(pSuite, "test of Errors()", testError)) )
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* Run all tests using the CUnit Basic interface */
   CU_basic_set_mode(CU_BRM_VERBOSE);
   CU_basic_run_tests();
   CU_cleanup_registry();
   return CU_get_error();
}
