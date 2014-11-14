
all: bloodbank_tester generate_test

clean:
	rm -f *.o bloodbank_tester

bloodbank_tester: bloodbank_tester.cpp bloodbank_api_test.cpp
	g++ bloodbank_tester.cpp bloodbank_api_test.cpp -o bloodbank_tester -std=c++11

generate_test: test_writer.cpp
	g++ test_writer.cpp -o make_tests

