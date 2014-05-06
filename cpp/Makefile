
CGI=solver.cgi
SRC_COMMON=Simulator.cpp GameMatrix.cpp
SRC_CGI=solver.cpp ${SRC_COMMON}
TST=test
SRC_TST=test.cpp ${SRC_COMMON}

all: clean ${TST} ${CGI}

${TST}:
	${CXX} -O2 ${SRC_TST} -I. -pthread -Wl,-Bstatic -lgtest -Wl,-Bdynamic -o ${TST}

${CGI}:
	${CXX} ${SRC_CGI} -I. -o ${CGI}

run: ${CGI}
	./${CGI}

check: ${TST}
	./${TST}

clean:
	rm -f ${CGI} ${TST} *.o

