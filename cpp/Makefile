
CGI=solver.cgi
TEST_LIB=solver.so
SRC=solver.cpp GameMatrix.cpp

all: test_lib cgi

test_lib:
	${CXX} -fPIC -shared ${SRC} -I. -o ${TEST_LIB}

cgi:
	${CXX} -g ${SRC} -I. -o ${CGI}

check:
	@echo "NO TEST YET"

clean:
	rm -f *.so *.cgi *.o

