
CGI=solver.cgi
SRC_COMMON=Simulator.cpp GameMatrix.cpp
SRC_CGI=solver.cpp ${SRC_COMMON}
TST=test
SRC_TST=test.cpp ${SRC_COMMON}

all: clean ${TST} ${CGI}

${TST}: clean_test
	${CXX} -O2 ${SRC_TST} -I. -pthread -Wl,-Bstatic -lgtest -Wl,-Bdynamic -o ${TST}

${CGI}:
	${CXX} ${SRC_CGI} -I. -o ${CGI}

run: ${CGI}
	./${CGI}

check: ${TST}
	./${TST}

cg:
	${CXX} -DNO_RANDOM -g ${SRC_TST} -I. -pthread -Wl,-Bstatic -lgtest -Wl,-Bdynamic -o ${TST}
	valgrind --tool=cachegrind ./${TST} --gtest_filter=*SimpleGame*

clean: clean_test clean_cgi

clean_test:
	rm -f ${TST}

clean_cgi:
	rm -f ${CGI}
