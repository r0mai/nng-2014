
all: bloodbank_tester

clean:
	rm -f *.o bloodbank_tester

bloodbank_tester: bloodbank_tester.cpp bloodbank_api_test.cpp
	g++ bloodbank_tester.cpp bloodbank_api_test.cpp -o bloodbank_tester
