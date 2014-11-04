#ifndef __BLOODBANK_API_H
#define __BLOODBANK_API_H

#include <vector>

class Batch
{
public:
	virtual ~Batch() {}

	/**
	 * Creates a new test within the current batch. 
	 * @return The testID of the new test.
	 */
	virtual size_t addTest()= 0;

	/**
	 * Returns the number of test within the current batch.
	 * Valid testIDs are between: 0 - getNumberOfTests()-1
	 *
	 * @return The testID of the new test
	 */
	virtual size_t getNumberOfTests() const= 0;

	/**
	 * Adds a tiny drop of blood from the given sample to the tests. All samples in the test are
	 * mixed and evaluated together. If any of them is bad blood, than the whole test is going
	 * to yield positive result. Otherwise negative.
	 *
	 * @param testID id of the test
	 * @param sampleID id of the blood sample to be added to the test
	 */
	virtual void addSample(size_t testID, size_t sampleID)= 0;

	/**
	 * After the batch is evaluated this function returns whether any of the samples in the
	 * given testID had any bad blood.
	 *
	 * @param testID test to be queried
	 * @return Result of the test. If true at least one of the samples in the test were bad. 
	 *         False means all samples are safe.
	 */
	virtual bool getTestResult(size_t testID) const= 0;

	/**
	 * Returns the sampleIDs that were mixed in the given testID.
	 *
	 * @param testID ID of the test to be queried
	 * @param samples contents of this vector will be overwritten with the sampleids belonging 
	 *                to the given test
	 */
	virtual void getSamples(size_t testID, std::vector<size_t>& samples) const= 0;
};

class BloodBank
{
public:
	virtual ~BloodBank() {}

	/**
	 * Returns the number of samples in the bank. 
	 * Meaning that the valid sampleID-s are from in range: 0 - getNumberOfSamples()-1
	 */
	virtual size_t getNumberOfSamples() const= 0;

	/**
	 * Creates a new batch for testing. You can run multiple tests at the same time and you'll get 
	 * the results for all the tests after you evaluate the batch.
	 *
	 * @return new batch
	 */
	virtual Batch* createBatch()= 0;

	/**
	 * Marks a given sample safe. At start all samples are marked as unsafe and will be discarded. 
	 * The goal is to mark at least 80% of the samples safe by the end of the runtests function.
	 * Any sample not marked as safe will be discarded. Marking bad blood as safe means that the
	 * algorithm failed and won't be evaluated further.
	 *
	 * @param sampleID blood sample to marked as safe
	 */
	virtual void markSafe(size_t sampleID)= 0;

	/**
	 * Evaluates the given batch. After the evaluation the getTestResult function can be used to 
	 * query the results of the batch. You can only call the function 3 times.
	 *
	 * @param batch The batch to be evaluated
	 */
	virtual void evaluateBatch(Batch* batch)= 0;
};

/**
 * This is the function that you have to implement. Here you get a reference to a BloodBank
 * instance that you can use to create and evaluate batches. Within the batches you can
 * create multiple test that all will be tested at the same time when the evaluateBatch
 * method of the bank is called.
 * You can only call evaluateBatch 3 times within this function. By the time this function
 * returns at least 80% of the samples should be marked as safe, and no bad blood should be
 * marked safe. Violation of these two rules means that the algorithm is automaticcaly
 * disqualified.
 * Please note that this function can be called multiple times with different BloodBank
 * instances within a given execution of the test environment, so if you use global or
 * similar variables please don't forget to reinitialize them when this function is called
 */
void runtests(BloodBank& bank);

#endif