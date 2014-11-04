#include <vector>
#include <algorithm>

#include "bloodbank_api.h"

void runtests(BloodBank& bank)
{
	Batch* first= bank.createBatch();

	for(size_t i= 0; i < bank.getNumberOfSamples(); )
	{
		size_t testID= first->addTest();
		first->addSample(testID, i);
		++i;
		if (i < bank.getNumberOfSamples())
		{
			first->addSample(testID, i);
			++i;
		}
	}
	bank.evaluateBatch(first);

	Batch* second= bank.createBatch();
	std::vector<size_t> samples;
	for(size_t i= 0; i < first->getNumberOfTests(); ++i)
	{
		first->getSamples(i, samples);
		if (first->getTestResult(i))
		{
			// BAD Result, retest
			for(std::vector<size_t>::iterator s= samples.begin(); s != samples.end(); ++s)
			{
				size_t testID= second->addTest();
				second->addSample(testID, *s);
			}
		} else
		{
			for(std::vector<size_t>::iterator s= samples.begin(); s != samples.end(); ++s)
			{
				bank.markSafe(*s);
			}
		}
	}
	bank.evaluateBatch(second);

	for(size_t i= 0; i < second->getNumberOfTests(); ++i)
	{
		if (! second->getTestResult(i))
		{
			// Negative test, keep
			second->getSamples(i, samples);
			bank.markSafe(samples[0]);
		}
	}
}