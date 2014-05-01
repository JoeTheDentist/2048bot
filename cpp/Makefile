
CGI=solver.cgi
SRC_CGI=solver.cpp GameMatrix.cpp
TST=test
SRC_TST=test.cpp GameMatrix.cpp

all: ${TST} ${CGI}

${TST}:
	${CXX} ${SRC_TST} -I. -Wl,-Bstatic -lgtest -Wl,-Bdynamic -o ${TST}

${CGI}:
	${CXX} ${SRC_CGI} -I. -o ${CGI}

run: ${CGI}
	./${CGI}

check: ${TST}
	./${TST}

clean:
	rm -f ${CGI} ${TST} *.o

