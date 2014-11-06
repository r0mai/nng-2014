#define _CRT_SECURE_NO_WARNINGS

#include <cstring>
#include <vector>
#include <iostream>
#include <fstream>
#include <map>
#include <algorithm>

#include "bloodbank_api.h"

#define assert(x) if (x) ; else { std::cout << "ASSERT failed: " << #x << std::endl; exit(1); }

class BatchImp : public Batch
{
	friend class BloodBankImp; 

	struct Test
	{
		typedef std::vector<size_t> samples_t;
		samples_t samples;
		int result;

		Test(): result(-1) {}
	};

	typedef std::vector<BatchImp::Test> tests_t;
	tests_t mTests;

public:
	BatchImp() {}
	virtual ~BatchImp() {}

	virtual size_t addTest();
	virtual size_t getNumberOfTests() const;
	virtual void addSample(size_t testID, size_t sampleID);
	virtual bool getTestResult(size_t testID) const;
	virtual void getSamples(size_t testID, std::vector<size_t>& samples) const;
};

size_t BatchImp::addTest()
{
	mTests.push_back(BatchImp::Test());	

	return mTests.size()-1;
}

size_t BatchImp::getNumberOfTests() const
{
	return mTests.size();
}

void BatchImp::addSample(size_t testID, size_t sampleID)
{
	mTests[testID].samples.push_back(sampleID);
}

bool BatchImp::getTestResult(size_t testID) const
{
	const BatchImp::Test& test= mTests[testID];
	assert(test.result != -1);
	return test.result == 1;
}

void BatchImp::getSamples(size_t testID, std::vector<size_t>& samples) const
{
	const BatchImp::Test& test= mTests[testID];
	samples.assign(test.samples.begin(), test.samples.end());
}

const size_t MAX_BATCH_COUNT= 3;

class BloodBankImp : public BloodBank
{
	struct Sample
	{
		bool BadBlood;
		bool Safe;
		Sample () : BadBlood(0), Safe(0) {}
	};
	typedef std::vector<Sample> samples_t;
	samples_t mSamples;
	size_t mBatchCount;
	size_t mTestCount;

	void loadSamples(const std::string& filename);

public:
	BloodBankImp(const std::string& filename); 
	virtual ~BloodBankImp() {}

	virtual size_t getNumberOfSamples() const;
	virtual Batch* createBatch();
	virtual void markSafe(size_t sampleID);
	virtual void evaluateBatch(Batch* batch);

	size_t done(bool quiet= false);
};

BloodBankImp::BloodBankImp(const std::string& filename)
:	mBatchCount(0)
,	mTestCount(0)
{
	loadSamples(filename);
}

void BloodBankImp::loadSamples( const std::string& filename )
{
	std::ifstream f(filename.c_str());
	size_t samplecount;
	f >> samplecount;
	mSamples.resize(samplecount);
	for(size_t i= 0; i < samplecount; ++i)
	{
		size_t tmp;
		f >> tmp;
		mSamples[i].BadBlood= tmp != 0;
	}
}

size_t BloodBankImp::getNumberOfSamples() const
{
	return mSamples.size();
}

Batch* BloodBankImp::createBatch()
{
	return new BatchImp();
}

void BloodBankImp::markSafe( size_t sampleID )
{
	mSamples[sampleID].Safe= true;
}

void BloodBankImp::evaluateBatch( Batch* batch )
{
	assert(mBatchCount < MAX_BATCH_COUNT);
	++mBatchCount;
	BatchImp& btch= *static_cast<BatchImp*>(batch);
	for(BatchImp::tests_t::iterator t= btch.mTests.begin(); t != btch.mTests.end(); ++t)
	{
		BatchImp::Test& test= *t;
		++mTestCount;
		test.result= 0;
		for(BatchImp::Test::samples_t::iterator i= test.samples.begin(); i != test.samples.end(); ++i)
		{
			if (mSamples[*i].BadBlood)
			{
				test.result= 1;
				break;
			}
		}
	}
}

size_t BloodBankImp::done(bool quiet)
{
	size_t MAX_DISCARDED= mSamples.size() / 5;
	size_t discarded= 0;
	size_t safe= 0;
	size_t bad= 0;
	std::cout << "Test count: " << mTestCount << std::endl;
	if (!quiet)
	{
		std::cout << "Batch count: " << mBatchCount << std::endl;
	}
	for(samples_t::iterator s= mSamples.begin(); s != mSamples.end(); ++s)
	{
		if (s->Safe)
		{
			if (s->BadBlood)
			{
				std::cout << "ERROR: Bad sample marked as safe! ID: " << (s-mSamples.begin()) << std::endl;
				exit(1);
			} else
			{
				++safe;
			}
		} else
		{
			++discarded;
			if (s->BadBlood)
			{
				++bad;
			} 
		}
	}
	if (!quiet)
	{
		std::cout << "Identified safe: " << safe << " / " << mSamples.size() << std::endl;
		std::cout << "Discarded  safe: " << discarded - bad << " / " << mSamples.size() << std::endl;
		std::cout << "Discarded   bad: " << bad << " / " << mSamples.size() << std::endl;
		std::cout << "Discarded total: " << discarded << " / " << mSamples.size() << std::endl;
	}
	if (discarded > MAX_DISCARDED)
	{
		std::cout << "ERROR: Too many discarded good samples: " << discarded << " Max: " << MAX_DISCARDED << std::endl;
		exit(1);
	}

	return mTestCount;
}


int main(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cerr << "Usage: " << argv[0] << " <testfile>" << std::endl;
		exit(1);
	}

	if (strcmp(argv[1], "all") == 0)
	{
		size_t tests= 0;
		for(int i= 1; i <= 100; ++i)
		{
			char filename[255];
			sprintf(filename, "blood_%03d.txt", i);
			BloodBankImp bank(filename);
			runtests(bank);
			std::cout << i << ".";
			tests+= bank.done(true);
		}
		std::cout << "Total TESTS:" << tests << std::endl;
	} else
	{
		BloodBankImp bank(argv[1]);

		runtests(bank);

		bank.done();
	}
}

