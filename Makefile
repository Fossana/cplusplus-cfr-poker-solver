CC=gcc
CXX=g++
RM=rm # -f
CPPFLAGS=-g
LDFLAGS=-g 
LDLIBS=-ltbb -ltcmalloc
SRCS= Action.cpp card_utility.cpp GameTree.cpp\
    program.cpp TerminalNode.cpp ActionNode.cpp\
	CfrTask.cpp Hand.cpp RangeManager.cpp Trainer.cpp\
    BestResponse.cpp ChanceNodeChild.cpp  HandEvaluator.cpp\
    ShowdownTask.cpp TreeBuildSettings.cpp\
    BestResponseTask.cpp ChanceNode.cpp\
    PlayerState.cpp State.cpp

OBJS=$(subst .cpp,.o,$(SRCS))

all: tool

tool: $(OBJS)
	$(CXX) $(LDFLAGS) -o tool $(OBJS) $(LDLIBS)

depend: .depend

.depend: $(SRCS)
	@rm ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

clean:
	$(RM) $(OBJS)

distclean: clean
	$(RM) *~ .depend

include .depend
