TARGET = solution
TESTER = tester.jar

CPPFLAGS = -MMD -MP
CXXFLAGS = -Wall -std=c++11 -O2

TESTER_URL = http://www.topcoder.com/contest/problem/MazeFixing/tester.jar

SEED = 1

.PHONY: test visualize batch100 clean distclean

test: $(TARGET) $(TESTER)
	java -jar $(TESTER) -exec ./$(TARGET) -seed $(SEED)

visualize: $(TARGET) $(TESTER)
	java -jar $(TESTER) -exec ./$(TARGET) -vis -seed $(SEED)

batch100: $(TARGET) $(TESTER)
	parallel --jobs `nproc` java -jar $(TESTER) -exec ./$(TARGET) -seed ::: `seq 100` | tee /dev/stderr | grep Score | awk '{m+=$$3} END{print "Total Score = "m}'

$(TARGET): main.o
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $^

$(TESTER):
	curl --output $(TESTER) $(TESTER_URL)

clean:
	$(RM) $(TARGET) $(shell find . -name '*.d' -or -name '*.o')

distclean: clean
	$(RM) $(TESTER)

-include $(shell find . -name '*.d')
